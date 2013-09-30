########################################################################
# $Id: DVTutorial_4.py,v 1.7 2010-01-28 09:47:45 pkoppenb Exp $
#
# Options for exercise 4
#
# @author Patrick Koppenburg
# @author Juan Palacios
# @date 2008-06-03
#
########################################################################
from Gaudi.Configuration import *
MessageSvc().Format = "% F%60W%S%7W%R%T %0W%M"
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
DaVinci().EvtMax = 10000                          # Number of events
DaVinci().DataType = "2012"                    # 
DaVinci().Simulation = True                    # When running on MC
#
# Add our own stuff
#
DaVinci().UserAlgorithms = [ seq ]
DaVinci().MainOptions  = ""                    # None
########################################################################
#
# To run in shell :
# gaudirun.py solutions/DaVinci4/DVTutorial_4.py options/Bs2JpsiPhi_Sim08a.py
#
########################################################################
