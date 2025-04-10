#include <stdio.h>
#include "pico/stdlib.h"
#include "boards/sparkfun_promicro_rp2350.h"
#include "hardware/gpio.h"
#include "hardware/adc.h"
#include "can2040.h"

// CAN2040 defines
static struct can2040 cbus;
static struct can2040_msg msg;
static uint32_t pending_error = 0;

static void can2040_cb(struct can2040 *cd, uint32_t notify, struct can2040_msg *msg) {
    if(notify == CAN2040_NOTIFY_RX) {
        uint32_t state = msg->data[0];
        gpio_put(2, state);
    } 
}

static void PIOx_IRQHandler(void) {
    can2040_pio_irq_handler(&cbus);
}

void canbus_setup(void) {
    uint32_t pio_num = 0;
    uint32_t sys_clock = 125000000, bitrate = 1000000;
    uint32_t gpio_tx = 3;
    uint32_t gpio_rx = 4; 

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
    // adc_init();
    canbus_setup();

    gpio_init(2);
    gpio_set_dir(2, GPIO_OUT);
    // adc_gpio_init(26); // A0 on Sparkfun
    // adc_select_input(0); // ADC0 is 26


    while (1) {
        // uint16_t adc_result = adc_read();
        // printf("adc_result: %d\n", adc_result);
        // msg.id = 1;
        // msg.dlc = 2;
        // msg.data[0] = adc_result & 0xFF;
        // msg.data[1] = (adc_result >> 8) & 0xFF;
        // int result = can2040_transmit(&cbus, &msg);
        // printf("can2040_transmit(%02X%02X) -> %d\n", msg.data[1], msg.data[0], result);

        // sleep_ms(1000);
    }
}






