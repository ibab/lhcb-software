"""
@file ZooTuplePlayer.py

implement something similar to the Draw() method of TTree for Zoo tuples
to facilitate a quick look at a Zoo tuple

since the framework for plotting is rather useful, we use the same code which
checks if a candidate is accepted for plotting to implement a ZooTuple
stripping utility

@author Manuel Schiller <manuel.schiller@nikhef.nl>
@date 2011-09-27

"""

import ZooPy
import ROOT
import sys

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

    def runOverZooP(self, collection, cuts, maxevts, startevt,
	    candaction = None, evtaction = None):
	"""
	runOverZooP runs over a given collection of a ZooTuple, applying the
	given cuts to each of the candidates of the collection; for each
	accepted candidate, a user-defined action can be executed, and for
	each event with accepted candidates, a user-defined per-event action
	can be executed

	parameters are:
	collection	name of the collection to read candidates from
    	cuts            list of zero or more functions or lambda expressions
    			which evaluate to True of False; a candidate is
    			accepted as passing all the cuts if all expressions
    			evaluate to True
    	maxevents       maximum number of events to process
    	startevt	event from which to start plotting
	candaction	function or lambda expression to execute in case of an
			accepted candidate, or None if no action is to be
			executed for an accepted candidates
			(default: None)
	evtaction	function or lambda expression to be executed for an
			event with accepted candidates, or None if no action
			is to be executed for events with accepted candidates
			(default: None)
	
	The function or lambda expression passed for candaction is called with
	two parameters, the candidate and the Zoo event structure; the
	function or lambda expression passed for evtaction is called with a
	single parameter, the Zoo event structure.

	The list of cuts is defined an used analogously to that of the
	DrawZooP method, see the documentation there to get an idea of how
	this is supposed to work.
	"""
	zoops = ROOT.ZooObjRefArray(ROOT.ZooP)()
	zooev = ROOT.ZooEv()
	self.tree.SetBranchAddress(collection, ROOT.AddressOf(zoops))
	self.tree.SetBranchAddress("Event", ROOT.AddressOf(zooev))
	# prepare to loop over events in Zoo tuple
	nevents = self.tree.GetEntries()
	if startevt >= nevents:
	    print 'runOverZooP: no events left!'
	    return None
	if maxevts > 0 and startevt + maxevts < nevents:
	    nevents = startevt + maxevts
	self.tree.SetCacheLearnEntries(1000)
	nbytes = 0
	for i in range(startevt, nevents):
	    # read next event
	    self.tree.LoadTree(i)
	    nbytes += self.tree.GetEntry(i)
	    acceptevt = False
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
		# candidate accepted, event accepted
		acceptevt = True
		# perform per candidate action
		if candaction != None:
		    candaction(zp, zooev)
	    if (i - startevt) % 1024 == 0:
		print ("\r% 9d/% 9d events read (%9.3f GBytes)" % \
			tuple([ (i - startevt), nevents, nbytes / 1024. / 1024. / 1024. ]) ),
		sys.stdout.flush()
	    # if at least one candidate in the event was accepted, perform the
	    # per-event action
	    if acceptevt and evtaction != None:
		evtaction(zooev)
	# free branches
	self.tree.SetBranchAddress(collection, ROOT.MakeNullPointer())
	self.tree.SetBranchAddress("Event", ROOT.MakeNullPointer())
	print ("\r% 9d/% 9d events read (%9.3f GBytes)" % \
		tuple([ nevents, nevents, nbytes / 1024. / 1024. / 1024. ]) )
	return nevents

    def runOverZooMCP(self, cuts, maxevts, startevt,
	    mcpaction = None, evtaction = None):
	"""
	runOverZooP runs over all MC particles of a ZooTuple, applying the
	given cuts to each of them; for each accepted MC particle, a
	user-defined action can be executed, and for each event with accepted
	MC particles, a user-defined per-event action can be executed

	parameters are:
    	cuts            list of zero or more functions or lambda expressions
    			which evaluate to True of False; a MC particle is
    			accepted as passing all the cuts if all expressions
    			evaluate to True
    	maxevents       maximum number of events to process
    	startevt	event from which to start plotting
	mcpaction	function or lambda expression to execute in case of an
			accepted MC particle, or None if no action is to be
			executed for an accepted MC particle
			(default: None)
	evtaction	function or lambda expression to be executed for an
			event with accepted MC particles, or None if no action
			is to be executed for events with accepted MC
			particles
			(default: None)
	
	The function or lambda expression passed for candaction is called with
	two parameters, the candidate and the Zoo event structure; the
	function or lambda expression passed for evtaction is called with a
	single parameter, the Zoo event structure.

	The list of cuts is defined an used analogously to that of the
	DrawZooP method, see the documentation there to get an idea of how
	this is supposed to work.
	"""
	# initialise branches
	zooev = ROOT.ZooEv()
	self.tree.SetBranchAddress("Event", ROOT.AddressOf(zooev))
	# prepare to loop over events in Zoo tuple
	nevents = self.tree.GetEntries()
	if startevt >= nevents:
	    print 'runOverZooMCP: no events left!'
	    return None
	if maxevts > 0 and startevt + maxevts < nevents:
	    nevents = startevt + maxevts
	self.tree.SetCacheLearnEntries(1000)
	nbytes = 0
	for i in range(startevt, nevents):
	    # read next event
	    self.tree.LoadTree(i)
	    nbytes += self.tree.GetEntry(i)
	    # loop over list of ZooMCPs
	    zoomcps = zooev.GetMCP()
	    acceptevt = False
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
		# MC particle accepted
		acceptevt = True
		if None != mcpaction:
		    mcpaction(zmcp, zooev)
	    if (i - startevt) % 1024 == 0:
		print ("\r% 9d/% 9d events read (%9.3f GBytes)" % \
			tuple([ (i - startevt), nevents, nbytes / 1024. / 1024. / 1024. ]) ),
		sys.stdout.flush()
	    if acceptevt and None != evtaction:
		evtaction(zooev)
	print ("\r% 9d/% 9d events read (%9.3f GBytes)" % \
		tuple([ nevents, nevents, nbytes / 1024. / 1024. / 1024. ]) )
        # free branches
        self.tree.SetBranchAddress("Event", ROOT.MakeNullPointer())
	return nevents

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
	# use the runOverZooP method to do all the tedious cut evaluation work
	# for us, passing a per-candidate action which fills the histogram
	# with the required variables for an accepted candidate
	retVal = self.runOverZooP(collection, cuts, maxevts, startevent,
		lambda zp, zev: self.histo.Fill(*[ f(zp, zev) for f in variables ]))
	if None == retVal:
	    return None
	## initialise branche	# draw histogram
	self.histo.Draw(drawopts)
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
	# use the runOverZooMCP method to do all the tedious cut evaluation
	# work for us, passing a per-MC particle action which fills the
	# histogram with the required variables for an accepted MC particle
	retVal = self.runOverZooMCP(cuts, maxevts, startevent,
		lambda zp, zev: self.histo.Fill(*[ f(zp, zev) for f in variables ]))
	if None == retVal:
	    return None
        # draw histogram
        self.histo.Draw(drawopts)
        # return histogram
        return self.histo

# vim: sw=4:tw=78:ft=python
