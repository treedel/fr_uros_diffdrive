#include <uros_wrapper_node.hpp>

// Configuring logger
#define LIBRARY_LOG_NAME "UROS_WRAPPER_NODE"
#define LIBRARY_LOG_LEVEL  LOG_INFO

#include <uart_logger.hpp>
#include <logging_stack.h>


UrosWrapperNode::UrosWrapperNode() {
    LogDebug( ("Node constructor called") );
    this->pxUrosMutex = NULL;
    this->ucTotalPublishers = 0;
    this->ucTotalSubscriptions = 0;
}

// API Functions
// Publisher and related functions
unsigned char UrosWrapperNode::ucCreatePublisher(
    const char* pcTopicName, 
    const rosidl_message_type_support_t* pxTypeSupport,
    BaseType_t xReliable
) {
    LogInfo( ("Creating publisher. topic='%s'", pcTopicName) );
    unsigned char ucNewIndex = this->ucTotalPublishers;

    // If we reached max publishers
    if (ucNewIndex >= MAX_NODE_PUBLISHERS)
    {
        LogError( ("Max publishers created") );
        vErrorSequence();
    }

    rcl_publisher_t xPublisherTmp;

    if (xSemaphoreTake(*(this->pxUrosMutex), portMAX_DELAY) == pdFALSE)
    {
        LogError( ("Unable to take microROS mutex") );
        vErrorSequence();
    }

    // Create a publisher based on type selected
    rcl_ret_t ret;
    if (xReliable)
    {
        ret = rclc_publisher_init_default(
            &xPublisherTmp,
            &(this->xNode),
            pxTypeSupport,
            pcTopicName
        );
    }
    else
    {
        ret = rclc_publisher_init_best_effort(
            &xPublisherTmp,
            &(this->xNode),
            pxTypeSupport,
            pcTopicName
        );
    }

    xSemaphoreGive(*(this->pxUrosMutex));

    if (ret != RCL_RET_OK)
    {
        LogError( ("Unable to initialize publisher") );
        vErrorSequence();
    }

    (this->xPublishers[ucNewIndex]).vInitPublisher(
        xPublisherTmp,
        pcTopicName
    );

    (this->ucTotalPublishers)++;
    return ucNewIndex;
}

UrosWrapperPublisher* UrosWrapperNode::pxGetPublisher(unsigned char ucPublisherIndex) {
    // Check whether publisher is available and return
    if (ucPublisherIndex < this->ucTotalPublishers)
    {
        return &(this->xPublishers[ucPublisherIndex]);
    }
    return NULL;
}

BaseType_t UrosWrapperNode::xPublishMessage(
    unsigned char ucPublisherIndex,
    const void* pvMessage,
    rmw_publisher_allocation_t* pxAllocation
) {
    LogDebug( ("Publishing message. topic='%d'", ucPublisherIndex) );
    rcl_publisher_t* pxPublisher = this->pxGetPublisher(ucPublisherIndex)->xGetPublisher();

    if (xSemaphoreTake(*(this->pxUrosMutex), portMAX_DELAY) == pdFALSE)
    {
        LogError( ("Unable to take microROS mutex") );
        vErrorSequence();
    }
    rcl_ret_t xRet = rcl_publish(
        pxPublisher,
        pvMessage,
        pxAllocation
    );
    xSemaphoreGive(*(this->pxUrosMutex));

    return (xRet == RCL_RET_OK) ? pdTRUE : pdFALSE;
}

// Subscription and related functions
unsigned char UrosWrapperNode::ucCreateSubscription(
    const char* pcTopicName, 
    const rosidl_message_type_support_t* pxTypeSupport,
    void* pvMsg,
    rclc_subscription_callback_t xCallback,
    rclc_executor_handle_invocation_t xInvocation
) {
    LogInfo( ("Creating subscription. topic='%s'", pcTopicName) );
    unsigned char ucNewIndex = this->ucTotalSubscriptions;

    // If we reached max subscriptions
    if (ucNewIndex >= MAX_NODE_SUBSCRIPTIONS)
    {
        LogError( ("Max Subscribers created") );
        vErrorSequence();
    }

    rcl_subscription_t xSubscriptionTmp;

    if (xSemaphoreTake(*(this->pxUrosMutex), portMAX_DELAY) == pdFALSE)
    {
        LogError( ("Unable to take microROS mutex") );
        vErrorSequence();
    }

    // Subscription logic
    rcl_ret_t ret;
    ret = rclc_subscription_init_default(
        &xSubscriptionTmp,
        &(this->xNode),
        pxTypeSupport,
        pcTopicName
    );

    xSemaphoreGive(*(this->pxUrosMutex));

    if (ret != RCL_RET_OK)
    {
        LogError( ("Unable to initialize subscriber") );
        vErrorSequence();
    }

    (this->xSubscriptions[ucNewIndex]).vInitSubscription(
        xSubscriptionTmp,
        pcTopicName,
        pvMsg,
        xCallback,
        xInvocation
    );

    (this->ucTotalSubscriptions)++;
    return ucNewIndex;
}

UrosWrapperSubscription* UrosWrapperNode::pxGetSubscription(unsigned char ucSubscriptionIndex) {
    // Check whether subscription is available and return
    if (ucSubscriptionIndex < this->ucTotalSubscriptions)
    {
        return &(this->xSubscriptions[ucSubscriptionIndex]);
    }
    return NULL;
}