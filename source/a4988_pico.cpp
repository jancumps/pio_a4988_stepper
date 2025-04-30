module;

#include "hardware/gpio.h"

import stepper_driver;

export module a4988_pico;

export namespace a4988_pico {

class a4988_pico: public stepper_driver::stepper_driver {
public:
    a4988_pico( uint n_enable, uint ms1, uint ms2,  uint ms3) :
        n_enable_(n_enable), ms1_(ms1), ms2_(ms2), ms3_(ms3) {}
    virtual inline  bool init() override { 
        init_gpio();
        return true;
    }
    virtual bool microsteps(unsigned int microsteps) override { 
        unsigned int mode = true;
        switch (microsteps) {
        case 1:
            mode = 0x0000;
            break;
        case 2:
            mode = 0x0001;
            break;
        case 4:
            mode = 0x0002;
            break;
        case 8:
            mode = 0x0003;
            break;
        case 16:
            mode = 0x0004;
            break;
        default:
            assert(false); // develop check unsupported microstep
            mode = 0x0000;
        }
        gpio_put(ms1_, mode & 0b001);
        gpio_put(ms2_, mode & 0b010);
        gpio_put(ms1_, mode & 0b100);

        return true;
    }
    virtual void enable(bool enable) override {
        gpio_put(n_enable_, enable? 0 : 1);
    }
    
private:
    uint n_enable_;
    uint ms1_;
    uint ms2_;
    uint ms3_;

    virtual void init_gpio() {
        gpio_init(n_enable_);
        gpio_put(n_enable_, 1);
        gpio_set_dir(n_enable_, GPIO_OUT);

        gpio_init(ms1_);
        gpio_put(ms1_, 0);
        gpio_set_dir(ms1_, GPIO_OUT);

        gpio_init(ms2_);
        gpio_put(ms2_, 0);
        gpio_set_dir(ms2_, GPIO_OUT);

        gpio_init(ms3_);
        gpio_put(ms3_, 0);
        gpio_set_dir(ms3_, GPIO_OUT);
    }    
};

} // a4988_driver