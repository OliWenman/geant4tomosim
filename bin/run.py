# -*- coding: utf-8 -*-
#Used to find the .so file built using CMake
import sys
sys.path.insert(0, './../build/src')

import sim
import numpy as np

nDetY = 250
nDetZ = 210

TotalRotation = 180
nParticles = 10000000
NumberOfImages = 1

Sim = sim.PySim()
Sim.initialise(nDetY, nDetZ)
Sim.run(nParticles, NumberOfImages, TotalRotation)


