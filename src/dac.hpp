#ifndef DAC_H
#define DAC_H
typedef enum {
    VOLTAGE_OUT_OF_RANGE,
    SPI_NO_WRITE,
    OK
} eDAC_Error;

void dac_init(void);
eDAC_Error dac_set_voltage(double voltage);

#endif
