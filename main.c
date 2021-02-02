#include <stdio.h>
#include <pico/stdlib.h>
#include <pico-waveshare_ePaper_1_54in.h>

int main() {
    printf("Waiting 2s\n");
    gpio_put(PIN_DBG, 1);
    sleep_ms(2000);
    gpio_put(PIN_DBG, 0);

    initDevice();

    printf("Device initialized, waiting 2s\n");
    //gpio_put(PIN_DBG, 0);
    sleep_ms(2000);
    //gpio_put(PIN_DBG, 1);

    initDeviceRegisters();

    printf("Registers intialized, wating 2s\n");
    //gpio_put(PIN_DBG, 0);
    sleep_ms(2000);
    //gpio_put(PIN_DBG, 1);

    clearScreen();

    printf("Done. Waiting 2s\n");
    //gpio_put(PIN_DBG, 0);
    sleep_ms(2000);
    //gpio_put(PIN_DBG, 1);

    bool isRunning = 1;
    uint32_t seconds = 0;
    while(true) {
        gpio_put(PIN_DBG, 0);

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
