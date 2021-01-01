from itertools import count
from matplotlib.animation import FuncAnimation
from statistics import mean

import matplotlib.pyplot as plt
import pandas as pd
import random

import sys

args = sys.argv

if len(args) != 2:
    print("Incorrect number of arguments, terminating...")

    sys.exit(-1)

directory = sys.argv[1]

fig, (a1, a2) = plt.subplots(2)

def animate(k):
    a1.cla()
    a2.cla()

    fpath = "results/" + directory + "/main"

    data = pd.read_csv(fpath)

    x = data['episode']
    y = data['average']
    e = data['epsilon']

    if len(y) <= 0:
        return

    mx = max(y)
    mn = min(y)
    av = mean(y)

    a1.plot(x[-20:], y[-20:], label="Avergae Reward")
    a1.plot(x[-20:], [mx] * min(20, len(y)))
    a1.plot(x[-20:], [mn] * min(20, len(y)))
    a1.plot(x[-20:], [av] * min(20, len(y)))
    
    a2.plot(x[-20:], e[-20:], label="Average Epsilon")

    plt.xlabel("Episode")
    fig.suptitle("Averages")

    a1.legend()
    a2.legend()

    plt.tight_layout()

ani = FuncAnimation(plt.gcf(), animate, interval=1000)

plt.tight_layout()
plt.show()
