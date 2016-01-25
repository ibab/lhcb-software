# Configuration file for MC_LHCb_GENERIC RIVET analysis run on LHCb minbias events
from GaudiKernel import SystemOfUnits as units
from Gaudi.Configuration import *
from LHCbKernel.Configuration import *
from Gauss.Configuration import *
from Configurables import Generation, Gauss
from Configurables import RivetAnalysisHandler
# settings database tags independently
from Configurables import DDDBConf, CondDB, LHCbApp
#Need os. to add local directory path to RivetAnalysisHandler options
import os
#importOptions("$GAUSSOPTS/Gauss-DEV.py")

LHCbApp().DDDBtag   = "dddb-20130111"
DDDBConf(DataType = "2012")
CondDB().UseLatestTags = ["2012"]

# End of databases setup
# special beam conditions used for pp collisions at 7 TeV
importOptions("$APPCONFIGOPTS/Gauss/Beam3500GeV-uniformHeadOn-fix1.py")
#importOptions("$DECFILESROOT/options/30000000.py")
importOptions("$LBPYTHIA8ROOT/options/Pythia8.py")
importOptions("$GAUSSOPTS/GenStandAlone.py")
#importOptions("$DECFILESROOT/options/SwitchOffEvtGen.py")
importOptions("$GAUSSOPTS/Gauss-Job.py")

#GaussGen = GenInit("GaussGen")
#GaussGen.RunNumber = 1082
LHCbApp().EvtMax = 1000
LHCbApp().OutputLevel = INFO
Gauss().Histograms = "NONE"
Gauss().OutputType = "NONE"
Gauss().DatasetName = "myRivetGauss"
# force head on collisions:
#Gauss().BeamCrossingAngle = 0.0
# eliminate smearing of crossing angles
#Gauss().BeamBetaStar = 0.0

GenMonitor = GaudiSequencer( "GenMonitor" )
GenMonitor.Members += [ "RivetAnalysisHandler", ]
rivet = RivetAnalysisHandler()
# the filename of the .aida output (w/o extension!)
rivet.BaseFileName = "myRivetGaussMC"
# head of branch where histograms are stored in AIDA
rivet.RunName = "LHCbMBPy8"
rivet.Analyses = ["MC_LHCb_GENERIC",]
# to search Rivet plugins in current directory
rivet.AnalysisPath += [os.path.abspath('.'),]
# the following options allow setting externally 
# a cross-section value when it cannot be read from HepMC
# (mostly necessary if/when using HepMC 2.04)
rivet.forceXSection = False
#rivet.forceXSection = True
#rivet.xSectionValue = 32.123 * units.millibarn
# instruct the algorithm to correct the particle status IDs and 
# apply the crossing angle correction when necessary
rivet.CorrectStatusID = True
rivet.CorrectCrossingAngles = True
#rivet.CorrectCrossingAngles = False

HistogramPersistencySvc().OutputFile = ''
