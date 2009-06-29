__author__ = 'Diego Martinez Santos'
__date__ = '29/06/2009'


from Gaudi.Configuration import *
from Configurables import CombineParticles # , OfflineVertexFitter, OnOfflineTool
from CommonParticles.Utils import *
import GaudiKernel.SystemOfUnits as Units

###########################
# StdLooseDetachetKst2Kpi #
###########################

StdLooseDetachedKst2Kpi = CombineParticles("StdLooseDetachedKst2Kpi")
StdLooseDetachedKst2Kpi.DecayDescriptor = "[K*(892)0 -> K+ pi-]cc"
StdLooseDetachedKst2Kpi.InputLocations = ["StdNoPIDsKaons","StdNoPIDsPions"]
StdLooseDetachedKst2Kpi.DaughtersCuts = {"K+" :"(ISLONG) & (MIPCHI2DV(PRIMARY)>2.25)",
                                  "pi-":"(ISLONG) & (MIPCHI2DV(PRIMARY)>2.25)"}
StdLooseDetachedKst2Kpi.CombinationCut = "(ADAMASS('K*(892)0')<300*MeV)"
StdLooseDetachedKst2Kpi.MotherCut = "(VFASPF(VCHI2/VDOF)<36)&(MIPCHI2DV(PRIMARY)> 2.25)"
locations = updateDoD(StdLooseDetachedKst2Kpi)


