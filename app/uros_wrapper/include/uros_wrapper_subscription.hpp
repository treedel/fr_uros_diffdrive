#ifndef UROS_WRAPPER_SUBSCRIPTION_HPP
#define UROS_WRAPPER_SUBSCRIPTION_HPP

#include <FreeRTOS.h>
#include <semphr.h>
#include <queue.h>

#include <rcl/rcl.h>
#include <rclc/rclc.h>
#include <rclc/executor.h>
#include <rmw_microros/rmw_microros.h>

#include <uros_wrapper_config.hpp>


class UrosWrapperSubscription {
    private:
    rcl_subscription_t xSubscription;
    char pcTopicName[TOPIC_NAME_MAX_SIZE];
    void* pvMsg;
    rclc_subscription_callback_t xCallback;
    rclc_executor_handle_invocation_t xInvocation;

    public:
    UrosWrapperSubscription();

    void vInitSubscription(
        rcl_subscription_t xSubscription,
        const char* pcTopicName,
        void* pvMsg,
        rclc_subscription_callback_t xCallback,
        rclc_executor_handle_invocation_t xInvocation
    );

    // Setters
    void vSetTopicName(const char* pcTopicName);

    // Getters
    void pcSetTopicName(const char* pcTopicName);
    const char* pcGetTopicName();
    rcl_subscription_t* xGetSubscription();
    void* pvGetMsg();
    rclc_subscription_callback_t xGetCallback();
    rclc_executor_handle_invocation_t xGetInvocation();
};

#endif