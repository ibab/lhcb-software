from Configurables import Tesla 
from Gaudi.Configuration import *

from Configurables import RecombineRawEvent, DecodeRawEvent
RecombineRawEvent()
DecodeRawEvent().DataOnDemand = True

## Tell HltANNSvc the ID of your line 
# NEEDED WHEN NOT RUNNING FROM A TCK
# YOU CAN FIND THE ID IN THE PYTHON IMPLEMENTATION
# IN Hlt/Hlt2Lines
from Configurables import HltANNSvc
Hlt2ID = HltANNSvc().Hlt2SelectionID
if "Hlt2Global" not in Hlt2ID : Hlt2ID.update( {  "Hlt2Global" : 2 } )
HltANNSvc().Hlt1SelectionID = { 'PV3D' : 10103
        ,'ProtoPV3D' : 10117
}
Hlt2ID.update(  { 'Hlt2IncPhiDecision'           : 50000
    ,'Hlt2IncPhiSidebandsDecision'  : 50003
    ,'Hlt2IncPhiTrackFitDecision'   : 50002
    } )
#from Configurables import ConfigTarFileAccessSvc
#ConfigTarFileAccessSvc().File='/afs/cern.ch/user/s/sbenson/cmtuser/Moore_v23r2/TCKData/config.tar'

Tesla().TriggerLines = ["Hlt2IncPhi"] # Enter your trigger line here
Tesla().EvtMax = -1
Tesla().OutputLevel = DEBUG

from GaudiConf.IOHelper import IOHelper
ioh = IOHelper()
ioh.setupServices()
ioh.inputFiles([
    "/PATH/TO/INPUT"
])
Tesla().outputFile = "OUTPUT"
