#include <Arduino.h>

volatile uint64_t pin_change_flag = 0;

extern "C"
{
  void *get_Serial()
  {
    return &Serial;
  }

  // capture pinchange

  void pin_change_callback()
  {
    pin_change_flag = 1;
  }

  void init_pinchange_event()
  {
    pinMode(PA0, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(PA0), pin_change_callback, CHANGE);
  }

  // void attach_interrupt(uint32_t pin, void (*callback)(), uint32_t mode)
  // {
  //   attachInterrupt(pin, callback, mode);
  // }

  uint64_t get_pinchange_event()
  {
    return pin_change_flag;
  }

  // void print_mock(const char *str)
  // {
  //   Serial.print("Print mock:");
  //   Serial.println(str);
  // }
}