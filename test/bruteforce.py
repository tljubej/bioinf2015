import sys


def find_all(a_str, sub):
    start = 0
    while True:
        start = a_str.find(sub, start)
        if start == -1: return
        yield start
        start += len(sub) # use start += 1 to find overlapping matches

if __name__ == '__main__':
    fin = open(sys.argv[1], 'r')
    finq = open(sys.argv[2], 'r')

    fin.readline()
    s = ''
    for i in fin:
        s += i.strip()

    sq = finq.read().strip()
    lsq = len(sq)

    # mems = []
    #
    # for i in range(len(s)):
    #     # if i%100000 == 0:
    #     #     print(i)
    #     if s[i:i+lsq] == sq:
    #         mems.append(i)

    print(list(find_all(s, sq)))
