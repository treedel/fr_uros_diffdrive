#include <lidar_publisher.hpp>


// Configuring logger
#define LIBRARY_LOG_NAME "LIDAR_PUBLISHER"
#define LIBRARY_LOG_LEVEL  LOG_INFO

#include <uart_logger.hpp>
#include <logging_stack.h>


static void prvLidarPublishTask(void* pvParameters)
{
    LogInfo( ("Starting publisher task") );
    LidarPublisher_t* pxLidarPublisher = (LidarPublisher_t*) pvParameters;
    UrosWrapper* obj = pxLidarPublisher->pxWrapper;
    unsigned char ucLidarPublisherIndex = pxLidarPublisher->ucPublisherIndex;
    unsigned char ucLidarNodeIndex = pxLidarPublisher->ucNodeIndex;
    unsigned long ulPeriodMs = pxLidarPublisher->ulPeriodMs;
    
    // Variable storing the message
    sensor_msgs__msg__LaserScan xMsg;

    // ------------
    const int NUM_READINGS = 360;
    // 3. Allocate memory for ranges array
    xMsg.ranges.data = (float*) malloc(NUM_READINGS * sizeof(float));
    xMsg.ranges.size = 0; // Current filled size
    xMsg.ranges.capacity = NUM_READINGS;

    // 4. Allocate memory for intensities array
    xMsg.intensities.data = (float*) malloc(NUM_READINGS * sizeof(float));
    xMsg.intensities.size = 0;
    xMsg.intensities.capacity = NUM_READINGS;

    // 5. Populate constant metadata
    xMsg.header.frame_id.data = "laser_frame";
    xMsg.header.frame_id.size = strlen(xMsg.header.frame_id.data);
    xMsg.header.frame_id.capacity = xMsg.header.frame_id.size + 1;

    xMsg.angle_min = -3.14159;      // -180 degrees in radians
    xMsg.angle_max = 3.14159;       // 180 degrees in radians
    xMsg.angle_increment = 0.01745; // 1 degree step in radians
    xMsg.time_increment = 0.0;
    xMsg.scan_time = 0.1;           // 10 Hz
    xMsg.range_min = 0.1;           // Minimum valid distance (meters)
    xMsg.range_max = 12.0;          // Maximum valid distance (meters)
    // ------------

    // Add dummy data
    for (int i = 0; i < NUM_READINGS; i++) {
        xMsg.ranges.data[i] = 5; 
        xMsg.intensities.data[i] = 0.8;
    }
    xMsg.ranges.size = NUM_READINGS;
    xMsg.intensities.size = NUM_READINGS;

    // Variables handling the timings
    TickType_t xLastWakeTime = xTaskGetTickCount();
    const TickType_t xPeriod = pdMS_TO_TICKS(ulPeriodMs);

    // Iteratively increment the data and publish
    while (1) {
        LogDebug( ("Publishing message") );
        BaseType_t ret = obj->pxGetNode(ucLidarNodeIndex)->xPublishMessage(ucLidarPublisherIndex, &xMsg, NULL);
        LogDebug( ("Message published") );

        if (ret != pdPASS)
        {
            LogError( ("Failed to publish message") );
            vErrorSequence();
        }
        vTaskDelayUntil(&xLastWakeTime, xPeriod);
    }
}

BaseType_t xLidarPublisherCreate(LidarPublisher_t* pxLidarPublisher, UrosWrapper* xUrosWrapper, unsigned char ucNodeIndex, const char* pcTopic, unsigned long ulPeriodMs)
{
    unsigned char ucLidarPublisherIndex = xUrosWrapper->pxGetNode(ucNodeIndex)->ucCreatePublisher(
        pcTopic,
        ROSIDL_GET_MSG_TYPE_SUPPORT(sensor_msgs, msg, LaserScan),
        pdTRUE
    );

    pxLidarPublisher->pxWrapper = xUrosWrapper;
    pxLidarPublisher->ucNodeIndex = ucNodeIndex;
    pxLidarPublisher->ucPublisherIndex = ucLidarPublisherIndex;
    pxLidarPublisher->ulPeriodMs = ulPeriodMs;
    return pdTRUE;
}

BaseType_t xLidarPublisherStart(LidarPublisher_t* pxLidarPublisher)
{
    UrosWrapper* xUrosWrapper = pxLidarPublisher->pxWrapper;

    LogInfo( ("Creating publisher task") );
    TaskHandle_t xPublisherTaskHandle;
    BaseType_t rc = xTaskCreate(
        prvLidarPublishTask,
        "Lidar Publisher",
        4096,
        (void*) pxLidarPublisher,
        3,
        &xPublisherTaskHandle
    );

    if (rc != pdPASS)
    {
        LogError( ("Unable to create publisher task") );
        return pdFALSE;
    }

    return pdTRUE;
}
