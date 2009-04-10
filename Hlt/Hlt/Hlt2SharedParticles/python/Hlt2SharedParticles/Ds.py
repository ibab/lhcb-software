### @file
#
#  Standard Ds
#
#  @author P. Koppenburg Patrick.Koppenburg@cern.ch
#  @date 2008-07-15
#
##
from Gaudi.Configuration import *
from Configurables import CombineParticles, PhysDesktop
from Hlt2SharedParticles.GoodParticles import GoodKaons, GoodPions
from HltConf.HltLine import bindMembers

__all__ = ( 'Ds2KKPi' )

Hlt2SharedDs2KKPi = CombineParticles("Hlt2SharedDs2KKPi")
Hlt2SharedDs2KKPi.addTool(PhysDesktop)
Hlt2SharedDs2KKPi.PhysDesktop.InputLocations = [ "Hlt2GoodKaons", "Hlt2GoodPions" ]
Hlt2SharedDs2KKPi.DecayDescriptor = "[D_s- -> K+ K- pi-]cc" 

Hlt2SharedDs2KKPi.DaughtersCuts = {"K+"  : "(PT>300*MeV) & (P>2*GeV) & (MIPCHI2DV(PRIMARY)>4)", "pi+" : "(PT>300*MeV) & (P>2*GeV) & (MIPCHI2DV(PRIMARY)>4)"}
# need a large window to have the Ds* in as well.
Hlt2SharedDs2KKPi.CombinationCut = "(ADAMASS('D_s-')<200*MeV) & (APT>1*GeV)"
Hlt2SharedDs2KKPi.MotherCut = "(VFASPF(VCHI2/VDOF)<6.67) & (MIPCHI2DV(PRIMARY)>9)"

Ds2KKPi = bindMembers( None, [ GoodKaons, GoodPions, Hlt2SharedDs2KKPi ] )
