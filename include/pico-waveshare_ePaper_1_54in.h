//
// Created by geoco on 27.01.2021.
//
#include <hardware/gpio.h>
#include <hardware/spi.h>
#include <pico/stdlib.h>
#include <stdio.h>

#ifndef PICO_PLAYGROUND_PICO_WAVESHARE_EPAPER_1_54IN_H
#define PICO_PLAYGROUND_PICO_WAVESHARE_EPAPER_1_54IN_H

#define PIN_MISO 4
#define PIN_CS 5
#define PIN_CLK 6
#define PIN_MOSI 7
#define PIN_RESET 8
#define PIN_DC 9
#define PIN_LED 25
#define PIN_DBG 15

#define SPI_PORT spi0

#define DEVICE_WIDTH 200
#define DEVICE_HEIGHT 200

void blink(uint8_t, uint32_t);
void initDevice();
void reset();
void sendCommand(uint8_t);
void sendData(uint8_t);
void waitUntilBusy();
void turnOnDisplay();
void initDeviceRegisters();
void clearScreen();
void deepSleep();

#endif//PICO_PLAYGROUND_PICO_WAVESHARE_EPAPER_1_54IN_H
