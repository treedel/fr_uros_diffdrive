#include <uart_logger.hpp>


#define LOG_BUFFER_SIZE     100

UartLogger logger;

void UartLogger::vUartLoggerInit()
{
    this->pxloggerTaskHandle = NULL;
    this->pxLogQueue = xQueueCreate(5, (sizeof(char) * LOG_BUFFER_SIZE));

    gpio_set_function(0, UART_FUNCSEL_NUM(uart0, 0));
    gpio_set_function(1, UART_FUNCSEL_NUM(uart0, 1));
    uart_init(uart0, 115200);

    BaseType_t rc = xTaskCreate(
        this->prvUartLoggerTask,
        "Logger",
        4096,
        (void*) pxLogQueue,
        configMAX_PRIORITIES-1,
        &pxloggerTaskHandle
    );
    if (rc != pdPASS)
    {
        vErrorSequence();
    }
}

void UartLogger::vUartLoggerWrite(const char* pcFormatString)
{
    char buffer[LOG_BUFFER_SIZE];
    strncpy(buffer, pcFormatString, LOG_BUFFER_SIZE);
    buffer[LOG_BUFFER_SIZE-1] = '\0';
    xQueueSend(this->pxLogQueue, buffer, 10);
}

void UartLogger::vUartLoggerWriteFormatted(const char* pcFormatString, ...)
{
    char buffer[LOG_BUFFER_SIZE];
    va_list args;
    va_start(args, pcFormatString);
    vsnprintf(buffer, sizeof(buffer), pcFormatString, args);
    va_end(args);

    vUartLoggerWrite(buffer);
}

void UartLogger::prvUartLoggerTask(void* pvParameters)
{
    QueueHandle_t pxLogQueue = (QueueHandle_t) pvParameters;
    char buffer[LOG_BUFFER_SIZE];
    while (1)
    {
        if (xQueueReceive(pxLogQueue, buffer, portMAX_DELAY) == pdPASS) {
            uart_puts(uart0, buffer);
        }
    }
}

BaseType_t UartLogger::xUartLoggerQueueEmpty()
{
    return (uxQueueMessagesWaiting(this->pxLogQueue) == 0);
}