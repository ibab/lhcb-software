## $Id: DaVinci-ReadETC.py,v 1.7 2009-06-18 12:52:52 pkoppenb Exp $
## ============================================================================
## CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.7 $
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
    "COLLECTION='TagCreator/EventTuple' DATAFILE='~/DVPresel_v23r1p1.root' TYP='POOL_ROOT' SEL='(StrippingB2Charged2Body>0) || (StrippingTopo>0)'" ]
from Configurables import  TagCollectionSvc

ApplicationMgr().ExtSvc  += [ TagCollectionSvc("EvtTupleSvc") ]

FileCatalog().Catalogs = [ "xmlcatalog_file:options/2008-InclJpsiDst.xml" ]
