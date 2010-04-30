## #####################################################################
# High pt jet finder
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
from Configurables import PatPV3D
from Configurables import HighPtJetFinder
PatPV = PatPV3D("PatPV")
JetFinder = HighPtJetFinder("JetFinder")
JetFinder.MinWeight = 6
JetFinder.MaxDeltaPhi = 7.5
JetFinder.MaxDeltaEta = 0.13
JetFinder.Min1stJetPt = 7.0
JetFinder.Min2ndJetPt = 0.0
JetFinder.JetMult = 2
## #####################################################################
DVSeq = GaudiSequencer("DVSeq")
DVSeq.IgnoreFilterPassed = False
DVSeq.Members += [PatPV]
DVSeq.Members += [JetFinder]
## #####################################################################
from Configurables import DaVinci
DaVinci().EvtMax = -1
DaVinci().PrintFreq = 100
DaVinci().SkipEvents = 0
DaVinci().DataType = "MC09"
DaVinci().Simulation = True
DaVinci().HistogramFile = "test_histo.root"
DaVinci().TupleFile = "test.root"    
DaVinci().UserAlgorithms = [DVSeq]
########################################################################








