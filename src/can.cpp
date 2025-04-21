#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "hardware/irq.h"
#include "can.hpp"
#include "dac.hpp"
#include "can.hpp"
extern "C" {
    #include "can2040.h"
}

sCAN_Header parse_id(uint32_t id) {
    return sCAN_Header {
        .priority = (uint8_t)((id >> 8) & 0b0111),
        .module  = (eModule)((id >> 5) & 0b0111),
        .direction = (eDirection)(id >> 4),
        .command = (uint8_t)(id & 0xF),
    };
}

static struct can2040 cbus;
static struct can2040_msg msg;

static volatile bool samplefresh = false;
static double sample = 0.0;


static void can2040_cb(struct can2040 *cd, uint32_t notify, struct can2040_msg *msg) {
    switch (notify) {
        case CAN2040_NOTIFY_TX: 
            break;
        case CAN2040_NOTIFY_RX: 
            // assume 2 bytes
            gpio_xor_mask(1 << USER_LED);
            sCAN_Header header = parse_id(msg->id);
            if(header.direction == TO && header.module == ISOLATION_EXPANSION_DEVICE) {
                uint16_t adc_taps = (((uint16_t)msg->data[0]) << 8) | (msg->data[1]);
                double voltage = adc_taps/4096.0 * 5;
                sample = voltage;
                samplefresh = true;
            }
    }
}

static void PIOx_IRQHandler(void) {
    can2040_pio_irq_handler(&cbus);
}

bool get_sample_fresh() {
    return samplefresh;
}

void reset_sample_fresh() {
    samplefresh = false;
}

double get_sample() {
    return sample;
}

void can_init(void) {
    gpio_init(USER_LED);
    gpio_set_dir(USER_LED, GPIO_OUT);
    gpio_put(USER_LED, 1);

    uint32_t pio_num = 0;
    uint32_t sys_clock = 125000000;
    uint32_t bitrate = ONE_MEG;
    uint32_t gpio_tx = CAN_TX;
    uint32_t gpio_rx = CAN_RX;

    can2040_setup(&cbus, pio_num);
    can2040_callback_config(&cbus, can2040_cb);

    irq_set_exclusive_handler(PIO0_IRQ_0, PIOx_IRQHandler);
    irq_set_priority(PIO0_IRQ_0, 1);
    irq_set_enabled(PIO0_IRQ_0, true);

    can2040_start(&cbus, sys_clock, bitrate, gpio_rx, gpio_tx);
}