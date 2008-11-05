##############################################################################
#$Id: DVTestB2DiMuon.py,v 1.1 2008-11-05 14:49:49 jpalac Exp $
#
# Example Qm test option using configurables.
#
# Author: Juan Palacios <juan.palacios@nikhef.nl>
#
##############################################################################
from DaVinci.Configuration import DaVinciApp
DaVinciApp().EvtMax = 500
# DaVinciApp().DDDBtag      = "DC06-default"
# DaVinciApp().condDBtag    = "DC06-default"
# DaVinciApp().useOracleCondDB = False
DaVinciApp().mainOptions  = "$DAVINCIROOT/tests/options/DVTestB2DiMuon.opts"
DaVinciApp().Input = [
    "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-lumi2/00001586/DST/0000/00001586_00000001_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
    "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-lumi2/00001586/DST/0000/00001586_00000002_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
    "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-lumi2/00001586/DST/0000/00001586_00000003_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
    "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-lumi2/00001586/DST/0000/00001586_00000004_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
    "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-lumi2/00001586/DST/0000/00001586_00000005_5.dst' TYP='POOL_ROOTTREE' OPT='READ'"]
##############################################################################
DaVinciApp().applyConf()
##############################################################################
