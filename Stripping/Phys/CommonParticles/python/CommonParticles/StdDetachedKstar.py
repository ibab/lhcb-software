"""
Configuration file for Standard Detached Kstar2Kpi,
with upper mass until 2.2 GeV
"""

__author__ = 'Jibo He'
__date__ = '22/01/2012'


from Gaudi.Configuration import *
from Configurables import CombineParticles # , OfflineVertexFitter, OnOfflineTool
from CommonParticles.Utils import *
import GaudiKernel.SystemOfUnits as Units

###########################
# StdDetachetKst2Kpi      #
###########################
StdDetachedKst2Kpi = CombineParticles("StdDetachedKst2Kpi")
StdDetachedKst2Kpi.DecayDescriptor = "[K*(892)0 -> K+ pi-]cc"
StdDetachedKst2Kpi.Inputs = ["Phys/StdLooseKaons/Particles",
                             "Phys/StdLoosePions/Particles"]

StdDetachedKst2Kpi.CombinationCut = "(AM<2.2*GeV) & (ADOCACHI2CUT(30, ''))"
StdDetachedKst2Kpi.MotherCut = "(VFASPF(VCHI2)<25)"

locations = updateDoD(StdDetachedKst2Kpi)


