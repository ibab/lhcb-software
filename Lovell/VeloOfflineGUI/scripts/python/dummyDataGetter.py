import sys
if (len(sys.argv) > 2):
	runNum = int(sys.argv[1])
	command = sys.argv[2]
	moduleNumber = int(sys.argv[3])

else: command = sys.argv[1]

if command == 'testPlotMethod':
	sys.stdout.write('1 ') # Decoder ID (1 for 1D histograms)
	sys.stdout.write('retrievedTest ') # Title of the graph.
	nPoints = 100
	sys.stdout.write(str(nPoints) + ' ') # Number of bins.

	# Xs - left hand bin sides (very general case).
	for i in range(nPoints): sys.stdout.write(str(i-nPoints/2) + " ") 

	# Corresponding ys (parabola in this case).
	for i in range(nPoints): sys.stdout.write(str((i-nPoints/2)**2 + 1000*moduleNumber) + " ") 
		

	# Statistics in order: nEntries, mean, RMS, underflow, overflow.
	sys.stdout.write(str(nPoints) + ' ') 
	sys.stdout.write('0 ')
	sys.stdout.write('0.0 ')
	sys.stdout.write('20.0 ')
	sys.stdout.write('0 ')
	sys.stdout.write('0 ')
	sys.stdout.write('xAxis ')
	sys.stdout.write('yAxis ')

elif command == 'testPlotMethod2D':
	sys.stdout.write('2 ') # Decoder ID (2 for 2d histograms)
	sys.stdout.write('retrieved2DTest ') # Title.
	nPointsX = 10
	nPointsY = 5
	sys.stdout.write(str(nPointsX) + ' ')
	sys.stdout.write(str(nPointsY) + ' ')
	for i in range(nPointsX):
		sys.stdout.write(str(i) + " ")

	for i in range(nPointsY):
		sys.stdout.write(str(i) + " ")

	# Bin heights, going across columns of first row, then next row...
	for i in range(nPointsX*nPointsY+1): 
		z = i
		sys.stdout.write(str(z) + " ")


	# Statistics in order: nEntries, mean, RMS, underflowX (integrated over underflowY), 
	# overflowX (integrated over overflowY), underflowY (integrated over underflowX),
	# overflowY (integrated over overflowX).
	sys.stdout.write(str(nPointsX*nPointsY) + ' ')
	sys.stdout.write('0 ')
	sys.stdout.write('0.0 ')
	sys.stdout.write('20.0 ')
	sys.stdout.write('0 ')
	sys.stdout.write('0 ')
	sys.stdout.write('0 ')
	sys.stdout.write('0 ')
	sys.stdout.write('xAxis ')
	sys.stdout.write('yAxis ')
	sys.stdout.write('N ')


elif command == 'run_view_config':
	sys.stdout.write('Tab Tab1 Top \n')
	sys.stdout.write('Tab Tab2 Top \n')
	sys.stdout.write('Tab Tab3 Tab1 \n')
	sys.stdout.write('Tab Tab4 Tab1 \n')
	sys.stdout.write('Plot testPlot1 Tab3 testPlotMethod multipleModules \n')
	sys.stdout.write('Plot testPlot2 Tab3 testPlotMethod singleModule \n')
	sys.stdout.write('Plot testPlot3 Tab3 testPlotMethod2D singleModule \n')
	sys.stdout.write('Plot testPlot4 Tab3 testPlotMethod2D singleModule \n')
	sys.stdout.write('Plot testPlot Tab4 testPlotMethod multipleModules \n')

elif command == 'run_list':
	sys.stdout.write('123\n')
	sys.stdout.write('101\n')
	sys.stdout.write('100\n')
