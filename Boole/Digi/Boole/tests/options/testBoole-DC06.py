##############################################################################
# File for running a Boole test with default DC06 geometry
##############################################################################

from Boole.Configuration import *

Boole().EvtMax       = 10
Boole().writeL0ETC   = True   # Write also ETC of L0 selected events
Boole().monitors    += ["SC"] # Add StatusCode check

Boole().applyConf()

# Do not compare times....
TimingAuditor().OutputLevel = WARNING 

#-- Main ('signal') event input
datasetName = "00001820_00000001"
EventSelector().Input = ["DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001820/SIM/0000/" + datasetName + "_1.sim' TYP='POOL_ROOTTREE' OPT='READ'"]

#-- Spillover events
EventSelector("SpilloverSelector").Input = [
    "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001820/SIM/0000/" + datasetName + "_2.sim' TYP='POOL_ROOTTREE' OPT='READ'"
   ,"DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001820/SIM/0000/" + datasetName + "_3.sim' TYP='POOL_ROOTTREE' OPT='READ'"
    ]

# Set the property used to build other file names
Boole().setProp( "datasetName", datasetName )
outputName = Boole().outputName()

#-- Save the monitoring histograms
HistogramPersistencySvc().OutputFile = Boole().histosName()

#-- Digi output stream
OutputStream("DigiWriter").Output = "DATAFILE='PFN:" + outputName + ".digi' TYP='POOL_ROOTTREE' OPT='REC'"

#-- ETC output stream
TagCollectionStream( "WR" ).Output = "Collection='EVTTAGS/TagCreator/1' ADDRESS='/Event' DATAFILE='" + datasetName + "-L0ETC.root' TYP='POOL_ROOTTREE' OPT='RECREATE'"
