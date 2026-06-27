#ifndef SIMPLE_SUBSCRIBER_H
#define SIMPLE_SUBSCRIBER_H

#include "uros_wrapper.h"
#include "status_led.h"
#include <std_msgs/msg/int32.h>

void vSimpleSubcriberInit(StatusLed_t* pxStatusLed);
void prvSimpleSubscriberCallback(const void* msgin);
void vSimpleSubscriberStart(UrosWrapperCore_t* pxUrosWrapper);

#endif