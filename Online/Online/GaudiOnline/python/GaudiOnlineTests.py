from OnlineEnv import *

pid = PartitionID
pnam = PartitionName

def _run(app,prt=True):                      return (app,end_config(prt))
#------------------------------------------------------------------------------------------------
def runDimReader(buffer):                    return _run(dimFileReaderApp(pid,pnam,buffer,True))
#------------------------------------------------------------------------------------------------
# HLT sender with input from MEP RESULT buffer, with decoding of fragments.
def runSender(target):                       return _run(dataSenderApp(pid,pnam,target,'RESULT',False,True))
#------------------------------------------------------------------------------------------------
# Typical sender with input from partitioned buffers, no MEP decoding.
def runDataSender(target,buffer,req='ALL'):  return _run(dataSenderApp(pid,pnam,target,buffer,True,False,req))
#------------------------------------------------------------------------------------------------
# Data receiver task; puts data into buffer 'buffer'
def runReceiver(buffer='OUT'):               return _run(dataReceiverApp(pid,pnam,buffer,True))
#------------------------------------------------------------------------------------------------
def runEvtProd():                            return _run(mepConverterApp(pid,pnam,bursts=True,freq=0.001))
#------------------------------------------------------------------------------------------------
def runEvtHolder():                          return _run(mepHolderApp(pid,pnam,'OUT',True))
#------------------------------------------------------------------------------------------------
def runMBMRead(percent=1,print_freq=0.0001):
  return _run(defaultFilterApp(pid,pnam,percent=percent,print_freq=print_freq))
#------------------------------------------------------------------------------------------------
#def runMDF2MBM(buffers,input=["DATA='file:///daqarea/lhcb/data/2008/RAW/LHCb/BEAM/32484/032484_0000081651.raw' SVC='LHCb::MDFSelector'"]):
def runMDF2MBM(buffers,input=["DATA='file:///dev/shm/032484_0000081651.raw' SVC='LHCb::MDFSelector'"]):
  return _run(mdf2mbmApp(pid,pnam,buffers=buffers,input=input,partitionBuffers=True))
def runMDF2MBMFile(buffers,fname,partitionBuffers=True):
  return _run(mdf2mbmApp(pid,pnam,buffers=buffers,input=["DATA='"+fname+"' SVC='LHCb::MDFSelector'"],partitionBuffers=partitionBuffers))

def runMDF2MBMRepro(buffers,fname,partitionBuffers=True):
  return _run(mdf2mbmReproApp(pid,pnam,buffers=buffers,input=["DATA='"+fname+"' SVC='LHCb::MDFSelector'"],partitionBuffers=partitionBuffers))

#------------------------------------------------------------------------------------------------
def runMDF2MBM2(buffers):
  import data;
  return _run(mdf2mbmApp(pid,pnam,buffers=buffers,input=data.Input,partitionBuffers=True),False)
#------------------------------------------------------------------------------------------------
def runEvtServer(buffer, partitionBuffers):
  return _run(evtServerApp(pid,pnam,buffer=buffer,partitionBuffers=partitionBuffers))
#------------------------------------------------------------------------------------------------
def runBuffer(buffer='Events', partitionBuffers=True):
  return _run(mbmInitApp(pid,pnam,flags='-s=8096 -e=64 -u=64 -i='+buffer+' -c',partitionBuffers=partitionBuffers))
#------------------------------------------------------------------------------------------------
def runOutBuffer():
  return _run(mbmInitApp(pid,pnam,flags='-s=8096 -e=64 -u=64 -i=OUT -c',partitionBuffers=True))
#------------------------------------------------------------------------------------------------
def runRecBuffer():
  return _run(mbmInitApp(pid,pnam,flags='-s=8096 -e=64 -u=64 -i=Events -c -s=8096 -e=64 -u=64 -i=Output -c',partitionBuffers=True))
#------------------------------------------------------------------------------------------------
def runDiskWR(buffer, partitionBuffers, decode, output):
  return _run(diskWRApp(pid,pnam,buffer,partitionBuffers,decode,output))
#------------------------------------------------------------------------------------------------
def runBufferCons(buffer, partitionBuffers, decode):
  return _run(bufferCons(pid,pnam,buffer,partitionBuffers,decode))
#------------------------------------------------------------------------------------------------
def runReformatter(routing=0x1):
  return _run(bufferReformatter(pid,pnam,input='RESULT',output='OUT',partitionBuffers=True,decode=True,routing=routing))
#------------------------------------------------------------------------------------------------
def runNetCons(source,load=1,print_freq=0.0005):
  algs = [storeExplorer(load=load,freq=print_freq)]
  return _run(netConsumerApp(pid,pnam,source,algs))
#------------------------------------------------------------------------------------------------
def runPanoramixSim(source,load=1,print_freq=1.0):
  algs = [storeExplorer(load=load,freq=print_freq)]
  app = panoramixSimApp(pid,pnam,source,algs)
  app.OutputLevel = 2
  return _run(app,True)
#------------------------------------------------------------------------------------------------
def runMepBuffer():
  flags = '-s=7000 -e=100 -u=5 -b=12 -f -i=MEP -c -s=200 -e=500 -u=14 -f -i=EVENT -c -s=200 -e=100 -u=14 -f -i=RESULT -c'
  return _run(mbmInitApp(pid,pnam,flags))
