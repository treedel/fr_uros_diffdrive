#ifndef UROS_WRAPPER_H
#define UROS_WRAPPER_H

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"

#include <rcl/rcl.h>
#include <rclc/rclc.h>
#include <rclc/executor.h>
#include <rmw_microros/rmw_microros.h>

//#include "pico_uart_transports.h"
#include "picow_udp_transports.h"

#include "FreeRTOS.h"
#include "semphr.h"

#include "udp_transport_config.h"
#include "status_led.h"


#define MAX_PUBLISHERS 5
#define MAX_SUBSCRIBERS 5
#define MAX_NODE_LENGTH 10
#define STATUS_LED_PIN PICO_DEFAULT_LED_PIN


typedef struct UrosWrapperPublisher_t
{

    char* pcTopicName;
    rcl_publisher_t* pxPublisher;
    StatusLed_t* pxStatusLed;

    SemaphoreHandle_t* pxPublishSemaphore;

} UrosWrapperPublisher_t;

typedef struct UrosWrapperSubscriber_t
{

    char* pcTopicName;
    rcl_subscription_t xSubscriber;
    void* pvMsg;
    rclc_subscription_callback_t xCallback;
    rclc_executor_handle_invocation_t xInvocation;

} UrosWrapperSubscriber_t;

// TODO: change the publisher type from rclc to uroswrapper
typedef struct UrosWrapperCore_t
{

    StatusLed_t* pxStatusLed;

    rcl_allocator_t xAllocator;
    rcl_node_t xNode;
    rclc_support_t xSupport;
    rclc_executor_t xExecutor;

    uint8_t n_publishers;
    uint8_t n_subscribers;
    rcl_publisher_t xPublishers[MAX_PUBLISHERS];
    UrosWrapperSubscriber_t xSubscribers[MAX_SUBSCRIBERS];

    SemaphoreHandle_t xPublishSemaphore;

    char pcNodeName[MAX_NODE_LENGTH];

} UrosWrapperCore_t;

bool prvUrosInitTransport();
UrosWrapperCore_t* vUrosInit(char* pcNodeName, StatusLed_t* pxStatusLed);
void vUrosSetup(UrosWrapperCore_t* pxUrosWrapper);
void vUrosRunTask(void* pvParameters);
void vUrosWrapperStart(UrosWrapperCore_t* pxUrosWrapper);

// TODO: Chnage v to px
UrosWrapperPublisher_t* vUrosWrapperAddPublisher(
    UrosWrapperCore_t* pxUrosWrapper,
    char* pcTopicName, 
    const rosidl_message_type_support_t* pxTypeSupport,
    BaseType_t xReliable
);
rcl_ret_t pxUrosWrapperPublish(
    UrosWrapperPublisher_t* pxUrosWrapperPublisher,
    const void* pvMessage,
    rmw_publisher_allocation_t* pxAllocation
);
rcl_ret_t pxUrosWrapperPublishBlink(
    UrosWrapperPublisher_t* pxUrosWrapperPublisher,
    const void* pvMessage,
    rmw_publisher_allocation_t* pxAllocation
);

void vUrosWrapperAddSubscriber(
    UrosWrapperCore_t* pxUrosWrapper,
    char* pcTopicName, 
    const rosidl_message_type_support_t* pxTypeSupport,
    void *pvMsg,
    rclc_subscription_callback_t xCallback,
    rclc_executor_handle_invocation_t xInvocation
);

#endif