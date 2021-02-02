cmake_minimum_required(VERSION 3.13.4)
set(PICO_SDK_FETCH_FROM_GIT on)
include(pico_sdk_import.cmake)

project(spi_test)
pico_sdk_init()
add_subdirectory(lib)
#add_executable(pico-epaper main.c)
add_executable(spi_test spi_test.c)
target_link_libraries(spi_test pico_stdlib hardware_spi hardware_gpio)

#target_link_libraries(pico-epaper pico_stdlib pico-waveshare_ePaper_1_54in)

#pico_enable_stdio_usb(pico-epaper 1)
#pico_enable_stdio_uart(pico-epaper 0)

#pico_add_extra_outputs(pico-epaper)


pico_enable_stdio_usb(spi_test 1)
pico_enable_stdio_uart(spi_test 0)

pico_add_extra_outputs(spi_test)
