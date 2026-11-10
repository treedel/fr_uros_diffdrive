#include "pico/stdlib.h"


extern "C" uint64_t fr_uros_get_runtime_counter( void )
{
    return time_us_64();
}