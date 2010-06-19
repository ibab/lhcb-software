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
                   Lines =  [], 
                   BadEventSelection = None
                 ) :
        self.lines = copy(Lines)
        for line in Lines : 
    	    line.declareAppended()

        self.algs = []
        self.seq = None
        self._name = name
        self.streamLine = None
        self.badEventLine = None
        self.badEventSelection = BadEventSelection


    def name(self) :
        return self._name
    

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

        # Make the line to mark bad events (those satisfying BadEventSelection)

	if self.badEventSelection != None : 
	    self.badEventLine = StrippingLine("Stream"+self.name()+"BadEvent", 
	                                      checkPV = False, algos = [ self.badEventSelection ] )
	    self.badEventLine.createConfigurable( TESPrefix + "/" + HDRLocation )


    def sequence ( self ) :
        if self.seq == None :
            # members = self.algs
            # members.append(self.streamLine.configurable())

            if self.badEventLine == None : 
                # Sequencer for all line configurables, including the stream decision line

                self.seq = GaudiSequencer("StrippingSequenceStream"+self.name(),
                                      ModeOR = True,
                                      ShortCircuit = False,
                                      Members = self.algs + [ self.streamLine.configurable() ] )
            else : 
                # If BadEventSelection is used, need to create another "protection" sequencer, 
                # so that stream sequencer is called only for good events 

                lineSeq = GaudiSequencer("StrippingProtectedSequence"+self.name(),
                                          ModeOR = True,
                                          ShortCircuit = False,
                                          Members = self.algs + [ self.streamLine.configurable() ] )

        	self.seq = GaudiSequencer("StrippingSequenceStream" + self.name(), 
        	                          ModeOR = True, 
        	                          ShortCircuit = True, 
        	                          Members = [ self.badEventLine.configurable(), lineSeq ] )
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
