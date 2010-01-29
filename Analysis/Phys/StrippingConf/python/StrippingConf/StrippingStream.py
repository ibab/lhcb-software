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
        
    def appendLines (self, lines) : 
	for line in lines : 
	    self.lines().append(line)
	    line.declareAppended()
	    
    def createConfigurables(self, TES = False) :
        from Configurables import StrippingCheck
	for line in self.lines() : 
	    if TES :
		self.sequence().Members += [ StrippingCheck(line.name(), 
				     InputLocation = "/Event/Strip/" + line.outputLocation() + "/Particles") ]
	    else :  
		line.createConfigurable()
		self.sequence().Members += [ line.configurable() ]

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
	    if output : 
		outputList.append(output)
	return outputList
