for i in range(256):
    print("""000110 00000 00000 00001 00000 000000
000000 00001 000000000000000000000
000000000000000000000000""", end="")
    j = i
    for n in range(7, -1, -1):
        p = 1 << n
        if j >= p:
            print(1, end="")
            j -= p
        else:
            print(0, end="")
    print()
    print()
