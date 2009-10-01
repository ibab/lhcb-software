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

class StrippingStream ( LHCbConfigurableUser ) :

    __slots__ = { "Lines" : [] }

    def __apply_configuration__ ( self ) :
        log.info("Configuring StrippingStream " + self.name() )

    def appendLine (self, line) :
	self.sequence().Members += [ line.configurable() ]
	self.lines().append(line)


    def lines( self ) :
        return self.getProp('Lines')
    
    def sequence ( self ) : 
	return GaudiSequencer("StrippingStream"+self.name(),
                              ModeOR = True,
                              ShortCircuit = False)


    def outputLocations (self) : 
	outputList = []
	for i in self.lines() :
	    output = i.outputLocation()
	    outputList.append(output)
	return outputList
