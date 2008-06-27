# =============================================================================
""" @namespace AlgoTools
@brief Methods for printing Gaudi sequencer hierarchy + algorithm properties

Constains a dictionary of algorithms usefull properties

See AlgoToolsExample() for examples on how to use this module.

@author Gabriel Guerrer - Gabriel.Guerrer@cern.ch
@date 2008-06-26
"""
# =============================================================================

import GaudiPython

gaudi = GaudiPython.AppMgr()

#-----------------------------------------------
# Dictionary
#-----------------------------------------------
AlgoPropertiesDICT = {
	'L0DUAlg': ['TCK'],
	'HltL0CaloPrepare': ['MinEt', 'CaloType', 'CaloCandidatesLocation', 'CaloMakerTool'],
	'HltL0MuonPrepare': ['MinPt'],
	'HltTrackFilter': ['InputSelection', 'FilterDescriptor'],
	'HltTrackUpgrade': ['InputSelection', 'RecoName'],
	'HltTrackMatch': ['InputSelection', 'InputSelection2', 'MatchName', 'MaxQuality'],
	'HltVertexMaker': ['InputSelection', 'FilterDescriptor'],
	'HltVertexFilter': ['InputSelection', 'FilterDescriptor'],
	'HltVertexUpgrade': ['InputSelection', 'RecoName'],
	'RawBankToSTLiteClusterAlg': ['clusterLocation'],
	'Tf::PatVeloRTracking': ['OutputTracksName'],
	'PatPV2D': ['InputTracksName', 'OutputVerticesName'],
	'PreLoadParticles': ['DecayDescriptor'],
	'CombineParticles': ['Factory', 'DecayDescriptors', 'DaughtersCuts', 'CombinationCut', 'MotherCut'],
	'FilterDesktop': ['FilterCriterion'],
	'DeterministicPrescaler': ['AcceptFraction', 'AcceptPattern']
	}

#-----------------------------------------------
# Helpers
#-----------------------------------------------
def algoexists(algo):
	"""
	"""
	result = True
	props = algo.properties()
	if len(props) == 0:
		result = False
	return result

#-----------------------------------------------
def issequencer(algo):
	"""
	"""
	result = True
	props = algo.properties()
	try:
		members = props['Members']
	except KeyError:
		result = False			
	return result

#-----------------------------------------------
def strtoclassinstance(string):
	"""
	"""
	ss = string.split('/')		
	if len(ss) == 1:
		algo = gaudi.algorithm(string)
		if issequencer(algo):
			clas = 'GaudiSequencer'
			instance = string
		else:
			clas = ''
			instance = string
	elif len(ss) == 2:
		clas = ss[0]
		instance = ss[1]			
	else:
		print 'Error - strtoclassinstance'
		return '', ''		
	return clas, instance	

#-----------------------------------------------
def getrootseqs():		
	""" Gets root sequencers
    @returns list of root sequencer
    @author Gabriel Guerrer - Gabriel.Guerrer@cern.ch
	"""	
	def filterrootseqs(seqstr, rootseqstrs):
		seq = gaudi.algorithm(seqstr)
		memberstrs = seq.properties()['Members'].value()
		for memberstr in memberstrs:
			clas, instance = strtoclassinstance(memberstr)
			if issequencer( gaudi.algorithm(instance) ):					
				try:
					rootseqstrs.remove(instance)
				except ValueError:
					continue
				filterrootseqs(instance, rootseqstrs)		

	
	allalgostrs = gaudi.algorithms()
	rootseqstrs = []

	for algostr in allalgostrs:
		if issequencer( gaudi.algorithm(algostr) ):
			rootseqstrs.append( algostr )		

	for seqstr in rootseqstrs:
		filterrootseqs(seqstr, rootseqstrs)	

	return rootseqstrs		

#-----------------------------------------------
# algotree
#-----------------------------------------------
def algotree(sequencername='', outputlvl=2, maxtreelvl=0, printtreelvl=0):
	""" prints Gaudi sequencer/algorithms hierarchy. Symbols for GaudiSequencers are: | - ModeOR, 
		* - No Short Circuit, ! - Ignore Filter Passed
	@param sequencername sequencer name string. if empty - print all root sequencers
	@param outputlvl 0- only instance names, 1- class type + 0, 2- 1 + extra info for classes
	@param maxtreelvl force maximum tree level printed
	@param printtreelvl internal for printing on recursive calls
    @author Gabriel Guerrer - Gabriel.Guerrer@cern.ch
	"""	
	
	sequencers = []

	# no specific sequencer defined, print all
	if sequencername == '':
		rootseqs = getrootseqs()
		for seq in rootseqs:
			algotree(seq, outputlvl, maxtreelvl, printtreelvl)
			print ' '	
	
	else:
		clas, instance = strtoclassinstance(sequencername)
		algo = gaudi.algorithm(instance)
		
		if outputlvl == 0:
			objinfo = instance
		else:
			if clas == '':
				objinfo = instance
			else:
				objinfo = clas+'/'+instance
		
		if not algoexists(algo):
			print 'Error - algotree : Algorithm ', sequencername, ' not defined.' 
			return
		
		if issequencer(algo):		
			modeor = algo.properties()['ModeOR'].value()
			modeorstr = ''
			if modeor: modeorstr='|'

			shortcircuit = algo.properties()['ShortCircuit'].value()
			shortcircuitstr = ''
			if not shortcircuit: shortcircuitstr = '*'

			ignorefilterpassed = algo.properties()['IgnoreFilterPassed'].value()
			ignorefilterpassedstr = ''
			if ignorefilterpassed: ignorefilterpassedstr='!'

			print printtreelvl*'   '+modeorstr+shortcircuitstr+ignorefilterpassedstr+objinfo

			printtreelvl+=1
			
			if printtreelvl != maxtreelvl:			
				memberstrs = algo.properties()['Members'].value()
				for memberstr in memberstrs:				
					algotree(memberstr, outputlvl, maxtreelvl, printtreelvl)
		
		# an algorithm
		else:
			print printtreelvl*'   '+objinfo
			
			# print extra info using properties dictionary
			if outputlvl == 2:
			
				try:
					propstrs = AlgoPropertiesDICT[clas]

					for propstr in propstrs:
						prop = algo.properties()[propstr].value()						
						
						print (printtreelvl+1)*'   '+propstr+' =',prop
												
				except KeyError:
					pass
		

#-----------------------------------------------
# Example
#-----------------------------------------------		
def AlgoToolsExample():
	"""
	"""
	
	print
	print '================================'
	print 'Print all root sequencers'
	print
	algotree()
	
	print
	print '================================'
	print 'Avaliable root sequencers'
	print
	print getrootseqs()
	
	print
	print '================================'
	print 'Hlt alleys'
	print
	algotree('Hlt', outputlvl=1, maxtreelvl = 3)
	
	print
	print '================================'
	print 'Hlt1HadronSequence'
	print
	algotree('Hlt1HadronSequence')
	
