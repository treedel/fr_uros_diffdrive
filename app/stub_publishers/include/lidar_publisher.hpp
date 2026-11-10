#include <FreeRTOS.h>
#include <task.h>

#include <uros_wrapper.hpp>

#include <sensor_msgs/msg/laser_scan.h>


typedef struct LidarPublisher_t {
    UrosWrapper* pxWrapper;
    unsigned char ucPublisherIndex;
    unsigned char ucNodeIndex;
    unsigned long ulPeriodMs;
} LidarPublisher_t;

static void prvLidarPublishTask(void* pvParameters);

BaseType_t xLidarPublisherCreate(LidarPublisher_t* pxLidarPublisher, UrosWrapper* xUrosWrapper, unsigned char ucNodeIndex, const char* pcTopic, unsigned long ulPeriodMs);
BaseType_t xLidarPublisherStart(LidarPublisher_t* pxLidarPublisher);