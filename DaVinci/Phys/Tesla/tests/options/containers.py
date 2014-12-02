# Options file for the QMTest to check that the containers have not 
# changed, either in values or in size.

from Configurables import Tesla 
from Gaudi.Configuration import *

from Configurables import RecombineRawEvent, DecodeRawEvent
RecombineRawEvent()
DecodeRawEvent().DataOnDemand = True

from Configurables import ConfigTarFileAccessSvc
ConfigTarFileAccessSvc().File='../../options/config.tar'

prefix = "Hlt2IncPhi"

Tesla().TriggerLines = ["Hlt2IncPhi"]
Tesla().ReportVersion = 2
Tesla().OutputPrefix = prefix
Tesla().OutputLevel = 4
Tesla().EvtMax = -1
Tesla().PreSplit = True

from GaudiConf.IOHelper import IOHelper
IOHelper().inputFiles( [ "PFN:root://castorlhcb.cern.ch//castor/cern.ch/user/s/sbenson/TempTestFiles/RemadeSel_phiphi_1k_Tutorial_0044.dst" ] ) 
#Tesla().outputFile = "Turbo.dst"

import GaudiPython
from Gaudi.Configuration import ApplicationMgr
from Configurables import LoKiSvc

gaudi = GaudiPython.AppMgr()
gaudi.initialize()

gaudi.algorithm('TeslaReportAlgo').Enable = False
gaudi.algorithm('Writer').Enable = False

tes = gaudi.evtsvc()

selRepLoc = "Hlt/SelReports"

partLoc ="/Event/" + prefix + "/Particles"
protoLoc ="/Event/" + prefix + "/Protos"
trackLoc ="/Event/" + prefix + "/Tracks"
vertLoc ="/Event/" + prefix + "/Vertices"

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
    #
    gaudi.executeEvent()
    gaudi.algorithm("TeslaReportAlgo").execute()

    if not tes['/Event/DAQ/RawEvent']:
        print "End of file"
        break
    
    parts   = tes[partLoc]
    tracks  = tes[trackLoc]
    verts   = tes[vertLoc]
    protos  = tes[protoLoc]
    if not parts.size()>0:
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
