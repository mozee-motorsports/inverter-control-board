#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "hardware/irq.h"
extern "C" {
    #include "can2040.h"
}

#define USER_LED 25
typedef enum {
    VOLTAGE_OUT_OF_RANGE,
    SPI_NO_WRITE,
    OK
} eDAC_Error;
eDAC_Error dac_set_voltage(double voltage);

/* CAN Stuff */

#define CAN_TX 27
#define CAN_RX 26
#define ONE_MEG 1000000

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
            uint16_t adc_taps = (((uint16_t)msg->data[0]) << 8) | (msg->data[1]);
            double voltage = adc_taps/4096.0 * 5;
            printf("adc_taps: %d\n", adc_taps);
            printf("voltage: %lf\n", voltage);

            dac_set_voltage(voltage);
            // // sample = voltage;
            // // samplefresh = true;
            // break;
    }
}

static void PIOx_IRQHandler(void) {
    can2040_pio_irq_handler(&cbus);
}

void can_init(void) {
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

/* DAC Stuff */
#define PIN_MISO 4
#define PIN_CS   5
#define PIN_SCK  2
#define PIN_MOSI 3
#define SPI_PORT spi0

void dac_init(void) {
    spi_init(spi0, 1000*1000);
    gpio_set_function(PIN_MISO, GPIO_FUNC_SPI);
    gpio_set_function(PIN_CS,   GPIO_FUNC_SIO);
    gpio_set_function(PIN_SCK,  GPIO_FUNC_SPI);
    gpio_set_function(PIN_MOSI, GPIO_FUNC_SPI);
    
    gpio_init(PIN_CS);
    gpio_set_dir(PIN_CS, GPIO_OUT);
} 



eDAC_Error dac_set_voltage(double voltage) {
    if ((voltage < 0) || (voltage > 5)) {
        return VOLTAGE_OUT_OF_RANGE;
    }

    double voltage_3_3 = (voltage/5.0)*3.3;
    uint16_t voltage_u16 = (0x7FF)*(voltage_3_3/3.3);

    uint8_t voltage_upper = (voltage_u16 >> 4) & 0xFF; 
    uint8_t voltage_lower = (voltage_u16 << 4) & 0xFF;

    uint8_t spi_data[3] = {
        0x00,
        voltage_upper,
        voltage_lower,
    };

    gpio_put(PIN_CS, 0);
    int32_t len = spi_write_blocking(SPI_PORT, spi_data, 3);
    gpio_put(PIN_CS, 1);

    if(len <= 0) {
        return SPI_NO_WRITE; 
    } 
    return OK;
}


/* main + superloop */
int main() {
    stdio_init_all();

    can_init();
    dac_init();
    dac_set_voltage(0);
    gpio_init(USER_LED);
    gpio_set_dir(USER_LED, GPIO_OUT);

    gpio_put(USER_LED, 1);

    while (true) {
        // if(samplefresh) {

            // gpio_put(USER_LED, 0);
            // samplefresh = false;
            // sleep_ms(1000);
            // gpio_put(USER_LED, 1);
        // }
    }
}
