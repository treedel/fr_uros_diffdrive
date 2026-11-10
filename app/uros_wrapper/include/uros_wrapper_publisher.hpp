#ifndef UROS_WRAPPER_PUBLISHER_HPP
#define UROS_WRAPPER_PUBLISHER_HPP

#include <FreeRTOS.h>
#include <semphr.h>
#include <queue.h>

#include <rcl/rcl.h>
#include <rclc/rclc.h>
#include <rclc/executor.h>
#include <rmw_microros/rmw_microros.h>

#include <uros_wrapper_config.hpp>


class UrosWrapperPublisher {
    private:
    rcl_publisher_t xPublisher;
    char pcTopicName[TOPIC_NAME_MAX_SIZE];

    public:
    UrosWrapperPublisher();

    void vInitPublisher(
        rcl_publisher_t xPublisher,
        const char* pcTopicName
    );

    // Getters
    void vSetTopicName(const char* pcTopicName);
    const char* pcGetTopicName();
    rcl_publisher_t* xGetPublisher();
};

#endif