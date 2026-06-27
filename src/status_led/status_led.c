#include "status_led.h"


void prvStatusLedGpioInit(unsigned long ulLedPin)
{
    gpio_init(ulLedPin);
    gpio_set_dir(ulLedPin, GPIO_OUT);
    gpio_put(ulLedPin, 0);
}

void prvStatusLedGpioSet(unsigned long ulLedPin, BaseType_t xLedState)
{
    gpio_put(ulLedPin, xLedState);
}


void prvStatusLedTask(void* pvParameters)
{
    StatusLed_t* pxStatusLed = (StatusLed_t*) pvParameters;
    while(1) {
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        if (xSemaphoreTake(pxStatusLed->xLedSemaphore, portMAX_DELAY) == pdTRUE) {
            vStatusLedFlash(pxStatusLed, 25, 25);
            xSemaphoreGive(pxStatusLed->xLedSemaphore);
        }
    }
}

StatusLed_t* pxStatusLedInit(unsigned long ulLedPin)
{
    StatusLed_t* pxStatusLed = pvPortMalloc(sizeof(StatusLed_t));
    configASSERT(pxStatusLed != NULL);
    pxStatusLed->ulStatusLedPin = ulLedPin;
    pxStatusLed->xLedSemaphore = xSemaphoreCreateBinary();
    configASSERT(pxStatusLed->xLedSemaphore != NULL);
    xSemaphoreGive(pxStatusLed->xLedSemaphore);

    prvStatusLedGpioInit(ulLedPin);

    BaseType_t xTaskCreated = xTaskCreate(
        prvStatusLedTask,
        "Status LED",
        4096,
        (void*) pxStatusLed,
        2,
        &(pxStatusLed->xStatusLedTaskHandle)
    );
    configASSERT(xTaskCreated == pdPASS);

    return pxStatusLed;
}

void vStatusLedSet(StatusLed_t* pxStatusLed, BaseType_t xLedState)
{
    prvStatusLedGpioSet(pxStatusLed->ulStatusLedPin, xLedState);
}

void vStatusLedFlash(StatusLed_t* pxStatusLed, unsigned long ulOnPeriodMs, unsigned long ulOffPeriodMs)
{
    vStatusLedSet(pxStatusLed, pdTRUE);
    vTaskDelay(pdMS_TO_TICKS(ulOnPeriodMs));
    vStatusLedSet(pxStatusLed, pdFALSE);
    vTaskDelay(pdMS_TO_TICKS(ulOffPeriodMs));
}

void vStatusLedActivityTrigger(StatusLed_t* pxStatusLed) {
    if(pxStatusLed->xStatusLedTaskHandle != NULL)
    {
        xTaskNotifyGive(pxStatusLed->xStatusLedTaskHandle);
    }
}
