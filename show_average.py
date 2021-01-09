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

fig, (a1, a2, a3) = plt.subplots(3)

def animate(k):
    fpath = "results/" + directory + "/main"

    data = pd.read_csv(fpath)

    x = data['episode']
    y = data['average']
    e = data['epsilon']

    a1.cla()
    a2.cla()
    a3.cla()

    if len(y) <= 0:
        return

    mx = max(y)
    mn = min(y)
    av = mean(y)

    a1.plot(x, y, label="Average Reward")
    a1.plot(x, [mx] * len(y))
    a1.plot(x, [mn] * len(y))
    a1.plot(x, [av] * len(y))

    a2.plot(x, y.rolling(15, min_periods=1).mean(), label="Smoothed Average")
    a3.plot(x, e, label="Average Epsilon")

    plt.xlabel("Episode")
    fig.suptitle("Averages")

    a1.legend()
    a2.legend()
    a3.legend()

    plt.tight_layout()

ani = FuncAnimation(plt.gcf(), animate, interval=1000)

plt.tight_layout()
plt.show()
