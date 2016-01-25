#Configuration file for MC generic RIVET analyses run on LHCb MB events
from Gauss.Configuration import *
from GaudiKernel.Configurable import *
from GaudiKernel import SystemOfUnits
from Configurables import RivetAnalysisHandler
#Needed to add local directory path to RivetAnalysisHandler options
import os

importOptions("$APPCONFIGOPTS/Gauss/Beam3500GeV-md100-2011-nominalBeamLine-fix1.py")
importOptions("$GAUSSOPTS/GenStandAlone.py")
importOptions("$LBPYTHIA8ROOT/options/Pythia8.py")
#importOptions("$APPCONFIGOPTS/Gauss/Pythia8_CT09MCS_Tune1_v48r3.py")
importOptions("$DECFILESROOT/options/SwitchOffEvtGen.py")

GaussGen = GenInit("GaussGen")
GaussGen.FirstEventNumber = 1
GaussGen.RunNumber        = 1082

LHCbApp().EvtMax = 500
#LHCbApp().OutputLevel = WARNING
LHCbApp().OutputLevel = INFO
Gauss().Histograms = "NONE"
Gauss().OutputType = "NONE"
#Gauss().OutputType = "GEN"
Gauss().DatasetName = "GaussTestMC"
#force head on collisions:
#Gauss().BeamCrossingAngle = 0.0
#Gauss().BeamBetaStar = 0.0

GenMonitor = GaudiSequencer( "GenMonitor" )
GenMonitor.Members += [ "RivetAnalysisHandler" ]
rivet = RivetAnalysisHandler()
rivet.BaseFileName = "LHCb_MC_test"
rivet.RunName = "LHCbMBPy8"
##rivet.RunName = ""
rivet.Analyses = ["MC_LHCb_GENERIC","MC_IDENTIFIED"]
#to search Rivet plugins in current directory
rivet.AnalysisPath += [os.path.abspath('.'),]
#rivet.AnalysisPath += [os.path.abspath(os.getcwd()),]
#rivet.forceXSection = True
#rivet.xSectionValue = 91.1*SystemOfUnits.millibarn
rivet.CorrectStatusID = True
rivet.CorrectCrossingAngles = True

HistogramPersistencySvc().OutputFile = ''
#HistogramPersistencySvc().OutputFile = 'mytest.root'
