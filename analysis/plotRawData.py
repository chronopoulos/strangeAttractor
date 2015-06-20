#!/usr/bin/python

import sys
import numpy as np
import matplotlib.pyplot as plt

if len(sys.argv) < 2:
    print 'Usage: ./plotSwing.py <filename>'
    sys.exit(1)

filename = sys.argv[1]

data = np.loadtxt(filename, comments=';')

for i in range(8):
    plt.subplot(8,1,i+1)
    plt.plot(data[:,i])
    #plt.ylim([0,1024])

plt.show()
