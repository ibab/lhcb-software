########################################################################
#
# Options for a typical DaVinci job
#
# @author Patrick Koppenburg
# @date 2008-08-06
#
########################################################################
from os import environ
from Gaudi.Configuration import *
from Configurables import GaudiSequencer, PrintTree, PhysDesktop, BTagging
########################################################################
#
# Standard configuration
#
# @todo : Replace by a python Configuration, like in Brunel
#
importOptions( "$DAVINCIROOT/options/DaVinciCommon.opts" )
########################################################################
#
# Trigger. Uncomment what you need. Hlt1 needs L0, Hlt2 doesn't.
#
# importOptions( "$L0DUROOT/options/ReplaceL0DUBankWithEmulated.opts" )
#
# Temporary hack to re-run L0
#
#importOptions('$L0DUROOT/options/L0DUBankSwap.opts')
importOptions('$L0DUROOT/options/DefaultTCK.opts')
#
#
importOptions( "$HLTSYSROOT/options/Hlt1.opts" )
importOptions( "$HLTSYSROOT/options/Hlt2.opts" )
########################################################################
#
# enable standard particles monitors
#
importOptions( "$COMMONPARTICLESROOT/options/EnableStandardParticleMonitors.opts" )
########################################################################
#
# Run some DC06 preselection
#
importOptions( "$B2DILEPTONROOT/options/DoPreselBu2LLK.opts" )
########################################################################
#
# Example of running some algorithms
#
ApplicationMgr().TopAlg += [ GaudiSequencer("ExampleSeq") ]
tree = PrintTree("PrintBu2LLK")
GaudiSequencer("ExampleSeq").Members += [ tree ]
tree.addTool( PhysDesktop() )
tree.PhysDesktop.InputLocations = [ "Phys/PreselBu2LLK" ]
########################################################################
#
# Flavour tagging. Don't ask why you'd be tagging a B+...
#
# Uncommenting this does not work. Why ?
#
# importOptions( "$FLAVOURTAGGINGOPTS/BTagging.opts" )
# tag = BTagging()
# tag.addTool( PhysDesktop() ) 
# tag.PhysDesktop.InputLocations = [ "Phys/PreselBu2LLK" ]
########################################################################
#
# Histogram File
#
HistogramPersistencySvc().OutputFile = "DVHistos_1.root"
########################################################################
#
# nTuple File
#
ApplicationMgr().ExtSvc += [ "NTupleSvc" ]
NTupleSvc().Output=["FILE1 DATAFILE='DVNtuples.root' TYP='ROOT' OPT='NEW'"]
########################################################################
#
# Event numbers
#
ApplicationMgr().EvtMax = 1000 
EventSelector().FirstEvent = 1
EventSelector().PrintFreq  = 100
########################################################################
#
# Some test data
#
importOptions( "$DAVINCIROOT/options/DaVinciTestData.opts" )
########################################################################

