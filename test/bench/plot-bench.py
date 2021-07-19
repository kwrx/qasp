#!/bin/env python
import matplotlib.pyplot as plt
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



    plt.plot(range(len(x)), x, 'r-', label='QASP')
    plt.plot(range(len(y)), y, 'y-', label='QASP (C)')
    plt.plot(range(len(z)), z, 'b-', label='QASP (A)')
    plt.plot(range(len(w)), w, 'g-', label='DEMACS')
    plt.legend(loc='upper left')
    plt.xlabel('number of instances')
    plt.ylabel('time (s)')
    plt.title('Benchmark')
    plt.grid(True)
    plt.show()


plot_file(input)
