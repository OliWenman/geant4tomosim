# -*- coding: utf-8 -*-
#Used to find the .so file built using CMake
import sys
sys.path.insert(0, './../build/src')

from G4Units import *
import sim
import numpy as np

nDetY = 250
nDetZ = 210
DetectorDimensions = [0.1, 0.1, 0.1]*mm

NumberOfImages = 3
TotalRotation = 180*deg
nParticles = 10000000

Sim = sim.PySim()
Sim.initialise(nDetY, nDetZ)
Sim.run(nParticles, NumberOfImages, TotalRotation)


