import sys
import random

if __name__ == '__main__':
    f = open(sys.argv[1], 'r')
    of = open(sys.argv[2], 'w')
    l = int(sys.argv[3])
    n = int(sys.argv[4])

    f.readline()

    s = ''
    for i in f:
        s += i.strip()

    slen = len(s)
    for i in range(n):
        rnd = random.randint(0, slen-l)
        print('@{}:{}_{}/1'.format(rnd, l, i), file=of)
        print(s[rnd:rnd+l], file=of)

    f.close()
    of.close()
