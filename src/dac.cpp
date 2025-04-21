#include "dac.hpp"
extern "C" {
    #include "hardware/spi.h"
    #include "hardware/structs/spi.h"
    #include "hardware/gpio.h"
    #include "pico/stdlib.h"
}

typedef enum {
    VOLTAGE_OUT_OF_RANGE,
    OK
} eDAC_Error;


eDAC_Error dac_set_voltage(double voltage) {
    if ((voltage < 0) || (voltage > 3.3)) {
        return VOLTAGE_OUT_OF_RANGE;
    }
    uint16_t voltage_u16 = 4095*(voltage/3.3);
    uint8_t spi_data[3] = {
        0x00,
        (voltage_u16 >> 8),
        ((voltage_u16 << 4) & 0xFF),
    };

    spi_write_blocking(SPI_PORT, spi_data, 3);
}