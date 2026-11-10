#ifndef UART_LOGGER_CONFIG_H
#define UART_LOGGER_CONFIG_H

#include <cstdarg>
#include <cstdio>
#include <cstring>

#include <hardware/uart.h>
#include <hardware/gpio.h>

#include <FreeRTOS.h>
#include <task.h>
#include <queue.h>


extern void vErrorSequence();

class UartLogger
{
    private:
    QueueHandle_t pxLogQueue;
    TaskHandle_t pxloggerTaskHandle;

    public:
    void vUartLoggerInit();
    void vUartLoggerWrite(const char* pcFormatString);
    void vUartLoggerWriteFormatted(const char* pcFormatString, ...);
    static void prvUartLoggerTask(void* pvParameters);
    BaseType_t xUartLoggerQueueEmpty();
};

extern UartLogger logger;

#define SdkLog(message)  logger.vUartLoggerWriteFormatted message

#endif
