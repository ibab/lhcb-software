###############################################################
from Gaudi.Configuration import *
###############################################################
#
# 1) Define a sequence
#
from Configurables import GaudiSequencer
testseq = GaudiSequencer("TestSeq")

# Setup the selection
from Configurables import RichDstarToD0PiSelConf
RichDstarToD0PiSelConf().Sequencer = testseq

###############################################################
#
# 5) Configure the application
#
from Configurables import DaVinci
#DaVinci().HistogramFile = "DVHistos.root"     # Histogram file
DaVinci().EvtMax = 1000                        # Number of events
DaVinci().DataType = "DC06"                    # Default is "DC06"
DaVinci().Simulation   = True                  # It's MC
#
# Add our own stuff
#
DaVinci().UserAlgorithms = [ testseq ]
DaVinci().MainOptions  = ""                    # None
###############################################################
