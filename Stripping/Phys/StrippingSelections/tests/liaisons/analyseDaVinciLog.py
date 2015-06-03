__author__ = 'Greig Cowan'

from optparse import OptionParser
import re
import matplotlib.pyplot as plt

parser = OptionParser()
parser.add_option("-p", "--plot", dest="filename", default="plot.png",
                  help="write report to FILE")
parser.add_option("-c", "--cands", action="store_true", dest="candidates", default=False,
                  help="Make a plot of candidates")
parser.add_option("-l", "--lines", action="store_true", dest="lines", default=False,
                  help="Make a plot of number of lines")
(options, args) = parser.parse_args()

memory = re.compile('(.*)(INFO Memory has changed from )([0-9]*)( to )([0-9]*)')
event = re.compile('(.*)(SUCCESS Reading Event record )([0-9]*)([0-9]*)')
cands = re.compile('(.*)(# selected)([0-9]*)')
ncands = re.compile('(.*)([0-9]*)([0-9]*)([0-9]*)([0-9]*)([0-9]*)')
lines = re.compile('(.*)(ADDING)(.*)')
algos = re.compile('([0-9])(.*)(CombineParticles )')
algosf = re.compile('([0-9])(.*)(FilterDesktop )')

def getPoints(filename):
	totcands = 0
	lastMem = None
	f = open(filename)
	l = f.readlines()
	x = []
	y = []
	print filename
	for line in l:
		resMem = memory.match(line)
		resEv = event.match(line)
		lcands = cands.match(line)
		if resEv:
			lastEv = resEv
#			print 'EV:', lastEv.group(3)
		if resMem:
			print 'MEM:', resMem.group(5), lastEv.group(3)
#			print resMem.group(5), lastEv.group(3)
			if lastMem:
				x.append( int(lastEv.group(3))-1 )
				y.append( float(lastMem.group(5))/1000/1000 )
			else : # first time
				x.append(1)
				y.append( float(resMem.group(5))/1000/1000 )
			x.append( int(lastEv.group(3)) )
			y.append( float(resMem.group(5))/1000/1000 )
#			print x[-1], y[-1]
			lastMem = resMem
		if (lcands):
#			print line[120:130]
			nc = int(float((line[120:130]).replace(' ','')))
			totcands += nc

	x.append( int(lastEv.group(3)))
	y.append( float(lastMem.group(5))/1000/1000 )
	print filename, totcands, float(lastMem.group(5))/1000/1000
	print len(x), len(y)
	return x, y, totcands

def getLines(filename):
	f = open(filename)
	l = f.readlines()
	nl = 0
	nc = 0
	nf = 0
	for line in l:
		if lines.match(line): nl+=1
		if algos.match(line): 
#			print line
			nc+=1
		if algosf.match(line): nf+=1
	print filename, 'has', nl, 'lines,', nc, 'combines,', nf, 'filters'
	return nl, nc, nf

tms = []
tcs = []
tnl = []
tnc = []
tnf = []
#if options.candidates: plt.subplot(121) 
for log in args:
	x, y, tc = getPoints( log )
	tms.append(y[-1])
	tcs.append(tc)
#	print x, y
	plt.plot(x, y, 'o-')
	if options.lines: 
		nl, nc, nf = getLines( log )
		tnl.append(nl)
		tnc.append(nc)
		tnf.append(nf)
		
plt.ylabel('Memory (GB)')
plt.xlabel('Event number')
plt.savefig(options.filename)

if options.candidates: 
#	plt.subplot(122)
	plt.figure()
	plt.plot(tcs,tms,'o')
	plt.ylabel('Memory (GB)')
	plt.xlabel('Candidates')
	plt.savefig(options.filename.replace(".png","_c.png"))

if options.lines: 
	plt.figure()
	plt.subplot(131)
	plt.plot(tnl,tms,'o')
	plt.ylabel('Memory (GB)')
	plt.xlabel('Lines')
	plt.subplot(132)
	plt.plot(tnc,tms,'o')
#	plt.ylabel('Memory (GB)')
	plt.xlabel('CombineParticles')
	plt.subplot(133)
	plt.plot(tnf,tms,'o')
#	plt.ylabel('Memory (GB)')
	plt.xlabel('FilterDesktops')
	plt.savefig(options.filename.replace(".png","_l.png"))
	
