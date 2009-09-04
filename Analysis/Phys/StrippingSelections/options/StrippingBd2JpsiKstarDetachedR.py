__author__ = 'Diego Martinez Santos'
__date__ = 'June 30, 2009'

from Gaudi.Configuration import *
from StrippingConf.StrippingLine import StrippingLine, StrippingMember
from Configurables import FilterDesktop, CombineParticles, OfflineVertexFitter, OnOfflineTool
import GaudiKernel.SystemOfUnits as Units


###########################################
# J/psi Robust := no error estimates used #
###########################################

DetachedRobustDiMuon = CombineParticles("StripDetachedRobustDiMuon")
DetachedRobustDiMuon.InputLocations = [ "Phys/StdLooseMuons" ]
DetachedRobustDiMuon.DecayDescriptor = "J/psi(1S) -> mu+ mu-"
DetachedRobustDiMuon.CombinationCut = "(ADAMASS('J/psi(1S)')<60*MeV) & (AMAXDOCA('') < 0.08*mm )"
DetachedRobustDiMuon.addTool(OnOfflineTool())
DetachedRobustDiMuon.OnOfflineTool.OfflinePVRelatorName = 'GenericParticle2PVRelator__p2PVWithIP_OfflineDistanceCalculatorName_'
DetachedRobustDiMuon.MotherCut = "(BPVVDSIGN > 1.8*mm)"


###########################################
# Kstar Robust := no error estimates used #
###########################################

RobustTightMassKst2Kpi = CombineParticles("StripRobustTightMassKst2Kpi")
RobustTightMassKst2Kpi.DecayDescriptor = "[K*(892)0 -> K+ pi-]cc"
RobustTightMassKst2Kpi.InputLocations = ["Phys/StdNoPIDsKaons","Phys/StdNoPIDsPions"]
RobustTightMassKst2Kpi.addTool(OnOfflineTool())
RobustTightMassKst2Kpi.OnOfflineTool.OfflinePVRelatorName = 'GenericParticle2PVRelator__p2PVWithIP_OfflineDistanceCalculatorName_'
RobustTightMassKst2Kpi.DaughtersCuts = {"K+" :"(ISLONG) & (MIPDV(PRIMARY)>0.07*mm)",
                                        "pi-":"(ISLONG) & (MIPDV(PRIMARY)>0.07*mm)"}
RobustTightMassKst2Kpi.CombinationCut = "(ADAMASS('K*(892)0')<40*MeV)"
RobustTightMassKst2Kpi.MotherCut = "(MIPDV(PRIMARY)>0.06*mm)"


######
# Bd #
######

Bd2JpsiKstDetRobust = CombineParticles("StripBd2JpsiKstDetRobust")
Bd2JpsiKstDetRobust.DecayDescriptor = "[B0 -> J/psi(1S) K*(892)0]cc"
Bd2JpsiKstDetRobust.InputLocations = ["Phys/StripDetachedRobustDiMuon",
                             "Phys/StripRobustTightMassKst2Kpi"]
# Set the OfflineVertexFitter to keep the 4 tracks and not the J/Psi Kstar:
Bd2JpsiKstDetRobust.addTool( OfflineVertexFitter() )
Bd2JpsiKstDetRobust.VertexFitters.update( { "" : "OfflineVertexFitter"} )
Bd2JpsiKstDetRobust.OfflineVertexFitter.useResonanceVertex = False
Bd2JpsiKstDetRobust.addTool(OnOfflineTool())
Bd2JpsiKstDetRobust.OnOfflineTool.OfflinePVRelatorName = 'GenericParticle2PVRelator__p2PVWithIP_OfflineDistanceCalculatorName_'
Bd2JpsiKstDetRobust.ReFitPVs = True
#Bd2JpsiKstDetRobust.DaughtersCuts ={"J/psi(1S)" : "(BPVVDSIGN > 1.8*mm )"} 
Bd2JpsiKstDetRobust.CombinationCut = "ADAMASS('B0') < 500.*MeV"
Bd2JpsiKstDetRobust.MotherCut = "(MIPDV(PRIMARY)<0.06*mm)"

############################################
# Create StrippingLine with this selection #
############################################

Bd2JpsiKstDetRobustLine = StrippingLine('Bd2JpsiKstDetRobustLine'
               , prescale = 1
               , algos = [DetachedRobustDiMuon, RobustTightMassKst2Kpi, Bd2JpsiKstDetRobust]
               , stream = 'Bmuon'
               )
