class A:
    def callback_fn(self):
        print("hello")


@C
def call_function(callback: Callable[[], None], arg: int, arg2: int) -> None:
    pass


a=A()

call_function(a.callback_fn, 123, 987)
