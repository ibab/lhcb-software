### @file
#
#  Standard Di-muon
#
#  @author P. Koppenburg Patrick.Koppenburg@cern.ch
#  @date 2008-07-15
#
##
from Gaudi.Configuration import *
from Hlt2SharedParticles.BasicParticles import Muons
from Configurables import CombineParticles, PhysDesktop, FilterDesktop
from HltConf.HltLine import bindMembers


__all__ = ( 'DiMuon' , 'Jpsi2MuMu' , 'HighPtJpsi2MuMu' )


Hlt2SharedDiMuon = CombineParticles("Hlt2SharedDiMuon")
Hlt2SharedDiMuon.DecayDescriptor = "J/psi(1S) -> mu+ mu-"
Hlt2SharedDiMuon.CombinationCut = "AALL"
Hlt2SharedDiMuon.MotherCut = "(VFASPF(VCHI2/VDOF)<25)"
Hlt2SharedDiMuon.addTool(PhysDesktop)
Hlt2SharedDiMuon.PhysDesktop.InputLocations = [ "Hlt2Muons" ]

###
#
#  Standard J/psi -> mumu
#
#  @author P. Koppenburg Patrick.Koppenburg@cern.ch
#  @date 2008-11-17
#
##
Hlt2SharedJpsi2MuMu = FilterDesktop("Hlt2SharedJpsi2MuMu")
Hlt2SharedJpsi2MuMu.Code = "ADMASS('J/psi(1S)')<100*MeV"
Hlt2SharedJpsi2MuMu.addTool(PhysDesktop)
Hlt2SharedJpsi2MuMu.PhysDesktop.InputLocations  = [ "Hlt2SharedDiMuon" ]


###
#
#  High PT J/psi -> mumu
#
#  @author P. Koppenburg Patrick.Koppenburg@cern.ch
#  @date 2008-11-17
#
##
Hlt2SharedHighPtJpsi2MuMu = FilterDesktop("Hlt2SharedHighPtJpsi2MuMu")
Hlt2SharedHighPtJpsi2MuMu.Code = "(PT>1*GeV) & (MINTREE(ABSID=='mu+',PT)>800*MeV) & (MAXTREE(ABSID=='mu+',TRCHI2DOF)<8) & (ADMASS('J/psi(1S)')<80*MeV)"
Hlt2SharedHighPtJpsi2MuMu.addTool(PhysDesktop)
Hlt2SharedHighPtJpsi2MuMu.PhysDesktop.InputLocations  = [ "Hlt2SharedJpsi2MuMu" ]


### exported symbols...
DiMuon           = bindMembers( None, [ Muons, Hlt2SharedDiMuon ] )
Jpsi2MuMu        = bindMembers( None, [           DiMuon, Hlt2SharedJpsi2MuMu ] )
HighPtJpsi2MuMu  = bindMembers( None, [                             Jpsi2MuMu, Hlt2SharedHighPtJpsi2MuMu ] )
