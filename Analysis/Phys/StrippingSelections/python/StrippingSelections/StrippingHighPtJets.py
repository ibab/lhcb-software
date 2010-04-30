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
from Configurables import HighPtJetFinder
from StrippingConf.StrippingLine import StrippingLine

PatPV = PatPV3D("PatPV")
JetFinder = HighPtJetFinder("JetFinder")
JetFinder.MinWeight = 7
JetFinder.MaxDeltaPhi = 8.0
JetFinder.MaxDeltaEta = 0.12
JetFinder.JetMult = 2
JetFinder.Min1stJetPt = 0.0
JetFinder.Min2ndJetPt = 0.0

# Create line for high-pt jets selection
line1 = StrippingLine('HighPtJetsSel'
                      , prescale = 1.0
                      , algos = [PatPV,JetFinder]
                     )
