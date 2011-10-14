"""

   Simple test application to read LHCb dst files
   For reading other root files: change the data access
   if '/Event/Rec/Header' is not present.

   Requires: 'SetupProject LHCb'

   Script inherited from T.Ruf
   M.Frank  CERN/LHCb

"""
import os, sys, time
from Configurables import ApplicationMgr, EventSelector, Gaudi__RootCnvSvc
from Gaudi.Configuration import *
from GaudiKernel import *

input_file = 'castor://castorlhcb.cern.ch//castor/cern.ch/grid/lhcb/LHCb/Collision11/SEMILEPTONIC.DST/00012569/0000/00012569_00000004_1.semileptonic.dst?svcClass=lhcbdisk'
input_file = 'root://castorlhcb.cern.ch//castor/cern.ch/grid//lhcb/data/2010/BHADRON.DST/00008399/0000/00008399_00001052_1.bhadron.dst?svcClass=lhcbdisk'

#------------------------------------------------------------------------------------------------
def storeExplorer(load=1,freq=0.0001,name='StoreExplorerAlg'):
  from Configurables import StoreExplorerAlg
  alg                = StoreExplorerAlg(name)
  alg.Load           = load
  alg.PrintFreq      = freq
  return alg

#------------------------------------------------------------------------------------------------

if len(sys.argv)>1:
  input_file = sys.argv[1]

appConf = ApplicationMgr(OutputLevel = INFO) 
appConf.HistogramPersistency  = "NONE";
appConf.ExtSvc.append('Gaudi::IODataManager/IODataManager')
appConf.ExtSvc.append('Gaudi::RootCnvSvc/RootCnvSvc')
appConf.TopAlg.append(storeExplorer(freq=1.0))

EventDataSvc().RootCLID         = 1
EventDataSvc().EnableFaultHandler = True
root = Gaudi__RootCnvSvc('RootCnvSvc')
root.CacheBranches = []
root.VetoBranches = ['*']

#root.OutputLevel  = 2
# Enable specialized branch caching:
root.CacheBranches = [
    '_Event.*',
    '_Event_R.*',
    '_Event_Rec.*',
    '_Event_Rec_R.*',
    '_Event_Rec_Header.*'
    '_Event_Rec_Header_R.*'
    ]
root.VetoBranches = [
#    '_Event_pRec_*',
#    '_Event_Semileptonic_*'
    ]

EventPersistencySvc().CnvServices.append(root)
EventSelector().Input = [ "DATA='PFN:"+input_file+"' SVC='Gaudi::RootEvtSelector'" ]
EventSelector().PrintFreq = 1000
MessageSvc().OutputLevel = 3

def update():
 statistic = {}
 test = open('/proc/self/io')
 io = test.readlines()
 test.close()
 for l in io:
   temp = l.split(':')
   statistic[temp[0]]=int(temp[1])
 statistic['time']=time.time()
 return statistic

def printDelta(s0,s1):
 for s in s1:
  if s=='time':  print '%15s : %10.2F sec'%(s,(s1[s]-s0[s])) 
  else:          print '%15s : %10.2F MB'%(s,(s1[s]-s0[s])/1.E6) 
  
import GaudiPython
appMgr = GaudiPython.AppMgr()
sel    = appMgr.evtsel()
evt    = appMgr.evtsvc()
print sel.Input

start = update()
# test 1
N=0
while 1>0:
 rc = appMgr.run(1)
 if N==10: start=update()
 #if not evt['/Event/DAQ/RawEvent']:
 #    print 'Failed to access /Event/DAQ/RawEvent'
 if not evt['/Event/Rec/Header']:
     print 'Failed to access /Event/Rec/Header'
     break
 N+=1
 if N > 100000: break
end = update()
print 'Read %d events'%N
printDelta(start,end) 
