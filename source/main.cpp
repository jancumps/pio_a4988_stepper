#include "hardware/pio.h" // some constant definitions used

#include "pico/stdlib.h"  // for demo section (printf)
#include <array>          // for demo section (commands container)
#include <iterator>       // for demo section (commands container)
#include <span>           // for demo section (commands container)

                          // this project uses ti drv8711. 
                          // But the stepper code is driver independent.
                          // the imports get the generic register definitions 
                          // default configuaration, and
                          // the Pico spi specific communication code.
                          // each class that deals with this IC has drv8711 in the name
                          // those are the only code parts that are stepper IC specific
                          // if you have another driver, that's what you have to replace.
import stepper_driver;    // wakeup class
import a4988_pico;      // driver classes and registers

import stepper;           // PIO stepper lib

#define MICROSTEP_16
// #define MICROSTEP_8
// #define MICROSTEP_2

const uint dir = 14U; // implies that step is gpio 15

const uint n_enable = 6U;
const uint ms1 = 7U;
const uint ms2 = 8U;
const uint ms3 = 9U;

// config what PIO and IRQ channel to use
const auto piostep = pio1;
// use pio irq channel 0. Can be 0 or 1
const uint pio_irq = 0;
// state machine
const uint sm = 2U;

#ifdef MICROSTEP_16
const float clock_divider = 8; // works well for 16 microsteps
const uint microstep_x = 16;
#elifdef MICROSTEP_8
const float clock_divider = 6; // works well for 8 microsteps
const uint microstep_x = 8;
#elifdef MICROSTEP_2
const float clock_divider = 14; // works well for 2 microsteps
const uint microstep_x = 2;
#else
const float clock_divider = 24; // works well for no microsteps
const uint microstep_x = 1;
#endif

using motor_t = stepper::stepper_callback_controller;
motor_t motor1(piostep, sm);

// object to manage the a4988 IC used for motor1
using driver_t = a4988_pico::a4988_pico;
driver_t driver1(n_enable, ms1, ms2, ms3);

void init_pio() {
    // program the pio used for the motors
    // do this only once per used pio
    motor_t::pio_program(piostep);

    // initialise and enable the motor state machine
    motor1.register_pio_interrupt(pio_irq, true);
    motor1.pio_init(dir, clock_divider);
    motor1.enable(true);
}

void init_everything() {
    stdio_init_all();

    driver1.init();
    driver1.microsteps(microstep_x);

    init_pio();
}

// stepper demo: execute a series of commands ================================

using commands_t = std::span<stepper::command>;	

volatile int steps_countdown = 0U;
void on_complete(const motor_t &stepper) {
    if (&motor1 == &stepper) {
        steps_countdown =  steps_countdown - 1;
        printf("motor1 executed command %d\n", motor1.commands());
    }
}

void run_with_delay(const commands_t & cmd, uint32_t delay) {
    printf("delay: %d\n", delay);
    motor1.set_delay(delay);

    steps_countdown = cmd.size();
    for(auto c : cmd) {
        motor1.take_steps(c);
    }
    while(steps_countdown) {}
    printf("interrupts expected: %d, received %d\n", cmd.size(), motor1.commands());
    motor1.reset_commands();
    sleep_ms(500); // pause for demo purpose    
}

void full_demo(const commands_t & cmd) {
    // wake up the driver IC. 
    // It goes back to low power when this object leaves the scope
    stepper_driver::wakeup w(driver1);
    sleep_ms(1); // see datasheet

    run_with_delay(cmd, 4300);
    // run_with_delay(cmd, 7000);
    // run_with_delay(cmd, 9000);
    // run_with_delay(cmd, 20000);
}

int main() {
    init_everything();
    std::array<stepper::command, 4> cmd{{
        {60 * microstep_x, false},
        {120 * microstep_x, true}, 
        {120 * microstep_x, false},
        {60 * microstep_x, true}}};

    motor1.on_complete_callback(on_complete); 

    while (true) {
        full_demo(cmd);
        sleep_ms(500); // cool off
    }
    return 0;
}