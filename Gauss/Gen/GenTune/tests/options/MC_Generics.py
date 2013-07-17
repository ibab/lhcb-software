#Configuration file for MC generic RIVET analyses run on LHCb MB events
from Gauss.Configuration import *
from GaudiKernel.Configurable import *
from GaudiKernel import SystemOfUnits
from Configurables import RivetAnalysisHandler
#Needed to add local directory path to RivetAnalysisHandler options
import os

importOptions("$GENTUNEROOT/options/beamOpts/Beam3500GeV-md100-2011-nominalBeamLine-fix1.py")
importOptions("$GAUSSOPTS/GenStandAlone.py")
importOptions("$DECFILESROOT/options/30000000.py")
importOptions("$LBPYTHIAROOT/options/Pythia.py")
importOptions("$DECFILESROOT/options/SwitchOffEvtGen.py")

GaussGen = GenInit("GaussGen")
GaussGen.FirstEventNumber = 1
GaussGen.RunNumber        = 1082

LHCbApp().EvtMax = 250
LHCbApp().OutputLevel = WARNING
Gauss().Histograms = "NONE"
Gauss().OutputType = "NONE"
Gauss().DatasetName = "GaussTestMC"
#force head on collisions:
#Gauss().BeamCrossingAngle = 0.0
#Gauss().BeamBetaStar = 0.0

GenMonitor = GaudiSequencer( "GenMonitor" )
GenMonitor.Members += [ "RivetAnalysisHandler" ]
rivet = RivetAnalysisHandler()
rivet.BaseFileName = "LHCb_MC_hist"
rivet.RunName = "LHCbMBPy6"
rivet.Analyses = ["MC_GENERIC","MC_IDENTIFIED"]
rivet.StreamName = "/Rivet"
#to search Rivet plugins in current directory
rivet.AnalysisPath += [os.path.abspath('.'),]
rivet.forceXSection = True
rivet.xSectionValue = 91.1*SystemOfUnits.millibarn
rivet.CorrectStatusID = True
rivet.CorrectCrossingAngles = True

HistogramPersistencySvc().OutputFile = ''
