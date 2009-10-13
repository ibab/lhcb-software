from Gaudi.Configuration import *
from Configurables import GaudiSequencer, CombineParticles
# Units
from GaudiKernel.SystemOfUnits import *

SeqPreselBd2Kstaree = GaudiSequencer("SeqPreselBd2Kstaree")

DiLeptonForBd2LLKstar = CombineParticles("DiLeptonForBd2LLKstar")
Kstar2KPiForBd2LLKstar = CombineParticles("Kstar2KPiForBd2LLKstar")
PreselBd2Kstaree = CombineParticles("PreselBd2Kstaree")

SeqPreselBd2Kstaree.Members += [ DiLeptonForBd2LLKstar ]
SeqPreselBd2Kstaree.Members += [ Kstar2KPiForBd2LLKstar ]
SeqPreselBd2Kstaree.Members += [ PreselBd2Kstaree ]

# DiLepton (e+e-)
DiLeptonForBd2LLKstar.InputLocations = ["StdLooseElectrons"]
DiLeptonForBd2LLKstar.DecayDescriptor = "J/psi(1S) -> e+ e-"
DiLeptonForBd2LLKstar.DaughtersCuts = {"e+": "(MINTREE(ABSID=='e+',PT)>300*MeV) & (MIPCHI2DV(PRIMARY)>2.25)" }
DiLeptonForBd2LLKstar.CombinationCut = "AM<5500*MeV"  #"(30*MeV<AM) & (AM<1000*MeV)" applied later
DiLeptonForBd2LLKstar.MotherCut = "(VFASPF(VCHI2/VDOF)<15)" #(BPVVD>1.0*mm) applied later

# Kstar
Kstar2KPiForBd2LLKstar.InputLocations = ["StdTightPions", "StdTightKaons"]
Kstar2KPiForBd2LLKstar.DecayDescriptor = "[K*(892)0 -> K+ pi-]cc"
Kstar2KPiForBd2LLKstar.DaughtersCuts = {"K+": "(MINTREE(ABSID=='K+',PT)>400*MeV) & (MINTREE(ABSID=='K+',P)>3000*MeV) & (MIPCHI2DV(PRIMARY)>4)",
                                        "pi-": "(MINTREE(ABSID=='pi-',PT)>300*MeV) & (MINTREE(ABSID=='pi-',P)>3000*MeV) & (MIPCHI2DV(PRIMARY)>4)"
                                        }
Kstar2KPiForBd2LLKstar.CombinationCut = "(ADAMASS('K*(892)0')<200*MeV)" # <130 MeV applied later
Kstar2KPiForBd2LLKstar.MotherCut = "(VFASPF(VCHI2/VDOF)<15)" # (BPVIPCHI2()>1.0) & (BPVVDCHI2>1.0) applied later

# Bd2Kstar
PreselBd2Kstaree.InputLocations = ["DiLeptonForBd2LLKstar", "Kstar2KPiForBd2LLKstar"]
PreselBd2Kstaree.DecayDescriptor = "[B0 -> K*(892)0 J/psi(1S)]cc"
PreselBd2Kstaree.DaughtersCuts = {"K*(892)0": "ALL",
                                  "J/psi(1S)": "ALL"}
PreselBd2Kstaree.CombinationCut = "(ADAMASS('B0')<1000*MeV)" # 300 MeV applied later

PreselBd2Kstaree.MotherCut = "(BPVIPCHI2()<64) & (VFASPF(VCHI2/VDOF)<(15.0/4.0)) & (BPVVDCHI2>16) & (BPVDIRA>0.99969989) " # theta<24.5 rad

#PreselBd2Kstaree.KillOverlap = False 

# final selections
hardee    = "(INTREE( (ID=='J/psi(1S)') & (BPVVD>1.0*mm) ))"
hardKstar = "(INTREE( (ABSID=='K*(892)0') & (ADMASS('K*(892)0')<130*MeV) & (BPVIPCHI2()>1.0) & (BPVVDCHI2>1.0) ))" 
hardB     = "(ADMASS('B0')<1000*MeV) & (BPVIP()<0.05*mm)"

from Configurables import FilterDesktop
SelBd2eeKstar = FilterDesktop("SelBd2eeKstar")
SelBd2eeKstar.InputLocations  = ["PreselBd2Kstaree"]
SelBd2eeKstar.Code = hardee + " & " + hardKstar + " & " + hardB

from StrippingConf.StrippingLine import StrippingLine
# Create the line for this selection
line = StrippingLine('Bd2eeKstar'
               , prescale = 1
               , algos = [ SelBd2eeKstar ]
               )
