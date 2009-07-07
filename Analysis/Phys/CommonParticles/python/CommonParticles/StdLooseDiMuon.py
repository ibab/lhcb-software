"""
Configuration file for Standard Loose DiMuon, very loose for penguins
"""

__author__ = 'Rob Lambert'
__date__ = '07/07/2009'


from Gaudi.Configuration import *
from Configurables import CombineParticles 
from CommonParticles.Utils import *
import GaudiKernel.SystemOfUnits as Units

###########################
# StdLooseDiMuon #
###########################

StdLooseDiMuon = CombineParticles("StdLooseDiMuon")
StdLooseDiMuon.DecayDescriptor = "J/psi(1S) -> mu+ mu-"
StdLooseDiMuon.InputLocations = ["StdLooseMuons"]
StdLooseDiMuon.CombinationCut = "(ADAMASS('J/psi(1S)')<3000*MeV)"
StdLooseDiMuon.MotherCut = "(VFASPF(VCHI2/VDOF)<64)"
locations = updateDoD(StdLooseDiMuon)


