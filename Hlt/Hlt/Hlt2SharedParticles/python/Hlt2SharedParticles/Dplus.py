### @file
#
#  Standard D+
#
#  @author P. Koppenburg Patrick.Koppenburg@cern.ch
#  @date 2008-07-15
#
##
from Gaudi.Configuration import *
from Configurables import CombineParticles, FilterDesktop
from Hlt2SharedParticles.GoodParticles import GoodKaons, GoodPions
from HltConf.HltLine import bindMembers

__all__ = ( 'Dplus2KPiPi','Dplus2KKPi','Dplus2KPiPiCS','Dplus2PiPiPi' )

##########################################################################################
# D0 -> K Pi
#
Hlt2SharedDplus2KPiPi = CombineParticles("Hlt2SharedDplus2KPiPi")
Hlt2SharedDplus2KPiPi.DecayDescriptor = "[D+ -> K- pi+ pi+]cc" 
Hlt2SharedDplus2KPiPi.DaughtersCuts = { "K+"  : "ALL",
                                        "pi+" : "ALL" }

#Note that the cut of 300 MeV around the *D_s* mass is NOT a typo, it is motivated by
#the requirements of the B->Dh HLT2 selection
Hlt2SharedDplus2KPiPi.CombinationCut = "(ADAMASS('D_s+')<300*MeV) & (APT>1.5*GeV)" 
Hlt2SharedDplus2KPiPi.MotherCut = "(VFASPF(VCHI2/VDOF) < 10 ) & (BPVDIRA > 0) " 

Hlt2SharedDplus2KPiPi.InputLocations = [ GoodKaons.outputSelection(), GoodPions.outputSelection() ]
Dplus2KPiPi = bindMembers( None, [ GoodKaons, GoodPions, Hlt2SharedDplus2KPiPi ] )
##########################################################################################
# D+ -> K K Pi is a clone of D+ -> K Pi Pi 
#
Hlt2SharedDplus2KKPi = Hlt2SharedDplus2KPiPi.clone("Hlt2SharedDplus2KKPi")
Hlt2SharedDplus2KKPi.DecayDescriptor = "[D+ -> K- K+ pi+]cc"

Dplus2KKPi = bindMembers( None, [ GoodKaons, GoodPions, Hlt2SharedDplus2KKPi ] )
##########################################################################################
# Need a second D+ -> K Pi Pi where the pions have opposite charge, the CS mode of the Ds
#
Hlt2SharedDplus2KPiPiCS = Hlt2SharedDplus2KPiPi.clone("Hlt2SharedDplus2KPiPiCS")
Hlt2SharedDplus2KPiPiCS.DecayDescriptor = "[D+ -> K+ pi+ pi-]cc"

Dplus2KPiPiCS = bindMembers( None, [ GoodKaons, GoodPions, Hlt2SharedDplus2KPiPiCS ] )
##########################################################################################
# D+ -> 3 pi is a clone of D+ -> K Pi Pi 
#
Hlt2SharedDplus2PiPiPi = Hlt2SharedDplus2KPiPi.clone("Hlt2SharedDplus2PiPiPi")
Hlt2SharedDplus2PiPiPi.InputLocations = [ GoodPions.outputSelection() ]
Hlt2SharedDplus2PiPiPi.DecayDescriptor = "[D+ -> pi- pi+ pi+]cc"

Dplus2PiPiPi = bindMembers( None, [ GoodPions, Hlt2SharedDplus2PiPiPi ] )
