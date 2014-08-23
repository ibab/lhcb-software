from Configurables import Moore

from Configurables import RecombineRawEvent
RecombineRawEvent()

Moore().ForceSingleL0Configuration = False
evtmax=3000
Moore().EvtMax = evtmax
Moore().OutputLevel =3
Moore().UseTCK = False
Moore().ThresholdSettings = 'Physics_September2012'
Moore().DataType   = "2012"
Moore().Split = ''
Moore().Simulation = True
Moore().CheckOdin = False
Moore().EnableDataOnDemand = True
Moore().WriterRequires = []
Moore().DDDBtag   = "dddb-20130929-1"
Moore().CondDBtag = "sim-20131023-vc-md100"


from GaudiConf import IOHelper
IOHelper().inputFiles( [ "PFN:root://castorlhcb.cern.ch//castor/cern.ch/user/s/sbenson/TempTestFiles/00026133_00000013_1.allstreams.dst" ] ) 

from Configurables import Hlt__Service
Hlt__Service().Pedantic = False
from Configurables import DataOnDemandSvc
from Configurables import DstConf, CaloDstUnPackConf
CaloDstUnPackConf().Enable=True
DstConf().EnableUnpack= ["Reconstruction","Stripping"]
from Configurables import L0MuonAlg
L0MuonAlg("L0Muon").L0DUConfigProviderType="L0DUConfigProvider"
from Configurables import L0Conf
L0Conf().EnableL0DecodingOnDemand = True
L0Conf().FastL0DUDecoding = True
L0Conf().DecodeL0DU = True
L0Conf().TCK = '0x0045'

#from Configurables import Hlt2Conf
#Hlt2Conf().Hlt1TrackOption = "Rerun"

import GaudiPython
from Gaudi.Configuration import ApplicationMgr
from Configurables import LoKiSvc

appMgr = ApplicationMgr()
#
from Configurables import EventNodeKiller
enk = EventNodeKiller('KillTrigRawEvent')
enk.Nodes = [ "Hlt","Hlt1","Hlt2" ]
appMgr.TopAlg.insert( 0,  enk.getFullName() ) 
#
appMgr.ExtSvc += ['DataOnDemandSvc'] #breaks for real data
appMgr.ExtSvc += ['ToolSvc', 'DataOnDemandSvc', LoKiSvc()]
#appMgr.TopAlg += [ seq ]
gaudi = GaudiPython.AppMgr(outputlevel=4)
gaudi.initialize()

gaudi.algorithm('Hlt').Enable = False
gaudi.algorithm('KillTrigRawEvent').Enable = False
triggerTisTosTool = gaudi.toolsvc().create('TriggerTisTos', interface='ITriggerTisTos')
tes = gaudi.evtsvc()

hlt1names = [ 
        "Hlt1TrackAllL0Decision"
        ]

hlt2names = [ 
        "Hlt2IncPhiDecision"
        ]

offlineLoc ="/Event/AllStreams/Phys/BetaSBs2PhiPhiLine/Particles"
count_hlt1_Dec={}
count_hlt1_TOS={}
for trigger in hlt1names:
    count_hlt1_Dec.update( { trigger : 0 } )
    count_hlt1_TOS.update( { trigger : 0 } )
count_hlt2_Dec={}
count_hlt2_TOS={}
for trigger in hlt2names:
    count_hlt2_Dec.update( { trigger : 0 } )
    count_hlt2_TOS.update( { trigger : 0 } )

i=0
n=0
#while True:
while n<evtmax:
    n+=1
    gaudi.run(1) 
    #
    gaudi.executeEvent()
    gaudi.algorithm("KillTrigRawEvent").execute()
    gaudi.algorithm("Hlt").execute()
    #if i==0:
    #    tes.dump()

    if not tes['/Event/DAQ/RawEvent']:
        print "End of file"
        break
    
    decReps = tes['/Event/Hlt/DecReports']
    selReps = tes['/Event/Hlt/SelReports']
    l0dec_reports = tes['/Event/HltLikeL0/DecReports']
    
    cands = tes[offlineLoc]
    if not cands:
        continue
    for cand in cands:
        i+=1
        triggerTisTosTool.setOfflineInput(cands[0])
        #
        for trigger in hlt1names:
            triggerTisTosTool.setTriggerInput(trigger)
            classified = triggerTisTosTool.tisTosTobTrigger()
            if classified.decision() == True:
                count_hlt1_Dec[trigger]+=1
            if classified.tos() == True:
                count_hlt1_TOS[trigger]+=1
        #
        for trigger in hlt2names:
            triggerTisTosTool.setTriggerInput(trigger)
            classified = triggerTisTosTool.tisTosTobTrigger()
            if classified.decision() == True:
                count_hlt2_Dec[trigger]+=1
            if classified.tos() == True:
                count_hlt2_TOS[trigger]+=1
# print efficiencies
print "Hlt1 efficiencies"
print "Trigger"+"   "+"Eff. (Decision)"+"   "+"Eff. (TOS)"
for trigger in hlt1names:
    print trigger+"   "+str( round(float(count_hlt1_Dec[trigger]) / float(i),2) )+"   "+str( round(float(count_hlt1_TOS[trigger]) / float(i),2) )
print "Hlt2 efficiencies"
print "Trigger"+"   "+"Eff. (Decision)"+"   "+"Eff. (TOS)"
for trigger in hlt2names:
    print trigger+"   "+str( round(float(count_hlt2_Dec[trigger]) / float(i),2) )+"   "+str( round(float(count_hlt2_TOS[trigger]) / float(i),2) )
