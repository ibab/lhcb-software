## $Id: DaVinci-ReadETC.py,v 1.8 2009-06-23 13:06:35 pkoppenb Exp $
## ============================================================================
## CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.8 $
## ============================================================================
#
# Example options to read a user-defined ETC
#
from Gaudi.Configuration import *
from Configurables import DaVinci
####################################################
importOptions("$STRIPPINGSELECTIONSROOT/options/StrippingBs2JpsiPhi.py")  # import selections, but don't run stripping
from Configurables import StrippingAlg
selection = StrippingAlg("StrippingBs2JpsiPhiLine")
####################################################

DaVinci().EvtMax = -1
DaVinci().PrintFreq  = 1
DaVinci().DataType = "MC09"
DaVinci().UserAlgorithms = [ selection ]

DaVinci().Input   = [
    "COLLECTION='TagCreator/EventTuple' DATAFILE='DVPresel_ETC2.root' TYP='POOL_ROOT' SEL='(StripBs2JpsiPhi>0.5)'" ]

from Configurables import  TagCollectionSvc
ApplicationMgr().ExtSvc  += [ TagCollectionSvc("EvtTupleSvc") ]

FileCatalog().Catalogs = [ "xmlcatalog_file:$DAVINCIROOT/options/MC09-Bs2JpsiPhiDst.xml" ]
