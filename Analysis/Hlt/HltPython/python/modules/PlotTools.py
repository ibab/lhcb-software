###################################################################################
"""
 PlotTools - 	From disk to screen in 3 movements:
				
				1) Define your plotting style
				
				2) Produce histograms/graphs from a Tree file with class PlotVar/PlotVar2d
				
				3) Display them with the plot() method

				+ extra tools

	Usage example:
	
	# first load signal tree and set style
	fs=TFile('decay.root','READ')
	ts=fs.Get('t;1')
	ts.SetNameTitle('decay','B -> decay')
	lstyle(ts, 'black', 2)
	
	# then load mbias tree for same selection
	fb=TFile('mbias.root','READ')
	tb=fb.Get('t;1')
	tb.SetNameTitle('mbias','mbias')
	lstyle(tb, 'red', 2, 'dash3')

	# define variables to be studied
	l0hdec = 	PlotVar('l0hdec', 2, 0, 2)
	pt = 		PlotVar('pt', 40, 0, 15000, '>')
	ip = 		PlotVar('ip', 40, 0, 3, '<')
	dalitz = 	PlotVar2d('s13:s23', 0, 30, 0, 30)
	
	# set signal tree as active and create histograms/graphs
	# they will inherit the parent tree style
	setree(ts)
	
	hs1 = l0hdec()
	hs2 = pt('l0hdec == 1')
	hs3 = pt.eff()	
	hs4 = ip('l0hdec == 1')	
	hs5 = ip.eff()	
	gs1 = dalitz('l0hdec == 1')
	gs2 = dalitz('l0hdec == 1 && pt > 1000')
	gs3 = dalitz('l0hdec == 1 && ip > .1')
	
	# set mbias tree as active
	setree(tb)
	
	hb1 = l0hdec()
	hb2 = pt('l0hdec == 1')
	hb3 = pt.eff()	
	hb4 = ip('l0hdec == 1')	
	hb5 = ip.eff()	
	
	# make a efficiency x retention graph
	effret = hs2g(hs3, hb3 )
	
	# create a legend and link to histograms 
	leg = getleg([ts, tb]) # will use tree title
	leg2objs(leg, [hs2, hs3, hs4, hs5])
	
	# create canvas, displaying them on screen
	# log scale, grids, drawing options and custom pad division can be set with extra options to pad
	# its possible to display signal and mbias histos in the same pad
	# plot will display legends associated to an object with leg2obj() if option leg is passed
	canv1 = plot( [hs1, hs2, hs3, hs4, hs5], 'c:2-3, 1:gx, 2:ly, 5:gx, 5:gy' )
	canv2 = plot( [gs1, gs2, gs3], '2:LEGO, 3:BOX' )
	canv3 = plot( [[hs2, hb2], [hs3, hb3], [hs4, hb4], [hs5, hb5], effret ], 'leg' )
	
	# write canvas to a single ps file
	writeplots([can1, canv2, canv3], 'file.ps')


Gabriel Guerrer - 15/08/07
"""
###################################################################################	

from ROOT import TCanvas, TLegend, THStack
from ROOT import TGraph, TH1I, TH1F, TH1D
from ROOT import TTree, TCut
from ROOT import gROOT, gStyle, gDirectory
from array import array

#---------------------------------------------------
# Dictionaries
#---------------------------------------------------

COLOR =			{					#http://root.cern.ch/root/htmldoc/TAttFill.html
				'white'		:0, 
				'black'		:1, 
				'gray'		:15,
				'graylight'	:16,
				'graydark'	:12,
				'brown'		:28, 			
				'brownlight':43, 			
				'red'		:2, 
				'redbrown'	:46, 
				'blue'		:4, 
				'bluelight'	:7, 
				'bluenice'	:38, 
				'green'		:8,
				'greenlemon':3,
				'greenlight':30,
				'yellow'	:5, 
				'yellowdark':41, 
				'pink'		:6, 
				'purple'	:9,
				'purplelight':40
				}

LINESTYLE = 	{					#http://root.cern.ch/root/htmldoc/TAttLine.html
				'solid':1,   	# ---------
				'dash1':2, 		# - - - - -
				'dash2':7,		# -- -- --
				'dash3':9,		# ---- ----
				'dot-dot':3, 	# .........
				'dash-dot1':4,	# -.-.-.-.-
				'dash-dot2':5,	# --.--.--.
				'dash-dot3':6,	# --...--...				
				'dash-dot4':8,	# --..--..				
				'dash-dot5':10	# ---- . ----
				}

