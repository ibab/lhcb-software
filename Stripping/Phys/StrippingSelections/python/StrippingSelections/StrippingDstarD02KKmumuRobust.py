#
# @author B. VIAUD
# @date 2010-March-10
#
## #####################################################################

from Gaudi.Configuration import *
from Configurables import GaudiSequencer, CombineParticles, FilterDesktop
from StrippingConf.StrippingLine import StrippingLine, StrippingMember, bindMembers

from CommonParticles.StdLoosePions import StdLoosePions
from CommonParticles.StdLooseKaons import StdLooseKaons
from CommonParticles.StdLooseMuons import StdLooseMuons

# Make the D0->K+K-mu+mu-.
D0KKmumu = CombineParticles("StripD0KKmumuForDstar2D0KKmumuRobust")
D0KKmumu.InputLocations = [ "StdLooseKaons","StdLooseMuons" ]
D0KKmumu.DecayDescriptor = "D0 -> K+ K- mu+ mu-"
D0KKmumu.CombinationCut = "( ADAMASS('D0')<50*MeV)"
D0KKmumu.MotherCut = "(PT > 1500.0*MeV ) & (BPVVD > 0.5*mm)"
D0KKmumu.DaughtersCuts = {
    "K+" : "(PT > 450.0*MeV ) & (P > 1000*MeV)  & (PIDK-PIDpi > -5.0) ",
    "mu+" : "(PT > 200.0*MeV ) & (P > 1000*MeV) & (PIDmu-PIDpi > -10.0)"
}


#Conjugate decay
from Configurables import ConjugateNeutralPID
Conjugate_D0KKmumu= ConjugateNeutralPID("StripD0barKKmumuForDstar2D0KKmumuRobust")
Conjugate_D0KKmumu.InputLocations = ["StripD0KKmumuForDstar2D0KKmumuRobust"]

# Reconstruct D*+ -> D0 pi+  (+cc)


Dstar_D0pi   = CombineParticles("StripDstar2D0KKmumuRobust");
Dstar_D0pi.DecayDescriptor   = "[D*(2010)+ -> D0 pi+]cc"
Dstar_D0pi.InputLocations = [ "StripD0KKmumuForDstar2D0KKmumuRobust", "StripD0barKKmumuForDstar2D0KKmumuRobust", "StdLoosePions" ]


# Set up delta mass cuts:

str_cutsDstarComb_MandDoca = "(ADAMASS('D*(2010)+')<50*MeV)"
Dstar_D0pi.CombinationCut = str_cutsDstarComb_MandDoca
strDstarMotherCut_PT = '(PT > 2500*MeV)'
strDstarMotherCut_DM = '(MM - CHILD(MM,1) < 154*MeV) & (MM - CHILD(MM,1) > 138*MeV)'

###### dau cuts
Dstar_D0pi.DaughtersCuts = { "pi+" : "(PT >150*MeV) & (P > 1000*MeV) & ('pi+' == ABSID)"}
    

# Mother cut for DM signal window (tighter):
Dstar_D0pi.MotherCut = strDstarMotherCut_DM + ' & ' + strDstarMotherCut_PT     

# Now create our StrippingLine object with this selection.

line1 = StrippingLine('Dstar2D0KKmumuRobust', prescale = 1.0, algos = [D0KKmumu, Conjugate_D0KKmumu, Dstar_D0pi ])
