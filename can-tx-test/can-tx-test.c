#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "hardware/i2c.h"
#include "hardware/irq.h"
#include "hardware/platform_defs.h"
#include "can2040.h"

// SPI Defines
// We are going to use SPI 0, and allocate it to the following GPIO pins
// Pins can be changed, see the GPIO function select table in the datasheet for information on GPIO assignments
#define SPI_PORT spi0
#define PIN_MISO 16
#define PIN_CS   17
#define PIN_SCK  18
#define PIN_MOSI 19

// I2C defines
// This example will use I2C0 on GPIO8 (SDA) and GPIO9 (SCL) running at 400KHz.
// Pins can be changed, see the GPIO function select table in the datasheet for information on GPIO assignments
#define I2C_PORT i2c0
#define I2C_SDA 8
#define I2C_SCL 9


// CAN2040 defines
static struct can2040 cbus;
static struct can2040_msg msg;

static void can2040_cb(struct can2040 *cd, uint32_t notify, struct can2040_msg *msg) {
    static const float conversion_factor = 3.3f / (1 << 12); 
    if (notify == CAN2040_NOTIFY_TX) {
        // uint16_t adc_value = (((uint16_t) msg->data[1]) << 8) | (uint16_t)msg->data[0];
        // uint16_t voltage = adc_value * conversion_factor;
        // if (voltage > 1.5) {
        //     gpio_put(0, 1);
        // } else {
        //     gpio_put(0, 0);
        // }
    }
}

static void PIOx_IRQHandler(void) {
    can2040_pio_irq_handler(&cbus);
}

void canbus_setup(void) {
    uint32_t pio_num = 0;
    uint32_t sys_clock = 125000000, bitrate = 1000000;
    uint32_t gpio_tx = 27;
    uint32_t gpio_rx = 26;

    // Setup canbus
    can2040_setup(&cbus, pio_num);
    can2040_callback_config(&cbus, can2040_cb);

    // Enable irqs
    irq_set_exclusive_handler(PIO0_IRQ_0, PIOx_IRQHandler);
    irq_set_priority(PIO0_IRQ_0, 1);
    irq_set_enabled(PIO0_IRQ_0, true);

    // Start canbus
    can2040_start(&cbus, sys_clock, bitrate, gpio_rx, gpio_tx);

}

int main() {
    stdio_init_all();
    canbus_setup();
    sleep_ms(5000);

    uint8_t LED_STATE = 0;

    int toggle = 0;
    while (true) {
        printf("Enter 1 or 0 To Turn LED ON or OFF:\n");
        scanf("%d", &LED_STATE);
        gpio_put(0, LED_STATE);
        sleep_ms(100);

        msg.id = 1;
        msg.dlc = 1;
        msg.data[0] = LED_STATE;
        int success = can2040_transmit(&cbus, &msg);
        printf("can2040_transmit(%d) -> %d\n", LED_STATE, success);
    }
}
