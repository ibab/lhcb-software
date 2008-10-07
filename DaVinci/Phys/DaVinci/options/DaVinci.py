########################################################################
#
#$Id: DaVinci.py,v 1.11 2008-10-07 13:43:47 jpalac Exp $
#
# Options for a typical DaVinci job
#
# @author Patrick Koppenburg
# @date 2008-08-06
#
########################################################################
from os import environ
from Gaudi.Configuration import *
from Configurables import GaudiSequencer, PrintTree, PhysDesktop, BTagging, MagneticFieldSvc, MakeResonances
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
from HltConf.Configuration import *
HltConf().replaceL0DUBankWithEmulated = True
old = False
if old :
    HltConf().hltType = 'Physics_Hlt1'
    #HltConf().hltType = 'Physics_Hlt1+Hlt2'
else :
    HltConf().oldStyle = False
    HltConf().hltType = 'PA+LU+VE+MU+HA+EL+PH'

HltConf().applyConf()
#
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

MagneticFieldSvc().OutputLevel = 1 
MakeResonances("DiLeptonForPreselBu2LLK").OutputLevel = 1 
