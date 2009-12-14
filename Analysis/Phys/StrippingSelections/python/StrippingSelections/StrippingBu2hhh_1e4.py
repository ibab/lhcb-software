####################################################################################
#
# Options for pre-selecting B->hhh' events Rejection Factor of 10^-4 after L0XHlt1.
#
# @author Jussara  Miranda, Alvaro Gomes
# @date 2009-08-26
#
####################################################################################       

from Configurables import GaudiSequencer, CombineParticles
from StrippingConf.StrippingLine import StrippingLine

StripBu2hhh = CombineParticles("StripBu2hhh_1e4")
StripBu2hhh.InputLocations  = ["StdLoosePions"]
StripBu2hhh.DecayDescriptor = "[B+ -> pi+ pi+ pi-]cc"

StripBu2hhh.CombinationCut = "(AM<6000*MeV) & (AM>4000*MeV) & ((APT1+APT2+APT3)>4500*MeV) & (AVAL_MAX(MIPDV(PRIMARY), PT)>0.05) & (ANUM(PT>900*MeV)>=2) & ((ACHILD(TRCHI2DOF,1)+ACHILD(TRCHI2DOF,2)+ACHILD(TRCHI2DOF,3))<4) & ((ACHILD(MIPCHI2DV(PRIMARY),1)+ACHILD(MIPCHI2DV(PRIMARY),2)+ACHILD(MIPCHI2DV(PRIMARY),3))>200)"
StripBu2hhh.DaughtersCuts = {"pi+" : "(MIPDV(PRIMARY)>0.005)"}
StripBu2hhh.MotherCut = "(BPVTRGPOINTINGWPT<.1) & (VFASPF(VCHI2)<8) & (VFASPF(VMINVDDV(PRIMARY))>3) & (BPVDIRA>0.999) & (MAXTREE((('pi+'==ABSID)) ,PT)>1500*MeV) & (MIPDV(PRIMARY)<0.04) & (PT>1000*MeV) & (BPVVDCHI2>170)"

# Create the line for this selection
line = StrippingLine('Bu2hhhLine_1e4'
               , prescale = 1
               ,  algos = [ StripBu2hhh ]
               )
