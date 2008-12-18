########################################################################
#
# $Id: DaVinci.py,v 1.20 2008-12-18 09:57:52 pkoppenb Exp $
#
# Options for a typical DaVinci job
#
# @author Patrick Koppenburg
# @date 2008-08-06
#
########################################################################
from Gaudi.Configuration import *
########################################################################
########################################################################
################ First define all things to run ########################
########################################################################
########################################################################
#
# If you want to import .opts options, do this first
#
importOptions("$DAVINCIROOT/options/PreloadUnits.opts")
########################################################################
#
# Hlt always goes first ? 
#
########################################################################
#
# Some preselection. This defines a GaudiSequencer.
#
importOptions("$B2DILEPTONROOT/options/DoPreselBu2LLK.opts")
from Configurables import GaudiSequencer
preselSeq = GaudiSequencer("SeqPreselBu2LLK")
########################################################################
#
# Some Monitoring stuff
#
from Configurables import GaudiSequencer, PrintTree, PhysDesktop
exampleSeq = GaudiSequencer("ExampleSeq")
tree = PrintTree("PrintBu2LLK")
exampleSeq.Members += [ tree ]
tree.addTool( PhysDesktop() )
tree.PhysDesktop.InputLocations = [ "PreselBu2LLK" ]
########################################################################
#
# Flavour tagging. Don't ask why you'd be tagging a B+...
#
from Configurables import BTagging, PhysDesktop
tag = BTagging("BTagging")
tag.addTool( PhysDesktop() )
tag.PhysDesktop.InputLocations = [ "PreselBu2LLK" ]
########################################################################
#
# Standard configuration
#
from Configurables import DaVinci
DaVinci().EvtMax = 1000
DaVinci().SkipEvents = 0
DaVinci().DataType = "DC06" # Default is "DC06"
DaVinci().Simulation   = True
DaVinci().HistogramFile = "DVHistos_1.root" # Histogram file
DaVinci().TupleFile = "DVNtuples.root"  # Ntuple
DaVinci().UserAlgorithms = [ preselSeq, exampleSeq, tag ]
# DaVinci().MainOptions  = "" # None
########################################################################
#
# To run : gaudirun.py options/DaVinci.py options/DaVinciTestData.py
#
