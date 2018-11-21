# -*- coding: utf-8 -*-
#Used to find the .so file built using CMake
import sys
sys.path.insert(0, './../src')

import sim
import numpy as np
from matplotlib import pyplot as plt

Sim = sim.PySim()
Sim.initialize();

imagestack = []

for i in range(1):
    Sim.run();
    Image = Sim.lastImage();
    imagestack.append(Image)

fig = plt.figure()
plt.imshow(Image, cmap="viridis")
plt.title('Last Projection')

#Add a color bar which maps values to colors.
plt.colorbar()

plt.show()


import h5py

h5file = h5py.File('/dls/tmp/test.h5', 'w')
dataset = h5file.create_dataset('data', data=np.dstack(imagestack)) # shape=(210,250,180)
# dataset[:,:,34] = Image[:,:]
h5file.close()



