## $Id: DVTestAllPreselections.py,v 1.4 2009-03-24 10:43:21 pkoppenb Exp $
## ============================================================================
## CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.4 $
## ============================================================================
from Gaudi.Configuration import *
importOptions("$STDOPTS/PreloadUnits.opts")
from Configurables import DaVinci
DaVinci().MainOptions  = "$STRIPPINGROOT/options/Presel.opts"
DaVinci().EvtMax = 100
DaVinci().PrintFreq  = 1
DaVinci().InputType = "RDST"

##-- GAUDI data cards generated on 9/15/07 3:53 PM
##-- For Event Type = 10000000 / Data type = RDST 1
##--     Configuration = DC06 - phys-v4-lumi2
##--     RDST 1 datasets produced by Brunel - v30r17
##--     From DIGI 1 datasets produced by ANY
##--     From SIM 1 datasets produced by ANY
##--     Database version = Default
##--     Cards content = physical-physical
##--     
##-- Datasets replicated at CERN
##-- 200 dataset(s) - NbEvents = 1987826
##-- 

DaVinci().Input = [
    "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v4-lumi2/00001896/RDST/0000/00001896_00000004_1.rdst' TYP='POOL_ROOTTREE' OPT='READ'",
    "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v4-lumi2/00001896/RDST/0000/00001896_00000006_1.rdst' TYP='POOL_ROOTTREE' OPT='READ'",
    "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v4-lumi2/00001896/RDST/0000/00001896_00000011_1.rdst' TYP='POOL_ROOTTREE' OPT='READ'",
    "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v4-lumi2/00001896/RDST/0000/00001896_00000012_1.rdst' TYP='POOL_ROOTTREE' OPT='READ'",
    "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v4-lumi2/00001896/RDST/0000/00001896_00000015_1.rdst' TYP='POOL_ROOTTREE' OPT='READ'",
    "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v4-lumi2/00001896/RDST/0000/00001896_00000019_1.rdst' TYP='POOL_ROOTTREE' OPT='READ'",
    "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v4-lumi2/00001896/RDST/0000/00001896_00000022_1.rdst' TYP='POOL_ROOTTREE' OPT='READ'" ]
    
########################################################################
MessageSvc().Format = "% F%60W%S%7W%R%T %0W%M"
