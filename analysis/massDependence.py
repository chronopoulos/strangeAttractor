#!/usr/bin/python

import numpy as np
import matplotlib.pyplot as plt

alpha = np.linspace(-0.5, 0.5, 100)

mu1s = np.linspace(0., 3., 4)
mu2s = np.linspace(0., 3., 4)

def coeff(alpha, mu1, mu2):
    """
    alpha is array
    mu1, mu2 are scalars
    """
    cL = alpha + mu1*(0.5+alpha) - mu2*(0.5-alpha)
    cR = 1./12 + alpha**2 + mu1*(0.5+alpha)**2 + mu2*(0.5-alpha)**2
    return cL/cR

#fig = plt.Figure()
i = 0
for mu1 in mu1s:
    for mu2 in mu2s:
        i += 1
        plt.subplot(4,4,i)
        c = coeff(alpha, mu1, mu2)
        plt.plot(alpha, c)
        plt.axhline(y=0, color='k')
        plt.ylim([-2,2])
        plt.title('mu1=%.1f, mu2=%.1f' % (mu1, mu2))

plt.show()
