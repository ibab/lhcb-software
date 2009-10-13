########################################################################
#
# Options for pre-selecting B->hhh' events
#
# @author Jussara  Miranda
# @date 2009-08-26
#
########################################################################                   

from Configurables import GaudiSequencer, CombineParticles
from StrippingConf.StrippingLine import StrippingLine

StripBu2hhh = CombineParticles("StripBu2hhh")
StripBu2hhh.InputLocations  = ["StdLoosePions"]
StripBu2hhh.DecayDescriptor = "[B+ -> pi+ pi+ pi-]cc"

StripBu2hhh.CombinationCut = "(AM<6000*MeV) & (AM>4000*MeV)&((APT1+APT2+APT3)>6000*MeV)"
StripBu2hhh.DaughtersCuts =  { "pi+":"(MIPDV(PRIMARY)>0.035)&(PT>400*MeV)&(TRCHI2<50)"}
StripBu2hhh.MotherCut = "(BPVTRGPOINTINGWPT<.12)&(VFASPF(VCHI2)<7) & (BPVVD>6) & (MIPDV(PRIMARY)<0.04) &(PT>3000*MeV) & (BPVDIRA>0.9998)&(MAXTREE((('pi+'==ABSID)) ,MIPDV(PRIMARY))>0.25)&(MAXTREE((('pi+'==ABSID)) ,PT)>1600*MeV)" 

# Create the line for this selection
line = StrippingLine('Bu2hhhLine'
               , prescale = 1
               ,  algos = [ StripBu2hhh ]
               )
