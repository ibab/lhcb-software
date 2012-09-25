#Configuration file for MC generic RIVET analyses run on LHCb MB events
from Gaudi.Configuration import *
from LHCbKernel.Configuration import *
from Gauss.Configuration import *
from Configurables import Generation
from Configurables import RivetAnalysisHandler
#Needed to add local directory path to RivetAnalysisHandler options
import os
importOptions("$GAUSSOPTS/Gauss-DEV.py")
importOptions("$DECFILESROOT/options/30000000.py")
importOptions("$LBPYTHIAROOT/options/Pythia.py")
importOptions("$GAUSSOPTS/GenStandAlone.py")
#importOptions("$DECFILESROOT/options/SwitchOffEvtGen.py")
importOptions("$GAUSSOPTS/Gauss-Job.py")

#GaussGen = GenInit("GaussGen")
#GaussGen.RunNumber = 1082
LHCbApp().EvtMax = 1000
LHCbApp().OutputLevel = WARNING
Gauss().Histograms = "NONE"
Gauss().Output = "NONE"
Gauss().DatasetName = "myRivetGauss"
#force head on collisions:
#Gauss().BeamCrossingAngle = 0.0
#Gauss().BeamBetaStar = 0.0

GenMonitor = GaudiSequencer( "GenMonitor" )
GenMonitor.Members += [ "RivetAnalysisHandler", ]
rivet = RivetAnalysisHandler()
#the filename of the .aida output (w/o extension!)
rivet.BaseFileName = "myRivetGaussMC"
#head of branch where histograms are stored in AIDA
rivet.RunName = "LHCbMBPy6"
rivet.Analyses = ["MC_LHCb_GENERIC",]
rivet.StreamName = "/Rivet"
#to search Rivet plugins in current directory
rivet.AnalysisPath += [os.path.abspath('.'),]
#the following options allow setting externally 
#a cross-section value when it cannot be read from HepMC
#(mostly necessary if/when using HepMC 2.04)
rivet.xSectionNeeded = False
rivet.xSectionValue = 32.123
#instruct the plugin to correct the particle status IDs and 
#apply the crossing angle correction when necessary
rivet.CorrectStatusID = True
rivet.CorrectCrossingAngles = True

HistogramPersistencySvc().OutputFile = ''
