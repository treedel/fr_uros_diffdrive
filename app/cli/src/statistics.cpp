#include <statistics.hpp>


const CLI_Command_Definition_t xRunTimeStats = {
	"run-time-stats",
	"\r\nrun-time-stats:\r\n Displays how much processing time each FreeRTOS task has used\r\n",
	prvRunTimeStatsCommand,
	0
};

portBASE_TYPE prvRunTimeStatsCommand( char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString )
{
    const char * const pcHeader = "Task            Abs Time      % Time\r\n****************************************\r\n";
	strcpy( pcWriteBuffer, pcHeader );
	vTaskGetRunTimeStatistics( pcWriteBuffer + strlen( pcHeader ), xWriteBufferLen - strlen( pcHeader ) );
    pcWriteBuffer[strlen(pcWriteBuffer)] = '\n';
	return pdFALSE;
}