//
// Created by geoco on 02.02.2021.
//
#include <stdio.h>
#include <pico/stdlib.h>
#include <hardware/gpio.h>
#include <hardware/spi.h>

#define PIN_MISO 4
#define PIN_CS 5
#define PIN_CLK 6
#define PIN_MOSI 7
#define PIN_RESET 8
#define PIN_DC 9
#define PIN_LED 25
#define PIN_DBG 15

#define SPI_PORT spi0

static inline void cs_select() {
    asm volatile("nop \n nop \n nop");
    gpio_put(PIN_CS, 0);// Active low
    asm volatile("nop \n nop \n nop");
}

static inline void cs_deselect() {
    asm volatile("nop \n nop \n nop");
    gpio_put(PIN_CS, 1);
    asm volatile("nop \n nop \n nop");
}

void sendCommand(uint8_t reg) {
    cs_select();
 //   gpio_put(PIN_DC, 0);
    spi_write_blocking(SPI_PORT, &reg, 1);
    cs_deselect();
}

int main()
{
    stdio_init_all();
    spi_init(SPI_PORT, 2000 * 1000);
    spi_set_format(SPI_PORT, 8, SPI_CPOL_1, SPI_CPHA_0, SPI_MSB_FIRST);

    printf("initDevice()\n");

    gpio_set_function(PIN_CS, GPIO_FUNC_SPI);
    gpio_set_function(PIN_CLK, GPIO_FUNC_SPI);
    //gpio_set_function(PIN_MISO, GPIO_FUNC_SPI);
    gpio_set_function(PIN_MOSI, GPIO_FUNC_SPI);

    gpio_init(PIN_CS);
    gpio_set_dir(PIN_CS, GPIO_OUT);
    gpio_put(PIN_CS, 0);

    gpio_init(PIN_RESET);
    gpio_set_dir(PIN_RESET, GPIO_OUT);
    gpio_put(PIN_RESET, 1);

    gpio_init(PIN_DC);
    gpio_set_dir(PIN_DC, GPIO_OUT);
    gpio_put(PIN_DC, 0);

    gpio_init(PIN_LED);
    gpio_set_dir(PIN_LED, GPIO_OUT);

    gpio_init(PIN_DBG);
    gpio_set_dir(PIN_DBG, GPIO_OUT);

    uint8_t counter = 0;
    while (1)
    {
        sendCommand(counter++);
        gpio_put(PIN_DBG, 0);
        gpio_put(PIN_DBG, 1);
        gpio_put(PIN_LED, 0);
    }
}
