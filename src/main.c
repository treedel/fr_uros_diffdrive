#include "FreeRTOS.h"
#include "task.h"

#include "uros_wrapper.h"
#include "simple_publisher.h"
#include "status_led/status_led.h"
#include "simple_subscriber.h"

void vUrosBootTask(void* pvParameters)
{
    StatusLed_t* pxStatusLed = pxStatusLedInit(PICO_DEFAULT_LED_PIN);
    UrosWrapperCore_t* xUrosWrapper = vUrosInit("Diffbot", pxStatusLed);

    vSimpleSubcriberInit(pxStatusLed);
    vStartSimplePublisher(xUrosWrapper, "counter1", 100);
    vStartSimplePublisher(xUrosWrapper, "counter2", 100);
    vStartSimplePublisher(xUrosWrapper, "counter3", 100);
    vStartSimplePublisher(xUrosWrapper, "counter4", 100);
    vStartSimplePublisher(xUrosWrapper, "counter5", 100);

    vSimpleSubscriberStart(xUrosWrapper);
    vUrosSetup(xUrosWrapper);

    vUrosWrapperStart(xUrosWrapper);

    while (1)
    {
        vTaskDelay(portMAX_DELAY);
    }
}

int main( void )
{
    stdio_init_all();

    BaseType_t xTaskCreated = xTaskCreate(
        vUrosBootTask,
        "uROS Bootup",
        4096,
        NULL,
        4,
        NULL
    );
    configASSERT(xTaskCreated == pdPASS);

    vTaskStartScheduler();
    
    while (1)
    {
        vTaskDelay(portMAX_DELAY);
    }
}
