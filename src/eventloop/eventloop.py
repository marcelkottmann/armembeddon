from typing import Callable

class Event:
    name: str
    payload: int


@C
def eventloop_wait_for_interrupt() -> None:
    pass


@C
def eventloop_get_event() -> Event:
    pass


@C
def get_pinchange_event()->int:
    pass


class EventLoop:
    def __init__(self):
        self.all_event_handler:dict[str, Callable[[int], None]] = {}

    def add_event_handler(self, event_name: str, handler_function:Callable[[int], None]):
        print(len(self.all_event_handler))
        self.all_event_handler[event_name] = handler_function
        print(len(self.all_event_handler))
        
    def run(self):
        if len(self.all_event_handler) == 0:
            print("No event handlers registered. Exiting event loop.")
            return

        print("endless loop")
        while True:
            eventloop_wait_for_interrupt()
            event = get_pinchange_event()
            handler = self.all_event_handler["pinchange"]
            if handler is not None:
                handler(event)