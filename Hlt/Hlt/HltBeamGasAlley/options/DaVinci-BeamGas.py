###############################################################
# Essential job-options to run the beam-gas alleys in DaVinci #
###############################################################

from os import environ
from Gaudi.Configuration import *
from Configurables import GaudiSequencer

########################################################################
importOptions( "$DAVINCIROOT/options/DaVinciCommon.opts" )

# Remove one of the standard sequences in DaVinciMainSeq
# Because it causes a crash and we don't need it
GaudiSequencer('DaVinciMainSeq').Members.remove('GaudiSequencer/ProtoPRecalibration')
########################################################################

# WE Dont't need nothing from the TRIGGER ?

#
# Trigger. Uncomment what you need. Hlt1 needs L0, Hlt2 doesn't.
#
#from HltConf.Configuration import HltConf
#
## enable if you want to rerun L0
#HltConf().replaceL0BanksWithEmulated = True
#
## pick one of 'Hlt1', 'Hlt2', or 'Hlt1+Hlt2'
#HltConf().hltType = 'Hlt1'
#HltConf().hltType = 'Hlt2'
#HltConf().hltType = 'Hlt1+Hlt2'
## don't forget to actually apply the configuration!!!
#HltConf().applyConf()
#
#########################################################################
# enable standard particles monitors#
#importOptions( "$COMMONPARTICLESROOT/options/EnableStandardParticleMonitors.opts" )
#########################################################################

#############################
# Inlude the BeamGas Alleys #
#############################
importOptions('../options/BeamGasAlleys.py')


#########################################################################
# Histogram File; Nothing interesting is produced by the algorithms
HistogramPersistencySvc().OutputFile = "BeamGasHistos_1.root"
#########################################################################

#########################################################################
# Input Events
#########################################################################
ApplicationMgr().EvtMax = 100
EventSelector().FirstEvent = 1
EventSelector().PrintFreq  = 50

#MinBias L0-passed
#EventSelector().Input = ["DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001888/DST/0000/00001888_00000011_5.dst' TYP='POOL_ROOTTREE' OPT='READ'"]

#BeamGas with pp; 1291 events
EventSelector().Input = ["DATAFILE='PFN:rfio:/afs/cern.ch/lhcb/group/vertex/vol4/massi/BEAMGAS_MINBIAS/DIGI/pseudol0/beamgas.digi' TYP='POOL_ROOTTREE' OPT='READ'"]

#BeamGas only
#EventSelector().Input = ["DATAFILE='PFN:castor:/castor/cern.ch/user/m/massi/BOOLE/beam12only/inelastic/beam12only.digi' TYP='POOL_ROOTTREE' OPT='READ'"]
 
#133 Nasty Background events. A subset from 3M MBL0 events (the same sample as above - DC06-L0-v1-lumi2) 
#EventSelector.Input = ["DATAFILE='PFN:rfio:/afs/cern.ch/lhcb/group/vertex/vol6/phopchev/myDSTFiles/backup/nastyBGr-3M-maxBinMoreThan_4-fsc0-cc0.dst' TYP='POOL_ROOTTREE' OPT='READ'"]

 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
