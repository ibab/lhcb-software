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
from Configurables import CheatedSelection

cheatsel = CheatedSelection("CheatedSelection")
cheatsel.Inputs = [ "Phys/TaggingPions" ]
cheatsel.AssociatorInputData = [ "Phys/CheatedSelection/Particles" ]
cheatsel.OutputLevel = 6

########################################################################
#
# Flavour tagging. 
#
location = "Phys/CheatedSelection"

from Configurables import BTagging, BTaggingTool, BTaggingChecker

tag = BTagging("BTagging")
tag.Inputs = [ location ]
tag.OutputLevel = 3

tag.addTool( BTaggingTool )
tag.BTaggingTool.ChoosePVCriterium = "PVbyIP" #needed by CheatedSel


# Flavour tagging Checker:
tagcheck = BTaggingChecker("BTaggingChecker")
tagcheck.Inputs = [ location ]
tagcheck.TagsLocation = location+"/FlavourTags"

MessageSvc().Format = "% F%40W%S%7W%R%T %0W%M"
########################################################################
#
# Standard configuration
#
from Configurables import DaVinci
DaVinci().EvtMax     = 500                       # Number of events
DaVinci().SkipEvents = 0                          # Events to skip
DaVinci().PrintFreq  = 1
DaVinci().DataType = "2010"
DaVinci().CondDBtag = 'sim-20101210-vc-md100'
DaVinci().DDDBtag = "head-20101206"
DaVinci().Simulation    = True
from Configurables import StoreExplorerAlg, PrintDecayTree
PrintDecayTree().Inputs = [ location ] 
DaVinci().MoniSequence = [   #PrintDecayTree()
                             cheatsel
                            ,tag
                            ,tagcheck
                            ]  # The algorithms


########################################################################
#
# example data file
# bsjpsiphi
#DaVinci().Input = [ "DATAFILE='PFN:/usera/jonesc/NFS/data/MC/MC10/DST/BtaggingTest/00008922_00000001_1.allstreams.dst' TYP='POOL_ROOTTREE' OPT='READ'" ]
DaVinci().Input = [ "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/freezer/lhcb/MC/MC10/ALLSTREAMS.DST/00008922/0000/00008922_00000001_1.allstreams.dst?svcClass=lhcbdisk' TYP='POOL_ROOTTREE' OPT='READ'" ]
#from Configurables import CondDB
#CondDB().LocalTags['SIMCOND'] = ['sim-20100429-vc-md100']
