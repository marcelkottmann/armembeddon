from import_test import TEST_EXPORT

lst = []
for i in range(15):
    lst.append(str(i))

for i in lst:
    print(i)

print("Hello world !" + str(11) + "elf")

lst.clear()


class A:
    def __init__(self, val: str):
        self.val = val


lst2 = []
for i in range(15):
    lst2.append(A("Hello" + str(i * 2)))

print(TEST_EXPORT)
