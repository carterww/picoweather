cmake_minimum_required(VERSION 3.13)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# This project will only be for picow
set(PICO_BOARD "pico_w")

# initialize pico-sdk from submodule
# note: this must happen before project()
include(external/pico-sdk/pico_sdk_init.cmake)

project(picoweather)

# initialize the Raspberry Pi Pico SDK
pico_sdk_init()

set(SRCS src/main.c src/pw_log.c src/drivers/s12sd.c src/drivers/bh1750.c)

add_executable(picoweather ${SRCS})

target_include_directories(picoweather PRIVATE ./src)

target_link_libraries(picoweather pico_stdlib hardware_adc hardware_i2c hardware_spi)
if (PICO_CYW43_SUPPORTED)
    target_link_libraries(picoweather pico_cyw43_arch_none)
endif()

# Enable stdio over USB instead of UART
pico_enable_stdio_usb(picoweather 1)
pico_enable_stdio_uart(picoweather 0)

pico_add_extra_outputs(picoweather)
