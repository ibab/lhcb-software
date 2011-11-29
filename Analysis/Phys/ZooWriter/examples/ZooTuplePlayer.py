"""
@file ZooTuplePlayer.py

implement something similar to the Draw() method of TTree for Zoo tuples
to facilitate a quick look at a Zoo tuple

@author Manuel Schiller <manuel.schiller@nikhef.nl>
@date 2011-09-27

"""

import ZooPy
import ROOT

class ZooTuplePlayer:
    """
    @class ZooTuplePlayer
    
    provides functionality similar to TTree::Draw() in ROOT for Zoo tuples
    
    @author	Manuel Schiller <manuel.schiller@nikhef.nl>
    @date 	2011-11-24
    
    feel free to add missing functionality and fix bugs, but *please* keep the
    comments with the documentation up to date!
    """
    def __init__(self, tree):
	"""
	constructor

	parameters are:
	    tree        tree from which to read Zoo tuple (can also be a TChain)
	"""
	self.tree = tree
	self.histo = None

    def DrawZooP(self, collection, title, variables = [], cuts = [],
	    weight = lambda zp, zev: 1.0, maxevts = -1, startevent = 0,
	    drawopts = ''):
        """
        DrawZooP draws a ZooP collection from a ZooTuple tree
    
        parameters are:
    	collection      name of the particle collection to draw
    	variables       array of 1 to 3 functions or lambda expressions which
    			evaluate to the quantities to be drawn
    	cuts            list of zero or more functions or lambda expressions
    			which evaluate to True of False; a candidate is
    			accepted as passing all the cuts if all expressions
    			evaluate to True
    			(default: empty list means no cuts)
    	weight          a function or lambda expression which evaluates to the
    			weight of the candidate (default: 1.0)
    	maxevents       maximum number of events to process
    			(default: -1 meaning all events in the tree)
    	startevent	event from which to start plotting
    	drawopts	drawing options for histogram
    
	DrawZooP returns the histogram produced by the operation or None in
	case of failure.
    
        All functions/lambda expressions are passed the candidate ZooP as first
        argument and the Event as second argument. Lambda expressions are
        essentially "anonymous functions" which can be used to generate variables
        to plot and cuts "on the fly":
    
    	hist = DrawZooP('MyBs',
    		'B^{0}_{s} p vs p_{t};p [Gev];p_{T} [GeV]',
    			[ lambda zp,zev: zp.momentum().P() / 1e3,
    			  lambda zp,zev: zp.momentum().Pt() / 1e3 ],
    			[ lambda zp,zev: zp.momentum().Eta() >= 2. and
    				     zp.momentum().Eta() <= 5. ])
    
        This would read B_s candidates from the MyBs collection in the tree,
        producing a scatter plot of the B_s momentum versus B_s transverse
        momentum for B_s candidates which fall in the eta range from 2 to 5.
    
	Lambda expressions can be assigned to variables, e.g. to test them or
	to avoid having to type the same thing repeatedly. For the
	eta-criterion, one might do:
    
    	inEtaRange = lambda zp, zev: zp.momentum().Eta() >= 2. and
    					 zp.momentum().Eta() <= 5.
    
	inEtaRange can then be used like a normal function taking two
	arguments.  For more complex cuts or expressions to plot, an ordinary
	function might be easier to handle. lambda expressions are convenient
	for quick experiments, though.
        """
	# check dimensionality of plot - is it something we can handle
	ndim =  len(variables)
	if ndim < 1 or ndim > 3:
	    print 'DrawZooP: plots have to have between one and three dimensions!'
	    return None
	# book histogram
	if None != self.histo:
	    self.histo = None
	if 1 == ndim:
	    self.histo = \
		    ROOT.TH1D('DrawZooP_' + collection, title, 100, 0, 0)
	if 2 == ndim:
	    self.histo = \
		    ROOT.TH2D('DrawZooP_' + collection, title, 100, 0, 0, 100, 0, 0)
	if 3 == ndim:
	    self.histo = \
		    ROOT.TH3D('DrawZooP_' + collection, title, 100, 0, 0, 100, 0, 0, 100, 0, 0)
	# append weight to variable array
	variables.append(weight)
	# initialise branches
	zoops = ROOT.ZooObjRefArray(ROOT.ZooP)()
	zooev = ROOT.ZooEv()
	self.tree.SetBranchAddress(collection, ROOT.AddressOf(zoops))
	self.tree.SetBranchAddress("Event", ROOT.AddressOf(zooev))
	# prepare to loop over events in Zoo tuple
	nevents = self.tree.GetEntries()
	if startevent >= nevents:
	    print 'DrawZooP: no events left!'
	    return None
	if maxevts > 0 and startevent + maxevts < nevents:
	    nevents = startevent + maxevts
	self.tree.SetCacheLearnEntries(1000)
	for i in range(startevent, nevents):
	    # read next event
	    self.tree.LoadTree(i)
	    self.tree.GetEntry(i)
	    # loop over candidates in collection
	    for j in range(0, zoops.size()):
		zp = zoops[j]
		# work out if the candidate satisfies the cuts
		accept = True
		for cut in cuts:
		    if not cut(zp, zooev):
			accept = False
			break
		if not accept:
		    continue
		# yes, fill histogram
		self.histo.Fill(*[ f(zp, zooev) for f in variables ])
	# draw histogram
	self.histo.Draw(drawopts)
	# free branches
	self.tree.SetBranchAddress(collection, ROOT.MakeNullPointer())
	self.tree.SetBranchAddress("Event", ROOT.MakeNullPointer())
	# return histogram
	return self.histo

    def DrawZooMCP(self, title, variables = [], cuts = [], 
	    weight = lambda zp, zev: 1.0, maxevts = -1, startevent = 0,
	    drawopts = ''):
        """
        DrawZooMCP draws by looping over all ZooMCP from a ZooTuple tree
    
        parameters are:
    	variables       array of 1 to 3 functions or lambda expressions which
    			evaluate to the quantities to be drawn
    	cuts            list of zero or more functions or lambda expressions
    			which evaluate to True of False; a candidate is
    			accepted as passing all the cuts if all expressions
    			evaluate to True
    			(default: empty list means no cuts)
    	weight          a function or lambda expression which evaluates to the
    			weight of the candidate (default: 1.0)
    	maxevents       maximum number of events to process
    			(default: -1 meaning all events in the tree)
    	startevent	event from which to start plotting
    	drawopts	drawing options for histogram
    
	DrawZooMCP returns the histogram produced by the operation or None in
	case of failure.
    
	All functions/lambda expressions are passed the candidate ZooMCP as
	first argument and the Event as second argument. Lambda expressions
	are essentially "anonymous functions" which can be used to generate
	variables to plot and cuts "on the fly":
    
    	hist = DrawZooMCP('B^{0}_{s} p vs p_{t};p [Gev];p_{T} [GeV]',
    			[ lambda zp,zev: zp.momentum().P() / 1e3,
    			  lambda zp,zev: zp.momentum().Pt() / 1e3 ],
    			[ lambda zp,zev: zp.momentum().Eta() >= 2. and
    				     zp.momentum().Eta() <= 5. ])
    
	This would read B_s (MC) candidates from the MyBs collection in the
	tree, producing a scatter plot of the B_s momentum versus B_s
	transverse momentum for B_s candidates which fall in the eta range
	from 2 to 5.
    
	Lambda expressions can be assigned to variables, e.g. to test them or
	to avoid having to type the same thing repeatedly. For the
	eta-criterion, one might do:
    
    	inEtaRange = lambda zp, zev: zp.momentum().Eta() >= 2. and
    					 zp.momentum().Eta() <= 5.
    
	inEtaRange can then be used like a normal function taking two
	arguments. For more complex cuts or expressions to plot, an ordinary
	function might be easier to handle. lambda expressions are convenient
	for quick experiments, though.
        """
	# check dimensionality of plot - is it something we can handle
	ndim =  len(variables)
	if ndim < 1 or ndim > 3:
	    print 'DrawZooMCP: plots have to have between one and three dimensions!'
	    return None
	# book histogram
	if None != self.histo:
	    self.histo = None
	if 1 == ndim:
	    self.histo = \
		    ROOT.TH1D('DrawZooMCP', title, 100, 0, 0)
	if 2 == ndim:
	    self.histo = \
		    ROOT.TH2D('DrawZooMCP', title, 100, 0, 0, 100, 0, 0)
	if 3 == ndim:
	    self.histo = \
		    ROOT.TH3D('DrawZooMCP', title, 100, 0, 0, 100, 0, 0, 100, 0, 0)
	# append weight to variable array
	variables.append(weight)
	# initialise branches
	zooev = ROOT.ZooEv()
	self.tree.SetBranchAddress("Event", ROOT.AddressOf(zooev))
	# prepare to loop over events in Zoo tuple
	nevents = self.tree.GetEntries()
	if startevent >= nevents:
	    print 'DrawZooP: no events left!'
	    return None
	if maxevts > 0 and startevent + maxevts < nevents:
	    nevents = startevent + maxevts
	self.tree.SetCacheLearnEntries(1000)
	for i in range(startevent, nevents):
	    # read next event
	    self.tree.LoadTree(i)
	    self.tree.GetEntry(i)
	    # loop over list of ZooMCPs
	    zoomcps = zooev.GetMCP()
	    for j in range(0, zoomcps.size()):
		zmcp = zoomcps[j]
		# work out if the candidate satisfies the cuts
		accept = True
                for cut in cuts:
                    if not cut(zmcp, zooev):
                        accept = False
                        break
                if not accept:
                    continue
                # yes, fill histogram
                self.histo.Fill(*[ f(zmcp, zooev) for f in variables ])
        # draw histogram
        self.histo.Draw(drawopts)
        # free branches
        self.tree.SetBranchAddress("Event", ROOT.MakeNullPointer())
        # return histogram
        return self.histo

# vim: sw=4:tw=78:ft=python
