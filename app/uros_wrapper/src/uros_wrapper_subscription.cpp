#include <string.h>
#include <uros_wrapper_subscription.hpp>

UrosWrapperSubscription::UrosWrapperSubscription() {

}

void UrosWrapperSubscription::vInitSubscription(
    rcl_subscription_t xSubscription,
    const char* pcTopicName,
    void* pvMsg,
    rclc_subscription_callback_t xCallback,
    rclc_executor_handle_invocation_t xInvocation
) {
    this->xSubscription = xSubscription;
    strcpy(this->pcTopicName, pcTopicName);
    this->pvMsg = pvMsg;
    this->xCallback = xCallback;
    this->xInvocation = xInvocation;

}

// Setters
void UrosWrapperSubscription::vSetTopicName(const char* pcTopicName) {
    strcpy(this->pcTopicName, pcTopicName);
}

// Getters
const char* UrosWrapperSubscription::pcGetTopicName() {
    return this->pcTopicName;
}

rcl_subscription_t* UrosWrapperSubscription::xGetSubscription() {
    return &(this->xSubscription);
}

void* UrosWrapperSubscription::pvGetMsg() {
    return this->pvMsg;
}

rclc_subscription_callback_t UrosWrapperSubscription::xGetCallback() {
    return this->xCallback;
}

rclc_executor_handle_invocation_t UrosWrapperSubscription::xGetInvocation() {
    return this->xInvocation;
}
