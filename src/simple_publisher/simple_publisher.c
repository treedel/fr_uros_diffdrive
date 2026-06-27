#include <std_msgs/msg/int64.h>

#include "uros_wrapper.h"
#include "simple_publisher.h"

void vPublisherTask(void* pvParameters)
{
    SimplePublisher_t* pxSimplePublisher = (SimplePublisher_t*) pvParameters;
    UrosWrapperPublisher_t* pxUrosWrapperPublisher = pxSimplePublisher->pxUrosWrapperPublisher;
    unsigned long i = 0;

    TickType_t xLastWakeTime = xTaskGetTickCount();
    const TickType_t xPeriod = pdMS_TO_TICKS(pxSimplePublisher->ulDelayPeriod);

    std_msgs__msg__Int64 xMsg;
    xMsg.data = 0;
    while (1) {
        rcl_ret_t xRet = pxUrosWrapperPublishBlink(pxUrosWrapperPublisher, &xMsg, NULL);
        xMsg.data++;
        vTaskDelayUntil(&xLastWakeTime, xPeriod);
    }
}

void vStartSimplePublisher(UrosWrapperCore_t* pxUrosWrapper, char* pcTopicName, unsigned long ulDelayPeriod, BaseType_t xReliable)
{

    UrosWrapperPublisher_t* pxUrosWrapperPublisher = vUrosWrapperAddPublisher(
        pxUrosWrapper,
        pcTopicName,
        ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Int64),
        xReliable
    );

    SimplePublisher_t* pxSimplePublisher = pvPortMalloc(sizeof(SimplePublisher_t));
    pxSimplePublisher->pxUrosWrapperPublisher = pxUrosWrapperPublisher;
    pxSimplePublisher->ulDelayPeriod = ulDelayPeriod;

    TaskHandle_t xSimplePublisherHandle;
    BaseType_t xTaskCreated = xTaskCreate(
        vPublisherTask,
        pcTopicName,
        4096,
        (void*) pxSimplePublisher,
        3,
        &xSimplePublisherHandle
    );
    configASSERT(xTaskCreated == pdPASS);

#if defined(configUSE_CORE_AFFINITY) && (configUSE_CORE_AFFINITY == 1)
    if (xTaskCreated == pdPASS) {
        vTaskCoreAffinitySet(xSimplePublisherHandle, (1 << 0));
    }
#endif
}
