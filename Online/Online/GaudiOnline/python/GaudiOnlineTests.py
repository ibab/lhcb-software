from OnlineEnv import *

def _run(app):     end_config()

PartitionID=0x7de1
pid = PartitionID
pnam = PartitionName

#------------------------------------------------------------------------------------------------
def runSender(target): _run(dataSenderApp(pid,pnam,target,'RESULT',False,True))
#------------------------------------------------------------------------------------------------
def runReceiver():     _run(dataReceiverApp(pid,pnam,'OUT',True))
#------------------------------------------------------------------------------------------------
def runEvtProd():      _run(mepConverterApp(pid,pnam,bursts=True,freq=0.001))
#------------------------------------------------------------------------------------------------
def runEvtHolder():    _run(mepHolderApp(pid,pnam))
#------------------------------------------------------------------------------------------------
def runMBMRead(percent=1,print_freq=0.0001):
  _run(defaultFilterApp(pid,pnam,percent=percent,print_freq=print_freq))
#------------------------------------------------------------------------------------------------
#def runMDF2MBM(buffers,input=["DATA='file:///daqarea/lhcb/data/2008/RAW/LHCb/BEAM/32484/032484_0000081651.raw' SVC='LHCb::MDFSelector'"]):
def runMDF2MBM(buffers,input=["DATA='file://032484_0000081651.raw' SVC='LHCb::MDFSelector'"]):
  _run(mdf2mbmApp(pid,pnam,buffers=buffers,input=input,partitionBuffers=True))
#------------------------------------------------------------------------------------------------
def runMDF2MBM2(buffers):
  import data;
  _run(mdf2mbmApp(pid,pnam,buffers=buffers,input=data.Input,partitionBuffers=True))
#------------------------------------------------------------------------------------------------
def runEvtServer(buffer, partitionBuffers):
  _run(evtServerApp(pid,pnam,buffer=buffer,partitionBuffers=partitionBuffers))
#------------------------------------------------------------------------------------------------
def runOutBuffer():
  _run(mbmInitApp(pid,pnam,flags='-s=8096 -e=64 -u=64 -i=OUT -c',partitionBuffers=True))
#------------------------------------------------------------------------------------------------
def runRecBuffer():
  _run(mbmInitApp(pid,pnam,flags='-s=8096 -e=64 -u=64 -i=Events -c -s=8096 -e=64 -u=64 -i=Output -c',partitionBuffers=True))
#------------------------------------------------------------------------------------------------
def runDiskWR(buffer, partitionBuffers, decode, output):
  _run(diskWRApp(pid,pnam,buffer,partitionBuffers,decode,output))
#------------------------------------------------------------------------------------------------
def runNetCons(source,load=1,print_freq=0.0005):
  algs = [storeExplorer(load=load,freq=print_freq)]
  _run(netConsumerApp(pid,pnam,source,algs))
#------------------------------------------------------------------------------------------------
def runMepBuffer():
  flags = '-s=7000 -e=100 -u=5 -b=12 -f -i=MEP -c -s=200 -e=500 -u=14 -f -i=EVENT -c -s=200 -e=100 -u=14 -f -i=RESULT -c'
  _run(mbmInitApp(pid,pnam,flags))
