"""
High level configuration tools for StrippingConf, to be invoked by Moore and DaVinci
"""

__author__  = "Anton Poluektov <A.O.Poluektov@warwick.ac.uk>"

#
# Configuration of stripping framework. 
#

from os import environ
from pprint import *
from Gaudi.Configuration import *
from LHCbKernel.Configuration import *
from GaudiConf.Configuration import *
from Configurables import GaudiSequencer
from Configurables import LHCbConfigurableUser
from StrippingLine import StrippingLine
from StrippingStream import StrippingStream

class StrippingConf ( LHCbConfigurableUser ) :

    __slots__ = {
		  "TES"        	      : False     # If True, just check that TES location is not empty, 
		                                  # instead of running selections (used for SETC tagger)
		}

    def __apply_configuration__ ( self ) :
        log.info("Configuring StrippingConf")

#
# Return the list of all active StrippingStreams. 
#
    def activeStreams (self) :
        return GaudiSequencer(self.name() + "ActiveStreams",
                              ModeOR = True,
                              ShortCircuit = False).Members

#
# Return GaudiSequencer containing stream sequencers
#
    def sequence (self) : 
	return GaudiSequencer("StrippingGlobal",
	                      ModeOR = True, 
	                      ShortCircuit = False)


#
# Return list of selection names for TagCreator
#
    def selections (self) : 
	_selections = [ "StrippingGlobal" ]
	for stream in self.activeStreams() : 
	    streamName = stream.sequence().name()
	    _selections += [ streamName ]
	    for line in stream.lines() :
		_selections += [ line.name() ]
	return _selections

#
# Append StrippingStream
#
    def appendStream(self, stream) : 
	stream.createConfigurables(self.getProp("TES"))
	self.activeStreams().append(stream)
	self.sequence().Members += [ stream.sequence() ]
