#include <uros_wrapper.hpp>

// Configuring logger
#define LIBRARY_LOG_NAME "UROS_WRAPPER"
#define LIBRARY_LOG_LEVEL  LOG_INFO

#include <uart_logger.hpp>
#include <logging_stack.h>


// microROS transport init
BaseType_t UrosWrapper::prvUrosTransportInit() {
    LogDebug( ("Initializing microROS transport") );
    rmw_uros_set_custom_transport(
        true,
        NULL,
        pico_serial_transport_open,
        pico_serial_transport_close,
        pico_serial_transport_write,
        pico_serial_transport_read
    );
    LogDebug( ("microROS transport initialized") );
    return pdTRUE;
}

// microROS FreeRTOS tasks
void UrosWrapper::prvMicroRosExecutorTask(void* pvParameters) {
    LogDebug( ("Running microROS executor task") );
    // Get the object
    UrosWrapper* obj = (UrosWrapper*) pvParameters;

    // Run the executor
    while (1) {
        if (xSemaphoreTake(obj->xUrosMutex, portMAX_DELAY) == pdFALSE)
        {
            continue;
        }
        rclc_executor_spin_some(&(obj->xRclcExecutor), 0);
        xSemaphoreGive(obj->xUrosMutex);
        vTaskDelay(pdMS_TO_TICKS(1));
    }
}

// Constructors
UrosWrapper::UrosWrapper() {
    // Create a mutex that will control the usage of microROS
    this->xUrosMutex = xSemaphoreCreateMutex();
    if (this->xUrosMutex == NULL)
    {
        LogError( ("Unable to create microROS mutex") );
        vErrorSequence();
    }

    this->ucTotalNodes = 0;
}

// microROS init
BaseType_t UrosWrapper::xInitializeUros() {
    LogInfo( ("Initializing microROS") );

    if (xSemaphoreTake(this->xUrosMutex, portMAX_DELAY) == pdFALSE)
    {
        LogDebug( ("Unable to take microROS mutex") );
        vErrorSequence();
    }

    // microROS transport pre-init and function init
    if (prvUrosTransportInit() == pdFALSE)
    {
        LogDebug( ("Unable to initialize microROS transport") );
        vErrorSequence();
    }

    // Fetch the microROS allocator
    this->xRclAllocator = rcl_get_default_allocator();

    // Ping the agent until timeout is reached
    LogInfo( ("Waiting for microROS agent") );
    rmw_ret_t ping_ret = rmw_uros_ping_agent(RCL_MAX_AGENT_TIMEOUT, RCL_MAX_AGENT_ATTEMPTS);
    if (ping_ret != RCL_RET_OK)
    {
        LogError( ("Unable to ping microROS agent. ret='%d'", (int)ping_ret) );
        vErrorSequence();
    }

    // Initialize the microROS support
    LogInfo( ("Agent ping succeeded; initializing microROS support") );
    if (rclc_support_init(&(this->xRclcSupport), 0, NULL, &(this->xRclAllocator)) != RCL_RET_OK)
    {
        LogError( ("Unable to initialize microROS support") );
        vErrorSequence();
    }

    // Synchronize time with agent
    rmw_uros_sync_session(RCL_MAX_AGENT_TIMEOUT);

    xSemaphoreGive(this->xUrosMutex);

    LogInfo( ("microROS initialized") );

    return pdTRUE;
}

