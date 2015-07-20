# Options file for the QMTest to check that the containers have not
# changed, either in values or in size.

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
Tesla().OutputLevel = 2
Tesla().EvtMax = -1
Tesla().Pack = False

from PRConfig.TestFileDB import test_file_db
input = test_file_db['Tesla_Bsphiphi_MC12wTurbo']
input.run(configurable=Tesla())

import GaudiPython
from Gaudi.Configuration import ApplicationMgr
from Configurables import LoKiSvc

from GaudiKernel import ROOT6WorkAroundEnabled
if ROOT6WorkAroundEnabled('ROOT-7492'):
    # trigger autoloading of LHCbKernel dictionary
    GaudiPython.gbl.LHCb.LHCbID
    # trigger autoloading of DigiEvent dictionary
    GaudiPython.gbl.LHCb.CaloDigit

gaudi = GaudiPython.AppMgr()
gaudi.initialize()

gaudi.algorithm("TeslaReportAlgo"+prefix).Enable = False
gaudi.algorithm("DstWriter").Enable = False

tes = gaudi.evtsvc()

selRepLoc = "Hlt2/SelReports"

partLoc ="/Event/Turbo/" + prefix + "/Particles"
protoLoc ="/Event/Turbo/" + prefix + "/Protos"
trackLoc ="/Event/Turbo/" + prefix + "/Tracks"
vertLoc ="/Event/Turbo/" + prefix + "/Vertices"

i_partTot=0.0
i_trackTot=0.0
i_protoTot=0.0
i_vertTot=0.0
#
n=0
n_pop=0

#
meanMass = 0.0
meanTrackChi2 = 0.0
meanVertexChi2 = 0.0
meanDLLK = 0.0
#
meanSizePart = 0.0
meanSizeTrack = 0.0
meanSizeVert = 0.0
meanSizeProto = 0.0

#while True:
while n<1000:
    n+=1
    #
    gaudi.run(1)
    if not tes['/Event/DAQ/RawEvent']:
        print "End of file"
        break
    #
    gaudi.executeEvent()
    gaudi.algorithm("TeslaReportAlgo"+prefix).execute()


    parts   = tes[partLoc]
    tracks  = tes[trackLoc]
    verts   = tes[vertLoc]
    protos  = tes[protoLoc]
    if not parts:
        continue
    else:
        n_pop+=1.0
    for part in parts:
        i_partTot+=1.0
        meanMass += part.measuredMass()
    for track in tracks:
        i_trackTot+=1.0
        meanTrackChi2 += track.chi2PerDoF()
    for vert in verts:
        i_vertTot+=1.0
        meanVertexChi2 += vert.chi2()
    for proto in protos:
        i_protoTot+=1.0
        meanDLLK += proto.info(603,0)
meanSizePart = i_partTot / n_pop
meanMass = meanMass / ( i_partTot  )
meanSizeTrack = i_trackTot / n_pop
meanTrackChi2 = meanTrackChi2 / ( i_trackTot  )
meanSizeVert = i_vertTot / n_pop
meanVertexChi2 = meanVertexChi2 / ( i_vertTot )
meanSizeProto = i_protoTot / n_pop
meanDLLK = meanDLLK / ( i_protoTot )

# print container sizes and mean of interested quantities
print "Particle Check ***********************************"
print "Average mass = "+str(round(meanMass,1))
print "Average container size = "+str(round(meanSizePart,1))
print "Track Check **************************************"
print "Average chi2 = "+str(round(meanTrackChi2,2))
print "Average container size = "+str(round(meanSizeTrack,1))
print "Vertex Check *************************************"
print "Average chi2 = "+str(round(meanVertexChi2,3))
print "Average container size = "+str(round(meanSizeVert,1))
print "Proto Check **************************************"
print "Average DLLK = "+str(round(meanDLLK,3))
print "Average container size = "+str(round(meanSizeProto,1))
