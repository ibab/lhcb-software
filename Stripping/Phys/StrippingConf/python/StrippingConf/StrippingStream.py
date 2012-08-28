#!/usr/bin/env python

#
# StrippingStream
#

__all__ = ( 
	    'StrippingStream'
	  )

from Gaudi.Configuration import *
from Gaudi.Configuration import GaudiSequencer, Sequencer, Configurable
from Configurables import LHCbConfigurableUser
from copy import copy

class StrippingStream ( object ) :

    def __init__ ( self,
                   name = 'StrippingStream',
                   Lines =  [],                                    # List of stream lines
                   BadEventSelection = "Override",                 # Bad event selection algo
                                                                   # By default, it will be 
                                                                   # overridden in StrippingConf, 
                                                                   # but can be set to None here
                                                                   # to cancel bad event check

                   AcceptBadEvents = None,                         # If None, will be overridden 
                                                                   # in StrippingConf
		   MaxCandidates = "Override", 
		   MaxCombinations = "Override", 
		   TESPrefix = None
                 ) :
        self.lines = copy(Lines)
        for line in Lines : 
    	    line.declareAppended()

        self.algs = []
        self.seq = None
        self._name = name
        self.streamLine = None                   # Line with OR of all stream lines, created in createConfigurables()
        self.eventSelectionLine = None           # Line to mark bad events, created in createConfigurables()
        self.BadEventSelection = BadEventSelection
        self.AcceptBadEvents = AcceptBadEvents
        self.MaxCandidates = MaxCandidates
        self.MaxCombinations = MaxCombinations
	self.TESPrefix = TESPrefix               # Prefix for DecReports location, configured when appending stream
        self.HDRLocation = 'Phys/DecReports'     # DecReports location, configured when appending to StrippingConf

    def name(self) :
        return self._name

    def clone(self, name) : 
	return StrippingStream(name, Lines = copy(self.lines), 
	                       BadEventSelection = self.BadEventSelection, 
	                       AcceptBadEvents = self.AcceptBadEvents, 
	                       MaxCandidates = self.MaxCandidates, 
	                       MaxCombinations = self.MaxCombinations)

    def appendLines (self, lines) : 
	for line in lines : 
	    self.lines.append(line)
	    line.declareAppended()
	    

    def createConfigurables(self) :
        from Configurables import StrippingCheck

        # Create configurables

	if self.TESPrefix == None : self.TESPrefix = self._name

	for line in self.lines : 
	    if line.MaxCandidates == "Override" : 
	    	line.MaxCandidates = self.MaxCandidates
	    if line.MaxCombinations == "Override" : 
	    	line.MaxCombinations = self.MaxCombinations

	    if line.prescale() > 0. : 
		line.createConfigurable( self.TESPrefix, self.HDRLocation )
        	log.debug("ADDING configurable " + line.configurable().name() + "to stream " + self.name() )
        	self.algs.append(line.configurable())
    	    else : 
    		log.warning("Line " + line.name() + " has zero prescale, skipping" )

        # Make the line for stream decision (OR of all stream lines)

	linesSeq = GaudiSequencer("StrippingStreamSeq"+self.name(),
                                  ModeOR = True,
                                  #ShortCircuit = False,
                                  OutputLevel = WARNING, 
                                  Members = self.algs)

	from StrippingLine import StrippingLine

	self.streamLine = StrippingLine("Stream"+self.name(), checkPV = False, algos = [ linesSeq ] )
	self.streamLine.createConfigurable( self.TESPrefix, self.HDRLocation )
	self.streamLine.declareAppended()

        # If the BadEventsSelection was not given neither in StrippingConf nor in StrippingStream
        
        if self.BadEventSelection == "Override" : 
            self.BadEventSelection = None

        # Make the line to mark bad events (those satisfying BadEventSelection)
        
	if self.BadEventSelection != None : 
	    self.eventSelectionLine = StrippingLine("Stream"+self.name()+"BadEvent", 
	                                      checkPV = False, algos = [ self.BadEventSelection ] )
	    self.eventSelectionLine.createConfigurable( self.TESPrefix, self.HDRLocation )
	    self.eventSelectionLine.declareAppended()


    def sequence ( self ) :
        if self.seq == None :

            if self.eventSelectionLine == None : 
                # If we don't care about bad events, build a flat 
                # sequencer for all line configurables, including the stream decision line

                self.seq = GaudiSequencer("StrippingSequenceStream"+self.name(),
                                      ModeOR = True,
                                      ShortCircuit = False,
                                      OutputLevel = WARNING, 
                                      Members = self.algs + [ self.streamLine.configurable() ] )
            else : 
                # If BadEventSelection is used, the stream sequencer has to be "protected" by 
                # another one, which ensures that it runs only for good events 
                
            	lineSeq = GaudiSequencer("StrippingProtectedSequence"+self.name(),
                                          ModeOR = True,
                                          ShortCircuit = False,
                                          OutputLevel = WARNING, 
                                          Members = self.algs + [ self.streamLine.configurable() ] )
                                          
                # The structure differs depending on whether we want to accept or reject bad events

            	if self.AcceptBadEvents != False : 
            	
            	    # If we want to accept bad events, create the sequencer in OR mode with ShortCircuit=True
            	    # If the bad event condition passes, the stream sequencer will not run

        	    self.seq = GaudiSequencer("StrippingSequenceStream" + self.name(), 
                                          OutputLevel = WARNING, 
        	                          Members = [ self.eventSelectionLine.configurable(), lineSeq ] )
            	    self.seq.ModeOR = True 
        	    self.seq.ShortCircuit = True
        	    
        	else : 
        	    
        	    # If we want to reject bad events, things are more complicated. 
        	    # We need a logical NOT of the bad event condition. 
        	    
        	    # To get it, we first run the bad event condition, and ignore its result. 
        	    # This is achieved by the GaudiSequencer in IgnoreFilterPassed mode
        	    # that has only the BadEventSelection as a member. It always passes. 
        	    
        	    badEventSeq = GaudiSequencer("StrippingBadEventSequence"+self.name(), 
        					 Members = [ self.eventSelectionLine.configurable() ], 
                                                 OutputLevel = WARNING, 
        	                                 IgnoreFilterPassed = True)
        	    
        	    # Now create a LoKi filter that negates the result of the bad event selection
        	    
        	    from Configurables import LoKi__VoidFilter as Filter 
        	    
        	    goodEventFilter = Filter( "StrippingGoodEventCondition"+self.name() , 
        	        Code      = " ALG_EXECUTED('%s') & ~ALG_PASSED('%s') " 
        	                  % (self.eventSelectionLine.name(), self.eventSelectionLine.name()) ,
        	        Preambulo = [ "from LoKiHlt.algorithms import *" ] 
        	    )
        	    
        	    # Finally, the AND sequencer that will run the bad event selection (its result will 
        	    # be ignored since it's wrapped into a GaudiSequencer with IgnoreFilterPassed), 
        	    # then good event LokiFilter (it will pass only for good events)
        	    # then the stream sequence. 
        	    
        	    self.seq = GaudiSequencer("StrippingSequenceStream" + self.name(), 
                                          OutputLevel = WARNING, 
        	                          Members = [ badEventSeq, goodEventFilter, lineSeq ] )

        return self.seq


    def outputLocations (self) : 
	outputList = []
	for i in self.lines :
	    output = i.outputLocation()
	    if output : 
		outputList.append(output)
	return outputList


    def filterMembers( self ) : 
	_members = []
	for line in self.lines : 
	    lineMembers = line.filterMembers()
	    for i in lineMembers : 
		if i not in _members : _members.append(i) 
	return _members
