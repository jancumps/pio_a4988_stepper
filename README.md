# pio_a4988_stepper
Raspberry PIO autonomous stepper motor driver  
Demo program for [pio_stepper_lib](https://github.com/jancumps/pio_stepper_lib): C++ library to run stepper motors in PIO state machines

see blog: [Stepper Motor Control with Raspberry Pico PI (for DRV8711 IC, but works the same) driver- Part 5: a more autonomous PIO](https://community.element14.com/products/raspberry-pi/b/blog/posts/stepper-motor-control-with-raspberry-pico-pi-and-drv8711-driver--part-5-a-more-autonomous-pio)  

- 4 motors can be controlled per PIO
- supports A4988 (but can be ported to any driver that supports STEP and DIR operation)
- can handle as many commands as PIO FIFO accepts without waiting (default 8).  
- Each command can autonomously handle 2147483647 steps, and the spin direction
- can notify the calling program when a command is finished

Example motor instruction batch of 6 instructions:  
```
stepper::stepper_callback_controller motor1(piostep, sm);

// object to manage the a4988 IC used for motor1
using driver_t = a4988_pico::a4988_pico;
driver_t driver1(n_enable, ms1, ms2, ms3);

void on_complete(stepper::stepper_callback_controller &stepper) {
    if (&motor1 == &stepper) {
        printf("motor1 executed command %d\n", motor1.commands());
    }
}

int main() {

    motor1.on_complete_callback(on_complete);

    std::array<command, 6> cmd{{
        {200, true}, 
        {200, false},
        {200, false},
        {400, false},
        {250, true},
        {350, true}}
    };

    // wake up the driver IC. 
    // It goes back to low power when this object leaves the scope
    stepper_driver::wakeup w(driver1);
    sleep_ms(1); // see datasheet

    motor1.set_delay(delay);
    for(auto c : cmd) {
        motor1.take_steps(c);
    }

        // ...
```