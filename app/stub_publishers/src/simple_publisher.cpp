#include <simple_publisher.hpp>

// Configuring logger
#define LIBRARY_LOG_NAME "SIMPLE_PUBLISHER"
#define LIBRARY_LOG_LEVEL  LOG_INFO

#include <uart_logger.hpp>
#include <logging_stack.h>


static void prvSimplePublishTask(void* pvParameters)
{
    LogInfo( ("Starting publisher task") );
    SimplePublisher_t* pxSimplePublisher = (SimplePublisher_t*) pvParameters;
    UrosWrapper* obj = pxSimplePublisher->pxWrapper;
    unsigned char ucSimplePublisherIndex = pxSimplePublisher->ucPublisherIndex;
    unsigned char ucSimpleNodeIndex = pxSimplePublisher->ucNodeIndex;
    unsigned long ulPeriodMs = pxSimplePublisher->ulPeriodMs;
    
    // Variable storing the message
    std_msgs__msg__Int64 xMsg;

    // Variables handling the timings
    TickType_t xLastWakeTime = xTaskGetTickCount();
    const TickType_t xPeriod = pdMS_TO_TICKS(ulPeriodMs);

    // Iteratively increment the data and publish
    xMsg.data = 0;
    while (1) {
        LogDebug( ("Publishing message. message='%d'", xMsg.data) );
        BaseType_t ret = obj->pxGetNode(ucSimpleNodeIndex)->xPublishMessage(ucSimplePublisherIndex, &xMsg, NULL);

        if (ret != pdPASS)
        {
            LogError( ("Failed to publish message", xMsg.data) );
            vErrorSequence();
        }

        xMsg.data++;
        vTaskDelayUntil(&xLastWakeTime, xPeriod);
    }
}

BaseType_t xSimplePublisherCreate(SimplePublisher_t* pxSimplePublisher, UrosWrapper* xUrosWrapper, unsigned char ucNodeIndex, const char* pcTopic, unsigned long ulPeriodMs)
{
    unsigned char ucSimplePublisherIndex = xUrosWrapper->pxGetNode(ucNodeIndex)->ucCreatePublisher(
        pcTopic,
        ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Int64),
        pdFALSE
    );

    pxSimplePublisher->pxWrapper = xUrosWrapper;
    pxSimplePublisher->ucNodeIndex = ucNodeIndex;
    pxSimplePublisher->ucPublisherIndex = ucSimplePublisherIndex;
    pxSimplePublisher->ulPeriodMs = ulPeriodMs;
    return pdTRUE;
}

BaseType_t xSimplePublisherStart(SimplePublisher_t* pxSimplePublisher)
{
    UrosWrapper* xUrosWrapper = pxSimplePublisher->pxWrapper;

    LogInfo( ("Creating publisher task") );
    TaskHandle_t xPublisherTaskHandle;
    BaseType_t rc = xTaskCreate(
        prvSimplePublishTask,
        "Publisher",
        4096,
        (void*) pxSimplePublisher,
        3,
        &xPublisherTaskHandle
    );

    if (rc != pdPASS)
    {
        LogError( ("Unable to create publisher task") );
        return pdFALSE;
    }

    return pdTRUE;
}
