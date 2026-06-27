#include "uros_wrapper.h"


void prvUrosBootError(UrosWrapperCore_t* pxUrosWrapper) {
    if (xSemaphoreTake((pxUrosWrapper->pxStatusLed)->xLedSemaphore, portMAX_DELAY) == pdTRUE) {
        while(1)
        {
            vStatusLedFlash((pxUrosWrapper->pxStatusLed), 1000, 1000);
        }
        xSemaphoreGive((pxUrosWrapper->pxStatusLed)->xLedSemaphore);
    }
}

bool prvUrosInitTransport()
{

    return rmw_uros_set_custom_transport(
        false,          // must be false for UDP
        &picow_params,
        picow_udp_transport_open,
        picow_udp_transport_close,
        picow_udp_transport_write,
        picow_udp_transport_read
    ) == RMW_RET_OK;
}

UrosWrapperCore_t* vUrosInit(char* pcNodeName, StatusLed_t* pxStatusLed)
{
    printf("Initializing wifi\n");
    if (cyw43_arch_init()) {
        printf("failed to initialise\n");
        vTaskDelete(NULL);
    }

    printf("Switching to Station mode\n");
    cyw43_arch_enable_sta_mode();

    printf("Connecting to WiFi...\n");
    if (cyw43_arch_wifi_connect_timeout_ms(WIFI_SSID, WIFI_PASSWORD, CYW43_AUTH_WPA2_AES_PSK, 30000)) {
        printf("failed to connect.\n");
        vTaskDelete(NULL);
    } else {
        printf("Connected.\n");
    }
    
    UrosWrapperCore_t* pxUrosWrapper = pvPortMalloc(sizeof(UrosWrapperCore_t));
    configASSERT(pxUrosWrapper != NULL);
    configASSERT(pxStatusLed != NULL);
    configASSERT(pcNodeName != NULL);
    configASSERT(strlen(pcNodeName) < MAX_NODE_LENGTH);

    strcpy(pxUrosWrapper->pcNodeName, pcNodeName);
    pxUrosWrapper->n_publishers = 0;
    pxUrosWrapper->n_subscribers = 0;
    pxUrosWrapper->pxStatusLed = pxStatusLed;

    pxUrosWrapper->xPublishSemaphore = xSemaphoreCreateMutex();
    configASSERT(pxUrosWrapper->xPublishSemaphore != NULL);

    if (!prvUrosInitTransport())
    {
        prvUrosBootError(pxUrosWrapper);
    }

    pxUrosWrapper->xAllocator = rcl_get_default_allocator();

    const int timeout_ms = 1000;
    const uint8_t attempts = 120;

    if (rmw_uros_ping_agent(timeout_ms, attempts) != RCL_RET_OK) {
        prvUrosBootError(pxUrosWrapper);
    }

    rclc_support_init(&(pxUrosWrapper->xSupport), 0, NULL, &(pxUrosWrapper->xAllocator));
    rclc_node_init_default(&(pxUrosWrapper->xNode), pxUrosWrapper->pcNodeName, "", &(pxUrosWrapper->xSupport));

    // Synchronize time with the agent
    rmw_uros_sync_session(timeout_ms);

    vTaskDelay(pdMS_TO_TICKS(1000));

    return pxUrosWrapper;
}

void vUrosSetup(UrosWrapperCore_t* pxUrosWrapper)
{
    rclc_executor_init(&(pxUrosWrapper->xExecutor), &((pxUrosWrapper->xSupport).context), 1, &(pxUrosWrapper->xAllocator));

    for (int i = 0; i < pxUrosWrapper->n_subscribers; i++) {
        // Add subscription to executor
        rcl_ret_t rc = rclc_executor_add_subscription(
            &(pxUrosWrapper->xExecutor),
            &((pxUrosWrapper->xSubscribers)[i].xSubscriber),
            (pxUrosWrapper->xSubscribers)[i].pvMsg,
            (pxUrosWrapper->xSubscribers)[i].xCallback,
            (pxUrosWrapper->xSubscribers)[i].xInvocation
        );
    }
}

