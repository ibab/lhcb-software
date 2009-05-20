## $Id: DaVinci-WriteETC.py,v 1.5 2009-05-20 14:18:08 pkoppenb Exp $
## ============================================================================
## CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.5 $
## ============================================================================
from Gaudi.Configuration import *
######################################################
# StripETC options
#

from Configurables import EventTuple, TupleToolSelResults

tag = EventTuple("TagCreator")
tag.EvtColsProduce = True
tag.ToolList = [ "TupleToolEventInfo", "TupleToolRecoStats", "TupleToolSelResults"  ]
tag.addTool(TupleToolSelResults )
tag.TupleToolSelResults.Selections  = [ "StrippingB2DPiPostScaler", "StrippingB2KstarMuMu_10HzPostScaler" ]
####################################################
importOptions("$STRIPPINGSELECTIONSROOT/options/Stripping.py")
####################################################
# main config
from Configurables import DaVinci
DaVinci().EvtMax = 5000
DaVinci().PrintFreq  = 100
DaVinci().DataType = "2008"
DaVinci().MoniSequence = [ tag ]
DaVinci().ETCFile = "DVPresel_ETC2.root"
#
# The data that corresponds to 2008-InclJpsiDst.xml :
#
DaVinci().Input = [
"   DATAFILE='PFN:/castor/cern.ch/grid/lhcb/MC/2008/DST/00003401/0000/00003401_00000001_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
"   DATAFILE='PFN:/castor/cern.ch/grid/lhcb/MC/2008/DST/00003401/0000/00003401_00000002_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
"   DATAFILE='PFN:/castor/cern.ch/grid/lhcb/MC/2008/DST/00003401/0000/00003401_00000003_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
"   DATAFILE='PFN:/castor/cern.ch/grid/lhcb/MC/2008/DST/00003401/0000/00003401_00000004_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
"   DATAFILE='PFN:/castor/cern.ch/grid/lhcb/MC/2008/DST/00003401/0000/00003401_00000005_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
"   DATAFILE='PFN:/castor/cern.ch/grid/lhcb/MC/2008/DST/00003401/0000/00003401_00000006_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
"   DATAFILE='PFN:/castor/cern.ch/grid/lhcb/MC/2008/DST/00003401/0000/00003401_00000007_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
"   DATAFILE='PFN:/castor/cern.ch/grid/lhcb/MC/2008/DST/00003401/0000/00003401_00000009_5.dst' TYP='POOL_ROOTTREE' OPT='READ'"]

