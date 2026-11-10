#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "pico/cyw43_arch.h"

#include <cli.hpp>
#include <uros_wrapper.hpp>
#include <lidar_publisher.hpp>
#include <simple_publisher.hpp>

/* Configuring logger */
#define LIBRARY_LOG_NAME "MAIN"
#define LIBRARY_LOG_LEVEL  LOG_INFO

#include <uart_logger.hpp>
#include <logging_stack.h>


int main();
void main_core1(); 
void prvMainTaskCore0(void* pvParameters);
void vErrorSequence();

int main()
{
    multicore_launch_core1(main_core1);
    
    TaskHandle_t xMainTaskHandle;
    BaseType_t rc = xTaskCreate(
        prvMainTaskCore0,
        "Main",
        4096,
        NULL,
        2,
        &xMainTaskHandle
    );
    
    vTaskStartScheduler();

    while (true)
    {
    }
}

/* TODO: Will later be used to offload the overall load */
void main_core1()
{
    cyw43_arch_init();
    
    while (1)
    {
        cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, pdTRUE);
        sleep_ms(1000);
        cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, pdFALSE);
        sleep_ms(1000);
    }
}

void prvMainTaskCore0(void* pvParameters)
{
    LidarPublisher_t xLidarPublisher;
    SimplePublisher_t xSimplePub1, xSimplePub2, xSimplePub3;

    logger.vUartLoggerInit();

    LogInfo( ("Starting core-0 main task") );

    vStartCli();

    UrosWrapper xUrosWrapper;
    if (xUrosWrapper.xInitializeUros() == pdFALSE)
    {
        vErrorSequence();
    }

    unsigned char ucLidarNodeIndex = xUrosWrapper.ucCreateNode("", "LidarPublisher");

    if (xLidarPublisherCreate(&xLidarPublisher, &xUrosWrapper, ucLidarNodeIndex, "lidar", 100) == pdFALSE ||
        xSimplePublisherCreate(&xSimplePub1, &xUrosWrapper, ucLidarNodeIndex, "pub1", 100) == pdFALSE ||
        xSimplePublisherCreate(&xSimplePub2, &xUrosWrapper, ucLidarNodeIndex, "pub2", 100) == pdFALSE ||
        xSimplePublisherCreate(&xSimplePub3, &xUrosWrapper, ucLidarNodeIndex, "pub3", 100) == pdFALSE)
    {
        vErrorSequence();
    }

    if (xUrosWrapper.xStartUrosWrapper() == pdFALSE ||
        xLidarPublisherStart(&xLidarPublisher) == pdFALSE ||
        xSimplePublisherStart(&xSimplePub1) == pdFALSE ||
        xSimplePublisherStart(&xSimplePub2) == pdFALSE ||
        xSimplePublisherStart(&xSimplePub3) == pdFALSE)
    {
        vErrorSequence();
    }

    while (1)
    {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void vErrorSequence()
{
    /* Wait until all the log items are handled */
    while(!logger.xUartLoggerQueueEmpty());

    cyw43_arch_init();

    while (1)
    {
        cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, pdTRUE);
        sleep_ms(1000);
        cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, pdFALSE);
        sleep_ms(1000);
    }
}
