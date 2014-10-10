from Configurables import Tesla 
from Gaudi.Configuration import *

from Configurables import RecombineRawEvent, DecodeRawEvent
RecombineRawEvent()
DecodeRawEvent().DataOnDemand = True

from Configurables import ConfigTarFileAccessSvc
ConfigTarFileAccessSvc().File='/afs/cern.ch/work/s/sbenson/public/forTeslaExtendedReps/config.tar'

Tesla().TriggerLine = "Hlt2CharmHadD02HHXDst_hhX"
Tesla().ReportVersion=2
Tesla().OutputLevel = DEBUG
Tesla().EvtMax = -1

from GaudiConf.IOHelper import IOHelper
ioh = IOHelper()
ioh.setupServices()
ioh.inputFiles([
"HLTOUTPUT"
])
Tesla().outputFile = "Turbo.dst"
