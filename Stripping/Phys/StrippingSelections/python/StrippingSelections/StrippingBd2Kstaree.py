###################################################################
#
# Marie-Helene Schune, schunem@lal.in2p3.fr
# Jibo.He@cern.ch
#
from Gaudi.Configuration import *
from Configurables import GaudiSequencer, CombineParticles, FilterDesktop
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
DiLeptonForBd2LLKstar.DaughtersCuts = {"e+": "(PT>300*MeV) & (MIPCHI2DV(PRIMARY)>1)" }
DiLeptonForBd2LLKstar.CombinationCut = "AM<5500*MeV"
DiLeptonForBd2LLKstar.MotherCut = "(VFASPF(VCHI2/VDOF)<25)" 

# Kstar
Kstar2KPiForBd2LLKstar.InputLocations = ["StdTightPions", "StdTightKaons"]
Kstar2KPiForBd2LLKstar.DecayDescriptor = "[K*(892)0 -> K+ pi-]cc"
Kstar2KPiForBd2LLKstar.DaughtersCuts = {"K+":  "(PT>350*MeV) & (P>3000*MeV) & (MIPCHI2DV(PRIMARY)>3)",
                                        "pi-": "(PT>300*MeV) & (P>3000*MeV) & (MIPCHI2DV(PRIMARY)>3)"
                                        }
Kstar2KPiForBd2LLKstar.CombinationCut = "(ADAMASS('K*(892)0')<200*MeV)" 
Kstar2KPiForBd2LLKstar.MotherCut = "(VFASPF(VCHI2/VDOF)<25)" 

# Bd-> ee Kstar
PreselBd2Kstaree.InputLocations = ["DiLeptonForBd2LLKstar", "Kstar2KPiForBd2LLKstar"]
PreselBd2Kstaree.DecayDescriptor = "[B0 -> K*(892)0 J/psi(1S)]cc"
PreselBd2Kstaree.DaughtersCuts = {"K*(892)0": "ALL",
                                  "J/psi(1S)": "ALL"}
PreselBd2Kstaree.CombinationCut = "(ADAMASS('B0')<1200*MeV)" 
PreselBd2Kstaree.MotherCut = "(BPVIPCHI2()<64) & (VFASPF(VCHI2/VDOF)<(36.0/4.0)) & (BPVVDCHI2>9) & (BPVDIRA>0.999)" 

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
               , algos = [ SeqPreselBd2Kstaree, SelBd2eeKstar ]
               )
