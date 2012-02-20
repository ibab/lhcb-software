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
	self.nbytes = 0
	# maximum read-ahead cache size (128 M)
	self.cache = 128 << 20
	# maximum virtual memory (256 M)
	self.maxvirt = 256 << 20
	# default basket size: 512 k
	self.basketsize = 1 << 19
	# set default compression level and algorithm
	if ROOT.gROOT.GetVersion() >= '5.30':
	    # from version 5.30 onwards, we have LZMA compression available;
	    # is compresses better, but needs more time
	    # fortunately, the fast setting of LZMA is comparable in speed to
	    # what was available in old ROOT versions at the best setting, but
	    # results in smaller files
	    self.compressionLevel = 3
	    self.compressionAlg = ROOT.kLZMA
	else:
	    self.compressionLevel = 8
	    self.compressionAlg = ROOT.kZLIB

    def _runOverZoo(self, collection, cuts, maxevts, startevt,
	    candaction = None, evtaction = None,
	    candalwaysaction = None, evtalwaysaction = None):
	"""
	implements runOverZooP/runOverZooMCP

	parameters and return values are like runOverZooP, except that
	collection may be None, in which case it runs over ZooMCPs instead of
	taking the specified collection of ZooPs
	"""
	tree = self.tree
	# activate the read-ahead cache of the tree
	tree.SetCacheSize(self.cache)
	tree.AddBranchToCache('*', True)
	# limit amount of in-core memory used by tree
	tree.SetMaxVirtualSize(self.maxvirt)
	# prepare needed branches
	zooev = ROOT.ZooEv()
	tree.SetBranchAddress("Event", ROOT.AddressOf(zooev))
	if None != collection:
	    zoops = ROOT.ZooObjRefArray(ROOT.ZooP)()
	    tree.SetBranchAddress(collection, ROOT.AddressOf(zoops))
	tree.SetCacheLearnEntries(128)
	# zero counters
	nevt = 0
	ncand = 0
	nacceptcand = 0
	nacceptevt = 0
	nbytes = 0
	self.nevt = nevt
	self.ncand = ncand
	self.nacceptcand = nacceptcand
	self.nacceptevt = nacceptevt
	self.nbytes = nbytes
	# prepare to loop over events in Zoo tuple
	nevents = tree.GetEntries()
	if startevt >= nevents:
	    print '_runOverZooP: no events left!'
	    return None
	if maxevts > 0 and startevt + maxevts < nevents:
	    nevents = startevt + maxevts
	# prepare the per-event/per-candidate action(s) so we do not have to
	# check for None in the loop
	nullaction = lambda x, y, z = None: None
	if None == candaction: candaction = nullaction
	if None == candalwaysaction: candalwaysaction = nullaction
	if None == evtaction: evtaction = nullaction
	if None == evtalwaysaction: evtalwaysaction = nullaction
	# body of loop over candidates in collection
	def candloopbody(zp, zooev):
	    candalwaysaction(self, zp, zooev)
	    # does candidate pass cuts
	    for c in cuts:
		if not c(zp, zooev): return 0
	    # perform per candidate action
	    candaction(self, zp, zooev)
	    return 1
	# loop over events
	for i in xrange(startevt, nevents):
	    # read next event
	    read = tree.GetEntry(i)
	    if read <= 0:
		print '_runOverZoo: trouble reading entry %d from tree, aborting!' % i
		break
	    nbytes += read
	    # prepare multiplexing for ZooP/ZooMCP case - for the latter, we
	    # need to get the list of MCPs from zooev in every new event
	    if None == collection:
		zoops = zooev.GetMCP()
	    if 0 == (i - startevt) & 0x3ff:
		print '\r\t% 9d/% 9d events read (%9.3f GBytes uncompressed)' % \
			( nevt, nevents, nbytes / (0. + (1 << 30)) ),
		sys.stdout.flush()
	    # execute loop over candidates and check for accepted ones
	    ncand += zoops.size()
	    nacc = sum(candloopbody(zoops[j], zooev)
		    for j in xrange(0, zoops.size()))
	    acceptevt = 0 < nacc
	    nacceptcand += nacc
	    nevt += 1
	    evtalwaysaction(self, zooev)
	    # if at least one candidate in the event was accepted, perform the
	    # per-event action
	    if acceptevt:
		nacceptevt += 1
		evtaction(self, zooev)
	# free branches
	if None != collection:
	    tree.SetBranchAddress(collection, ROOT.MakeNullPointer())
	tree.SetBranchAddress("Event", ROOT.MakeNullPointer())
	self.nevt = nevt
	self.ncand = ncand
	self.nacceptcand = nacceptcand
	self.nacceptevt = nacceptevt
	self.nbytes = nbytes
	print '\r\t% 9d/% 9d events read (%9.3f GBytes uncompressed)' % \
		( nevt, nevents, nbytes / (0. + (1 << 30)) )
	sys.stdout.flush()
	return nevt

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
	return self._runOverZoo(collection, cuts, maxevts, startevt,
		candaction, evtaction, candalwaysaction, evtalwaysaction)

    def runOverZooMCP(self, cuts, maxevts, startevt,
	    candaction = None, evtaction = None,
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
	candaction	function or lambda expression to execute in case of an
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
	return self._runOverZoo(None, cuts, maxevts, startevt,
		candaction, evtaction, candalwaysaction, evtalwaysaction)

    def _allocHisto(self, ndim, pfx, collection, title):
	"""
	allocate histogram and do sanity checks for DrawZooP/DrawZooMCP
	"""
	if ndim < 1 or ndim > 3:
	    print '_allocHisto: plots have to have between one and three dimensions!'
	    return None
	# book histogram
	if None != self.histo:
	    self.histo = None
	if "" != collection:
	    collection = '_'.join(collection)
	pfx.join(collection)
	if 1 == ndim:
	    self.histo = \
		    ROOT.TH1D(pfx, title, 100, 0, 0)
	if 2 == ndim:
	    self.histo = \
		    ROOT.TH2D(pfx, title, 100, 0, 0, 100, 0, 0)
	if 3 == ndim:
	    self.histo = \
		    ROOT.TH3D(pfx, title, 100, 0, 0, 100, 0, 0, 100, 0, 0)

    def _printStats(self, pfx):
	"""
	print some statistics about the last run (# events / #candidates
	accepted)
	"""
	print '%s: cuts selected % 9d out of %9d events (%6.2f%% efficiency)' % \
		( pfx, self.nacceptevt, self.nevt, 100. * self.nacceptevt / self.nevt )
	print '%s: cuts selected % 9d out of %9d candidates (%6.2f%% efficiency)' % \
		( pfx, self.nacceptcand, self.ncand, 100. * self.nacceptcand / self.ncand )

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
	self._allocHisto(len(variables), 'DrawZooP', collection, title)
	# append weight to variable array
	variables.append(weight)
	# use the runOverZooP method to do all the tedious cut evaluation work
	# for us, passing a per-candidate action which fills the histogram
	# with the required variables for an accepted candidate
	retVal = self.runOverZooP(collection, cuts, maxevts, startevent,
		lambda ztp, zp, zev:
			ztp.histo.Fill(*(f(zp, zev) for f in variables)))
	if None == retVal:
	    return None
	self._printStats('DrawZooP')
	# draw histogram
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
	self._allocHisto(len(variables), 'DrawZooMCP', '', title)
	# append weight to variable array
	variables.append(weight)
	# use the runOverZooMCP method to do all the tedious cut evaluation
	# work for us, passing a per-MC particle action which fills the
	# histogram with the required variables for an accepted MC particle
	retVal = self.runOverZooMCP(cuts, maxevts, startevent,
		lambda ztp, zp, zev:
			ztp.histo.Fill(*(f(zp, zev) for f in variables)))
	if None == retVal:
	    return None
	self._printStats('DrawZooMCP')
        # draw histogram
        self.histo.Draw(drawopts)
        # return histogram
        return self.histo

    def _getPerJobTree(self):
	"""
	return the per-job tree associated with the opened Zoo tuple
	"""
	print '_getPerJobTree: getting per-job tree for current set of Zoo tuples'
	perjobtreename = '%s%s' % (self.tree.GetName(), '_Shrubbery')
	if self.tree.Class().GetName() == 'TChain':
	    # if self.tree is a chain, we need to construct the equivalent
	    # chain for the per-job tree
	    perjobtree = ROOT.TChain(perjobtreename)
	    reduce(lambda x, y: perjobtree.AddFile(y),
		    map(lambda x: x.GetTitle(), self.tree.GetListOfFiles()),
		    [])
	else:
	    # self.tree is a TTree, we can just ask self.tree for the per-job
	    # tree
	    perjobtree = self.tree.GetFriend(perjobtreename)
	return perjobtree
    
    def _copyPerJobTree(self, outfile, perjobtree):
	"""
	copy the per job tree of a Zoo tuple from the opened Zoo tuple to the
	given outfile

	returns the copied per-job tree
	"""
	# set up tree structure for per job info tree
	strippedperjobtree = perjobtree.CloneTree(0)
	strippedperjobtree.SetDirectory(outfile)
	strippedperjobtree.SetBasketSize('*', self.basketsize)
	strippedperjobtree.SetAutoSave(self.cache)
	# activate the read-ahead cache of the tree
	perjobtree.SetCacheSize(self.cache)
	perjobtree.AddBranchToCache('*', True)
	# limit amount of in-core memory used by tree
	perjobtree.SetMaxVirtualSize(self.maxvirt)
	strippedperjobtree.SetMaxVirtualSize(self.maxvirt)
	# pre-copy the per job tree (we need all of it since it'll become
	# too hard to figure out how this goes together with the main Zoo tree
	# otherwise - fortunately, the per-job tree is small)
	nentriesperjob = perjobtree.GetEntries()
	for i in xrange(0, nentriesperjob):
	    perjobtree.GetEntry(i)
	    strippedperjobtree.Fill()
	    if 0 == i & 0xf:
		print '\r_copyPerJobTree: copying per job tree entry % 4d/% 4d' % \
			( i, nentriesperjob ),
		sys.stdout.flush()
	print '\r_copyPerJobTree: copying per job tree entry % 4d/% 4d' % \
		( nentriesperjob, nentriesperjob )
	return strippedperjobtree

    def _stripZoo(self, outfilename, maxevts, startevent, strippingaction):
	"""
	do the actual stripping:
	    - getting per-job tree/chain
	    - opening destination file
	    - copying per job tree
	    - call the appropriate stripping action

	returns the number of accepted events on success, None on failure
	"""
	print '_stripZoo: Zoo tree at %s' % str(self.tree)
	perjobtree = self._getPerJobTree()
	print '_stripZoo: per-job tree at %s' % str(perjobtree)
	print '_stripZoo: opening destination file %s' % outfilename
	# open ROOT file for writing
	if ROOT.gROOT.GetVersion() >= "5.30":
	    outfile = ROOT.TFile(outfilename, 'RECREATE', '',
		    ROOT.CompressionSettings(
			self.compressionAlg, self.compressionLevel))
        else:
	    outfile = ROOT.TFile(outfilename, 'RECREATE', '', self.compressionLevel)
	if outfile.IsZombie():
	    print '_stripZoo(): unable to open output file %s' % outfilename
	    return None
	print '_stripZoo: cloning tree structure'
	# set up tree structure for Zoo tree
	strippedtree = self.tree.CloneTree(0)
	strippedtree.SetDirectory(outfile)
	strippedtree.SetBasketSize('*', self.basketsize)
	strippedtree.SetAutoSave(self.cache)
	strippedtree.SetMaxVirtualSize(self.maxvirt)
	# copy the per-job tree
	strippedperjobtree = self._copyPerJobTree(outfile, perjobtree)
	print '_stripZoo: optimizing per-job tree'
	strippedperjobtree.OptimizeBaskets(self.basketsize)
	# don't need the original per-job tree any longer
	del perjobtree
	print '_stripZoo: writing per-job tree to destination file %s' % outfilename
	strippedperjobtree.Write()
	print '_stripZoo: looping over events to copy accepted events:'
	# we need a little helper function for the per-event tree(s)
	# use runOverZooP to do the actual work
	def writeAcceptedEvent(ztp, zev):
	    strippedtree.Fill()
	    if (ztp.nacceptevt == 4096):
		strippedtree.OptimizeBaskets(self.basketsize)
		# do some magic: modify writeAcceptedEvent after we optimized
		# baskets to get rid of the if statement
		self._stripZoo.writeAcceptedEvent = lambda : strippedtree.Fill()
	# do the actual work
	retVal = strippingaction(writeAcceptedEvent)
	if None == retVal:
	    print '_stripZoo: reading/writing events failed for some reason!'
	    print '_stripZoo: attempting to save what we have so far.'
	    strippedtree.Write()
	    print '_stripZoo: attempting to close destination file %s' % outfilename
	    outfile.Close()
	    return retVal
	# only write trees if we have selected events
	if 0 < self.nacceptevt:
	    # Optimize baskets before writing
	    if self.nacceptevt < 4096:
		strippedtree.OptimizeBaskets(self.basketsize)
	print '_stripZoo: writing tree to destination file %s' % outfilename
	strippedtree.Write()
	strippedperjobtree.Print()
	strippedtree.Print()
	print '_stripZoo: closing destination file %s' % outfilename
	outfile.Close()
	print '_stripZoo: all done'
	self._printStats('_stripZoo')
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
	return self._stripZoo(outfilename, maxevts, startevent,
		lambda evtact: self.runOverZooP(collection, cuts, maxevts, startevent,
		    # nothing to do per candidate if it passes the cuts
		    None,
		    # fill events which pass cuts into the stripped Zoo tree
		    evtact))

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
	return self._stripZoo(outfilename, maxevts, startevent,
		lambda evtact: self.runOverZooP(cuts, maxevts, startevent,
		    # nothing to do per candidate if it passes the cuts
		    None,
		    # fill events which pass cuts into the stripped Zoo tree
		    evtact))

# vim: sw=4:tw=78:ft=python
