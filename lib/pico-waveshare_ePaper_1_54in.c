//
// Created by geoco on 27.01.2021.
//
#include <pico-waveshare_ePaper_1_54in.h>

static char event_string[128];

void gpio_event_string(char *buf, uint32_t events);

void gpio_callback(uint gpio, uint32_t events) {
    char *gpio_descriptor;
    switch (gpio) {
        case PIN_CS:
            gpio_descriptor = "C/S";
            break;
        case PIN_DIN:
            gpio_descriptor = "DIN";
            break;
        case PIN_CLK:
            gpio_descriptor = "CLK";
            break;
        case PIN_DC:
            gpio_descriptor = "D/C";
            break;
        case PIN_RST:
            gpio_descriptor = "RST";
            break;
        case PIN_BUSY:
            gpio_descriptor = "BUSY";
            break;
        default:
            gpio_descriptor = "???";
    }
    gpio_event_string(event_string, events);

    printf("GPIO: %d=[%s] %s\n", gpio, gpio_descriptor, event_string);
    if (gpio == PIN_BUSY) {
        printf("BUSY: %d\n", gpio_get(PIN_BUSY));
    }
}

void blink(uint8_t amount, uint32_t time) {
    uint8_t i = 0;
    while (i < amount) {
        gpio_put(PIN_LED, 1);
        sleep_ms(time);
        gpio_put(PIN_LED, 0);
        i++;
    }
}

void initDevice() {

    printf("Initializing SPI\n");
    /* Enable SPI at 1MHz*/
    spi_init(SPI_PORT, 1000 * 1000);
    printf("SPI writable: %d\n", spi_is_writable(SPI_PORT));

    gpio_set_irq_enabled_with_callback(PIN_CS, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, &gpio_callback);
    gpio_set_irq_enabled_with_callback(PIN_DC, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, &gpio_callback);
    gpio_set_irq_enabled_with_callback(PIN_BUSY, GPIO_IRQ_LEVEL_HIGH, true, &gpio_callback);
    gpio_set_irq_enabled_with_callback(PIN_RST, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, &gpio_callback);
    gpio_set_irq_enabled_with_callback(PIN_DIN, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, &gpio_callback);

    gpio_set_function(PIN_DC, GPIO_FUNC_SPI);
    gpio_set_function(PIN_CS, GPIO_FUNC_SPI);
    gpio_set_function(PIN_CLK, GPIO_FUNC_SPI);
    gpio_set_function(PIN_DIN, GPIO_FUNC_SPI);

    // Chip select is active-low, so we'll initialise it to a driven-high state
    gpio_init(PIN_DC);
    gpio_init(PIN_CS);
    gpio_init(PIN_RST);
    gpio_init(PIN_LED);
    /**
     * set also clock and data pin to high level
     */

    gpio_set_dir(PIN_DC, GPIO_OUT);
    gpio_set_dir(PIN_CS, GPIO_OUT);
    gpio_set_dir(PIN_DIN, GPIO_OUT);
    gpio_set_dir(PIN_LED, GPIO_OUT);
    gpio_set_dir(PIN_RST, GPIO_OUT);
    gpio_set_dir(PIN_BUSY, GPIO_IN);

    gpio_put(PIN_CS, 1);
    printf("SPI initialized.\n");
    blink(1, 250);
}

void reset() {
    printf("Resetting Panel\n");
    gpio_put(PIN_RST, 1);
    sleep_ms(200);
    gpio_put(PIN_RST, 0);
    sleep_ms(10);
    gpio_put(PIN_RST, 1);
    sleep_ms(200);

    printf("Reset done.\n");
}
void sendCommand(uint8_t reg) {
    gpio_put(PIN_DC, 0);
    gpio_put(PIN_CS, 0);
    spi_write_blocking(SPI_PORT, &reg, 1);
    gpio_put(PIN_CS, 1);
}

void sendData(uint8_t reg) {
    gpio_put(PIN_DC, 1);
    gpio_put(PIN_CS, 0);
    spi_write_blocking(SPI_PORT, &reg, 1);
    gpio_put(PIN_CS, 1);
}
void waitUntilBusy() {
    while (gpio_get(PIN_BUSY) == 1) {
        printf("E-paper device is busy...\n");
        sleep_ms(100);
        blink(1, 250);
        printf("  ... still waiting ...\n");
    }
    printf("E-paper device is available again\n");
}

void turnOnDisplay() {
    sendCommand(0x22);
    sendData(0xf7);
    sendCommand(0x20);
    waitUntilBusy();
}

void initDeviceRegisters() {
    blink(2, 250);
    reset();
    blink(3, 250);
    waitUntilBusy();
    blink(4, 250);
    printf("Software reset\n");
    sendCommand(0x12);// swreset
    waitUntilBusy();
    blink(5, 250);
    printf("Driver output control\n");
    sendCommand(0x01);// driver output control
    sendData(0xc7);
    sendData(0x00);
    sendData(0x01);

    printf("Data entry mode\n");
    sendCommand(0x11);//data entry mode
    sendData(0x01);

    printf("Set x address\n");
    sendCommand(0x44);// set x address start/end position
    sendData(0x00);
    sendData(0x18);

    printf("Set y address\n");
    sendCommand(0x45);// y address start/end position
    sendData(0xc7);
    sendData(0x00);
    sendData(0x00);
    sendData(0x00);

    printf("Waveform\n");
    sendCommand(0x3c);// border waveform
    sendData(0x01);

    printf("???\n");
    sendCommand(0x018);// ???
    sendData(0x80);

    printf("Load temperature and waveform\n");
    sendCommand(0x22);// load temperature and waveform setting
    sendData(0xb1);

    sendCommand(0x4e);// ram x address count = 0
    sendData(0x00);
    sendData(0x4f);// ram y address count = 0x199
    sendData(0xc7);
    sendData(0x00);

    waitUntilBusy();
}
void clearScreen() {
    uint16_t width, height;
    width = (DEVICE_WIDTH % 8 == 0) ? DEVICE_WIDTH / 8 : DEVICE_WIDTH / 8 + 1;
    height = DEVICE_HEIGHT;

    uint16_t j = 0;
    uint16_t i = 0;
    sendCommand(0x24);
    for (; j < height; j++) {
        for (; i < width; i++) {
            sendData(0xff);
        }
    }

    j = 0;
    i = 0;
    sendCommand(0x26);
    for (; j < height; j++) {
        for (; i < width; i++) {
            sendData(0xff);
        }
    }
    turnOnDisplay();
}
void deepSleep() {
    sendCommand(0x10);
    sendData(0x01);

    waitUntilBusy();
}

static const char *gpio_irq_str[] = {
        "LEVEL_LOW", // 0x1
        "LEVEL_HIGH",// 0x2
        "EDGE_FALL", // 0x4
        "EDGE_RISE"  // 0x8
};

void gpio_event_string(char *buf, uint32_t events) {
    for (uint i = 0; i < 4; i++) {
        uint mask = (1 << i);
        if (events & mask) {
            // Copy this event string into the user string
            const char *event_str = gpio_irq_str[i];
            while (*event_str != '\0') {
                *buf++ = *event_str++;
            }
            events &= ~mask;

            // If more events add ", "
            if (events) {
                *buf++ = ',';
                *buf++ = ' ';
            }
        }
    }
    *buf++ = '\0';
}