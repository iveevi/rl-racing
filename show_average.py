from itertools import count
from matplotlib.animation import FuncAnimation
from statistics import mean

import matplotlib.pyplot as plt
import pandas as pd
import random
import numpy as np

import sys

args = sys.argv

if len(args) != 2:
    print("Incorrect number of arguments, terminating...")

    sys.exit(-1)

directory = sys.argv[1]

fig, (a1, a2, a3, a4, a5) = plt.subplots(5)

def animate(k):
    fpath = "results/" + directory + "/main"

    data = pd.read_csv(fpath)

    x = data['episode']
    y = data['average']
    e = data['epsilon']
    td = data['td']

    a1.cla()
    a2.cla()
    a3.cla()
    a4.cla()
    a5.cla()

    if len(y) <= 0:
        return

    mx = max(y)
    mn = min(y)
    av = mean(y)

    a1.plot(x, y, label="Reward")
    a1.plot(x, [mx] * len(y))
    a1.plot(x, [mn] * len(y))
    a1.plot(x, [av] * len(y))

    a2.plot(x, td, label="TD Error")

    a3.plot(x, y.rolling(15, min_periods=1).mean(), label="Smoothed Reward")

    a4.plot(x, td.rolling(15, min_periods=1).mean(), label="Smoothed TD Error")

    a5.plot(x, e, label="Epsilon")

    plt.xlabel("Episode")
    fig.suptitle("Averages")

    a1.legend()
    a2.legend()
    a3.legend()
    a4.legend()
    a5.legend()

    plt.tight_layout()

ani = FuncAnimation(plt.gcf(), animate, interval=1000)

plt.tight_layout()
plt.show()
