//
// Created by geoco on 27.01.2021.
//
#include <pico-waveshare_ePaper_1_54in.h>

static char event_string[128];

void gpio_event_string(char *buf, uint32_t events);

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
void startLed() {
    gpio_put(PIN_LED, 1);
}

void stopLed() {
    gpio_put(PIN_LED, 0);
}
void gpio_callback(uint gpio, uint32_t events) {
    char *gpio_descriptor;
    switch (gpio) {
        case PIN_CS:
            gpio_descriptor = "C/S";
            break;
        case PIN_MOSI:
            gpio_descriptor = "DIN";
            break;
        case PIN_CLK:
            gpio_descriptor = "CLK";
            break;
        case PIN_DC:
            gpio_descriptor = "D/C";
            break;
        case PIN_RESET:
            gpio_descriptor = "RST";
            break;
        case PIN_MISO:
            gpio_descriptor = "BUSY";
            break;
        default:
            gpio_descriptor = "???";
    }
    gpio_event_string(event_string, events);

    printf("GPIO: %d=[%s] %s\n", gpio, gpio_descriptor, event_string);
    if (gpio == PIN_MISO) {
        printf("BUSY: %d\n", gpio_get(PIN_MISO));
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
    stdio_init_all();
    /* Enable SPI at 1MHz*/
    printf("initDevice()\n");
    sleep_ms(3000);
    spi_init(SPI_PORT, 1000 * 1000);
    spi_set_format(SPI_PORT, 8, SPI_CPOL_0, SPI_CPHA_0, SPI_MSB_FIRST);
    printf("Initializing SPI\n");
    printf("SPI writable: %d\n", spi_is_writable(SPI_PORT));
    //    spi_set_format(SPI_PORT, 8, SPI_CPOL_0, SPI_CPHA_0, SPI_MSB_FIRST);
    //    gpio_set_irq_enabled_with_callback(PIN_CS, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, &gpio_callback);
    //    gpio_set_irq_enabled_with_callback(PIN_DC, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, &gpio_callback);
    //    gpio_set_irq_enabled_with_callback(PIN_BUSY, GPIO_IRQ_LEVEL_HIGH, true, &gpio_callback);
    //    gpio_set_irq_enabled_with_callback(PIN_RST, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, &gpio_callback);
    //gpio_set_irq_enabled_with_callback(PIN_CS, GPIO_IRQ_EDGE_RISE, true, &stopLed);
    //gpio_set_irq_enabled_with_callback(PIN_CS, GPIO_IRQ_EDGE_FALL, true, &startLed);
    gpio_set_irq_enabled_with_callback(PIN_MISO, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, &gpio_callback);
    //    gpio_set_irq_enabled_with_callback(PIN_CLK, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, &gpio_callback);

    gpio_set_function(PIN_CLK, GPIO_FUNC_SPI);
    gpio_set_function(PIN_CS, GPIO_FUNC_SPI);
    gpio_set_function(PIN_MISO, GPIO_FUNC_SPI);
    gpio_set_function(PIN_MOSI, GPIO_FUNC_SPI);

    // Chip select is active-low, so we'll initialise it to a driven-high state
    //gpio_init(PIN_CS);
    gpio_init(PIN_RESET);
    gpio_init(PIN_DC);
    gpio_init(PIN_LED);

    gpio_set_irq_enabled_with_callback(PIN_MOSI, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, &gpio_callback);
    /**
     * set also clock and data pin to high level
     */

    gpio_set_dir(PIN_DC, GPIO_OUT);
    gpio_set_dir(PIN_CS, GPIO_OUT);
    gpio_set_dir(PIN_LED, GPIO_OUT);
    gpio_set_dir(PIN_RESET, GPIO_OUT);
    gpio_set_dir(PIN_MISO, GPIO_IN);

    gpio_put(PIN_CS, 1);
    printf("SPI initialized.\n");
    blink(1, 250);
}

void reset() {
    printf("Resetting Panel\n");
    gpio_put(PIN_RESET, 1);
    sleep_ms(200);
    gpio_put(PIN_RESET, 0);
    sleep_ms(10);
    gpio_put(PIN_RESET, 1);
    sleep_ms(200);

    printf("Reset done.\n");
}
void sendCommand(uint8_t reg) {
    //gpio_put(PIN_CS, 0);
    cs_select();
    gpio_put(PIN_DC, 0);
    spi_write_blocking(SPI_PORT, &reg, 1);
    cs_deselect();
}

void sendData(uint8_t reg) {
    //gpio_put(PIN_CS, 0);
    cs_select();
    gpio_put(PIN_DC, 1);
    uint8_t bytes = spi_write_blocking(SPI_PORT, &reg, 1);
    cs_deselect();
}
void waitUntilBusy() {
    while (gpio_get(PIN_MISO) == 1) {
        printf("E-paper device is busy...\n");
        sleep_ms(100);
        //blink(1, 250);
        printf("  ... still waiting ...\n");
        printf("E-paper device is available again\n");
    }
}

void turnOnDisplay() {
    sendCommand(0x22);
    sendData(0xf7);
    sendCommand(0x20);
    waitUntilBusy();
}

void initDeviceRegisters() {
    // blink(2, 250);
    reset();
    sleep_ms(500);
    //  blink(3, 250);
    waitUntilBusy();
    //  blink(4, 250);

    printf("Software reset\n");
    sendCommand(0x12);// swreset
    sleep_ms(500);
    waitUntilBusy();
    //  blink(5, 250);
    printf("Driver output control\n");
    sendCommand(0x01);// driver output control
    sleep_ms(500);
    sendData(0xc7);
    sendData(0x00);
    sendData(0x01);
    sleep_ms(500);
    printf("Data entry mode\n");
    sendCommand(0x11);//data entry mode
    sleep_ms(500);
    sendData(0x01);
    sleep_ms(500);
    printf("Set x address\n");
    sendCommand(0x44);// set x address start/end position
    sleep_ms(500);
    sendData(0x00);
    sendData(0x18);
    sleep_ms(500);
    printf("Set y address\n");
    sendCommand(0x45);// y address start/end position
    sleep_ms(500);
    sendData(0xc7);
    sendData(0x00);
    sendData(0x00);
    sendData(0x00);
    sleep_ms(500);
    printf("Waveform\n");
    sendCommand(0x3c);// border waveform
    sleep_ms(500);
    sendData(0x01);
    sleep_ms(500);

    printf("???\n");
    sendCommand(0x018);// ???
    sleep_ms(500);
    sendData(0x80);
    sleep_ms(500);

    printf("Load temperature and waveform\n");
    sendCommand(0x22);// load temperature and waveform setting
    sleep_ms(500);
    sendData(0xb1);
    sleep_ms(500);

    sendCommand(0x4e);// ram x address count = 0
    sleep_ms(500);
    sendData(0x00);
    sendData(0x4f);// ram y address count = 0x199
    sendData(0xc7);
    sendData(0x00);
    sleep_ms(500);

    waitUntilBusy();
}
void clearScreen() {
    printf("Clearing screen\n");
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
    startLed();
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