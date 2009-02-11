## $Id: DaVinci-WriteETC.py,v 1.2 2009-02-11 18:05:53 pkoppenb Exp $
## ============================================================================
## CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.2 $
## ============================================================================
from Gaudi.Configuration import *
######################################################
# StripETC options
#

from Configurables import EventTuple, TupleToolSelResults

tag = EventTuple("TagCreator")
tag.EvtColsProduce = True
tag.ToolList = [ "TupleToolEventInfo", "TupleToolRecoStats", "TupleToolSelResults"  ]
tag.addTool(TupleToolSelResults())
tag.TupleToolSelResults.Selections  = [ "PreselB2DiMuon", "PreselB2EMu", "PreselBd2KstarMuMu", "PreselBd2Kstaree", "PreselBs2GammaMuMu", "PreselBs2MuMu", "PreselBu2KMuMu", "PreselBu2LLK", "PreselBs2PhiEtac", "PreselHeavyDimuon", "PreselJpsi2ee", "PreselUnbiasedJpsi2ee", "PreselXb2GammaX", "PreselB2HH", "PreselBd2KsPiPi", "PreselBd2PhiKs", "PreselBd2PhiKst", "PreselBs2Kst0Kst0", "PreselBs2PhiPhi", "PreselBu2HHH", "PreselLambdaB2LambdaOmega", "PreselLambdaB2LambdaPhi", "PreselLambdaB2LambdaRho", "PreselBs2DsDs", "PreselBs2DsH", "PreselBs2DsMuX", "PreselBs2DsRho", "PreselBd2DstarMu", "PreselBd2DstarPi", "PreselDstar2D0Pi_D02HH", "PreselDstar2D0Pi_D02HHHH", "PreselDstarNoPID", "PreselBd2D0Kst_D02HH", "PreselBd2D0Kst_D02KsKK", "PreselBd2D0Kst_D02KsPiPi", "PreselBu2D0H_D02HH", "PreselBu2D0K_D02HHHH", "PreselBu2D0K_D02KMuNu", "PreselBu2D0K_D02KPiPi0", "PreselBu2D0K_D02KsKK", "PreselBu2D0K_D02KsKPi", "PreselBu2D0K_D02KsPiPi", "PreselBu2D0Mu", "PreselBu2D0Tau2PiPiPi", "PreselBd2DPi", "PreselBd2KPiPi0", "PreselBd2PiPiPi0", "PreselBd2Rho0Rho0" ]
####################################################

####################################################
# main config
importOptions("$DAVINCIROOT/options/PreloadUnits.opts")
from Configurables import DaVinci
DaVinci().MainOptions  = "$STRIPPINGROOT/options/Presel.opts"
DaVinci().EvtMax = 500
DaVinci().PrintFreq  = 1
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

