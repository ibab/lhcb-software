########################################################################
#
# $Id: DaVinci.py,v 1.23 2008-12-22 15:33:27 pkoppenb Exp $
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
importOptions("$DAVINCIROOT/options/PreloadUnits.opts")
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
DaVinci().EvtMax = 1000                        # Number of events
DaVinci().SkipEvents = 0                       # Events to skip
DaVinci().DataType = "2008"                    # Default is "DC06"
DaVinci().Simulation   = True
DaVinci().HistogramFile = "DVHistos_1.root"    # Histogram file
DaVinci().TupleFile = "DVNtuples.root"         # Ntuple
DaVinci().UserAlgorithms = [ preselSeq, exampleSeq, tag ]
# DaVinci().MainOptions  = "" # None
########################################################################
# HLT
DaVinci().ReplaceL0BanksWithEmulated = True ## enable if you want to rerun L0
DaVinci().Hlt2IgnoreHlt1Decision = True     ## enable if you want Hlt2 irrespective of Hlt1
DaVinci().HltType = 'Hlt1+Hlt2'             ## pick one of 'Hlt1', 'Hlt2', or 'Hlt1+Hlt2'
DaVinci().HltOldStyle = False               ## Go for the new thing
########################################################################
#
# To run : gaudirun.py options/DaVinci.py options/DaVinciTestData.py
#
########################################################################
