#include "FreeRTOS.h"
#include "task.h"

#include <stdio.h>
#include "pico/stdlib.h"


void prvSetupHardware( void );
void prvHelloTask( void *pvParameters );

int main( void )
{
    prvSetupHardware();

    xTaskCreate(
        prvHelloTask,
        "Hello",
        configMINIMAL_STACK_SIZE,
        NULL,
        1,
        NULL
    );

    vTaskStartScheduler();
    
    while (1)
    {
    }
}

void prvSetupHardware( void )
{
    stdio_init_all();
}

void prvHelloTask( void *pvParameters )
{
    (void) pvParameters;

    while (1)
    {
        printf("Hello world\r\n");
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
