## @file 
#  Correltaions
#
#  @author P. Koppenburg
#  @date 2006-02-01
#
##--------------------------------------------------------------
from Gaudi.Configuration import *
from Configurables import HltCorrelations, GaudiSequencer

moni = GaudiSequencer("Hlt2MonitorSeq")
moni.Members += [ HltCorrelations("Hlt2SelectionsCorrs") ]
