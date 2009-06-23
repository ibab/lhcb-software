## $Id: DaVinci-WriteETC.py,v 1.6 2009-06-23 13:06:35 pkoppenb Exp $
## ============================================================================
## CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.6 $
## ============================================================================
#
# Example options to create a user-defined ETC
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
importOptions("$STRIPPINGSELECTIONSROOT/options/StrippingBs2JpsiPhi.py")  # import one selection, but don't run whole stripping
from Configurables import StrippingAlg
selection = StrippingAlg("StrippingBs2JpsiPhiLine")
####################################################
# main config
from Configurables import DaVinci
DaVinci().EvtMax = 1000
DaVinci().PrintFreq  = 100
DaVinci().DataType = "MC09"
DaVinci().UserAlgorithms = [ selection ]
DaVinci().MoniSequence = [ tag ]
DaVinci().ETCFile = "DVPresel_ETC2.root"

importOptions( "$DAVINCIROOT/options/MC09-Bs2JpsiPhiDst.py")
