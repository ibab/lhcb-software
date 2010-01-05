########################################################################
# $Id: DVTutorial_4.py,v 1.6 2010-01-05 16:36:42 jdickens Exp $
#
# Options for exercise 4
#
# @author Patrick Koppenburg
# @author Juan Palacios
# @date 2008-06-03
#
########################################################################
from Gaudi.Configuration import *
#######################################################################
#
# import the SelectionSequence
from DaVinci4.solutions.Bs2JpsiPhi import SeqBs2JpsiPhi
# get the GaudiSequencer with everything we need
seq = SeqBs2JpsiPhi.sequence()
#######################################################################
#
# Configure the application
#
from Configurables import DaVinci
DaVinci().HistogramFile = "DVHistos_4.root"    # Histogram file
DaVinci().EvtMax = 1000                          # Number of events
DaVinci().DataType = "2008"                    # Default is "MC09"
DaVinci().Simulation   = True                  # It's MC
#
# Add our own stuff
#
DaVinci().UserAlgorithms = [ seq ]
DaVinci().MainOptions  = ""                    # None
########################################################################
#
# To run in shell :
# gaudirun.py solutions/DaVinci4/DVTutorial_4.py options/Bs2JpsiPhi2008.py
#
########################################################################
