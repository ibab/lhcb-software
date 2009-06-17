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
		    "MainOptions"	: "$STRIPPINGSELECTIONSROOT/options/StrippingSelections.py" # Main options file to import
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

	output = (self.getProp('OutputType')).upper()

        if output not in [ "ETC", "DST", "NONE" ]:
            raise TypeError( "Invalid output type '%s'"%output )

# always define stripping lines

#        DaVinci().appendToMainSequence( [ GaudiSequencer("StrippingLineSequencer") ] )

	if output == "ETC" : 

#           The user wants to write ETC. 
#           Selections of all active stripping lines will go there. 
#           No division into streams at this point.

	    tag = EventTuple("TagCreator")
	    tag.EvtColsProduce = True
	    tag.ToolList = [ "TupleToolEventInfo", "TupleToolRecoStats", "TupleToolSelResults"  ]
	    tag.addTool(TupleToolSelResults())

#
#           Create a FilterDesktop for each selection just to make a more uniform selection 
#           names for ETC. This could be just
#	      tag.TupleToolSelResults.Selections = [ i.outputSelection() for i in lines ] 
#           but the outputSelection names can be weird. Is there a better way? 
#

	    seq = GaudiSequencer("StrippingGlobal")
	    seq.ModeOR = TRUE
	    for i in lines : 
	        name = i.name() + "Decision"
	        log.info("Created decision " + name)
		tag.TupleToolSelResults.Selections += [ i.name() ] 
		seq.Members += [ i.configurable() ] 
		
	    tag.TupleToolSelResults.Selections += ["StrippingGlobal"]

	    DaVinci().appendToMainSequence( [ seq ] )
	    DaVinci().appendToMainSequence( [ tag ] )
	
	if output == "DST" : 

#           The user wants to write DST. 
#           Selections of all active stripping lines will go into separate DST for each stream. 

	    streams = self.activeStreams()
	    dstPrefix = self.getProp("DSTPrefix")
	    
	    log.info(streams)
	    for i in streams : 
		seq = GaudiSequencer("StreamSequencer"+i)
		seq.ModeOR = 1    # Event is selected if at least one selection in the stream is True
		for line in lines : 
		    if line.stream() == i :
			seq.Members += [ line.configurable() ] 
		DaVinciWriteDst().DstFiles[ dstPrefix + i + ".dst" ] = seq

	if output == "NONE" : 

	    seq = GaudiSequencer("StreamSequencer")
	    seq.IgnoreFilterPassed = TRUE
	    for i in lines : 
		seq.Members += [ i.configurable() ] 

	    DaVinci().appendToMainSequence( [ seq ] )
	    