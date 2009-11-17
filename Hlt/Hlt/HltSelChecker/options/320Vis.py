### @file 
 #  Append this to get appropriate scenario
 #
 #  @author P. Koppenburg
 #  @date 2009-10-28
 #
from Configurables import DaVinci
from Gaudi.Configuration import *
DaVinci().HltThresholdSettings = 'Physics_320Vis_300L0_10Hlt1_Hlt2_Oct09'  # just an example
def noInit():
    GaudiSequencer("PhysInitSeq").Members = []
    GaudiSequencer("AnalysisInitSeq").Members = []
    from Configurables import DeterministicPrescaler
    DeterministicPrescaler("Hlt2TransparentPreScaler").AcceptFraction = 0.
    
appendPostConfigAction(noInit)

                    
