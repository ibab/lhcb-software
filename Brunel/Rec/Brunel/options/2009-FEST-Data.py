# Ecample options for processing FEST data taken in 2009

from Gaudi.Configuration import EventSelector
from Configurables       import Brunel

# Choose a run, number of files to process, number of events
run = 56721  # Run taken September 9th 2009, Boole data from spring 2009 
files = 10
Brunel().EvtMax = 1000

# Use default simulation database tags 
Brunel().Simulation = True

# Make sure Brunel is configured for MDF data
if Brunel().getProp("InputType").upper() == "MDF":
    for file in range(1,files+1):
        filename = "/castor/cern.ch/grid/lhcb/data/2009/RAW/FULL/FEST/FEST/%i/%06i_%010i.raw" % (run,run,file)
        EventSelector().Input += [ "DATAFILE='rfio:" + filename + "'  SVC='LHCb::MDFSelector'" ]
else:
    print "********************************************************************************"
    print "The 2009-FEST-Files.py files require Brunel be set up to process MDF data"
    print "********************************************************************************"
    import sys
    sys.exit()

# Default output files names are set up using value Brunel().DatasetName property
Brunel().DatasetName = "%i" % run
