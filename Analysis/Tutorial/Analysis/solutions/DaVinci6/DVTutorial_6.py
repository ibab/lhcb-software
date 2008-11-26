########################################################################
#
# Options for exercise 6
#
# @author Patrick Koppenburg
# @date 2008-06-03
#
########################################################################
from os import environ
import GaudiKernel.SystemOfUnits as Units 
from Gaudi.Configuration import *
from Configurables import PhysDesktop
from Configurables import TutorialChecker
#
# Standard configuration
#
importOptions( "$ANALYSISROOT/solutions/DaVinci4/DVTutorial_4b.py" )
#
# Standard Trigger configuration (not deafult)
#
from HltConf.Configuration import *
HltConf().replaceL0BanksWithEmulated = True         ## enable if you want to rerun L0  
HltConf().Hlt2IgnoreHlt1Decision = True             ## enable if you want Hlt2 irrespective of Hlt1
HltConf().hltType = 'Hlt1+Hlt2'                     ## pick one of 'Hlt1', 'Hlt2', or 'Hlt1+Hlt2'
#
# B tagging options
#
importOptions( "$FLAVOURTAGGINGROOT/options/BTaggingTool.opts" )
#
# Print Reconstructed Bs
#
checker = TutorialChecker() 
checker.addTool( PhysDesktop() )
checker.PhysDesktop.InputLocations = [ "Phys/Bs2JpsiPhi" ]
GaudiSequencer("TutorialSeq").Members.append(checker)

HistogramPersistencySvc().OutputFile = "DVHistos_6.root"
NTupleSvc().Output = ["FILE1 DATAFILE='Tuple.root' TYP='ROOT' OPT='NEW'"]

ApplicationMgr().EvtMax = 1000 
EventSelector().PrintFreq = 1 
