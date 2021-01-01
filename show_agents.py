from itertools import count
from matplotlib.animation import FuncAnimation

import matplotlib.pyplot as plt
import pandas as pd
import random

import sys

args = sys.argv

print("arg = " + str(len(args)))

if len(args) != 3:
    print("Incorrect number of arguments, terminating...")

    sys.exit(-1)

print("dir = " + sys.argv[1])
print("#agents = " + sys.argv[2])

directory = sys.argv[1]
agents = int(sys.argv[2])

print("agents = " + str(agents))

plt.style.use('fivethirtyeight')

def animate(k):
    plt.cla()

    for i in range(0, agents):
        fpath = "results/" + directory + "/agent_" + str(i)

        # print("Opening file " + fpath)

        data = pd.read_csv(fpath)

        x = data['episode']
        y = data['reward']

        plt.plot(x[-8:], y[-8:])

    plt.xlabel("Episode")
    plt.ylabel("Reward")

    plt.title("Agent Rewards")

    plt.tight_layout()

ani = FuncAnimation(plt.gcf(), animate, interval=1000)

plt.tight_layout()
plt.show()