FILLSTYLE = 	{ 					#http://root.cern.ch/root/htmldoc/TAttFill.html
				'hollow':0,
				'solid':1001, 
				'hatch':2001, 
				'a':3001, 
				'b':3002, 
				'c':3003, 
				'd':3004, 
				'e':3005, 
				'f':3006, 
				'g':3007
				}

MARKER = 		{					#http://root.cern.ch/root/htmldoc/TAttMarker.html
				'.':1,
				'+':2,
				'*':3,
				'o':4,
				'x':5,
				'smallcircle':6,
				'f_circle':20,	# f_ for full
				'f_square':21,
				'f_triup':22,
				'f_tridown':23,
				'f_star':30,
				'o_circle':24,	# o_ for open
				'o_square':25,
				'o_triup':26,
				'o_star':29,
				'o_diamond':27,
				'o_cross':28
				}

CANVASDIV = 	{					# n:(x,y) - n(total number of pads), x(pads in x), y(pads in y)
				1:(1,1), 2:(2,1), 3:(2,2), 4:(2,2),  
				5:(3,2), 6:(3,2), 7:(3,3), 8:(3,3), 
				9:(3,3), 10:(4,3), 11:(4,3), 12:(4,3), 
				13:(4,4), 14:(4,4), 15:(4,4), 16:(4,4)	
				}

#---------------------------------------------------
# Style 
#
# http://root.cern.ch/root/htmldoc/TStyle.html
#---------------------------------------------------

# histogram style, can be set on the run with lstyle() and fstyle() methods
gStyle.SetHistLineWidth(2)
gStyle.SetHistLineColor(COLOR['black'])
gStyle.SetHistLineStyle(LINESTYLE['solid'])
gStyle.SetHistFillStyle(FILLSTYLE['hollow'])
gStyle.SetHistFillColor(COLOR['white'])
gStyle.SetHistMinimumZero(1) # y minimum always 0

# stats style
gStyle.SetStatFontSize(.07)
gStyle.SetStatTextColor(COLOR['black'])
gStyle.SetStatColor(COLOR['white']) 
gStyle.SetStatX(.97) # end position x
gStyle.SetStatY(.97) # end position y
gStyle.SetStatW(.2)
gStyle.SetStatH(.1)
gStyle.SetStatBorderSize(1)
gStyle.SetOptStat(111111) # to display under/overflows

# title style
gStyle.SetTitleFontSize(.08)
gStyle.SetTitleTextColor(COLOR['black'])
gStyle.SetTitleFillColor(COLOR['white'])
gStyle.SetTitleBorderSize(1)
gStyle.SetTitleX(.1) # beginning position
gStyle.SetTitleY(.99) # end position
gStyle.SetTitleXOffset(1.05)
gStyle.SetTitleYOffset(1.1)

# frame style
gStyle.SetFrameBorderMode(0) # to remove 3D borders
gStyle.SetFrameFillColor(COLOR['white']) 

# canvas style
gStyle.SetCanvasBorderMode(0)
gStyle.SetCanvasColor(COLOR['white'])

# pad style
gStyle.SetPadBorderMode(0)
gStyle.SetPadColor(COLOR['white'])

# marker style
gStyle.SetMarkerSize(1.8)
gStyle.SetMarkerStyle(MARKER['smallcircle'])

# legend style
gStyle.SetLegendBorderSize(1)

# methods
def lstyle(obj, lcolor, lwidth=1, lstyle=LINESTYLE['solid']):
	"""
	Set line style, where obj = TH*, TTree, TGraph. 
	
	Tree style is inherited by histograms produced with geth(), fillh() and PlotVar methods.
	
	Color and style may be passed as int or str (to be converted using the respective dictionary).
	"""
	
	obj.SetLineWidth(lwidth)
	if isinstance(lcolor, int): 	obj.SetLineColor(lcolor)
	elif isinstance(lcolor, str): 	obj.SetLineColor(COLOR[lcolor])	
	if isinstance(lstyle, int):		obj.SetLineStyle(lstyle)
	elif isinstance(lstyle, str):	obj.SetLineStyle(LINESTYLE[lstyle])
	
