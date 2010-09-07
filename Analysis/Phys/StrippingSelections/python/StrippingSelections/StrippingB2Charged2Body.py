########################################################################
#
# Options for pre-selecting B->hh' and Bs->mu+ mu- events
#
# @author Angelo Carbone
# @rivisited by Stefano Perazzini
# @date 2010-00-04
#
########################################################################                   

from Configurables import GaudiSequencer, CombineParticles
from StrippingConf.StrippingLine import StrippingLine

PreselB2Charged2Body = CombineParticles("PreselB2Charged2Body")
PreselB2Charged2Body.InputLocations  = ["StdLoosePions"]
PreselB2Charged2Body.DecayDescriptor = "B0 -> pi+ pi-"
PreselB2Charged2Body.DaughtersCuts = { "pi+" : "(PT>700*MeV) & (MIPDV(PRIMARY)>0.08) & (TRCHI2DOF<5)"}
PreselB2Charged2Body.CombinationCut = "(AM > 4500*MeV) & (AM < 6500*MeV) & (AMAXDOCA('') < 0.08*mm) & (AMAXCHILD(MAXTREE('pi+'==ABSID,PT))>2400) & (AMAXCHILD(MAXTREE('pi+'==ABSID,MIPDV(PRIMARY)))>0.2)"
PreselB2Charged2Body.MotherCut = "(M>4800*MeV) & (M<5800*MeV) & (BPVIP()<0.06) & (BPVVDSIGN>1.8)"

# Create the line for this selection
line = StrippingLine('B2Charged2Body'
                     , prescale = 1
                     , algos = [ PreselB2Charged2Body ]
                     )

