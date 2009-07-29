########################################################################
#
# $Id: DaVinci.py,v 1.40 2009-07-29 17:02:46 pkoppenb Exp $
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


########################################################################
################ First define all things to run ########################
########################################################################
########################################################################
#
# If you want to import .opts options, do this first
#
importOptions("$STDOPTS/PreloadUnits.opts")
########################################################################
#
# Some preselection. This defines a GaudiSequencer.
#
importOptions("$DAVINCISYSROOT/tests/options/Do09selBu2LLK.py")
preselSeq = GaudiSequencer("Sel09Bu2LLKFilterSequence")
########################################################################
#
# Some Monitoring stuff
#
from Configurables import GaudiSequencer, PrintDecayTree
exampleSeq = GaudiSequencer("ExampleSeq")
tree = PrintDecayTree("PrintBu2LLK")
exampleSeq.Members += [ tree ]
tree.InputLocations = [ "Presel09Bu2LLK" ]
########################################################################
#
# Flavour tagging. Don't ask why you'd be tagging a B+...
#
from Configurables import BTagging
tag = BTagging("BTagging")
tag.InputLocations = [ "Presel09Bu2LLK" ]
########################################################################
#
# Standard configuration
#
from Configurables import DaVinci
DaVinci().EvtMax = 100                         # Number of events
DaVinci().SkipEvents = 0                       # Events to skip
DaVinci().DataType = "MC09"                    # Default is "MC09"
DaVinci().Simulation   = True
DaVinci().HistogramFile = "DVHistos_1.root"    # Histogram file
DaVinci().TupleFile = "DVNtuples.root"         # Ntuple
DaVinci().UserAlgorithms = [ preselSeq, tag ]  # The algorithms
DaVinci().MoniSequence = [ exampleSeq ]        # Monitoring
# DaVinci().MainOptions  = "" # None
########################################################################
# HLT
# DaVinci().ReplaceL0BanksWithEmulated = True ## enable if you want to rerun L0
# DaVinci().Hlt2Requires = 'L0+Hlt1'          ## change if you want Hlt2 irrespective of Hlt1
# DaVinci().HltType = 'Hlt1+Hlt2'             ## pick one of 'Hlt1', 'Hlt2', or 'Hlt1+Hlt2'
########################################################################
MessageSvc().Format = "% F%60W%S%7W%R%T %0W%M"
########################################################################
#
# To run : gaudirun.py options/DaVinci.py options/DaVinciTestData.py
#
########################################################################
