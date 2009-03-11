########################################################################
#
# $Id: DaVinci.py,v 1.34 2009-03-11 17:56:30 pkoppenb Exp $
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
importOptions("$B2DILEPTONROOT/options/DoPreselBu2LLK.opts")
preselSeq = GaudiSequencer("SeqPreselBu2LLK")
########################################################################
#
# Some Monitoring stuff
#
from Configurables import GaudiSequencer, PrintDecayTree, PhysDesktop
exampleSeq = GaudiSequencer("ExampleSeq")
tree = PrintDecayTree("PrintBu2LLK")
exampleSeq.Members += [ tree ]
tree.addTool( PhysDesktop )
tree.PhysDesktop.InputLocations = [ "PreselBu2LLK" ]
########################################################################
#
# Flavour tagging. Don't ask why you'd be tagging a B+...
#
from Configurables import BTagging, PhysDesktop
tag = BTagging("BTagging")
tag.addTool( PhysDesktop )
tag.PhysDesktop.InputLocations = [ "PreselBu2LLK" ]
########################################################################
#
# Standard configuration
#
from Configurables import DaVinci
DaVinci().EvtMax = 100                         # Number of events
DaVinci().SkipEvents = 0                       # Events to skip
#DaVinci().DataType = "2008"                    # Default is "DC06"
DaVinci().Simulation   = True
DaVinci().HistogramFile = "DVHistos_1.root"    # Histogram file
DaVinci().TupleFile = "DVNtuples.root"         # Ntuple
DaVinci().UserAlgorithms = [ preselSeq, tag ]  # The algorithms
DaVinci().MoniSequence = [ exampleSeq ]        # Monitoring
# DaVinci().MainOptions  = "" # None
########################################################################
# HLT
# DaVinci().ReplaceL0BanksWithEmulated = True ## enable if you want to rerun L0
DaVinci().Hlt2Requires = 'L0+Hlt1'          ## change if you want Hlt2 irrespective of Hlt1
#DaVinci().HltType = 'Hlt1+Hlt2'             ## pick one of 'Hlt1', 'Hlt2', or 'Hlt1+Hlt2'
########################################################################
#
# To run : gaudirun.py options/DaVinci.py options/DaVinciTestData.py
#
########################################################################
