## #####################################################################
# Track cluster finder
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
from Configurables import TrackClusterFinder
TrkClusterFinder = TrackClusterFinder("TrkClusterFinder")
TrkClusterFinder.MinWeightSeed = 6
TrkClusterFinder.ConeExtFactor = 4.0
TrkClusterFinder.MaxDeltaPhi   = 4.0
TrkClusterFinder.MaxDeltaEta   = 0.15
TrkClusterFinder.Min1stJetPt   = 10.0
TrkClusterFinder.Min2ndJetPt   = 7.0
TrkClusterFinder.JetMult       = 2
TrkClusterFinder.MaxRPV        = 1.0
## #####################################################################
DVSeq = GaudiSequencer("DVSeq")
DVSeq.IgnoreFilterPassed = False
DVSeq.Members += [TrkClusterFinder]
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








