#include <cli.hpp>

void vRegisterCommands()
{
	FreeRTOS_CLIRegisterCommand(&xRunTimeStats);
}

void vStartCli()
{
    vRegisterCommands();
    
    xTaskCreate(
        prvCliTask,
        "CLI",
        configMINIMAL_STACK_SIZE,
        NULL,
        2,
        NULL
    );
}

void prvCliTask( void *pvParameters )
{
    char command[128];
    size_t command_length = 0;
    char *output;

    (void)pvParameters;
    output = FreeRTOS_CLIGetOutputBuffer();

    vTaskDelay(pdMS_TO_TICKS(100));
    uart_puts(uart0, "\r\npico@uart0> ");

    while (1) {
        while ( !uart_is_readable(uart0) )
        {
            vTaskDelay(pdMS_TO_TICKS(1));
        }

        int character = uart_getc(uart0);

        if ( character == '\r' || character == '\n' )
        {
            BaseType_t more_output;

            command[command_length] = '\0';
            uart_puts(uart0, "\r\n");

            do
            {
                more_output = FreeRTOS_CLIProcessCommand(
                    command,
                    output,
                    configCOMMAND_INT_MAX_OUTPUT_SIZE
                );
                uart_puts(uart0, output);
            } while ( more_output == pdTRUE );

            command_length = 0;
            uart_puts(uart0, "pico@uart0> ");
        }
        else if ( character == '\b' || character == 127 )
        {
            if ( command_length > 0 )
            {
                command_length--;
                uart_puts(uart0, "\b \b");
            }
        }
        else if ( command_length < sizeof(command) - 1 )
        {
            command[command_length++] = (char)character;
            uart_putc(uart0, character);
        }
    }
}