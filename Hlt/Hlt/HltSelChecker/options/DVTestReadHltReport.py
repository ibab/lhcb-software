### @file
#
#   Hlt 2 selections test job
#
#   @author P. Koppenburg
#   @date 2008-08-04
# /
from Gaudi.Configuration import *
##################################################################
# 
# Uncomment to Read HltReport and printout candidates
#
from Configurables import GaudiSequencer
moni = GaudiSequencer("Hlt2MonitorSeq")
from Configurables import ReadHltReport
moni.Members += [ ReadHltReport() ]
ReadHltReport().PrintParticles = TRUE
# ReadHltReport().PrintHlt1 = True
#################################################################
importOptions ("$HLTSELCHECKERROOT/options/DVTestHlt2.py")
