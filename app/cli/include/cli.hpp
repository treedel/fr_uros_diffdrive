#ifndef CLI_H
#define CLI_H

#include <hardware/uart.h>

#include <statistics.hpp>

void vRegisterCommands();
void vStartCli();
void prvCliTask(void *pvParameters);

#endif