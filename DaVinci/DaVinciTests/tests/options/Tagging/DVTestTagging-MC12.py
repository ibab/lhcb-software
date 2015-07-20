########################################################################
#
# Example Options for BTagging algorithm
#
# @author Marco Musy
# @date 2010-09-29
#
########################################################################
from Gaudi.Configuration import *
from Configurables import GaudiSequencer
########################################################################
#
# Some selections. 
#
from Configurables import CheatedSelection, BDecayTool, MCDecayFinder

from GaudiKernel import ROOT6WorkAroundEnabled
if ROOT6WorkAroundEnabled('ROOT-7492'):
    # trigger autoloading of LHCbKernel dictionary
    GaudiPython.gbl.LHCb.LHCbID
    # trigger autoloading of DigiEvent dictionary
    GaudiPython.gbl.LHCb.CaloDigit


myDecay = '[B+ -> (J/psi(1S) => mu+ mu- ) K+]cc'

cheatsel = CheatedSelection("CheatedSelection")
cheatsel.Inputs = [ "Phys/TaggingPions" ]
cheatsel.AssociatorInputData = [ "Phys/CheatedSelection/Particles" ]
cheatsel.OutputLevel = 5

cheatsel.addTool( BDecayTool )
cheatsel.BDecayTool.addTool( MCDecayFinder )
cheatsel.BDecayTool.MCDecayFinder.Decay = myDecay 
########################################################################
#
# Flavour tagging. 
#
location = "Phys/CheatedSelection"

####
from Configurables import BTagging, BTaggingTool, BTaggingChecker, BDecayTool, MCDecayFinder

tag = BTagging("BTagging")
tag.Inputs = [ location ]
#tag.OutputLevel = 5
tag.addTool( BDecayTool )
tag.BDecayTool.addTool( MCDecayFinder )
tag.BDecayTool.MCDecayFinder.Decay = myDecay
tag.addTool( BTaggingTool )
tag.BTaggingTool.ChoosePVCriterium = "PVbyIP" #needed by CheatedSel


# Reco14
from FlavourTagging.Tunings import TuneTool
TuneTool(tag,"Stripping23") 

# Flavour tagging Checker:
tagcheck = BTaggingChecker("BTaggingChecker")
tagcheck.Inputs = [ location ]
tagcheck.TagsLocation = location+"/FlavourTags"
#tagcheck.OutputLevel = 1
tagcheck.addTool( BDecayTool )
tagcheck.BDecayTool.addTool( MCDecayFinder )
tagcheck.BDecayTool.MCDecayFinder.Decay = myDecay
MessageSvc().Format = "% F%40W%S%7W%R%T %0W%M"
########################################################################
#
# Standard configuration
#
from Configurables import DaVinci
DaVinci().EvtMax     = 1000                       # Number of events
DaVinci().SkipEvents = 0                          # Events to skip
DaVinci().PrintFreq  = 100
DaVinci().DataType = "2012"
DaVinci().CondDBtag = 'sim-20121025-vc-mu100'
DaVinci().DDDBtag = "dddb-20120831"
DaVinci().Simulation    = True
from Configurables import StoreExplorerAlg, PrintDecayTree
PrintDecayTree().Inputs = [ location ] 
DaVinci().MoniSequence = [   #PrintDecayTree()
                             cheatsel
                            ,tag
                            ,tagcheck
                            ]  # The algorithms

#from GaudiConf import IOHelper 
#IOHelper().inputFiles(['PFN:root://eoslhcb.cern.ch//eos/lhcb/freezer/lhcb/MC/2012/ALLSTREAMS.DST/00021062/0000/00021062_00000001_1.allstreams.dst']) # Bu2JpsiK 

