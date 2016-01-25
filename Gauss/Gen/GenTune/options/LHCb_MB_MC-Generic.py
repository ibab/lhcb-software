# Configuration file for generic RIVET analyses run on LHCb MB events
# A fully working RIVET analysis module example can be found in 'example' sub-directory.
from GaudiKernel import SystemOfUnits as units
from Gaudi.Configuration import *
from Gauss.Configuration import *
from Configurables import RivetAnalysisHandler
#Needed to add local directory path to RivetAnalysisHandler options
import os
# Gaudi MessageService log levels
# 0 - NIL
# 1 - VERBOSE
# 2 - DEBUG
# 3 - INFO
# 4 - WARN
# 5 - ERROR
# 6 - FATAL
# 7 - ALWAYS

importOptions("$GAUSSOPTS/Gauss-DEV.py")
# Use a special kind of beam setting (single pp collision in CM frame === lab frame)
# - see special settings below if no such beam options exist for needed beam energy!
importOptions("$APPCONFIGOPTS/Gauss/Beam6500GeV-uniformHeadOn-fix1.py")
#importOptions("$DECFILESROOT/options/30000000.py") #minbias
importOptions("$LBPYTHIA8ROOT/options/Pythia8.py")
importOptions("$GAUSSOPTS/GenStandAlone.py")
#importOptions("$DECFILESROOT/options/SwitchOffEvtGen.py")
importOptions("$GAUSSOPTS/Gauss-Job.py")

GaussGen = GenInit("GaussGen")
GaussGen.RunNumber = 1082
LHCbApp().EvtMax = 10000
#LHCbApp().setProp("OutputLevel", 3)
#LHCbApp().setProp("OutputLevel", 4)
LHCbApp().OutputLevel = WARNING

Gauss().Histograms = "NONE"
Gauss().OutputType = "NONE"
Gauss().DatasetName = "GaussDevWHepMC"

#Special settings for beam conditions:

#** ignore multiple pp collisions, force nPV===1 (algorithm will complain if not)
from Configurables import Generation
gaussGen = Generation("Generation")
gaussGen.PileUpTool = "FixedNInteractions"

#** force head-on pp collisions:
# - theoreticians always assume pp collision is in CM frame
# - you may experience some problems in generating event with settings below (issue under investigation)
# - see CorrectCrossingAngles option below to control CM boost at runtime
#Gauss().BeamCrossingAngle = 0.0
#Gauss().BeamBetaStar = 0.0

GenMonitor = GaudiSequencer( "GenMonitor" )
GenMonitor.Members += [ "RivetAnalysisHandler" ]
rivet = RivetAnalysisHandler()
rivet.BaseFileName = "testBird"
rivet.RunName = "LHCbMBPy8"
rivet.Analyses = ["MC_GENERIC","MC_IDENTIFIED"]
#to search Rivet plugins in current directory (when developing/LbRivetPlugins)
#(no need if running pre-defined analyses as above)
#rivet.AnalysisPath += [os.path.abspath('.'),]
rivet.forceXSection = False
# Do not set any xsection value if running PYTHIA8 (provided by event generator)
#rivet.forceXSection = True
#rivet.xSectionValue = 32.123 * units.millibarn
#rivet.xSectionValue = 91.1 * units.millibarn
rivet.CorrectStatusID = True
#rivet.CorrectCrossingAngles = False
rivet.CorrectCrossingAngles = True

#HistogramPersistencySvc().OutputFile = rivet.RunName + '.root'
HistogramPersistencySvc().OutputFile = ''
