# -*- coding: utf-8 -*-
#Used to find the .so file built using CMake
import sys
sys.path.insert(0, './../build/src')

import sim
import numpy as np

Sim = sim.PySim()
Sim.initialize();
Sim.run();

#Sim.kill()


