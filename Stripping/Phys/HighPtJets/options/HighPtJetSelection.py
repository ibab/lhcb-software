## #####################################################################
# High pt jet selection
# 
# @author: Marcin Kucharczyk
# @date 2010-04-03
# 
## #####################################################################
from os import environ, path
from math import *
from Gaudi.Configuration import *
from Configurables import GaudiSequencer
from GaudiKernel.SystemOfUnits import *
from CommonParticles.Utils import *
from Configurables import LoKi__FastJetMaker
from Configurables import CombineParticles, PhysDesktop
from Configurables import HighPtJetSelection
## Selection
jetMaker = LoKi__FastJetMaker()
jetMaker.RParameter = 1.0
highPtJetSel = HighPtJetSelection("highPtJetSel")
highPtJetSel.InputLocations = ["Phys/StdNoPIDsPions",
                               "Phys/StdNoPIDsDownPions.py",
                               "Phys/StdLooseAllPhotons"]
highPtJetSel.addTool(jetMaker)
highPtJetSel.MinPartPt   = 0.2
highPtJetSel.MinPartP    = 1.0
highPtJetSel.MinPartE    = 2.0
highPtJetSel.Min1stJetPt = 10.0
highPtJetSel.Min2ndJetPt = 7.0
highPtJetSel.JetMult     = 2
highPtJetSel.MaxRPV      = 1.0
## #####################################################################
DVSeq = GaudiSequencer("DVSeq")
DVSeq.IgnoreFilterPassed = False
DVSeq.Members += [highPtJetSel]
## #####################################################################
from Configurables import DaVinci
DaVinci().EvtMax = -1
DaVinci().PrintFreq = 100
DaVinci().SkipEvents = 0
DaVinci().DataType = "2010"
DaVinci().Simulation = False
DaVinci().HistogramFile = "test_histo.root"
DaVinci().TupleFile = "test.root"    
DaVinci().UserAlgorithms = [DVSeq]
########################################################################








