#import matplotlib.pyplot as plt
#from matplotlib import cm
#from matplotlib.ticker import LinearLocator, FormatStrFormatter

from scipy.ndimage import rotate
import numpy as np
import os
from matplotlib import pyplot as plt
plt.ion() # turn on interactive mode, non-blocking `show`

Filepath = './Data_Output/Text/HitData/'
onlyfiles = next(os.walk(Filepath))[2] #counts how many files there are

for Number in range(1,len(onlyfiles)+1):
      	
	Image = 'Image' + str(Number) + '.txt'
	LoadImage = Filepath + Image
	matrix = np.loadtxt(LoadImage, dtype=int)
	RotatedMatrix = rotate(matrix, 90)

	fig = plt.figure()
	plt.imshow(matrix, cmap="Greys")
	plt.title('Image ' +str(Number))

	# Add a color bar which maps values to colors.
	plt.colorbar()

	plt.show()
	
        Input = raw_input("Press [space] to continue. Press [enter] to quit. ") # wait for input from the user
	print Input

	plt.close()    # close the figure to show the next one.	

	if Input == '':
		break
