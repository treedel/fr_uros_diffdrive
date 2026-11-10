#ifndef UROS_WRAPPER_HPP
#define UROS_WRAPPER_HPP

#include <FreeRTOS.h>
#include <semphr.h>
#include <queue.h>

#include <rcl/rcl.h>
#include <rclc/rclc.h>
#include <rclc/executor.h>
#include <rmw_microros/rmw_microros.h>

#include <uros_wrapper_publisher.hpp>
#include <uros_wrapper_subscription.hpp>
#include <uros_wrapper_node.hpp>
#include <uros_wrapper_config.hpp>

extern "C" {
#include <pico_uart_transport.h>
}


class UrosWrapper {
    private:
    // microROS variables
    rcl_allocator_t xRclAllocator;
    rclc_support_t xRclcSupport;
    rclc_executor_t xRclcExecutor;

    // Node storage
    unsigned char ucTotalNodes;
    UrosWrapperNode xUrosWrapperNodes[MAX_NODES];
    
    // FreeRTOS thread sync handles
    SemaphoreHandle_t xUrosMutex;
    QueueHandle_t xPublishQueue;

    // microROS transport init
    BaseType_t prvUrosTransportInit();

    // microROS FreeRTOS tasks
    static void prvMicroRosExecutorTask(void* pvParameters);

    public:
    // Constructors
    UrosWrapper();

    // microROS init
    BaseType_t xInitializeUros();

    // Returns a node index which can then be used to add publishers/subscriptions
    unsigned char ucCreateNode(const char* pcNodeNamespace, const char* pcNodeName);

    // Getter function to fetch Node data pointer
    UrosWrapperNode* pxGetNode(unsigned char ucNodeIndex);

    // Function to start the microROS executor task and register all the events
    BaseType_t xStartUrosWrapper();
};

#endif