from Configurables import Tesla 
from Gaudi.Configuration import *

from Configurables import RecombineRawEvent, DecodeRawEvent
RecombineRawEvent()
DecodeRawEvent().DataOnDemand = True

# Path to private TCK
from Configurables import ConfigTarFileAccessSvc
ConfigTarFileAccessSvc().File='/afs/cern.ch/user/s/sbenson/cmtuser/Moore_v23r2/TCKData/config.tar'

Tesla().TriggerLines = ["Hlt2IncPhi"] # Enter your trigger line here
Tesla().EvtMax = -1

from GaudiConf.IOHelper import IOHelper
ioh = IOHelper()
ioh.setupServices()
ioh.inputFiles([
    "PATH/TO/HLT/PROCESSED/DATA"
])
Tesla().outputFile = "PATH/TO/OUTPUT"
