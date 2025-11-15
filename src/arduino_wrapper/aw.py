from typing import Callable


# wrapper to get rid of IDE warnings, not actually used in codon
def C(func):
    return func


class HardwareSerial:
    def println(self, data: str | int):
        if isinstance(data, int):
            data = str(data)
        _ZN5Print7printlnEPKc(self, data.c_str())
        pass


@C
def _ZN5Print7printlnEPKc(this: HardwareSerial, data: Ptr[byte]) -> None:
    pass


@C
def get_Serial() -> HardwareSerial:
    pass


@C
def pinMode(pin: Int[32], mode: Int[32]) -> None:
    pass


@C
def digitalWrite(pin: Int[32], value: Int[32]) -> None:
    pass


AttachPinChangeInterruptCallback = Callable[[], None]


@C
def _Z15attachInterruptmSt8functionIFvvEEm(
    pin: Int[32],
    callback: AttachPinChangeInterruptCallback,
    mode: Int[32],
) -> None:
    pass
