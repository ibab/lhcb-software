########################################################################
#
# Options for exercise 6
#
# @author Patrick Koppenburg
# @date 2008-06-03
#
########################################################################
import GaudiKernel.SystemOfUnits as Units 
from Gaudi.Configuration import *
from Configurables import PhysDesktop
from Configurables import TutorialChecker
#
# Standard configuration
#
importOptions( "$ANALYSISROOT/solutions/DaVinci4/DVTutorial_4.py" )
#
# Standard Trigger configuration (not default)
#
from HltConf.Configuration import *
HltConf().replaceL0BanksWithEmulated = True 
HltConf().Hlt2IgnoreHlt1Decision = True             ## enable if you want Hlt2 irrespective of Hlt1
HltConf().hltType = 'Hlt1+Hlt2'                     ## pick one of 'Hlt1', 'Hlt2', or 'Hlt1+Hlt2'
HltConf().applyConf()
#
# B tagging options
#
# importOptions( "$FLAVOURTAGGINGROOT/options/BTaggingTool.opts" )
#
# Checker
#
checker = TutorialChecker() 
checker.addTool( PhysDesktop() )
checker.PhysDesktop.InputLocations = [ "Bs2JpsiPhi" ]
ApplicationMgr().TopAlg += [ checker ]

HistogramPersistencySvc().OutputFile = "DVHistos_6.root"
NTupleSvc().Output = ["FILE1 DATAFILE='Tuple.root' TYP='ROOT' OPT='NEW'"]

ApplicationMgr().EvtMax = 100
EventSelector().PrintFreq = 1 
