#include "simple_subscriber.h"


StatusLed_t* prvStatusLed;

void vSimpleSubcriberInit(StatusLed_t* pxStatusLed)
{
    prvStatusLed = pxStatusLed;
}

void prvSimpleSubscriberCallback(const void* msgin)
{
    vStatusLedActivityTrigger(prvStatusLed);
}

void vSimpleSubscriberStart(UrosWrapperCore_t* pxUrosWrapper)
{
    std_msgs__msg__Int32* pxIntBuffer = pvPortMalloc(sizeof(std_msgs__msg__Int32));
    vUrosWrapperAddSubscriber(
        pxUrosWrapper,
        "trigger",
        ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Int32),
        pxIntBuffer,
        prvSimpleSubscriberCallback,
        ON_NEW_DATA
    );
}