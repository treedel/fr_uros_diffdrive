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
set(sdkVersion 2.2.0)
set(toolchainVersion 14_2_Rel1)
set(picotoolVersion 2.2.0-a4)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================
set(PICO_BOARD pico2 CACHE STRING "Board type")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)
include(FreeRTOS_Kernel_import.cmake)

project(fr_uros_diffdrive C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

link_directories(${CMAKE_CURRENT_LIST_DIR}/lib/uros/libmicroros)

# Add executable. Default name is the project name, version 0.1
add_executable(fr_uros_diffdrive 
    ${CMAKE_CURRENT_LIST_DIR}/src/main.c
    ${CMAKE_CURRENT_LIST_DIR}/src/microros/pico_uart_transport.c
    ${CMAKE_CURRENT_LIST_DIR}/src/status_led/status_led.c
    ${CMAKE_CURRENT_LIST_DIR}/src/uros_wrapper/uros_wrapper.c
    ${CMAKE_CURRENT_LIST_DIR}/src/simple_publisher/simple_publisher.c
    ${CMAKE_CURRENT_LIST_DIR}/src/simple_subscriber/simple_subscriber.c
)

pico_set_program_name(fr_uros_diffdrive "fr_uros_diffdrive")
pico_set_program_version(fr_uros_diffdrive "0.1")

# Modify the below lines to enable/disable output over UART/USB
pico_enable_stdio_uart(fr_uros_diffdrive 0)
pico_enable_stdio_usb(fr_uros_diffdrive 1)
add_compile_definitions(PICO_UART_ENABLE_CRLF_SUPPORT=0)
add_compile_definitions(PICO_STDIO_ENABLE_CRLF_SUPPORT=0)
add_compile_definitions(PICO_STDIO_DEFAULT_CRLF=0)

# Add the standard library to the build
target_link_libraries(fr_uros_diffdrive
    pico_stdlib
    FreeRTOS-Kernel
    FreeRTOS-Kernel-Heap4
    microros
)

# Add the standard include files to the build
target_include_directories(fr_uros_diffdrive PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}
    ${CMAKE_CURRENT_LIST_DIR}/include
    ${CMAKE_CURRENT_LIST_DIR}/include/FreeRTOS-Kernel
    ${CMAKE_CURRENT_LIST_DIR}/include/status_led
    ${CMAKE_CURRENT_LIST_DIR}/include/microros
    ${CMAKE_CURRENT_LIST_DIR}/lib/uros/libmicroros/include
    ${CMAKE_CURRENT_LIST_DIR}/include/uros_wrapper
    ${CMAKE_CURRENT_LIST_DIR}/include/simple_publisher
    ${CMAKE_CURRENT_LIST_DIR}/include/simple_subscriber
)

pico_add_extra_outputs(fr_uros_diffdrive)
