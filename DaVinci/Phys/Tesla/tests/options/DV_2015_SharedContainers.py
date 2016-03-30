from Gaudi.Configuration import *
from LHCbKernel.Configuration import *
from Configurables import GaudiSequencer,RawEventJuggler
from Configurables import DaVinci
from Configurables import DecodeRawEvent

from Configurables import DstConf
DstConf().Turbo=True

DaVinci()
DaVinci().EvtMax=-1
DaVinci().DataType="2015"

from GaudiConf import IOHelper
IOHelper().inputFiles( ["tesla_2015_TCK.dst"] , clear=True ) 

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

tes = gaudi.evtsvc()

vertLoc = "Turbo/Primary"
protoLoc = "Turbo/Protos"
trackLoc = "Turbo/Tracks"
rpidLoc = "Turbo/RichPIDs"
mpidLoc = "Turbo/MuonPIDs"
hypoLoc = "Turbo/CaloHypos"
clusterLoc = "Turbo/CaloClusters"

n_events=0
n_vert=0
n_proto=0
n_track=0
n_rpid=0
n_mpid=0
n_hypo=0
n_cluster=0

version='v10r0_0x00fa0051'
from TurboStreamProd.helpers import *
from TurboStreamProd import prodDict
lines = streamLines(prodDict,version,'DiMuon',debug=True)
lines += streamLines(prodDict,version,'Charm',debug=True)
lines += streamLines(prodDict,version,'CharmSpec',debug=True)

while True:
    gaudi.run(1)
    #
    if not tes['/Event/DAQ/RawEvent']:
        print "End of file"
        break
    #
    n=0
    for line in lines:
        rep = tes["Hlt2/SelReports"].selReport(line+"Decision")
        if rep:
            n+=1
    if n==0:
        continue

    n_events+=1
    n_vert+=tes[vertLoc].size()
    n_proto+=tes[protoLoc].size()
    n_track+=tes[trackLoc].size()
    n_rpid+=tes[rpidLoc].size()
    n_mpid+=tes[mpidLoc].size()
    n_hypo+=tes[hypoLoc].size()
    n_cluster+=tes[clusterLoc].size()

print "Average size of PV container = "+str( float(n_vert)/float(n_events) )
print "Average size of ProtoParticle container = "+str( float(n_proto)/float(n_events) )
print "Average size of Track container = "+str( float(n_track)/float(n_events) )
print "Average size of RICH PID container = "+str( float(n_rpid)/float(n_events) )
print "Average size of Muon PID container = "+str( float(n_mpid)/float(n_events) )
print "Average size of CaloHypo container = "+str( float(n_hypo)/float(n_events) )
print "Average size of CaloCluster container = "+str( float(n_cluster)/float(n_events) )

