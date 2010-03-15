########################################################################
#
# $Id: DaVinci.py,v 1.43 2010-03-15 15:57:42 pkoppenb Exp $
#
# Options for a typical DaVinci job
#
# @author Patrick Koppenburg
# @date 2008-08-06
#
########################################################################
from Gaudi.Configuration import *
from Configurables import GaudiSequencer
########################################################################
#
# Standard configuration
#
from Configurables import DaVinci
DaVinci().EvtMax = 100                         # Number of events
DaVinci().SkipEvents = 0                       # Events to skip
DaVinci().DataType = "2009"                    # Must be given
DaVinci().Simulation   = False
DaVinci().HistogramFile = "DVHistos_1.root"    # Histogram file
DaVinci().TupleFile = "DVNtuples.root"         # Ntuple
DaVinci().UserAlgorithms = [  ]  # The algorithms
DaVinci().MoniSequence = [ ]        # Monitoring
# DaVinci().MainOptions  = "" # None
########################################################################
# HLT
# DaVinci().ReplaceL0BanksWithEmulated = True ## enable if you want to rerun L0
# DaVinci().Hlt2Requires = 'L0+Hlt1'          ## change if you want Hlt2 irrespective of Hlt1
# DaVinci().Hlt = True             ## 
########################################################################
MessageSvc().Format = "% F%60W%S%7W%R%T %0W%M"
########################################################################
#
# To run : gaudirun.py options/DaVinci.py options/DaVinciTestData.py
#
########################################################################