void vUrosRunTask(void* pvParameters)
{
    UrosWrapperCore_t* pxUrosWrapper = (UrosWrapperCore_t*) pvParameters;
    while (1) {
        if (xSemaphoreTake(pxUrosWrapper->xPublishSemaphore, portMAX_DELAY) == pdTRUE) {
            rclc_executor_spin_some(&(pxUrosWrapper->xExecutor), 0);
            xSemaphoreGive(pxUrosWrapper->xPublishSemaphore);
        }
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

void vUrosWrapperStart(UrosWrapperCore_t* pxUrosWrapper)
{
    TaskHandle_t xExecutorHandle;
    BaseType_t xTaskCreated = xTaskCreate(
        vUrosRunTask,
        "uROS Executor",
        4096,
        (void*) pxUrosWrapper,
        5,
        &xExecutorHandle
    );
    configASSERT(xTaskCreated == pdPASS);

#if defined(configUSE_CORE_AFFINITY) && (configUSE_CORE_AFFINITY == 1)
    if (xTaskCreated == pdPASS) {
        vTaskCoreAffinitySet(xExecutorHandle, (1 << 0));
    }
#endif
}

UrosWrapperPublisher_t* vUrosWrapperAddPublisher(
    UrosWrapperCore_t* pxUrosWrapper,
    char* pcTopicName, 
    const rosidl_message_type_support_t* pxTypeSupport,
    BaseType_t xReliable
)
{
    configASSERT(pxUrosWrapper != NULL);
    configASSERT(pxUrosWrapper->n_publishers < MAX_PUBLISHERS);

    if (xReliable)
    {
        rclc_publisher_init_default(
            &(pxUrosWrapper->xPublishers[pxUrosWrapper->n_publishers]),
            &(pxUrosWrapper->xNode),
            pxTypeSupport,
            pcTopicName
        );
    }
    else
    {
        rclc_publisher_init_best_effort(
            &(pxUrosWrapper->xPublishers[pxUrosWrapper->n_publishers]),
            &(pxUrosWrapper->xNode),
            pxTypeSupport,
            pcTopicName
        );
    }

    (pxUrosWrapper->n_publishers)++;

    UrosWrapperPublisher_t* pxUrosWrapperPublisher = pvPortMalloc(sizeof(UrosWrapperPublisher_t));
    configASSERT(pxUrosWrapperPublisher != NULL);
    pxUrosWrapperPublisher->pcTopicName = pcTopicName;
    pxUrosWrapperPublisher->pxPublisher = &(pxUrosWrapper->xPublishers[(pxUrosWrapper->n_publishers) - 1]);
    pxUrosWrapperPublisher->pxStatusLed = pxUrosWrapper->pxStatusLed;
    pxUrosWrapperPublisher->pxPublishSemaphore = &pxUrosWrapper->xPublishSemaphore;

    return pxUrosWrapperPublisher;
}

rcl_ret_t pxUrosWrapperPublish(
    UrosWrapperPublisher_t* pxUrosWrapperPublisher,
    const void* pvMessage,
    rmw_publisher_allocation_t* pxAllocation
)
{
    if (xSemaphoreTake(*(pxUrosWrapperPublisher->pxPublishSemaphore), pdMS_TO_TICKS(10)) != pdTRUE) {
        return RCL_RET_TIMEOUT;
    }

    rcl_ret_t xRet = rcl_publish(
        pxUrosWrapperPublisher->pxPublisher,
        pvMessage,
        pxAllocation
    );
    xSemaphoreGive(*(pxUrosWrapperPublisher->pxPublishSemaphore));

    return xRet;
}

rcl_ret_t pxUrosWrapperPublishBlink(
    UrosWrapperPublisher_t* pxUrosWrapperPublisher,
    const void* pvMessage,
    rmw_publisher_allocation_t* pxAllocation
)
{
    rcl_ret_t xRet = pxUrosWrapperPublish(pxUrosWrapperPublisher, pvMessage, pxAllocation);
    vStatusLedActivityTrigger(pxUrosWrapperPublisher->pxStatusLed);

    return xRet;
}

void vUrosWrapperAddSubscriber(
    UrosWrapperCore_t* pxUrosWrapper,
    char* pcTopicName, 
    const rosidl_message_type_support_t* pxTypeSupport,
    void* pvMsg,
    rclc_subscription_callback_t xCallback,
    rclc_executor_handle_invocation_t xInvocation
)
{
    UrosWrapperSubscriber_t* pxSubscriber = &((pxUrosWrapper->xSubscribers)[pxUrosWrapper->n_subscribers]);
    pxSubscriber->pcTopicName = pcTopicName;
    pxSubscriber->pvMsg = pvMsg;
    pxSubscriber->xCallback = xCallback;
    pxSubscriber->xInvocation = xInvocation;

    rcl_ret_t xRet = rclc_subscription_init_default(
        &pxSubscriber->xSubscriber,
        &(pxUrosWrapper->xNode),
        pxTypeSupport,
        pcTopicName
    );

    (pxUrosWrapper->n_subscribers)++;
}
