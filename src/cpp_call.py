# import time
from arduino_wrapper import (
    get_Serial,
    pinMode,
    digitalWrite,
    attachInterrupt,
)
# from eventloop.eventloop import EventLoop

# Serial=get_Serial()

# print(Serial)
# print(":sksksksksHello2hhhhhjaja:")
# Serial.println(1)
# print("done")

type int32 = Int[32]

# INPUT = Int[32](0x0)
# OUTPUT = Int[32](0x1)
PA0 = Int[32](0xC0)
# PC13 = Int[32](31)
# ON = Int[32](1)
# OFF = Int[32](0)

# def callback_function():
#     print("Pin change detected!")

# def py_main():
#     print("Started v2")
#     print(INPUT)
#     print(OUTPUT)
#     print(PC13)
#     print(PC13)
#     pinMode(PC13, OUTPUT)

#     while True:
#         print("Loop2")
#         digitalWrite(PC13, ON)
#         time.sleep(1)
#         digitalWrite(PC13, OFF)
#         time.sleep(1)
#         attach_pinchange_interrupt(PC13, callback_function)


# if __name__ == "__main__":
#     py_main()


@C
def init_pinchange_event()->None:
    pass

init_pinchange_event()

# event_loop = EventLoop()
# event_loop.add_event_handler("pinchange", lambda data: print("data received"+str(data)))
# event_loop.run()


# flag: bool = False
# def set_flag():
#     global flag
#     flag = True



# CHANGE=Int[32](0x2)
# attachInterrupt(PA0, set_flag,CHANGE)

# while True:
#     if flag:
#         print("Pin change detected!")
#         flag = False
