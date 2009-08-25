#!/usr/bin/env python

#
# StrippingStream
#

__all__ = ( 
	    'StrippingStream',
	  )

from copy import deepcopy 
from Gaudi.Configuration import GaudiSequencer, Sequencer, Configurable

class StrippingStream ( object ) :

    __slots__ = ( 'Seq', 'Lines','Name')

    def __init__ ( self       ,    
                   name  = '' ) :
        self.Name  = deepcopy ( name  )
        self.Seq = GaudiSequencer("StrippingStream"+self.Name)
        self.Seq.ModeOR = True
        self.Seq.ShortCircuit = False
        self.Lines = []

    def appendLine (self, line) : 
	self.Seq.Members += [ line.configurable() ]
	self.Lines.append(line)

    def name   ( self ) :
        return self.Name

    def sequencer ( self ) : 
	return self.Seq

    def lines (self) : 
	return self.Lines

    def outputLocations (self) : 
	outputList = []
	for i in self.Lines :
	    output = i.outputSelection()
	    outputList.append("/Phys/"+output)
	return outputList
