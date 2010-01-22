"""
Configuration file for Standard Very Loose Kstar2Kpi
"""

__author__ = 'P. Koppenburg'
__date__ = '22/01/2010'


from Gaudi.Configuration import *
from Configurables import CombineParticles # , OfflineVertexFitter, OnOfflineTool
from CommonParticles.Utils import *
import GaudiKernel.SystemOfUnits as Units

###########################
# StdVeryLooseDetachetKst2Kpi #
###########################

StdVeryLooseDetachedKst2Kpi = CombineParticles("StdVeryLooseDetachedKst2Kpi")
StdVeryLooseDetachedKst2Kpi.DecayDescriptor = "[K*(892)0 -> K+ pi-]cc"
StdVeryLooseDetachedKst2Kpi.InputLocations = ["StdLooseKaons","StdLoosePions"]
StdVeryLooseDetachedKst2Kpi.DaughtersCuts = {"K+" :"(ISLONG) & (MIPCHI2DV(PRIMARY)>2.25)",
                                  "pi-":"(ISLONG) & (MIPCHI2DV(PRIMARY)>2.25)"}
StdVeryLooseDetachedKst2Kpi.CombinationCut = "(ADAMASS('K*(892)0')<300*MeV)"
StdVeryLooseDetachedKst2Kpi.MotherCut = "(VFASPF(VCHI2/VDOF)<100)"
locations = updateDoD(StdVeryLooseDetachedKst2Kpi)


