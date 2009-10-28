"""
High level configuration tools for StrippingConf, to be used with Moore and DaVinci
"""

__author__  = "Anton Poluektov <A.O.Poluektov@warwick.ac.uk>"

#
# Configuration of stripping framework. 
#

from os import environ
from pprint import *
from Gaudi.Configuration import *

class StrippingConf ( object ) :

    def __init__( self,
                  name = "",
                  TES = False,
                  Streams = [] ) :
        
        log.info("Initialising StrippingConf "+ name)
        if name == "" :
            self._name = "StrippingGlobal"
        else :
            self._name = name
        self.TES = TES
        self._streams = []
        self._streamSequencers = []
        self._sequence = None
        for stream in Streams :
            self.appendStream(stream)

    def activeStreams (self) :
        """
        Return the list of all active StrippingStreams. 
        """
        return self._streams

    def sequence (self) :
        """
        Return GaudiSequencer containing stream sequencers
        """
        if self._sequence == None :
            from Configurables import GaudiSequencer
            log.info("Initialising GaudiSequencer/"+ self._name)
            self._sequence = GaudiSequencer(self._name,
                                            ModeOR = True, 
                                            ShortCircuit = False,
                                            Members = self._streamSequencers)
        return self._sequence

    def selections (self) :
        """
        Return list of selection names for TagCreator
        """
        _selections = [ self._name ]
        for stream in self._streams : 
            streamName = stream.sequence().name()
            _selections.append(streamName)
            for line in stream.lines() :
                _selections.append(line.name())
        return _selections

    def appendStreams(self, streams) :
        """
        Append a list of streams to the configuration.
        """
        for stream in streams :
            self.appendStream(stream)
            
    def appendStream(self, stream) :
        """
        Append a StrippingStream to the configuration. Takes care of adding
        corresponding sequencer to _streamSequencers list.
        """
        log.info(self._name+ " appending stream "+ stream.name())
	stream.createConfigurables( self.TES )
	self._streams.append(stream)
        self._appendSequencer(stream)
        
    def _appendSequencer(self, stream) :
        self._streamSequencers.append(stream.sequence())
