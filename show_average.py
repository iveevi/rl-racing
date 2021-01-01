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

plt.style.use('fivethirtyeight')

def animate(k):
    plt.cla()

    fpath = "results/" + directory + "/main"

    data = pd.read_csv(fpath)

    x = data['episode']
    y = data['average']

    if len(y) <= 0:
        return

    mx = max(y)
    mn = min(y)
    av = mean(y)

    plt.plot(x[-8:], y[-8:], label="Mean Reward")
    plt.plot(x[-8:], [mx] * min(8, len(y)), label="Maximum of Mean")
    plt.plot(x[-8:], [mn] * min(8, len(y)), label="Minimum of Mean")
    plt.plot(x[-8:], [av] * min(8, len(y)), label="Average of Mean")

    plt.xlabel("Episode")
    plt.title("Average Rewards")

    plt.tight_layout()

ani = FuncAnimation(plt.gcf(), animate, interval=1000)

plt.tight_layout()
plt.show()
