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
	self.nevt = 0
	self.ncand = 0
	self.nacceptcand = 0
	self.nacceptevt = 0

    def runOverZooP(self, collection, cuts, maxevts, startevt,
	    candaction = None, evtaction = None,
	    candalwaysaction = None, evtalwaysaction = None):
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
	candalwaysaction
			action to be executed for every candidate or None if
			no action needs to be performed for every candidate
			(default: None)
	evtalwaysaction	action to be executed for every event or None if no
			action needs to be performed for every event
			(default: None)
	
	The function or lambda expression passed for candaction or
	candalwaysaction is called with three parameters, the ZooTuplePlayer,
	the candidate and the Zoo event structure; the function or lambda
	expression passed for evtaction or evtalwaysaction is called with two
	parameters, the ZooTuplePlayer and the Zoo event structure.

	The list of cuts is defined an used analogously to that of the
	DrawZooP method, see the documentation there to get an idea of how
	this is supposed to work.
	"""
	zoops = ROOT.ZooObjRefArray(ROOT.ZooP)()
	zooev = ROOT.ZooEv()
	self.tree.SetBranchAddress(collection, ROOT.AddressOf(zoops))
	self.tree.SetBranchAddress("Event", ROOT.AddressOf(zooev))
	self.nevt = 0
	self.ncand = 0
	self.nacceptcand = 0
	self.nacceptevt = 0
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
		self.ncand = self.ncand + 1
		if None != candalwaysaction:
		    candalwaysaction(self, zp, zev)
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
		self.nacceptcand = self.nacceptcand + 1
		# perform per candidate action
		if candaction != None:
		    candaction(self, zp, zooev)
	    if (i - startevt) % 1024 == 0:
		print ("\r% 9d/% 9d events read (%9.3f GBytes)" % \
			tuple([ (i - startevt), nevents, nbytes / 1024. / 1024. / 1024. ]) ),
		sys.stdout.flush()
	    self.nevt = self.nevt + 1
	    if None != evtalwaysaction:
		evtalwaysaction(self, zooev)
	    # if at least one candidate in the event was accepted, perform the
	    # per-event action
	    if acceptevt:
		self.nacceptevt = self.nacceptevt + 1
		if None != evtaction:
		    evtaction(self, zooev)
	# free branches
	self.tree.SetBranchAddress(collection, ROOT.MakeNullPointer())
	self.tree.SetBranchAddress("Event", ROOT.MakeNullPointer())
	print ("\r% 9d/% 9d events read (%9.3f GBytes)" % \
		tuple([ nevents, nevents, nbytes / 1024. / 1024. / 1024. ]) )
	return nevents

    def runOverZooMCP(self, cuts, maxevts, startevt,
	    mcpaction = None, evtaction = None,
	    candalwaysaction = None, evtalwaysaction = None):
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
	candalwaysaction
			action to be executed for every candidate or None if
			no action needs to be performed for every candidate
			(default: None)
	evtalwaysaction	action to be executed for every event or None if no
			action needs to be performed for every event
			(default: None)
	
	The function or lambda expression passed for candaction or
	candalwaysaction is called with three parameters, the ZooTuplePlayer,
	the candidate and the Zoo event structure; the function or lambda
	expression passed for evtaction or evtalwaysaction is called with two
	parameters, the ZooTuplePlayer and the Zoo event structure.

	The list of cuts is defined an used analogously to that of the
	DrawZooP method, see the documentation there to get an idea of how
	this is supposed to work.
	"""
	# initialise branches
	zooev = ROOT.ZooEv()
	self.tree.SetBranchAddress("Event", ROOT.AddressOf(zooev))
	# prepare to loop over events in Zoo tuple
	self.nevt = 0
	self.ncand = 0
	self.nacceptcand = 0
	self.nacceptevt = 0
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
		self.ncand = self.ncand + 1
		if None != candalwaysaction:
		    candalwaysaction(self, zp, zev)
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
		self.nacceptcand = self.nacceptcand + 1
		if None != mcpaction:
		    mcpaction(self, zmcp, zooev)
	    if (i - startevt) % 1024 == 0:
		print ("\r% 9d/% 9d events read (%9.3f GBytes)" % \
			tuple([ (i - startevt), nevents, nbytes / 1024. / 1024. / 1024. ]) ),
		sys.stdout.flush()
	    self.nevt = self.nevt + 1
	    if None != evtalwaysaction:
		evtalwaysaction(self, zooev)
	    if acceptevt:
		self.nacceptevt = self.nacceptevt + 1
		if None != evtaction:
		    evtaction(self, zooev)
	print ("\r% 9d/% 9d events read (%9.3f GBytes)" % \
		tuple([ nevents, nevents, nbytes / 1024. / 1024. / 1024. ]) )
        # free branches
        self.tree.SetBranchAddress("Event", ROOT.MakeNullPointer())
	return nevents

    def __allocHisto(self, ndim, pfx, collection, title):
	"""
	allocate histogram and do sanity checks for DrawZooP/DrawZooMCP
	"""
	if ndim < 1 or ndim > 3:
	    print '__allocHisto: plots have to have between one and three dimensions!'
	    return None
	# book histogram
	if None != self.histo:
	    self.histo = None
	if "" != collection:
	    collection = "_" + collection
	if 1 == ndim:
	    self.histo = \
		    ROOT.TH1D(pfx + collection, title, 100, 0, 0)
	if 2 == ndim:
	    self.histo = \
		    ROOT.TH2D(pfx + collection, title, 100, 0, 0, 100, 0, 0)
	if 3 == ndim:
	    self.histo = \
		    ROOT.TH3D(pfx + collection, title, 100, 0, 0, 100, 0, 0, 100, 0, 0)

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
	self.__allocHisto(ndim, 'DrawZooP', collection, title)
	# append weight to variable array
	variables.append(weight)
	# use the runOverZooP method to do all the tedious cut evaluation work
	# for us, passing a per-candidate action which fills the histogram
	# with the required variables for an accepted candidate
	retVal = self.runOverZooP(collection, cuts, maxevts, startevent,
		lambda zootupleplayer, zp, zev:
			self.histo.Fill(*[ f(zp, zev) for f in variables ]))
	if None == retVal:
	    return None
	## initialise branche	# draw histogram
	self.histo.Draw(drawopts)
	# return histogram
	return self.histo

    # FIXME: it should be possible to combine much of StripByZooP and
    # StripByZooMCP to avoid code duplication
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
	self.__allocHisto(ndim, 'DrawZooMCP', '', title)
	# append weight to variable array
	variables.append(weight)
	# use the runOverZooMCP method to do all the tedious cut evaluation
	# work for us, passing a per-MC particle action which fills the
	# histogram with the required variables for an accepted MC particle
	retVal = self.runOverZooMCP(cuts, maxevts, startevent,
		lambda zootupleplayer, zp, zev:
			self.histo.Fill(*[ f(zp, zev) for f in variables ]))
	if None == retVal:
	    return None
        # draw histogram
        self.histo.Draw(drawopts)
        # return histogram
        return self.histo

    def __getPerJobTree(self):
	"""
	return the per-job tree associated with the opened Zoo tuple
	"""
	print '__getPerJobTree: getting per-job tree for current set of Zoo tuples'
	if self.tree.Class().GetName() == 'TChain':
	    # if self.tree is a chain, we need to construct the equivalent
	    # chain for the per-job tree
	    perjobtree = ROOT.TChain(self.tree.GetName() + '_Shrubbery')
	    for tmp in self.tree.GetListOfFiles():
	        perjobtree.AddFile(tmp.GetTitle())
	else:
	    # self.tree is a TTree, we can just ask self.tree for the per-job
	    # tree
	    perjobtree = self.tree.GetFriend(self.tree.GetName() + '_Shrubbery')
	return perjobtree
    
    def __copyPerJobTree(self, outfile, perjobtree):
	"""
	copy the per job tree of a Zoo tuple from the opened Zoo tuple to the
	given outfile

	returns the copied per-job tree
	"""
	# set up tree structure for per job info tree
	strippedperjobtree = perjobtree.CloneTree(0)
	strippedperjobtree.SetDirectory(outfile)
	strippedperjobtree.SetBasketSize('*', 1 << 16)
	strippedperjobtree.SetAutoSave(1 << 24)
	# pre-copy the per job tree (we need all of it since it'll become
	# too hard to figure out how this goes together with the main Zoo tree
	# otherwise - fortunately, the per-job tree is small)
	nentriesperjob = perjobtree.GetEntries()
	for i in range(0, nentriesperjob):
	    perjobtree.LoadTree(i)
	    perjobtree.GetEntry(i)
	    strippedperjobtree.Fill()
	    if i % 16 == 0:
		print '\r__copyPerJobTree: copying per job tree entry % 4d/% 4d' % \
			tuple([ i, nentriesperjob ]),
		sys.stdout.flush()
	print '\r__copyPerJobTree: copying per job tree entry % 4d/% 4d' % \
		tuple([ nentriesperjob, nentriesperjob ])
	return strippedperjobtree

    def __stripZoo(self, outfilename, maxevts, startevent, strippingaction):
	"""
	do the actual stripping:
	    - getting per-job tree/chain
	    - opening destination file
	    - copying per job tree
	    - call the appropriate stripping action

	returns the number of accepted events on success, None on failure
	"""
	print '__stripZoo: Zoo tree at ' + str(self.tree)
	perjobtree = self.__getPerJobTree()
	print '__stripZoo: per-job tree at ' + str(perjobtree)
	print '__stripZoo: opening destination file ' + outfilename
	# open ROOT file for writing
	outfile = ROOT.TFile(outfilename, 'RECREATE', '', 9)
	if outfile.IsZombie():
	    print '__stripZoo(): unable to open output file ' + outfilename
	    return None
	print '__stripZoo: cloning tree structure'
	# set up tree structure for Zoo tree
	strippedtree = self.tree.CloneTree(0)
	strippedtree.SetDirectory(outfile)
	strippedtree.SetBasketSize('*', 1 << 16)
	strippedtree.SetAutoSave(1 << 24)
	# copy the per-job tree
	strippedperjobtree = self.__copyPerJobTree(outfile, perjobtree)
	strippedtree.AddFriend(strippedperjobtree)
	print '__stripZoo: looping over events to copy accepted events:'
	# we need a little helper function for the per-event tree(s)
	# use runOverZooP to do the actual work
	def writeAcceptedEvent(ztp, zev):
	    strippedtree.Fill()
	    if (ztp.nacceptevt == 4096):
		strippedtree.OptimizeBaskets(1 << 16)
	# do the actual work
	retVal = strippingaction(writeAcceptedEvent)
	if None == retVal:
	    print '__stripZoo: reading/writing events failed for some reason!'
	    print '__stripZoo: attempting to save what we have so far.'
	    strippedperjobtree.Write()
	    strippedtree.Write()
	    print '__stripZoo: attempting to close destination file ' + outfilename
	    outfile.Close()
	    return retVal
	# only write trees if we have selected events
	if 0 < self.nacceptevt:
	    # Optimize baskets before writing
	    if self.nacceptevt < 4096:
		strippedtree.OptimizeBaskets(1 << 16)
	print '__stripZoo: writing trees to destination file ' + outfilename
	strippedperjobtree.Write()
	strippedtree.Write()
	print '__stripZoo: closing destination file ' + outfilename
	outfile.Close()
	print '__stripZoo: all done'
	print '__stripZoo: cuts selected % 9d out of %9d events (%6.2f%% efficiency)' % \
		tuple([ self.nacceptevt, retVal, 100. * self.nacceptevt / retVal ])
	print '__stripZoo: cuts selected % 9d out of %9d candidates (%6.2f%% efficiency)' % \
		tuple([ self.nacceptcand, self.ncand, 100. * self.nacceptcand / self.ncand ])
	return self.nacceptevt

    def StripByZooP(self, outfilename, collection, cuts = [],
	    maxevts = -1, startevent = 0):
	"""
        StripByZooP strips a ZooTuple by applying the given cuts to particles
	in the specified collection, writing only events to the output file
	which pass the cuts.
    
        parameters are:
	outfilename	name of the output file
			be careful, the specified file is overwritten if it exists
    	collection      name of the particle collection to draw
    	cuts            list of zero or more functions or lambda expressions
    			which evaluate to True of False; a candidate is
    			accepted as passing all the cuts if all expressions
    			evaluate to True
    			(default: empty list means no cuts)
    	maxevents       maximum number of events to process
    			(default: -1 meaning all events in the tree)
    	startevent	event from which to start plotting
    
        StripByZooP returns the number of events written to the output file on
	success, None on failure.
	"""
	return self.__stripZoo(outfilename, maxevts, startevent,
		lambda evtact: self.runOverZooP(collection, cuts, maxevts, startevent,
		    # nothing to do per candidate if it passes the cuts
		    None,
		    # fill events which pass cuts into the stripped Zoo tree
		    evtact))

    # FIXME: it should be possible to combine much of StripByZooP and
    # StripByZooMCP to avoid code duplication
    def StripByZooMCP(self, outfilename, cuts = [],
	    maxevts = -1, startevent = 0):
	"""
        StripByZooMCP strips a ZooTuple by applying the given cuts to MC
	particles, writing only events to the output file which pass the cuts.
    
        parameters are:
	outfilename	name of the output file
			be careful, the specified file is overwritten if it exists
    	cuts            list of zero or more functions or lambda expressions
    			which evaluate to True of False; an MC particle is
    			accepted as passing all the cuts if all expressions
    			evaluate to True
    			(default: empty list means no cuts)
    	maxevents       maximum number of events to process
    			(default: -1 meaning all events in the tree)
    	startevent	event from which to start plotting
    
	StripByZooMCP returns the number of events written to the output file
	on success, None on failure.
	"""
	return self.__stripZoo(outfilename, maxevts, startevent,
		lambda evtact: self.runOverZooP(cuts, maxevts, startevent,
		    # nothing to do per candidate if it passes the cuts
		    None,
		    # fill events which pass cuts into the stripped Zoo tree
		    evtact))

# vim: sw=4:tw=78:ft=python
