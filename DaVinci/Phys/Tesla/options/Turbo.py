# SAMPLE FOR TUTORIAL: RunningTesla
# FOR LATEST SCRIPT SEE Turbo_NewLine.py

from Configurables import Tesla 
from Gaudi.Configuration import *

from Configurables import RecombineRawEvent, DecodeRawEvent
RecombineRawEvent()
DecodeRawEvent().DataOnDemand = True

from Configurables import ConfigTarFileAccessSvc
ConfigTarFileAccessSvc().File='/afs/cern.ch/work/s/sbenson/public/forTeslaExtendedReps/config.tar'

Tesla().TriggerLine = "Hlt2IncPhi"
Tesla().ReportVersion=2
Tesla().EvtMax = -1

from GaudiConf.IOHelper import IOHelper
ioh = IOHelper()
ioh.setupServices()
ioh.inputFiles([
    "root://eoslhcb.cern.ch//eos/lhcb/user/s/sbenson/DSTs/TeslaChain/RemadeSel_phiphi_1k_Tutorial_0044.dst"
])
Tesla().outputFile = "Turbo.dst"
