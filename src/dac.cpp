#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "hardware/irq.h"
#include "dac.hpp"

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

    dac_set_voltage(0);
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
