#include <Arduino.h>
#include "stm32f4xx_hal.h"

extern "C"
{
    void eventloop_wait_for_interrupt(){
        Serial.println("__WFI()");
        __enable_irq();
        __WFI();
    }
}