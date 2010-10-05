## #####################################################################
# Stripping selection for high-pt jets
#
# @author: Marcin Kucharczyk
# @based on Phys/HighPtJets package
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
from StrippingConf.StrippingLine import StrippingLine

TrkClusterFinder = TrackClusterFinder("TrkClusterFinder")
TrkClusterFinder.MinWeightSeed = 5
TrkClusterFinder.ConeExtFactor = 4.0
TrkClusterFinder.MaxDeltaPhi   = 6.0
TrkClusterFinder.MaxDeltaEta   = 0.2
TrkClusterFinder.Min1stJetPt   = 10.0
TrkClusterFinder.Min2ndJetPt   = 7.0
TrkClusterFinder.JetMult       = 2
TrkClusterFinder.MaxRPV        = 1.0

# Create line for high-pt jets selection
line1 = StrippingLine('HighPtJetsSel'
                      , prescale = 1.0
                      , algos = [TrkClusterFinder]
                     )
