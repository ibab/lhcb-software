#Configuration file for generic RIVET analyses run on LHCb MB events
from Gaudi.Configuration import *
from Gauss.Configuration import *
from Configurables import Generation
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
importOptions("$DECFILESROOT/options/30000000.py")
importOptions("$LBPYTHIAROOT/options/Pythia.py")
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
#force head on collisions:
#Gauss().BeamCrossingAngle = 0.0
#Gauss().BeamBetaStar = 0.0

GenMonitor = GaudiSequencer( "GenMonitor" )
GenMonitor.Members += [ "RivetAnalysisHandler" ]
rivet = RivetAnalysisHandler()
rivet.BaseFileName = "testBird"
rivet.RunName = "LHCbMBPy6"
rivet.Analyses = ["MC_GENERIC","MC_IDENTIFIED"]
rivet.StreamName = "/Rivet"
#to search Rivet plugins in current directory
#rivet.AnalysisPath += [os.path.abspath('.'),]
rivet.forceXSection = False
#rivet.forceXSection = True
rivet.xSectionValue = 32.123 #mb (!) depends on beam options
rivet.CorrectStatusID = True
#rivet.CorrectCrossingAngles = False
rivet.CorrectCrossingAngles = True

#HistogramPersistencySvc().OutputFile = rivet.RunName + '.root'
HistogramPersistencySvc().OutputFile = ''
