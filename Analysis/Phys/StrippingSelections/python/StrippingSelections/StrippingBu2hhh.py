########################################################################
#
# Options for pre-selecting B->hhh' events
#
# @author Jussara  Miranda, Alvaro Gomes
# @date 2009-08-26
#
########################################################################                   

from Configurables import GaudiSequencer, CombineParticles
from StrippingConf.StrippingLine import StrippingLine

StripBu2hhh = CombineParticles("StripBu2hhh")
StripBu2hhh.InputLocations  = ["StdLoosePions"]
StripBu2hhh.DecayDescriptor = "[B+ -> pi+ pi+ pi-]cc"

StripBu2hhh.CombinationCut = "(AM<6000*MeV) & (AM>4000*MeV) & ((APT1+APT2+APT3)>4500*MeV) & (AVAL_MAX(MIPDV(PRIMARY), PT)>0.05) & (ANUM(PT>900*MeV)>=2)"
StripBu2hhh.DaughtersCuts = {"pi+" : "(MIPDV(PRIMARY)>0.005)"}
StripBu2hhh.MotherCut = "(BPVTRGPOINTINGWPT<.12) & (VFASPF(VCHI2)<10) & (VFASPF(VMINVDDV(PRIMARY))>2) & (BPVDIRA>0.999) & (MAXTREE((('pi+'==ABSID)) ,PT)>1500*MeV) & (MIPDV(PRIMARY)<0.04) & (PT>1000*MeV)"

# Create the line for this selection
line = StrippingLine('Bu2hhhLine'
               , prescale = 1
               ,  algos = [ StripBu2hhh ]
               )
