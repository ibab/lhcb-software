#
#   Stripping selections test job
#
#   @author A. Poluektov
#   @date 2009-05-14
#

from Gaudi.Configuration import *
from Configurables import GaudiSequencer

MessageSvc().Format = "% F%60W%S%7W%R%T %0W%M"

#
# Stripping job configuration
#

from StrippingConf.Configuration import StrippingConf
from StrippingSelections.Streams import allStreams

sc = StrippingConf( Streams = allStreams)

from Configurables import EventTuple, TupleToolSelResults

tag = EventTuple("TagCreator")
tag.EvtColsProduce = True
tag.ToolList = [ "TupleToolEventInfo", "TupleToolRecoStats", "TupleToolSelResults"  ]
tag.addTool(TupleToolSelResults)

tag.TupleToolSelResults.Selections = sc.selections()  # Add the list of stripping selections to TagCreator

#
# Configure DaVinci
#

from Configurables import DaVinci

DaVinci().appendToMainSequence( [ sc.sequence() ] )   # Append the stripping selection sequence to DaVinci
DaVinci().appendToMainSequence( [ tag ] )             # Append the TagCreator to DaVinci
DaVinci().EvtMax = 100                         # Number of events
DaVinci().DataType = "2010"      
DaVinci().Simulation   = True                  # It is MC
DaVinci().ETCFile = "etc.root"

#-- GAUDI jobOptions generated on Mon Jan 17 17:13:58 2011
#-- Contains event types : 
#--   13144006 - 1 files - 10000 events - 4.13 GBytes
# MC 2010 Beam3500GeV-VeloClosed-MagDown-Nu3
# bs->Jpsi, Phi
from Gaudi.Configuration import * 

EventSelector().Input   = [
    "   DATAFILE='root://castorlhcb.cern.ch//castor/cern.ch/grid/lhcb/MC/2010/DST/00008405/0000/00008405_00000506_1.dst?svcClass=lhcbdata' TYP='POOL_ROOTTREE' OPT='READ'"]
