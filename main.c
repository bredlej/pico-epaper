#include <stdio.h>
#include <pico/stdlib.h>
#include <pico-waveshare_ePaper_1_54in.h>

int main() {
    stdio_init_all();
    sleep_ms(1000);
    printf("Initializing device\n");
    initDevice();
    sleep_ms(1000);
    printf("Initializing e-paper display\n");
    initDeviceRegisters();
    printf("Clearing screen\n");
    clearScreen();
    bool isRunning = 1;
    uint32_t seconds = 0;
    while(true) {
        sleep_ms(1000);
        if (isRunning) {
            printf(". ");
            seconds = seconds + 1;
            if (seconds > 10) {
                isRunning = 0;
                printf("Sleeping\n");
                deepSleep();
            }
        }
    }
}
