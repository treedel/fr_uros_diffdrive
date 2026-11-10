#include <FreeRTOS.h>
#include <task.h>

#include <uros_wrapper.hpp>

#include <std_msgs/msg/int64.h>


typedef struct SimplePublisher_t {
    UrosWrapper* pxWrapper;
    unsigned char ucPublisherIndex;
    unsigned char ucNodeIndex;
    unsigned long ulPeriodMs;
} SimplePublisher_t;

static void prvSimplePublishTask(void* pvParameters);

BaseType_t xSimplePublisherCreate(SimplePublisher_t* pxSimplePublisher, UrosWrapper* xUrosWrapper, unsigned char ucNodeIndex, const char* pcTopic, unsigned long ulPeriodMs);
BaseType_t xSimplePublisherStart(SimplePublisher_t* pxSimplePublisher);