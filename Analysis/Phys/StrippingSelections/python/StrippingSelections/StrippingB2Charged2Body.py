########################################################################
#
# Options for pre-selecting B->hh' and Bs->mu+ mu- events
#
# @author Angelo Carbone
# @date 2010-05-21
#
########################################################################                   

from Configurables import GaudiSequencer, CombineParticles
from StrippingConf.StrippingLine import StrippingLine

PreselB2Charged2Body = CombineParticles("PreselB2Charged2Body")
PreselB2Charged2Body.InputLocations  = ["StdLoosePions"]
PreselB2Charged2Body.DecayDescriptor = "B0 -> pi+ pi-"
PreselB2Charged2Body.CombinationCut = "(AMAXDOCA('') < 0.08*mm)"
PreselB2Charged2Body.DaughtersCuts = { "pi+" : "(PT>700*MeV) & (MIPDV(PRIMARY)>0.08) & (TRCHI2DOF<5)"}
PreselB2Charged2Body.MotherCut = "(M>4800*MeV) & (M<5800*MeV) & (BPVIP()<0.06) & (BPVVDSIGN>1.8) & (INTREE ( (ABSID=='pi+') & (PT>2400*MeV))) & (INTREE ( (ABSID=='pi+') & (MIPDV(PRIMARY)>0.2)))"


PreselB2Charged2Body1000 = CombineParticles("PreselB2Charged2Body1000")
PreselB2Charged2Body1000.InputLocations  = ["StdLoosePions"]
PreselB2Charged2Body1000.DecayDescriptor = "B0 -> pi+ pi-"
PreselB2Charged2Body1000.CombinationCut = "(AMAXDOCA('') < 0.15*mm)"
PreselB2Charged2Body1000.DaughtersCuts = { "pi+" : "(PT>500*MeV) & (MIPDV(PRIMARY)>0.05) & (TRCHI2DOF<5)"}
PreselB2Charged2Body1000.MotherCut = "(M>1000*MeV) & (BPVIP()<0.08) & (BPVVDSIGN>1.2) & (INTREE ( (ABSID=='pi+') & (PT>1500*MeV))) & (INTREE ( (ABSID=='pi+') & (MIPDV(PRIMARY)>0.12)))"

PreselB2Charged2Body4000 = CombineParticles("PreselB2Charged2Body4000")
PreselB2Charged2Body4000.InputLocations  = ["StdLoosePions"]
PreselB2Charged2Body4000.DecayDescriptor = "B0 -> pi+ pi-"
PreselB2Charged2Body4000.CombinationCut = "(AMAXDOCA('') < 0.15*mm)"
PreselB2Charged2Body4000.DaughtersCuts = { "pi+" : "(PT>500*MeV) & (MIPDV(PRIMARY)>0.05) & (TRCHI2DOF<5)"}
PreselB2Charged2Body4000.MotherCut = "(M>4000*MeV) & (BPVIP()<0.1) & (BPVVDSIGN>1.0)"




# Create the line for this selection
line = StrippingLine('B2Charged2Body'
                     , prescale = 1
                     , algos = [ PreselB2Charged2Body ]
                     )

line1 = StrippingLine('B2Charged2Body4000'
                     , prescale = 1
                     , algos = [ PreselB2Charged2Body4000 ]
                     )

line2 = StrippingLine('B2Charged2Body1000'
                     , prescale = 1
                      , algos = [ PreselB2Charged2Body1000 ]
                      )
