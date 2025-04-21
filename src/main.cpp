#include <stdio.h>
#include "pico/stdlib.h"
#include "can.hpp"
#include "dac.hpp"

/* main + superloop */
int main() {
    stdio_init_all();

    can_init();
    dac_init();

    while (true) {
        if (get_sample_fresh()) {
            dac_set_voltage(get_sample());
            reset_sample_fresh();
        }
    }
}