// Returns a node index which can then be used to add publishers/subscriptions
unsigned char UrosWrapper::ucCreateNode(const char* pcNodeNamespace, const char* pcNodeName) {
    LogInfo( ("Creating node. name='%s'", pcNodeName) );

    // Fetch the current number and store as index
    unsigned char ucNodeIndex = this->ucTotalNodes;

    // Copy the namespace and name into local variable
    UrosWrapperNode* node = &(this->xUrosWrapperNodes[ucNodeIndex]);

    // Copy the pointer to uros mutex
    node->pxUrosMutex = &(this->xUrosMutex);

    if (node == NULL)
    {
        LogError( ("Variable 'node' is uninitalized") );
        vErrorSequence();
    }

    // Copy the namespace and name
    strncpy(node->pcNodeNamespace, pcNodeNamespace, MAX_NODE_NAME_LENGTH);
    strncpy(node->pcNodeName, pcNodeName, MAX_NODE_NAME_LENGTH);

    // Initialize the node
    if (xSemaphoreTake(this->xUrosMutex, portMAX_DELAY) == pdFALSE)
    {
        LogError( ("Unable to take microROS mutex") );
        vErrorSequence();
    }

    rcl_ret_t ret = rclc_node_init_default(&(node->xNode), pcNodeName, pcNodeNamespace, &(this->xRclcSupport));

    xSemaphoreGive(this->xUrosMutex);

    if (ret != RCL_RET_OK)
    {
        LogError( ("Unable to initialize microROS node") );
        vErrorSequence();
    }

    // Increment the node counter
    (this->ucTotalNodes)++;

    return ucNodeIndex;
}

// Getter function to fetch Node data pointer
UrosWrapperNode* UrosWrapper::pxGetNode(unsigned char ucNodeIndex) {
    // Check whether node is available and return
    if (ucNodeIndex < this->ucTotalNodes)
    {
        return &(this->xUrosWrapperNodes[ucNodeIndex]);
    }
    return NULL;
}

// Function to start the microROS executor task and register all the events
BaseType_t UrosWrapper::xStartUrosWrapper() {
    LogInfo( ("Starting microROS wrapper") );

    // Initialize microROS executor
    unsigned int uiTotalHandles = 0;
    for (int i=0; i<(this->ucTotalNodes); i++)
    {
        uiTotalHandles += (this->pxGetNode(i)->ucTotalSubscriptions);
    }
    LogDebug( ("Fetched handles. count='%d'", uiTotalHandles) );

    if (uiTotalHandles == 0)
    {
        LogInfo( ("microROS wrapper started without executor") );
        return pdTRUE;
    }

    if (xSemaphoreTake(this->xUrosMutex, portMAX_DELAY) == pdFALSE)
    {
        LogError( ("Unable to take microROS mutex") );
        vErrorSequence();
    }

    LogDebug( ("Initializing executor") );
    rcl_ret_t ret = rclc_executor_init(
        &(this->xRclcExecutor),
        &((this->xRclcSupport).context),
        uiTotalHandles,
        &this->xRclAllocator
    );
    LogDebug( ("Initialized executor") );

    xSemaphoreGive(this->xUrosMutex);

    if (ret != RCL_RET_OK)
    {
        LogError( ("Unable to initialize microROS executor") );
        vErrorSequence();
    }

    for (int iNodeId = 0; iNodeId < this->ucTotalNodes; iNodeId++) {
        UrosWrapperNode* xNode = this->pxGetNode(iNodeId);

        for (int iSubscriptionId = 0; iSubscriptionId < xNode->ucTotalSubscriptions; iSubscriptionId++) {
            UrosWrapperSubscription* xUrosSubscriber = xNode->pxGetSubscription(iSubscriptionId);

            // Add subscription to executor
            rcl_ret_t rc = rclc_executor_add_subscription(
                &(this->xRclcExecutor),
                xUrosSubscriber->xGetSubscription(),
                xUrosSubscriber->pvGetMsg(),
                xUrosSubscriber->xGetCallback(),
                xUrosSubscriber->xGetInvocation()
            );
        }
    }

    TaskHandle_t xExecutorHandle;
    BaseType_t xTaskCreated = xTaskCreate(
        this->prvMicroRosExecutorTask,
        "uROS Executor",
        4096,
        (void*) this,
        2,
        &xExecutorHandle
    );

    if (xTaskCreated == pdFAIL)
    {
        LogError( ("Unable to create microROS executor task") );
        vErrorSequence();
    }

    LogInfo( ("microROS wrapper started") );
    return pdTRUE;
}
