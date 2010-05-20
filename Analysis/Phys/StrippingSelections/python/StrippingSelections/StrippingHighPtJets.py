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
from Configurables import PatPV3D
from Configurables import TrackClusterFinder
from StrippingConf.StrippingLine import StrippingLine

PatPV = PatPV3D("PatPV")
TrkClusterFinder = TrackClusterFinder("TrkClusterFinder")
TrkClusterFinder.MinWeightSeed = 6
TrkClusterFinder.ConeExtFactor = 4.0
TrkClusterFinder.MaxDeltaPhi   = 8.5
TrkClusterFinder.MaxDeltaEta   = 0.16
TrkClusterFinder.Min1stJetPt   = 3.0
TrkClusterFinder.Min2ndJetPt   = 0.0
TrkClusterFinder.JetMult       = 1

# Create line for high-pt jets selection
line1 = StrippingLine('HighPtJetsSel'
                      , prescale = 1.0
                      , algos = [PatPV,TrkClusterFinder]
                     )