#---------------------------------------------------
def fstyle(obj, fcolor, fstyle=FILLSTYLE['solid']):	
	"""
	Set fill style, where obj = TH*, TTree, TGraph. 
	
	Tree style is inherited by histograms produced with geth(), fillh() and PlotVar methods.
	
	Color and style may be passed as int or str (to be converted using the respective dictionary).
	"""
	
	if isinstance(fcolor, int): 	obj.SetFillColor(fcolor)
	elif isinstance(fcolor, str): 	obj.SetFillColor(COLOR[fcolor])	
	if isinstance(fstyle, int):		obj.SetFillStyle(fstyle)
	elif isinstance(fstyle, str):	obj.SetFillStyle(LINESTYLE[fstyle])

#---------------------------------------------------
# PlotVar
#---------------------------------------------------

# global instance which holds the active tree information
ACTIVETREE = None 

#---------------------------------------------------
def setree(tree):
	"""
	Sets the current Tree that will create PlotVar histos/graphs.
	Must be set before any PlotVar call. 
	"""
	
	global ACTIVETREE
	ACTIVETREE = tree

#---------------------------------------------------	
def checktree(tree):
	"""
	Checks if any tree was set with setree().
	"""

	if isinstance(tree, TTree):
		return True
	else:
		print 'PlotTools ERROR : You must define a tree with setree(tree)'
		return False

#---------------------------------------------------
class PlotVar:
	"""
	Produce 1d histograms from a Tree branch. Conditions/cuts may be applied.
	The correspondent Tree must be set wit setree() before any call.
	
	The histogram is created with the () operator. 
	It's also possible to obtain an efficiency histogram with eff() method.
	 
	Deals with under/overflows.
	"""
	
	name = ''
	nbins = 40
	lo = 0
	hi = 100
	op = '<'
	condition = ''
	cutat = 0
	
	histos = [] # parent for produced histograms
	
	#---------------------------------------------------	
	def __init__(self, name, nbins, lo, hi, op='>', condition='', cutat=0):
		""" 
		Create a PlotVar instance.
		
		name		- Tree branch name to be plotted
		npoints		- number of bins
		lo, hi		- range
		op			- '>' or '<', used in eff() method
		condition	- optional, Tree cut condition
		cutat 		- optional, later drawing methods may place a market at cutat value
		"""
		
		# initialize members
		self.name = name
		self.nbins = nbins
		self.lo = lo
		self.hi = hi
		self.op = op
		self.condition = condition
		self.cutat = cutat
		
	#---------------------------------------------------
	def __call__(self, cut='', opt=''):		
		"""
		Return an histogram from the active tree with cuts and options.
		The histogram style is inherited from the active tree.
		"""
		
		t = ACTIVETREE
		if checktree(t):
			# create histogram
			his = TH1D( self.name, self.name, self.nbins , self.lo, self.hi )
			
			#define cuts
			cuts = sumcuts(cut, self.condition)
			
			# fill histogram
			fillh(t, his, self.name, cuts, opt)
			
			# add to parent
			self.histos.append( his )
			
			return his
	
	#---------------------------------------------------
	def eff(self, op=0, cut='', opt='', norm=0):
		"""
		Returns the efficiency histogram. For more details see effint() documentation.
		"""
		
		# if no specific op passed, use the default one
		if op == 0:
			op = self.op
		
		t = ACTIVETREE
		if checktree(t):
			#define cuts
			cuts = sumcuts(cut, self.condition)
			
			# create histogram		
			his = self(cuts, opt)
			
			# create eff histogram
			heff = effint(his, op, norm)
			
			# add to parent
			self.histos.append( heff )
			
			return heff

#---------------------------------------------------
class PlotVar2d:
	"""
	Produce histograms from 2d Tree branches. Conditions/cuts may be applied.
	The correspondent Tree must be set wit setree() before any call.
	
	The histogram is created with the () operator. 
	"""
	
	names = ''
	xlo = 0
	xhi = 100
	ylo = 0
	yhi = 100
	condition = ''
	
	histos = [] # parent for produced histos
	
	#---------------------------------------------------	
	def __init__(self, names, xlo, xhi, ylo, yhi, condition=''):
		""" 
		Create a PlotVar2d instance.
		
		names				- Tree branch names to be plotted, ex.: 'x:y'
		xlo, xhi, ylo, yhi 	- x,y ranges
		condition			- optional, Tree cut condition
		"""
		
		# initialize members
		self.names = names
		self.xlo = xlo
		self.xhi = xhi
		self.ylo = ylo
		self.yhi = yhi
		self.condition = condition
		
	#---------------------------------------------------
	def __call__(self, cut='', opt=''):		
		"""
		Return a histo from the active tree with cuts and options.
		The histo style is inherited from the active tree.
		"""
		
		t = ACTIVETREE
		if checktree(t):			
			#define cuts
			cuts = sumcuts(cut, self.condition)
			
			# get histo
			h = geth(t, self.names, cuts, opt)
			
			# add to parent
			self.histos.append( h )
			
			return h

