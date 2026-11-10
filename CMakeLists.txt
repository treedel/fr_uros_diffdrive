# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Initialise pico_sdk from installed location
# (note this can come from environment, CMake cache etc)

# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.3.1)
set(toolchainVersion 15_2_Rel1)
set(picotoolVersion 2.3.1)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================
set(PICO_BOARD pico2_w CACHE STRING "Board type")

if (DEFINED ENV{FREERTOS_PATH} AND (NOT FREERTOS_PATH))
    set(FREERTOS_PATH $ENV{FREERTOS_PATH})
    message("Using FREERTOS_PATH from environment ('${FREERTOS_PATH}')")
    set(FREERTOS_KERNEL_PATH $ENV{FREERTOS_KERNEL_PATH})
message("Using FREERTOS_KERNEL_PATH from environment ('${FREERTOS_KERNEL_PATH}')")  
endif ()

# Pull in Raspberry Pi Pico SDK (must be before project)
include(${CMAKE_CURRENT_LIST_DIR}/cmake/pico_sdk_import.cmake)

# FreeRTOS
include(${CMAKE_CURRENT_LIST_DIR}/cmake/FreeRTOS_Kernel_import.cmake)

set(PROJECT_TITLE "fr_uros")

project(fr_uros C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Add executable. Default name is the project name, version 0.1
add_executable(fr_uros ${CMAKE_CURRENT_LIST_DIR}/main.cpp)

pico_set_program_name(fr_uros "fr_uros")
pico_set_program_version(fr_uros "0.1")

# Modify the below lines to enable/disable output over UART/USB
pico_enable_stdio_uart(fr_uros 0)
pico_enable_stdio_usb(fr_uros 1)

# The micro-ROS custom transport sends binary XRCE frames through stdio.
# Disable newline conversion so stdio does not alter frame bytes.
add_compile_definitions(PICO_UART_ENABLE_CRLF_SUPPORT=0)
add_compile_definitions(PICO_STDIO_ENABLE_CRLF_SUPPORT=0)
add_compile_definitions(PICO_STDIO_DEFAULT_CRLF=0)

# Add the standard library to the build
target_link_libraries(fr_uros
    FreeRTOS-Kernel
    FreeRTOS-Kernel-Heap4
    pico_stdlib
    pico_cyw43_arch_none
)

# Add the standard include files to the build
target_include_directories(fr_uros PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}
    ${CMAKE_CURRENT_LIST_DIR}/include/FreeRTOS-Kernel
    ${CMAKE_CURRENT_LIST_DIR}/port/FreeRTOS-Kernel
)

add_subdirectory(lib)
add_subdirectory(port)
add_subdirectory(app)

pico_add_extra_outputs(fr_uros)

