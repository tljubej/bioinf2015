import sys


def find_all(a_str, sub):
    start = 0
    while True:
        start = a_str.find(sub, start)
        if start == -1: return
        yield start
        start += len(sub) # use start += 1 to find overlapping matches

def find_mems(s, q, l):
    mems = []
    for i in range(len(q)):
        for j in range(len(s)):
            go = False
            if i==0 or j==0:
                go = True
            if i!=0 and j!=0:
                if q[i-1] != s[j-1]:
                    go = True
            if go:
                k = 0
                while s[j+k] == q[i+k]:
                    k += 1
                    if i+k >= len(q) or j+k >= len(s):
                        break

                if k >= l:
                    mems.append( (k, i, j) )

    return mems


def main():
    fin = open(sys.argv[1], 'r')
    finq = open(sys.argv[2], 'r')
    errf = open(sys.argv[3], 'w')
    l = int(sys.argv[4])

    fin.readline()
    s = ''
    for i in fin:
        s += i.strip()

    qc = 1
    while True:
        q = finq.readline().strip()
        if not q:
            break
        fmems = find_mems(s, q, l)
        rmems = []
        while True:
            line = finq.readline().strip()
            if not line:
                break
            rmems.append( tuple(int(i) for i in line.split('\t')) )

        rmems.sort()
        fmems.sort()
        if fmems == rmems:
            print('Query {} ALL OK:'.format(qc))
        else:
            print('Query {} WRONG:'.format(qc))
            # for m, qidx, sidx in rmems:
            #     print('--')
            #     print(q[qidx:qidx+m])
            #     print(s[sidx:sidx+m])
            print(q, file=errf)
            print('R:', rmems, file=errf)
            print('C:', fmems, file=errf)
            print(file=errf)

        print('R:', rmems)
        print('C:', fmems)

        print()

        qc+=1





if __name__ == '__main__':
    main()
