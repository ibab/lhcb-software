__author__ = 'Diego Martinez Santos'
__date__ = 'June 30, 2009'

from Gaudi.Configuration import *
from StrippingConf.StrippingLine import StrippingLine, StrippingMember
from Configurables import FilterDesktop, CombineParticles, OfflineVertexFitter,  OnOfflineTool
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


######
# Bu #
######

Bu2JpsiKDetRobust = CombineParticles("StripBu2JpsiKDetRobust")
Bu2JpsiKDetRobust.DecayDescriptor = "[B+ -> J/psi(1S) K+]cc"
Bu2JpsiKDetRobust.InputLocations = ["StripDetachedRobustDiMuon","StdNoPIDsKaons"]

Bu2JpsiKDetRobust.addTool( OfflineVertexFitter() )
Bu2JpsiKDetRobust.VertexFitters.update( { "" : "OfflineVertexFitter"} )
Bu2JpsiKDetRobust.OfflineVertexFitter.useResonanceVertex = False
Bu2JpsiKDetRobust.addTool(OnOfflineTool())
Bu2JpsiKDetRobust.OnOfflineTool.OfflinePVRelatorName = 'GenericParticle2PVRelator__p2PVWithIP_OfflineDistanceCalculatorName_'
Bu2JpsiKDetRobust.ReFitPVs = True
Bu2JpsiKDetRobust.DaughtersCuts ={"K+": "(ISLONG) & (MIPDV(PRIMARY)>0.1*mm)"} 
Bu2JpsiKDetRobust.CombinationCut = "ADAMASS('B+') < 500.*MeV"
Bu2JpsiKDetRobust.MotherCut = "(MIPDV(PRIMARY)<0.06*mm)"

############################################
# Create StrippingLine with this selection #
############################################

Bu2JpsiKDetRobustLine = StrippingLine('Bu2JpsiKDetRobustLine'
               , prescale = 1
               , algos = [DetachedRobustDiMuon,Bu2JpsiKDetRobust]
               , stream = 'BExclusive'
               )
