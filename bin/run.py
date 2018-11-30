# -*- coding: utf-8 -*-
#Used to find the .so file built using CMake
import sys
sys.path.insert(0, './../build/src')

from G4Units import *
import sim
import numpy as np
import matplotlib.pyplot as plt

nDetY = 25
nDetZ = 21
DetectorDimensions = [0.05, 0.05, 0.05]*mm

nBins = 2500
NumberOfImages = 1
TotalRotation = 180*deg
nParticles = 1000

Sim = sim.PySim()
Sim.initialise(nDetY, nDetZ, DetectorDimensions)

Sim.run(nParticles, NumberOfImages, TotalRotation, nBins)
x = Sim.lastEnergyBins()
y = Sim.lastEnergyFreq()

totalSum = 0

for value in range(nBins):
	totalSum = totalSum + y[value]

if totalSum > 0:

	#print "\nFluorescence values"
	#for element in range(nBins):
#
#		if y[element] > 0:
#			intensity = float(y[element])/totalSum*100
#			print round(x[element]*1000, 4), "keV, Relative intensity:", round(intensity, 2), "%"

	plt.plot(x*1000, y)
	plt.title("Fluorescence data")
	plt.ylabel("Number of photons")
	plt.xlabel("Energy of photons (keV)")
	plt.show()
else:
	print "\nNo fluorescence values detected"

