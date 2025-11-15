#import re

def convert_int_to_str():
    s = str(123)
    e = "123"

    if not s == e:
        return "Expected string '123' but got " + s


def convert_float_to_str():
    s = str(1.12)
    e = "1.12"

    if not s == e:
        return "Expected string '1.12' but got " + s


def str_str_concatenation():
    a = "A"
    b = "B"
    c = a + b
    if not c == "AB":
        return "Expected string 'AB' but got " + c


def str_int_concatenation():
    c = "A" + str(1)
    if not c == "A1":
        return "Expected string 'A1' but got " + c


def str_float_concatenation():
    c = "A" + str(1.23)
    if not c == "A1.23":
        return "Expected string 'A1.23' but got " + c


def print_test():
    print("Print Test", end="... ")


def run_test(test_fn):
    print(test_fn.__name__, "... ", end="")
    error = test_fn()
    if not error:
        print("SUCCESS!")
    else:
        print("FAILURE:", error)


def py_main():
    print("VERSION 0.0.20")
    a={}
    print(a)
    #result=re.match("Hello, World!", "Hello, World!")
    run_test(convert_int_to_str)
    run_test(str_str_concatenation)
    run_test(str_int_concatenation)
    run_test(str_float_concatenation)
    run_test(print_test)
    run_test(convert_float_to_str)
    s = input("Press Enter to continue...")
    print("You entered2!")


if __name__ == "__main__":
    py_main()




