from Configurables import Tesla 
from Gaudi.Configuration import *

Tesla().TriggerLines = ["Hlt2CharmHadMinBiasD02KPi"]
Tesla().OutputLevel = 4
Tesla().EvtMax = 2000
Tesla().outputFile = "Turbo_FSRcreated.dst"
Tesla().Mode="Online"
Tesla().Pack=True

from PRConfig.TestFileDB import test_file_db
#input = test_file_db['2012_raw_L041']
input = test_file_db['2012_raw_full']
input.run(configurable=Tesla())
#from GaudiConf import IOHelper
#IOHelper("MDF").inputFiles( ["mdf:root://eoslhcb.cern.ch//eos/lhcb/grid/prod/lhcb/swtest/lhcb/data/2012/RAW/FULL/LHCb/COLLISION12/114753/114753_0000000015.raw"] , clear=True ) 
