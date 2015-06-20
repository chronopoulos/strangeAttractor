#!/usr/bin/python -i

import sys
import numpy as np
import matplotlib.pyplot as plt
from sklearn.decomposition import PCA

if len(sys.argv) < 3:
    print 'Usage: ./plotSwing.py <rawDataFile> <pcFile>'
    sys.exit(1)

infile = sys.argv[1]
outfile = sys.argv[2]

data = np.loadtxt(infile, comments=';')

pca = PCA(n_components=4)
print 'fitting..'
pca.fit(data)
print '..done!'


# print components out to pcFile
with open(outfile, 'w') as f:
    for i in range(4):
        for j in range(8):
            f.write(str(pca.components_[i,j]))
            f.write(' ')
        f.write('\n')

