#ifndef SIMPLE_PUBLISHER_H
#define SIMPLE_PUBLISHER_H

#include "FreeRTOS.h"
#include "task.h"
#include "uros_wrapper.h"

typedef struct SimplePublisher_t {
    unsigned long ulDelayPeriod;
    UrosWrapperPublisher_t* pxUrosWrapperPublisher;
} SimplePublisher_t;

void vPublisherTask(void* pvParameters);
void vStartSimplePublisher(UrosWrapperCore_t* pxUrosWrapper, char* pcTopicName, unsigned long ulDelayPeriod);

#endif