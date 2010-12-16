
from Gaudi.Configuration import *
from Configurables import Moore
#from LHCbKernel.Configuration import *
#from GaudiConf.Configuration import *
from Configurables import L0Conf, HltConf

Moore().L0=True
Moore().ReplaceL0BanksWithEmulated = True
Moore().ForceSingleL0Configuration = True
Moore().EnableRunChangeHandler = False
Moore().HistogrammingLevel= ''
HistogramPersistencySvc().OutputFile = 'MooreHistoMCTest.root'
Moore().EnableMonitoring = True
Moore().EvtMax         = -1
Moore().UseDBSnapshot  = False
Moore().Verbose        = True
Moore().CheckOdin      = False
Moore().outputFile    = 'testMC.dst'

Moore().UseTCK=True
Moore().InitialTCK = '0x002E002A'
