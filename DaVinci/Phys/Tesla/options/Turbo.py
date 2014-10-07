from Configurables import Tesla, TeslaReportAlgo, GaudiSequencer
from Configurables import DecodeRawEvent, RecombineRawEvent
from Gaudi.Configuration import *

RecombineRawEvent()
DecodeRawEvent().DataOnDemand = True

from Configurables import ConfigTarFileAccessSvc
ConfigTarFileAccessSvc().File='/afs/cern.ch/work/s/sbenson/public/forTeslaExtendedReps/config.tar'

MyTesla = Tesla()

trig1 = TeslaReportAlgo("TeslaReportAlgo")
trig1.TriggerLine = "Hlt2IncPhi"
trig1.ReportVersion=2

MyTesla.EvtMax = -1
MyTesla.EnableDataOnDemand = True
MyTesla.outputFile = "DEFINEOUTPUTLOCATION"

from Configurables import EventSelector
EventSelector().PrintFreq = 1000

from GaudiConf.IOHelper import IOHelper
ioh = IOHelper()
ioh.setupServices()
ioh.inputFiles([
"/PATH/TO/DST/CONTIANING/EXTENDED/SELECTION/REPORTS"
])
