#!/bin/env python
import matplotlib.pyplot as plt
import seaborn as sns
import numpy as np
import sys


input=sys.argv[1]

if not input:
    print("Usage: plot-bench.py <input>")
    sys.exit(1)


def parse_file(path):
    with open(path) as f:
        lines = f.readlines()
        lines = lines[2:]
        lines = lines[:-1]
        lines = [l.strip() for l in lines]
        lines = [l for l in lines if l]
        lines = [l.split('|') for l in lines]
        lines = [l[2:-1] for l in lines]
        lines = [(float(l[0]), float(l[1]), float(l[2]), float(l[3])) for l in lines]
        lines = [l for l in lines]# if l[0] < 600 and l[1] < 600 and l[2] < 600 and l[3] < 600]
        return lines


def plot_file(path):

    lines = parse_file(path)
    
    x = [l[0] for l in lines]
    y = [l[1] for l in lines]
    z = [l[2] for l in lines]
    w = [l[3] for l in lines]

    x = [i for i in x if i < 600]
    y = [i for i in y if i < 600]
    z = [i for i in z if i < 600]
    w = [i for i in w if i < 600]

    x = np.cumsum(sorted(x))
    y = np.cumsum(sorted(y))
    z = np.cumsum(sorted(z))
    w = np.cumsum(sorted(w))

    
    sns.set_theme(style="whitegrid")


    plt.plot(range(1, len(x) + 1), x, '#FFA056', label='QASP', linewidth=2, marker='s', markerfacecolor='#FFD489')
    plt.plot(range(1, len(y) + 1), y, '#0B84A5', label='QASP (C)', linewidth=2, marker='s', markerfacecolor='#3EA7D8')
    plt.plot(range(1, len(z) + 1), z, '#CA472F', label='QASP (A)', linewidth=2, marker='s', markerfacecolor='#FD7A5F')
    plt.plot(range(1, len(w) + 1), w, '#9DD866', label='DEMACS', linewidth=2, marker='s', markerfacecolor='#BFFA99')
    plt.legend(loc='best')
    plt.grid(linestyle='-', linewidth=0.5, axis='x')
    plt.axis(xmin=0, ymin=0, ymax=600)
    plt.box(False)
    #plt.show()

    f = plt.gcf()
    # A4
    f.set_size_inches([8.27, 11.69])
    f.savefig(path + '.png', dpi=300, bbox_inches='tight')


plot_file(input)