#---------------------------------------------------
# Tree utils
#---------------------------------------------------

def geth(t, var, cut='', opt=''):
	"""
	Return the histogram produced with t.Project(h,'var',cut, opt). 
	If var == 1d (2d) , type(h) == TH1D (TH2F).
	
	Histo name is set to var. Title is var + {cuts}. 
	X,Y labels are set with x,y var names.
	
	histo inherits t style.
	"""
	
	#get the histo
	t.Project('htemp', var, cut, opt)
	h = gDirectory.Get('htemp')			
	
	# set name, title
	if cut != '':
		cuttitle = '{'+ cut +'}'
	else:
		cuttitle = ''
	h.SetNameTitle(var, var + ' '+ cuttitle)		
	
	# set x,y labels
	vartitles = var.split(':')
	if len(vartitles) == 1:		
		yvartitle = ''
		xvartitle = vartitles[0]		
	else:
		yvartitle = vartitles[0] + '       '
		xvartitle = vartitles[1]
	h.SetXTitle( xvartitle )
	h.SetYTitle( yvartitle )		
	
	# h will inherit t style
	lstyle(h, t.GetLineColor(), t.GetLineWidth(), t.GetLineStyle())		
	fstyle(h, t.GetFillColor(), t.GetFillStyle())	
	
	return h
#---------------------------------------------------
def fillh(t, h, var, cut='', opt=''):
	"""
	Fill h histogram with t.Draw('var',cut, opt). 
	
	h name is set to var. Title is var + {cuts}. 
	X,Y labels are set with x,y var names.
	
	h inherits t style.
	"""
	
	# fill the histogram
	t.Project(h.GetName(), var, cut, opt)	
	
	# set name, title
	if cut != '':
		cuttitle = '{'+ cut +'}'
	else:
		cuttitle = ''
	h.SetNameTitle(var, var + ' '+ cuttitle)		
	
	# set x,y labels
	vartitles = var.split(':')
	if len(vartitles) == 1:		
		yvartitle = ''
		xvartitle = vartitles[0]		
	else:
		yvartitle = vartitles[0] + '       '
		xvartitle = vartitles[1]
	h.SetXTitle( xvartitle )
	h.SetYTitle( yvartitle )	
	
	# h will inherit t style
	lstyle(h, t.GetLineColor(), t.GetLineWidth(), t.GetLineStyle())		
	fstyle(h, t.GetFillColor(), t.GetFillStyle())

#---------------------------------------------------
# Histo utils
#---------------------------------------------------

def effint(h, op, norm=0):	
	"""
	Returns the efficiency histogram, which is calculated in h bins, counting events forward(op='>') or backward(op='<').	
	By default the normalization is the total number of events (taking into account under/overflows).
	But it's also possible to force a normalization with norm parameter.
	
	Name(Title) is set to h name + '_eff' (name + ' eff ' + op).
	
	heff inherits h style.
	"""
	
	# get heff and set name
	heff= h.Clone()	
	name = h.GetName() 
	heff.SetNameTitle(name+'_eff', name+' eff '+op)

	# eval number of events
	nbins = h.GetNbinsX()
	overflow = h.GetBinContent(nbins+1)
	underflow = h.GetBinContent(0) 
	neven = h.Integral() + overflow + underflow
	
	#if no forced normalization is passed, the use total number of events
	if norm == 0:
		norm = neven

	# fill bins depending on op
	for i in range( nbins ):
		i+=1 # 0 is the underflow
		if op == '>':
			heff.SetBinContent(i, heff.Integral(i, nbins + 1) / norm )	
		elif op == '<':
			heff.SetBinContent(i, neven/norm - (heff.Integral(i, nbins + 1) / norm) )	
		else:
			print 'PlotTools ERROR : effint() - operation '+op+' inexistent'
			return 0

	# fix number of events
	heff.SetEntries( h.GetEntries() )		
	
	# heff inherits h style
	lstyle(heff, h.GetLineColor(), h.GetLineWidth(), h.GetLineStyle())		
	fstyle(heff, h.GetFillColor(), h.GetFillStyle())
	
	return heff

