#!/usr/bin/python

import numpy as np
import matplotlib.pyplot as plt

alpha = np.linspace(-0.5, 0.5, 9)

mu1 = np.linspace(0., 4., 100)
mu2 = np.linspace(0., 4., 100)

mu1mu1 = np.outer(mu1, np.ones(len(mu2)))
mu2mu2 = np.outer(np.ones(len(mu1)), mu2)


def coeff(alpha, mu1, mu2):
    """
    alpha is scalar
    mu1, mu2 are 2d "crossed" arrays
    returns a 2d array
    """
    cL = alpha + mu1*(0.5+alpha) - mu2*(0.5-alpha)
    cR = 1./12 + alpha**2 + mu1*(0.5+alpha)**2 + mu2*(0.5-alpha)**2
    return cL/cR

for i in range(9):
    plt.subplot(3,3,i+1)
    c = coeff(alpha[i], mu1mu1, mu2mu2)
    axesImage = plt.imshow(c, aspect='auto', extent=[0,4,0,4], origin='lower')
    axesImage.set_clim(-1.6,1.6)
    plt.colorbar(axesImage, use_gridspec=True)
    # note that the xy ordering is somewhat counterintuitive
    plt.xlabel('mu2')
    plt.ylabel('mu1')
    plt.title('alpha = %.3f' % alpha[i])
    

plt.show()
