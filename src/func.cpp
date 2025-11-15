#include <Arduino.h>

extern "C"
{
    void call_function(void (*callback)(), uint32_t arg, uint64_t arg2,uint64_t arg3 )
    {
        callback();
        Serial.println("Arguments received: ");
        Serial.println(arg,HEX);
        Serial.println(arg2);
        Serial.println(arg3);
    }
}