#---------------------------------------------------
def hs2g(h1, h2):
	"""
	Takes two histograms defined in the same range and with same binning to produce a graph.
	Each bin is translated in a point (x,y), where x = h1.GetBinContent(bin) and
	y = h2.GetBinContent(bin).
	
	Example of usage: create efficiency x retention graphs.
	"""
	
	# checks for same range and binning
	nbins1 = h1.GetNbinsX()
	nbins2 = h2.GetNbinsX()	
	lo1 = h1.GetXaxis().GetXmin()
	hi1 = h1.GetXaxis().GetXmax()
	lo2 = h2.GetXaxis().GetXmin()
	hi2 = h2.GetXaxis().GetXmax()	
	if nbins1 != nbins2:
		print 'PlotTools ERROR : hs2g() - h1 and h2 have different number of bins'
		return 0
	if lo1 != lo2 or hi1 != hi2:
		print 'PlotTools ERROR : hs2g() - h1 and h2 have different ranges'
		return 0	
	
	# fill arrays and create graph
	y1 = array('f', nbins1*[0.] )
	for i in range(nbins1):
		y1[i]= h1.GetBinContent(i+1)		
	y2 = array('f', nbins2*[0.] )
	for i in range(nbins2):
		y2[i]= h2.GetBinContent(i+1)		
	g = TGraph(nbins1, y1, y2)
	
	# set title and x,y labels
	title1 = h1.GetTitle()
	title2 = h2.GetTitle()	
	g.SetTitle(title2 + ' vs ' + title1)	
	g.GetXaxis().SetTitle( h1.GetYaxis().GetTitle() )
	g.GetYaxis().SetTitle( h2.GetYaxis().GetTitle() )
	
	# g inherits h1 style
	lstyle(g, h1.GetLineColor(), h1.GetLineWidth(), h1.GetLineStyle())
	fstyle(g, h1.GetFillColor(), h1.GetFillStyle())	
	
	return g

#---------------------------------------------------
# Generic utils
#---------------------------------------------------

def sumcuts(a, b):
	"""
	Sum cuts a and b, placing && when a or b are not empty strings.
	"""
	cuts = TCut(a)
	cuts += TCut(b)
	return cuts.GetTitle()

#---------------------------------------------------
def closeall():
	"""
	Close all open canvas.
	"""
	cs = gROOT.GetListOfCanvases()
	for c in cs:
		c.Close()

#---------------------------------------------------
def writeplots(canvs, outputfile): 	
	"""
	Write a list of canvas in PS format to outupfile.
	"""
	
	canvs[0].Print(outputfile+'[')
	for canv in canvs:		
		canv.Print(outputfile)
	canvs[0].Print(outputfile+']')

#---------------------------------------------------

# global instance that holds the legend links
LEGENDICT = {} 

def leg2objs(leg, objs):
	"""
	Link legend to list of objects. plot() method will draw linked legends with extraopt 'leg'.
	"""
	
	global LEGENDICT
	for obj in objs:
		LEGENDICT[obj] = leg
	
#---------------------------------------------------
def getleg( trees, drawopt='l', fillcolor=COLOR['white']):
	"""
	Produces a legend given a list of trees. Uses tree title as legend string. 
	
	Possible drawopt: 'l' - line, 'p' - marker, 'f' - filled box, 'ep' - marker with vertical error bar
	"""
	l = TLegend(.65,.45,.98,.68)
	l.SetFillColor(fillcolor)		
	for t in trees:
		l.AddEntry(t, t.GetTitle(), drawopt)
	return l

