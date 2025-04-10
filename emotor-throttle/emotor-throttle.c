#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "hardware/gpio.h"
#include "can2040.h"

// SPI Defines
// We are going to use SPI 0, and allocate it to the following GPIO pins
// Pins can be changed, see the GPIO function select table in the datasheet for information on GPIO assignments
// #define SPI_PORT spi0
// #define PIN_MISO 16
// #define PIN_CS   17
// #define PIN_SCK  18
// #define PIN_MOSI 19

#define CAN_RX 26
#define CAN_TX 27
#define USER_LED 25

static struct can2040 cbus;
static struct can2040_msg msg;
static uint32_t pending_error = 0;

static void can2040_cb(struct can2040 *cd, uint32_t notify, struct can2040_msg *msg) {
    if(notify == CAN2040_NOTIFY_RX) {
        uint32_t state = msg->data[0];
        gpio_put(USER_LED, !state);
    } 
}

static void PIOx_IRQHandler(void) {
    can2040_pio_irq_handler(&cbus);
}

void canbus_setup(void) {
    uint32_t pio_num = 0;
    uint32_t sys_clock = 125000000, bitrate = 1000000;
    uint32_t gpio_tx = CAN_TX;
    uint32_t gpio_rx = CAN_RX; 

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

int main()
{
    stdio_init_all();
    // canbus_setup();

    gpio_init(USER_LED);
    gpio_set_dir(USER_LED, GPIO_OUT);
    gpio_put(USER_LED, 1);

    gpio_init(CAN_TX);
    gpio_set_dir(CAN_TX, GPIO_OUT);


    while (1) { 
        gpio_put(CAN_TX, 0);
        sleep_ms(1000);
        gpio_put(CAN_TX, 1);
        sleep_ms(1000);


    }
}






