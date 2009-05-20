## $Id: DaVinci-ReadETC.py,v 1.6 2009-05-20 14:18:08 pkoppenb Exp $
## ============================================================================
## CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.6 $
## ============================================================================
from Gaudi.Configuration import *
from Configurables import DaVinci
####################################################
importOptions("$STRIPPINGSELECTIONSROOT/options/StrippingBd2KstarMuMu.py")
####################################################

DaVinci().EvtMax = -1
DaVinci().PrintFreq  = 1
DaVinci().DataType = "2008"

# the xml has been generated with the command : dirac-lhcb-generate-catalog LFN:/lhcb/MC/2008/DST/00003401/0000/00003401_0000000{1,2,3,4,5,6,7,9}_5.dst

DaVinci().Input   = [
    "COLLECTION='TagCreator/EventTuple' DATAFILE='DVPresel_ETC2.root' TYP='POOL_ROOT' SEL='(PreselBd2KstarMuMu>0)'" ]
from Configurables import  TagCollectionSvc

ApplicationMgr().ExtSvc  += [ TagCollectionSvc("EvtTupleSvc") ]

FileCatalog().Catalogs = [ "xmlcatalog_file:options/2008-InclJpsiDst.xml" ]
