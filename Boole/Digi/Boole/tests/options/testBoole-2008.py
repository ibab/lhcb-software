##############################################################################
# File for running a Boole test with default 2008 geometry
##############################################################################

from Boole.Configuration import *

Boole().EvtMax       = 10
Boole().useSpillover = False
Boole().DDDBtag      = "2008-default"
Boole().condDBtag    = "2008-default"
Boole().writeL0ETC   = True   # Write also ETC of L0 selected events
Boole().monitors    += ["SC"] # Add StatusCode check

Boole().applyConf()

# Do not compare times....
TimingAuditor().OutputLevel = WARNING 

datasetName = '11144103-100ev-20080613'
EventSelector().Input = ["DATAFILE='PFN:castor:/castor/cern.ch/user/g/gcorti/Gauss/2008/v31r2/" + datasetName + ".sim' TYP='POOL_ROOTTREE' OPT='READ'"]

# Set the property used to build other file names
Boole().setProp( "datasetName", datasetName )
outputName = Boole().outputName()

#-- Save the monitoring histograms
HistogramPersistencySvc().OutputFile = Boole().histosName()

#-- Digi output stream
OutputStream("DigiWriter").Output = "DATAFILE='PFN:" + outputName + ".digi' TYP='POOL_ROOTTREE' OPT='REC'"

#-- ETC output stream
TagCollectionStream( "WR" ).Output = "Collection='EVTTAGS/TagCreator/1' ADDRESS='/Event' DATAFILE='" + datasetName + "-L0ETC.root' TYP='POOL_ROOTTREE' OPT='RECREATE'"
