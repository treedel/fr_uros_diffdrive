#ifndef STATISTICS_H
#define STATISTICS_H

#include <FreeRTOS.h>
#include <task.h>

#include <FreeRTOS_CLI.h>

#include <stdint.h>
#include <stdio.h>
#include <string.h>

portBASE_TYPE prvRunTimeStatsCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString);

extern const CLI_Command_Definition_t xRunTimeStats;

#endif
