#include <stdio.h>
#include <pico/stdlib.h>
#include <pico-waveshare_ePaper_1_54in.h>

int main() {

    initDevice();
    initDeviceRegisters();
    clearScreen();

    bool isRunning = 1;
    uint32_t seconds = 0;
    while(true) {
        sleep_ms(100);
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
