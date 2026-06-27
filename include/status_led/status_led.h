#ifndef STATUS_LED_H
#define STATUS_LED_H

#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>

#include <hardware/gpio.h>


typedef struct StatusLed_t {
    unsigned long ulStatusLedPin;
    TaskHandle_t xStatusLedTaskHandle;
    SemaphoreHandle_t xLedSemaphore;
} StatusLed_t;

StatusLed_t* pxStatusLedInit(unsigned long ulLedPin);
void vStatusLedSet(StatusLed_t* pxStatusLed, BaseType_t xLedState);
void vStatusLedFlash(StatusLed_t* pxStatusLed, unsigned long ulOnPeriodMs, unsigned long ulOffPeriodMs);
void vStatusLedActivityTrigger(StatusLed_t* pxStatusLed);

#endif