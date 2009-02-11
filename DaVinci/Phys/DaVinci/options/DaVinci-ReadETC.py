## $Id: DaVinci-ReadETC.py,v 1.2 2009-02-11 18:05:53 pkoppenb Exp $
## ============================================================================
## CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.2 $
## ============================================================================
from Gaudi.Configuration import *
from Configurables import DaVinci
from Configurables import CheckSelResult

DaVinci().MainOptions = "$B2DILEPTONROOT/options/DoPreselBd2KstarMuMu.opts"
DaVinci().EvtMax = -1
DaVinci().PrintFreq  = 1
DaVinci().DataType = "2008"

# the xml has been generated with the command : dirac-lhcb-generate-catalog LFN:/lhcb/MC/2008/DST/00003401/0000/00003401_0000000{1,2,3,4,5,6,7,9}_5.dst

EventSelector().Input   = [
    "COLLECTION='TagCreator/EventTuple' DATAFILE='DVPresel_ETC2.root' TYP='POOL_ROOT' SEL='(PreselBd2KstarMuMu>0)'" ]
from Configurables import  TagCollectionSvc

ApplicationMgr().ExtSvc  += [ TagCollectionSvc("EvtTupleSvc") ]

importOptions( "$GAUDIPOOLDBROOT/options/GaudiPoolDbRoot.opts")
FileCatalog().Catalogs = [ "xmlcatalog_file:options/2008-InclJpsiDst.xml" ]
