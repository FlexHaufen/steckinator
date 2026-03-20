
#include <pico/stdlib.h>
#include "Driver/TMC2209Driver.h"

TMC2209Driver::TMC2209Driver(const StepperConfig& c) : cfg(c) {
    gpio_init(cfg.stepPin); gpio_set_dir(cfg.stepPin, GPIO_OUT);
    gpio_init(cfg.dirPin);  gpio_set_dir(cfg.dirPin,  GPIO_OUT);
    gpio_init(cfg.diagPin); gpio_set_dir(cfg.diagPin, GPIO_IN);
    if (cfg.enPin != 0) {
        gpio_init(cfg.enPin); gpio_set_dir(cfg.enPin, GPIO_OUT); gpio_put(cfg.enPin, 0); // active low usually
    }
    gpio_put(cfg.stepPin, 0);
}

void TMC2209Driver::enable(bool on) {
    if (cfg.enPin) gpio_put(cfg.enPin, !on);   // most TMC2209 EN is active LOW
}

void TMC2209Driver::setDirection(bool positive) {
    gpio_put(cfg.dirPin, positive ? 1 : 0);
}

void TMC2209Driver::stepOnce() {
    // respect minimum STEP pulse width (~1-2 µs) + cooldown
    gpio_put(cfg.stepPin, 1);
    busy_wait_us_32(2);
    gpio_put(cfg.stepPin, 0);
    busy_wait_us_32(1);   // you can tune this
    position += (gpio_get(cfg.dirPin) ? 1 : -1);
}

bool TMC2209Driver::isDiagActive() const {
    return gpio_get(cfg.diagPin);   // HIGH = stall/fault on most boards
}