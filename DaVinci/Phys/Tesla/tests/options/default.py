# Options file for the QMTest to check that Tesla produces no errors

from Configurables import Tesla 
from Gaudi.Configuration import *

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
HltANNSvc().InfoID = {
        "FastTTValidationTool/ValidateWithTT"        : 6350,
        "MatchVeloMuon"                              : 6400,
        "IsMuonTool"                                 : 6401,
        "PatForwardTool/LooseForward"                : 6299,
        "PatForwardTool/TightForward"                : 6300,
        'Hlt2Topo2BodyBBDTResponse'                  : 6502,
        'Hlt2Topo3BodyBBDTResponse'                  : 6303,
        'Hlt2Topo4BodyBBDTResponse'                  : 6304
        }

prefix = "Hlt2IncPhi"

Tesla().TriggerLines = ["Hlt2IncPhi"]
Tesla().OutputLevel = 4
Tesla().EvtMax = -1

from PRConfig.TestFileDB import test_file_db
input = test_file_db['Tesla_Bsphiphi_MC12wTurbo']
input.run(configurable=Tesla())
