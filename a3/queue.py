import matplotlib.pyplot as plt
import numpy as np

with open("./q3/tcp-example.tr", "r") as f:
    data = f.readlines()
    x, y = [], []
    q = {}
    td, tp, ctr = 0, 0, 0
    for i in data:
        i = i.strip().split()
        n = i[2].split('/')[2]
        if n != '1':
            continue
        seq = int(i[36][4:])
        t = float(i[1])
        if i[0] == '+':
            q[seq] = t
        elif i[0] == '-':
            if seq not in q:
                continue
            dl = t - q[seq]
            td += dl
            tp += 1
            if len(x) > 0 and x[-1] == t:
                y[-1] = (y[-1] * ctr + dl) / (ctr + 1)
                ctr += 1
            else:
                ctr = 1
                x.append(t)
                y.append(dl)
            q.pop(seq)

    plt.plot(x, y, "bo", markersize=3)
    plt.xlabel("Time (s)")
    plt.ylabel("Queuing delay (s)")
    plt.savefig('q3/queue.png')