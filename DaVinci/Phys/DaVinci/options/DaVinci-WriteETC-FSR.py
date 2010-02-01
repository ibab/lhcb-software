## $Id: DaVinci-WriteETC-FSR.py,v 1.1 2010-02-01 12:52:36 panmanj Exp $
## ============================================================================
## CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.1 $
## ============================================================================
#
# Example options to create a user-defined ETC
#
# @added FSR writing: Jaap Panman
#
from Gaudi.Configuration import *
######################################################
# StripETC options
#

from Configurables import EventTuple, TupleToolSelResults

tag = EventTuple("TagCreator")
tag.EvtColsProduce = True
tag.ToolList = [ "TupleToolEventInfo", "TupleToolRecoStats", "TupleToolSelResults"  ]
tag.addTool(TupleToolSelResults )
tag.TupleToolSelResults.Selections  = [ "StripBs2JpsiPhi" ]

####################################################
importOptions( "$STRIPPINGSELECTIONSROOT/options/STEP09/StrippingBs2JpsiPhi.py")
from Configurables import StrippingAlg
selection = StrippingAlg("StrippingBs2JpsiPhiLine")
####################################################
# main config
from Configurables import DaVinci
DaVinci().EvtMax = 10
DaVinci().PrintFreq  = 1
DaVinci().DataType = "2009"
DaVinci().UserAlgorithms = [ selection ]
DaVinci().MoniSequence = [ tag ]
DaVinci().ETCFile = "DVPresel_ETC_SPAN_DV_FSR.root"
DaVinci().WriteFSR = True

# input file
EventSelector().Input   = [
    #"   DATAFILE='PFN:LumiData_062910_0000000001_fsr.dst' TYP='POOL_ROOTTREE' OPT='READ'"
    "   DATAFILE='PFN:nolumi_span_063815_0000000001.dst' TYP='POOL_ROOTTREE' OPT='READ'"
  ]
FileCatalog().Catalogs = [ "xmlcatalog_file:MyCatalog.xml",
                           ]
