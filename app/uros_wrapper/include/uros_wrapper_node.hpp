#ifndef UROS_WRAPPER_NODE_HPP
#define UROS_WRAPPER_NODE_HPP

#include <FreeRTOS.h>
#include <semphr.h>
#include <queue.h>

#include <rcl/rcl.h>
#include <rclc/rclc.h>
#include <rclc/executor.h>
#include <rmw_microros/rmw_microros.h>

#include <uros_wrapper_publisher.hpp>
#include <uros_wrapper_subscription.hpp>
#include <uros_wrapper_config.hpp>


class UrosWrapperNode {
    private:
    // microROS pub/sub array
    UrosWrapperPublisher xPublishers[MAX_NODE_PUBLISHERS];
    UrosWrapperSubscription xSubscriptions[MAX_NODE_SUBSCRIPTIONS];

    public:
    // Node details
    SemaphoreHandle_t* pxUrosMutex;
    rcl_node_t xNode;
    char pcNodeName[MAX_NODE_NAME_LENGTH];
    char pcNodeNamespace[MAX_NODE_NAME_LENGTH];
    unsigned char ucTotalPublishers;
    unsigned char ucTotalSubscriptions;

    UrosWrapperNode();

    // API Functions
    // Publisher and related functions
    unsigned char ucCreatePublisher(
        const char* pcTopicName, 
        const rosidl_message_type_support_t* pxTypeSupport,
        BaseType_t xReliable
    );

    UrosWrapperPublisher* pxGetPublisher(unsigned char ucPublisherIndex);

    BaseType_t xPublishMessage(
        unsigned char ucPublisherIndex,
        const void* pvMessage,
        rmw_publisher_allocation_t* pxAllocation
    );

    // Subscription and related functions
    unsigned char ucCreateSubscription(
        const char* pcTopicName, 
        const rosidl_message_type_support_t* pxTypeSupport,
        void* pvMsg,
        rclc_subscription_callback_t xCallback,
        rclc_executor_handle_invocation_t xInvocation
    );

    UrosWrapperSubscription* pxGetSubscription(unsigned char ucSubscriptionIndex);
};

extern void vErrorSequence();

#endif