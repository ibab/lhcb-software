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
from Configurables import FilterDesktop
from Configurables import LHCbConfigurableUser
from Configurables import EventTuple, TupleToolSelResults
from Configurables import DaVinci, DaVinciWriteDst
from StrippingConf.StrippingLine import StrippingLine
from StrippingConf.StrippingLine import strippingLines
from StrippingConf.StrippingStream import StrippingStream

class StrippingConf( LHCbConfigurableUser ):
#    __used_configurables__ = [ DaVinci, DaVinciWriteDst ]
    __used_configurables__ = [ DaVinci ]

    __slots__ = { 
		    "ActiveLines"       : [],    # list of lines to be added
		    "ActiveStreams"     : [],    # list of active streams
		    "OutputType"	: "ETC", # Output type. Can be either "ETC" or "DST"
		    "DSTPrefix"		: "",    # Prefix for DST streams
		    "MainOptions"	: "$STRIPPINGSELECTIONSROOT/options/StrippingSelections.py", # Main options file to import
		    "StreamFile"	: {}, 
		    "StreamList"	: []
                }

#
# Return the list of all active lines. The line is active if:
#   * It is listed in ActiveLines (or ActiveLines is empty) AND 
#   * Its stream is listed in the ActiveStreams (or ActiveStreams is empty)
#
    def activeLines (self) : 
        linesList = self.getProp('ActiveLines')
        streamsList = self.getProp('ActiveStreams')
        lines = [ i for i in strippingLines() if ( (not linesList   or i.subname() in linesList ) and 
    						   (not streamsList or i.stream() in streamsList )) ]
	return lines
#
# Return the list of all active StrippingStreams. 
#
    def activeStreams (self) : 
	return self.StreamList

#
# Configuration method
#
    def __apply_configuration__ (self): 

	log.info("Stripping configuration")

#       Import main options file (descriptions of all lines)
        importOptions(self.getProp('MainOptions'))

#	Create a list of active stream names
        lines = self.activeLines()
        streams = []
        for line in lines : 
    	    if line.stream() not in streams : 
    		streams.append(line.stream())
    		log.info("StrippingConf: New active stream : "+line.stream())

#	Create streams
	self.StreamList = []
    	for streamName in streams : 
    	    self.StreamList.append(StrippingStream(name = streamName))
    	    log.info("StrippingConf: Created stream : "+streamName)

#	Append lines to streams
	for line in lines : 
	    for stream in self.StreamList :
		if line.stream() == stream.name() :
		    stream.appendLine(line)
		    log.info("StrippingConf: Appended line "+line.name()+" to stream "+line.stream())
        
	output = (self.getProp('OutputType')).upper()

        if output not in [ "ETC", "DST", "NONE" ]:
            raise TypeError( "Invalid output type '%s'"%output )

	if output == "ETC" : 

#           The user wants to write ETC. 
#           Selections of all active stripping lines will go there. 

	    tag = EventTuple("TagCreator")
	    tag.EvtColsProduce = True
	    tag.ToolList = [ "TupleToolEventInfo", "TupleToolRecoStats", "TupleToolSelResults"  ]
	    tag.addTool(TupleToolSelResults)

#           Sequencer for the global selection (logical OR of all selections)

	    strippingSeq = GaudiSequencer("StrippingGlobal")
	    strippingSeq.ModeOR = True
	    strippingSeq.ShortCircuit = False
	    tag.TupleToolSelResults.Selections += [ "StrippingGlobal" ]

	    for stream in self.activeStreams() : 
		tag.TupleToolSelResults.Selections += [ stream.sequence().name() ]
		log.info("StrippingConf: Stream "+stream.name()+" sequencer is "+stream.sequence().name())
		strippingSeq.Members += [ stream.sequence() ]
		for line in stream.lines() : 
		    tag.TupleToolSelResults.Selections += [ line.name() ]
		    log.info("StrippingConf: added selection "+line.name())
		print stream.outputLocations()

	    DaVinci().appendToMainSequence( [ strippingSeq ] )
	    DaVinci().appendToMainSequence( [ tag ] )
	
	if output == "DST" : 

#           The user wants to write DST. 
#           Selections of all active stripping lines will go into separate DST for each stream. 

	    dstPrefix = self.getProp("DSTPrefix")
	    streamFile = self.getProp("StreamFile")
	    
	    for stream in self.activeStreams() : 
	    
		if stream.name() not in streamFile : 
		    log.info("Output file for stream "+stream.name()+" not defined. Using default. ")
		    dstName = dstPrefix + stream.name() + ".dst"
		else :
		    dstName = dstPrefix + streamFile[stream.name()]

		DaVinciWriteDst().DstFiles[ dstName ] = stream.sequence()

	if output == "NONE" : 

	    strippingSeq = GaudiSequencer("StrippingGlobal")
	    strippingSeq.ModeOR = True
	    strippingSeq.ShortCircuit = False

	    for stream in self.activeStreams() : 
		strippingSeq.Members += [ stream.sequence() ] 

	    DaVinci().appendToMainSequence( [ strippingSeq ] )
