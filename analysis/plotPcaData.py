#!/usr/bin/python

import sys
import numpy as np
import matplotlib.pyplot as plt

if len(sys.argv) < 2:
    print 'Usage: ./plotPcaData.py <filename>'
    sys.exit(1)

filename = sys.argv[1]

data = np.loadtxt(filename, comments=';')
print data.shape

for i in range(4):
    plt.subplot(4,1,i+1)
    plt.plot(data[:,i])
    #plt.ylim([0,1024])

plt.show()
