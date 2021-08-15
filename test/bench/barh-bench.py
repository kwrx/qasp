#!/bin/env python
import matplotlib.pyplot as plt
import pandas as pd
import numpy as np
import scipy as sp

tests = [ 'Naïve', 'Controesempio', 'Anticipazioni']
times = [ 6.3, 2.0, 6.4 ]
#times = [ 173.7, 176.8, 180.2 ]
error = [ 2.1, 1.1, 2.1 ]
#error = [ 11.5, 12.4, 13.9 ]
colors = [ '#FFA056', '#0B84A5', '#CA472F' ]


plt.figure(figsize=(11.69, 2.5), dpi=300)
plt.barh(tests, times, xerr=error, align='center', color=colors)
plt.grid(True, linestyle='--', which='major', color='gray', alpha=.25)
plt.box(False)
plt.yticks(tests, fontsize=16)
plt.gca().invert_yaxis()
plt.tight_layout()
plt.savefig('./bench-sample-p1.png')