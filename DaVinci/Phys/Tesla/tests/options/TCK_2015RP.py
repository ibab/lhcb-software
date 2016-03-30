from Configurables import Tesla 
Tesla().Pack = True
Tesla().DataType = '2015'
Tesla().Simulation = False
Tesla().Mode = 'Offline'
Tesla().VertRepLoc = 'Hlt2'
Tesla().EvtMax=1000
Tesla().outputFile = "tesla_2015_TCK.dst"

from Gaudi.Configuration import *
version='v10r0_0x00fa0051'
from TurboStreamProd.helpers import *
from TurboStreamProd import prodDict
lines = streamLines(prodDict,version,'DiMuon',debug=True)
lines += streamLines(prodDict,version,'Charm',debug=True)
lines += streamLines(prodDict,version,'CharmSpec',debug=True)

Tesla().TriggerLines = lines

from PRConfig.TestFileDB import test_file_db
input = test_file_db['TeslaTest_TCK_0x022600a2']
input.run(configurable=Tesla())
