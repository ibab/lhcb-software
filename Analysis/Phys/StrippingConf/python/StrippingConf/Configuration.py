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

class StrippingConf( LHCbConfigurableUser ):
#    __used_configurables__ = [ DaVinci, DaVinciWriteDst ]
    __used_configurables__ = [ DaVinci ]

    __slots__ = { 
		    "ActiveLines"       : [],    # list of lines to be added
		    "ActiveStreams"     : [],    # list of active streams
		    "OutputType"	: "ETC", # Output type. Can be either "ETC" or "DST"
		    "DSTPrefix"		: "",    # Prefix for DST streams
		    "MainOptions"	: "$STRIPPINGSELECTIONSROOT/options/StrippingSelections.py", # Main options file to import
		    "StreamFile"	: {}
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
# Return the list of all active streams. Active streams are those for which 
# at least one active line exists. 
#
    def activeStreams (self) : 
        linesList = self.activeLines()
        streams = []
        for i in linesList : 
    	    if i.stream() not in streams :
    		streams.append(i.stream())
	return streams

#
# Configuration method
#
    def __apply_configuration__ (self): 

	log.info("Stripping configuration")

#       Import main options file (descriptions of all lines)
        importOptions(self.getProp('MainOptions'))

#       Attach configurables of all active stripping lines to sequencer

        lines = self.activeLines()
	streams = self.activeStreams()

	output = (self.getProp('OutputType')).upper()

        if output not in [ "ETC", "DST", "NONE" ]:
            raise TypeError( "Invalid output type '%s'"%output )

	if output == "ETC" : 

#           The user wants to write ETC. 
#           Selections of all active stripping lines will go there. 

# 	    Sequencer that will run all selections independently 
#            (thus IgnoreFilterPassed = TRUE)

	    strippingSeq = GaudiSequencer("StrippingSequencer")
	    strippingSeq.IgnoreFilterPassed = TRUE

	    tag = EventTuple("TagCreator")
	    tag.EvtColsProduce = True
	    tag.ToolList = [ "TupleToolEventInfo", "TupleToolRecoStats", "TupleToolSelResults"  ]
	    tag.addTool(TupleToolSelResults)

#           Sequencer for the global selection (logical OR of all selections)
#           We need this in addition to StrippingSequencer because in ModeOR=TRUE 
#           the sequencer will give up running the rest of algorithms after it found 
#           the one with positive decision. 

	    globalSeq = GaudiSequencer("StrippingGlobal")
	    globalSeq.ModeOR = TRUE
	    tag.TupleToolSelResults.Selections += ["StrippingGlobal"]

# 	    Define sequencers for stream selections
	    for stream in streams : 
		streamSeq = GaudiSequencer("StrippingStream"+stream)
		streamSeq.ModeOR = TRUE
		tag.TupleToolSelResults.Selections += [ "StrippingStream"+stream ]

#           Cycle through all selections are append them to sequencers
	    for i in lines : 
		tag.TupleToolSelResults.Selections += [ i.name() ] 
		strippingSeq.Members += [ i.configurable() ] 
		stream = i.stream();
		GaudiSequencer("StrippingStream"+stream).Members += [ i.configurable() ]
		globalSeq.Members += [ i.configurable() ]
		log.info("Line "+i.name()+" : Output selection - "+i.outputSelection())

	    strippingSeq.Members += [ globalSeq ]
	    for stream in streams : 
	        strippingSeq.Members += [ GaudiSequencer("StrippingStream"+stream) ]

	    DaVinci().appendToMainSequence( [ strippingSeq ] )
	    DaVinci().appendToMainSequence( [ tag ] )
	
	if output == "DST" : 

#           The user wants to write DST. 
#           Selections of all active stripping lines will go into separate DST for each stream. 

	    dstPrefix = self.getProp("DSTPrefix")
	    streamFile = self.getProp("StreamFile")
	    
	    log.info(streams)
	    for i in streams : 
	    
		if i not in streamFile : 
		    log.info("Output file for stream "+i+" not defined. Using default. ")
		    dstName = dstPrefix + i + ".dst"
		else :
		    dstName = dstPrefix + streamFile[i]

		seq = GaudiSequencer("StreamSequencer"+i)
		
		# Check if the file is already assigned to another stream
		if dstName in DaVinciWriteDst().DstFiles : 
		    log.info("Stream "+i+" will be written to already existing file "+dstName)
		    seq = DaVinciWriteDst().DstFiles[ dstName ]    # Use already defined sequencer

		else :

		    log.info("Stream "+i+" will be written to the new file "+dstName)
		    seq.ModeOR = 1    # Event is selected if at least one selection in the stream is True
		    DaVinciWriteDst().DstFiles[ dstName ] = seq

		for line in lines : 
		    if line.stream() == i :
			seq.Members += [ line.configurable() ]

	if output == "NONE" : 

	    seq = GaudiSequencer("StreamSequencer")
	    seq.IgnoreFilterPassed = TRUE
	    for i in lines : 
		seq.Members += [ i.configurable() ] 

	    DaVinci().appendToMainSequence( [ seq ] )
	    
