import sys, struct

if len(sys.argv) != 3:
    print("Usage: {} [input ascii file] [output binary file]".format(sys.argv[0], ))
    exit(1)

fin = open(sys.argv[1], "r")
fout = open(sys.argv[2], "wb")

code = 0
cnt = 0
for c in fin.read():
    if c == '0':
        code <<= 1
    elif c == '1':
        code <<= 1
        code |= 1
    else:
        if c != ' ' and c != '\n':
            print("Invalid character {}". format(c, ))
        continue

    # print(c, end = '')

    cnt += 1
    if cnt == 32:
        # print("")
        fout.write(struct.pack('I', code))

        code = 0
        cnt = 0
