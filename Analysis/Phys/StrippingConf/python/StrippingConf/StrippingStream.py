#!/usr/bin/env python

#
# StrippingStream
#

__all__ = ( 
	    'StrippingStream'
	  )

from GaudiConf.Configuration import *
from Gaudi.Configuration import GaudiSequencer, Sequencer, Configurable
from Configurables import LHCbConfigurableUser
from copy import copy

class StrippingStream ( object ) :


    def __init__ ( self,
                   name = 'StrippingStream',
                   Lines =  [],               # List of stream lines
                   BadEventSelection = None,  # Bad event selection algo; 
                                              # bad events will be stored to DST without running selections
                   GoodEventSelection = None  # Good events selection algo; bad events will be skipped
                 ) :
        self.lines = copy(Lines)
        for line in Lines : 
    	    line.declareAppended()

        self.algs = []
        self.seq = None
        self._name = name
        self.streamLine = None
        self.eventSelectionLine = None
        self.badEventSelection = BadEventSelection
        self.goodEventSelection = GoodEventSelection


    def name(self) :
        return self._name

    # Good or bad event selections can be given (not both). 
    # In both cases, the stream selections will be called 
    # if the event satisfies good event condition. 
    # The difference is: if badEventSelection is given, bad 
    # events will produce a positive decision in the stream 
    # sequencer (and thus, e.g. will be written to DST)
    # If goodEventSelection is given, bad events will be 
    # skipped. 

    def SetBadEventSelection(self, selection) : 
        self.badEventSelection = selection    


    def SetGoodEventSelection(self, selection) : 
        self.goodEventSelection = selection    


    def appendLines (self, lines) : 
	for line in lines : 
	    self.lines.append(line)
	    line.declareAppended()
	    

    def createConfigurables(self, TES = False, 
				  TESPrefix = 'Strip', 
				  HDRLocation = 'Phys/DecReports') :
        from Configurables import StrippingCheck

        # Create configurables
        
	for line in self.lines : 
	    if TES :
                print "ADDINGXX", alg.configurable(), "name ", alg.name(), "to StrippingStream.lines" 
                alg = StrippingCheck(line.name(),
                                     InputLocation = "/Event/" + TESPrefix + "/" + line.outputLocation() + "/Particles")
                self.algs.append( alg )
	    else :  
		line.createConfigurable( TESPrefix + "/" + HDRLocation )
                print "ADDING not TES", line.configurable(), "name ", line.configurable().name(), "to StrippingStream.lines" 
                self.algs.append(line.configurable())

        # Make the line for stream decision (OR of all stream lines)

	linesSeq = GaudiSequencer("StrippingStreamSeq"+self.name(),
                                      ModeOR = True,
                                      #ShortCircuit = False,
                                      Members = self.algs)

	from StrippingLine import StrippingLine

	self.streamLine = StrippingLine("Stream"+self.name(), checkPV = False, algos = [ linesSeq ] )
	self.streamLine.createConfigurable( TESPrefix + "/" + HDRLocation )
	self.streamLine.declareAppended()

        # Make the line to mark bad events (those satisfying BadEventSelection)

	if self.badEventSelection != None : 
	    self.eventSelectionLine = StrippingLine("Stream"+self.name()+"BadEvent", 
	                                      checkPV = False, algos = [ self.badEventSelection ] )
	    self.eventSelectionLine.createConfigurable( TESPrefix + "/" + HDRLocation )
	    self.eventSelectionLine.declareAppended()

        # Good event selector (both good and bad event selectors cannot be specified at the same time)

	if self.goodEventSelection != None : 
	    if self.badEventSelection != None : 
	        raise ValueError,"Good and bad event selectors cannot be given simultaneously, stream %s " % self.name()
	        
	    self.eventSelectionLine = StrippingLine("Stream"+self.name()+"BadEvent", 
	                                      checkPV = False, algos = [ self.goodEventSelection ] )
	    self.eventSelectionLine.createConfigurable( TESPrefix + "/" + HDRLocation )
	    self.eventSelectionLine.declareAppended()


    def sequence ( self ) :
        if self.seq == None :
            # members = self.algs
            # members.append(self.streamLine.configurable())

            if self.eventSelectionLine == None : 
                # Sequencer for all line configurables, including the stream decision line

                self.seq = GaudiSequencer("StrippingSequenceStream"+self.name(),
                                      ModeOR = True,
                                      ShortCircuit = False,
                                      Members = self.algs + [ self.streamLine.configurable() ] )
            else : 
                # If BadEventSelection or GoodEventSelection is used, need to create another 
                # "protection" sequencer. 

                lineSeq = GaudiSequencer("StrippingProtectedSequence"+self.name(),
                                          ModeOR = True,
                                          ShortCircuit = False,
                                          Members = self.algs + [ self.streamLine.configurable() ] )

        	self.seq = GaudiSequencer("StrippingSequenceStream" + self.name(), 
        	                          Members = [ self.eventSelectionLine.configurable(), lineSeq ] )

	        # If bad event selection is specified, the sequencer should work in OR mode, 
	        # so that stream sequencer is called only for good events. 

		if self.badEventSelection != None : 
            	    self.seq.ModeOR = True 
        	    self.seq.ShortCircuit = True

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
