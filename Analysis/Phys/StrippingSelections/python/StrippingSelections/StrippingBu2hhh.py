########################################################################
#
# Options for pre-selecting B->hhh' events
#
# @author Jussara  Miranda, Alvaro Gomes, Rafael Coutinho
# @date 2010-07-26
#
########################################################################                   

from Configurables import GaudiSequencer, CombineParticles
from StrippingConf.StrippingLine import StrippingLine

from Configurables import LoKi__VoidFilter as VoidFilter
from Configurables import LoKi__Hybrid__CoreFactory as CoreFactory
modules =  CoreFactory('CoreFactory').Modules
for i in [ 'LoKiTrigger.decorators' ] :
    if i not in modules : modules.append(i)
   
######################################################################## 

StripBu2hhh_NumTracksGEC = VoidFilter('StripBu2hhh_NumTracksGEC'
                  , Code = "TrSOURCE('Rec/Track/Best') >> (TrSIZE < 240 )"
                  )

StripBu2hhh = CombineParticles("StripBu2hhh")
StripBu2hhh.InputLocations  = ["StdLoosePions"]
StripBu2hhh.DecayDescriptor = "[B+ -> pi+ pi+ pi-]cc"

StripBu2hhh.CombinationCut = "(AM<6000*MeV) & (AM>4000*MeV) & ((APT1+APT2+APT3)>4500*MeV) & (AVAL_MAX(MIPDV(PRIMARY), PT)>0.05) & (ANUM(PT>900*MeV)>=2) & ((ACHILD(TRCHI2DOF,1)+ACHILD(TRCHI2DOF,2)+ACHILD(TRCHI2DOF,3))<4) & ((ACHILD(MIPCHI2DV(PRIMARY),1)+ACHILD(MIPCHI2DV(PRIMARY),2)+ACHILD(MIPCHI2DV(PRIMARY),3))>200)&(AMAXDOCA('LoKi::TrgDistanceCalculator') < .3)"
StripBu2hhh.DaughtersCuts = {"pi+" : "(MIPDV(PRIMARY)>0.005)"}
StripBu2hhh.MotherCut = "(BPVDIRA>0.999) & (VFASPF(VMINVDDV(PRIMARY))>3) & (VFASPF(VCHI2)<8) & (MAXTREE((('pi+'==ABSID)) ,PT)>1500*MeV) & (MIPDV(PRIMARY)<0.04) & (PT>1000*MeV) & (BPVTRGPOINTINGWPT<.1)"

# Create the line for this selection
line = StrippingLine('Bu2hhhLine'
               , prescale = 1
               ,  algos = [ StripBu2hhh_NumTracksGEC, StripBu2hhh ]
               )