#---------------------------------------------------
def plot(objs, extraopts=''):
	"""
	Display objs on screen, returning the correspondent canvas.
	Canvas division is automatically done using the CANVASDIV dictionary.
	
	objs - 	list of histos/graphs, ex.: [h1, h2, g1, g2] (if h1==0, place an empty pad)
			or list of lists, which will be drawn in the same canvas.
			ex.: [[h1, h2], h3] or [[g1, g2], h1]
			
	extraopts - 'c: 1-2' force canvas division
				'i: lx(ly)' log scale in x(y) in pad i
				'i: gx(gy)' grid in x(y) in pad i
 				'i: LEGO' drawopts in pad i
				'leg': draw legends liked with leg2objs()
	"""

	#---------------------------------------------------
	def processopts(extraopts, npads):
		"""
		Process plot extra options.
		"""	
		
		canvasdiv = CANVASDIV[npads]
		drawleg = 0	
		padopts = (npads+1) * ['']
		drawopts = (npads+1) * ['']			
		
		if extraopts != '':
			opts = extraopts.split(',')
			for opt in opts:

				# should draw legends?
				if opt.find('leg') != -1:
					drawleg = 1
					continue

				optdiv = opt.split(':')
				optl = optdiv[0]
				optr = optdiv[1]

				# force canvas division
				if optl == 'c':
					optrdiv = optr.split('-')
					canvasdiv = (int(optrdiv[0]), int(optrdiv[1]))

				# padopts
				elif optr == 'lx' or optr == 'ly' or optr == 'gx' or optr == 'gy':
		 			padopts[int(optl)]+= optr

				# drawopts
				else:
					drawopts[int(optl)]= optr	
				
		return canvasdiv, drawleg, padopts, drawopts		
		
	#---------------------------------------------------	
	def applypadopts(pad, padopts):
		"""
		Apply pad options.
		"""
		
		if padopts.find('lx') != -1:
			pad.SetLogx()
		if padopts.find('ly') != -1:
			pad.SetLogy()
		if padopts.find('gx') != -1:
			pad.SetGridx()
		if padopts.find('gy') != -1:
			pad.SetGridy()
			
	#---------------------------------------------------	

	# find number of pads and process extra options
	npads = len(objs)	
	canvasdiv, drawleg, padopts, drawopts = processopts(extraopts, npads)

	# create canvas and divide it
	c = TCanvas()
	c.SetWindowPosition(10,10)
	c.SetWindowSize(1240,920)	
	divx, divy = canvasdiv
	c.Divide( divx, divy )	
	c.STACKS = [] # stacks holder	
			
	# loop over objects, i is the pad number
	i=1
	for obj in objs:
	
		# aplly padopts to pad
		pad = c.cd(i)
		applypadopts(pad, padopts[i])
				
		# if 0, jump to next pad
		if obj == 0:
			i+=1
			continue

		# obj == histo
		if str(type(obj)).find('TH') != -1:
			obj.Draw( drawopts[i] )
			# if 'leg' opt, find and draw legend
			if drawleg:
				try:
					leg = LEGENDICT[obj]
					leg.Draw()
				except KeyError: # obj has no leg associated
					pass


		# obj == hprofile
		if str(type(obj)).find('TP') != -1:
			obj.Draw( drawopts[i] )
			# if 'leg' opt, find and draw legend
			if drawleg:
				try:
					leg = LEGENDICT[obj]
					leg.Draw()
				except KeyError: # obj has no leg associated
					pass
		
		# obj == grapg	
		elif type(obj) == TGraph:
			if drawopts[i] == '':
				obj.Draw('AL')			
			else:
				obj.Draw( drawopts[i] )
				
		# obj == []
		elif type(obj) == list:
		
			# obj == [[graph]]
			if type(obj[0]) == TGraph:
				obj[0].Draw('AL')
				
				objrange = range(len(obj))
				for j in objrange[1:]:
					obj[j].Draw()
											
			# obj == [[histo]]
			else:
		
				# histograms on same pad
				stack = THStack()
				for o in obj:					
					stack.Add(o)
				stack.Draw("nostack") # don't sum them
				
				# inherits obj[0] labels
				stack.GetXaxis().SetTitle(obj[0].GetXaxis().GetTitle())
				stack.GetYaxis().SetTitle(obj[0].GetYaxis().GetTitle())
				#stack.SetTitle(obj[0].GetTitle())
				
				# if 'leg' opt, find and draw legend
				if drawleg:
					try:
						leg = LEGENDICT[obj[0]]
						leg.Draw()
					except KeyError: # obj has no leg associated
						pass
	
				# append stack in canvas, otherwise deleted				
				c.STACKS.append(stack)	
				
		i+=1
	return c
