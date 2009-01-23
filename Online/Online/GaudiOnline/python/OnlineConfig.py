import Gaudi.Configuration as CFG
import Configurables as Configs

# Data type: banks from TES=1, 
#            compressed records data from address = 2
#            banks data from address (MDF bank first) = 3
MDF_NONE    = 1
MDF_RECORDS = 2
MDF_BANKS   = 3

mbm_requirements={}
mbm_requirements['MEP']   = "EvType=1;TriggerMask=0xffffffff,0xffffffff,0xffffffff,0xffffffff;VetoMask=0,0,0,0;MaskType=ANY;UserType=ALL;Frequency=PERC;Perc=100.0"
mbm_requirements['EVENT'] = "EvType=2;TriggerMask=0xffffffff,0xffffffff,0xffffffff,0xffffffff;VetoMask=0,0,0,0;MaskType=ANY;UserType=ONE;Frequency=PERC;Perc=100.0"
mbm_requirements['RESULT']= "EvType=2;TriggerMask=0xffffffff,0xffffffff,0xffffffff,0xffffffff;VetoMask=0,0,0,0;MaskType=ANY;UserType=ALL;Frequency=PERC;Perc=100.0"
mbm_requirements['OTHER'] = "EvType=2;TriggerMask=0xffffffff,0xffffffff,0xffffffff,0xffffffff;VetoMask=0,0,0,0;MaskType=ANY;UserType=%s;Frequency=PERC;Perc=100.0"

ApplicationMgr = CFG.ApplicationMgr

#------------------------------------------------------------------------------------------------
def patchExitHandler():
  "Remove the GaudiPython exithandler. It is only harmful in the online envoriment"
  import atexit
  handlers = []
  for i in atexit._exithandlers:
    if str(i[0]).find('function _atexit_ at ') == -1:
      handlers.append(i)
  atexit._exithandlers = handlers
  #print atexit._exithandlers
  
#------------------------------------------------------------------------------------------------
def printConfiguration():
  "Print the configuration setup."
  import pprint
  pprint.pprint(CFG.configurationDict())

#------------------------------------------------------------------------------------------------
def storeExplorer(load=1,freq=0.0001,name='StoreExplorerAlg'):
  alg                = CFG.StoreExplorerAlg(name)
  alg.Load           = load
  alg.PrintFreq      = freq
  return alg

#------------------------------------------------------------------------------------------------
def prescaler(percent=25,name='Prescaler'):
  alg                = CFG.Prescaler(name)
  alg.PercentPass    = percent
  alg.OutputLevel    = 4
  return alg

#------------------------------------------------------------------------------------------------
def diskWriter(output,input=MDF_BANKS,compress=0,genMD5=True,datatype=MDF_RECORDS,name='Writer'):
  alg                      = Configs.LHCb__MDFWriter(name)
  alg.Connection           = output
  alg.InputDataType        = input
  alg.DataType             = datatype
  alg.Compress             = compress
  alg.GenerateMD5          = genMD5
  return alg

#------------------------------------------------------------------------------------------------
def evtSender(target,name='Sender'):
  sender                   = Configs.LHCb__SocketDataSender(name)
  sender.DataSink          = target
  sender.Compress          = 0
  sender.InputDataType     = MDF_NONE
  sender.DataType          = MDF_BANKS
  return sender
  
#------------------------------------------------------------------------------------------------
def evtMerger(buffer='Events',name='Writer',location='/Event/DAQ/RawEvent',routing=0x1,datatype=MDF_NONE):
  merger                   = Configs.LHCb__RawEvent2MBMMergerAlg(name)
  merger.Buffer            = buffer
  merger.Compress          = 0
  #merger.DataType          = MDF_RECORDS
  merger.InputDataType     = datatype
  merger.BankLocation      = location
  merger.RoutingBits       = routing
  return merger
  
#------------------------------------------------------------------------------------------------
def fidManip(name,action,datatype,location):
  manip                   = Configs.LHCb__FIDManipAlg(name)
  manip.Action            = action
  manip.DataType          = datatype
  manip.BankLocation      = location
  return manip
#------------------------------------------------------------------------------------------------
def fidAddAlg(name='FidAdder',location='/Event/DAQ/RawEvent',datatype=MDF_NONE):
  return fidManip(name,1,datatype,location)
#------------------------------------------------------------------------------------------------
def fidRemoveAlg(name='FidRemover',location='/Event/DAQ/RawEvent',datatype=MDF_NONE):
  return fidManip(name,2,datatype,location)
#------------------------------------------------------------------------------------------------
def fidPrintAlg(name='FidPrint',location='/Event/DAQ/RawEvent',datatype=MDF_NONE,freq=100):
  prt = fidManip(name,3,datatype,location)
  prt.PrintFreq = freq
  return prt
#------------------------------------------------------------------------------------------------
def serialWriter(name='DstWriter',location='/Event/GaudiSerialize'):
  svc    = CFG.EventPersistencySvc()
  ser    = Configs.SerializeCnvSvc('SerializeCnvSvc')
  serial = CFG.OutputStream(name)
  ser.Banks = location
  svc.CnvServices.append('SerializeCnvSvc/SerializeCnvSvc')
  serial.Output = "DATAFILE='root.buffers' SVC='SerializeCnvSvc/SerializeCnvSvc' OPT='RECREATE'"
  return serial

#------------------------------------------------------------------------------------------------
def msgSvc():
  svc                  = CFG.MessageSvc()
  return svc

#------------------------------------------------------------------------------------------------
def mepManager(partitionID,partitionName,buffers,partitionBuffers=False,flags=None,name='MEPManager'):
  svc = Configs.LHCb__MEPManager(name)
  svc.Buffers           = buffers
  svc.PartitionID       = partitionID
  svc.PartitionName     = partitionName
  svc.PartitionBuffers  = partitionBuffers
  if flags is not None: svc.InitFlags = flags
  return svc
  
#------------------------------------------------------------------------------------------------
def evtServerRunable(mepMgr,buffer,request=None,name='Runable'):
  svc = Configs.LHCb__EventServerRunable(name)
  if request is not None: 
    if request == 'MEP':      svc.REQ = mbm_requirements['MEP']
    elif request == 'EVENT':  svc.REQ = mbm_requirements['EVENT']
    elif request == 'RESULT': svc.REQ = mbm_requirements['RESULT']
    else:                     svc.REQ = mbm_requirements['OTHER']%(str(request),)
  svc.MEPManager        = mepMgr
  svc.Input             = buffer
  return svc  
  
#------------------------------------------------------------------------------------------------
def evtRunable(mepMgr,name='Runable'):
  svc = Configs.LHCb__EventRunable(name)
  svc.MEPManager        = mepMgr
  return svc
  
#------------------------------------------------------------------------------------------------
def dimFileReaderRunable(name='Runable'):
  svc = Configs.LHCb__DimRPCFileReader(name)
  return svc
  
#------------------------------------------------------------------------------------------------
def onlineRunable(wait,name='Runable'):
  svc = Configs.LHCb__OnlineRunable(name)
  svc.Wait = wait
  return svc

#------------------------------------------------------------------------------------------------
def evtReceiver(buffer,name='Receiver'):
  svc = Configs.LHCb__SocketDataReceiver(name)
  svc.Buffer = buffer
  return svc

#------------------------------------------------------------------------------------------------
def rawPersistencySvc():
  svc                  = CFG.EventPersistencySvc()
  svc.CnvServices.append('LHCb::RawDataCnvSvc/RawDataCnvSvc')
  return svc

#------------------------------------------------------------------------------------------------
def serialPersistencySvc():
  svc                  = rawPersistencySvc()
  svc.CnvServices.append('SerializeCnvSvc/SerializeCnvSvc')
  return svc

#------------------------------------------------------------------------------------------------
def evtDataSvc():
  svc                  = CFG.EventDataSvc()
  svc.RootCLID         = 1
  svc.EnableFaultHandler = True
  return svc

#------------------------------------------------------------------------------------------------
def mbmSelector(input=None,type=None,decode=True):
  svc = Configs.LHCb__OnlineEvtSelector('EventSelector')
  svc.Decode     = decode
  if input is not None:
    svc.Input    = input
    if input == 'MEP':          svc.REQ1 = mbm_requirements['MEP']
    elif input == 'EVENT':      svc.REQ1 = mbm_requirements['EVENT']
    elif input == 'RESULT':     svc.REQ1 = mbm_requirements['RESULT']
    elif type is not None:      svc.REQ1 = mbm_requirements['OTHER']%(str(type),)
  return svc

#------------------------------------------------------------------------------------------------
def netSelector(input=None,type=None):
  svc = Configs.LHCb__NetworkEvtSelector('EventSelector')
  if input is not None:    svc.Input    = input
  if type is not None:     svc.REQ1 = mbm_requirements['OTHER']%(str(type),)
  return svc

#------------------------------------------------------------------------------------------------
def end_config(print_config=True):
  import GaudiPython
  gaudi = GaudiPython.AppMgr()
  patchExitHandler()
  if print_config: printConfiguration()
  return gaudi

#------------------------------------------------------------------------------------------------
def _application(histpersistency, evtsel=None, extsvc=None, runable=None, algs=None, evtloop=None):
  app                      = ApplicationMgr()
  app.HistogramPersistency = histpersistency
  if extsvc is not None:      app.ExtSvc   = extsvc
  if evtsel is not None:      app.EvtSel   = evtsel
  if runable is not None:     app.Runable  = runable
  if algs is not None:        app.TopAlg  += algs
  if evtloop is None:
    evtloop = Configs.EventLoopMgr('EventLoopMgr')
    #evtloop.Warnings = False
  app.EventLoop = evtloop
  if app.HistogramPersistency=="NONE":
    pers = Configs.HistogramPersistencySvc('HistogramPersistencySvc')
    #pers.Warnings = False
  return app

#------------------------------------------------------------------------------------------------
#
#  Some default applications used for dataflow
#
#------------------------------------------------------------------------------------------------
def mbmInitApp(partID, partName, flags,partitionBuffers=False):
  "MBM manager applications to initialize buffer managers."
  mepMgr = mepManager(partID,partName,[],partitionBuffers=partitionBuffers,flags=flags)
  return _application('NONE','NONE',extsvc=[Configs.MonitorSvc(),mepMgr],runable=onlineRunable(1))

#------------------------------------------------------------------------------------------------
def mepHolderApp(partID, partName,errBuffer=None,partitionBuffers=False):
  "MEP Holder application for usage of multi event packet buffers."
  runable              = Configs.LHCb__MEPHolderSvc('Runable')
  if errBuffer is None:
    mepMgr               = mepManager(partID,partName,['MEP'])
  else:
    mepMgr               = mepManager(partID,partName,['MEP',errBuffer],partitionBuffers)
    runable.HandleErrors = True
    runable.ErrorBuffer  = errBuffer
  extsvc               = [Configs.MonitorSvc(), mepMgr]
  runable.Requirements = [mbm_requirements['MEP']]
  evtloop              = Configs.LHCb__OnlineRunable('EmptyEventLoop')
  evtloop.Wait         = 3
  return _application('NONE',evtsel='NONE',extsvc=extsvc,runable=runable,evtloop=evtloop)

#------------------------------------------------------------------------------------------------
def mepConverterApp(partID, partName, bursts=True, freq=0.):
  "MEP to event converter application for usage of multi event packet buffers."
  monSvc               = Configs.MonitorSvc()
  mepMgr               = mepManager(partID,partName,['MEP','EVENT'])
  runable              = Configs.LHCb__MEPConverterSvc('Runable')
  runable.BurstMode    = False # bursts
  runable.PrintFreq    = freq
  runable.Requirements = [mbm_requirements['MEP']]
  evtloop              = Configs.LHCb__OnlineRunable('EmptyEventLoop')
  evtloop.Wait         = 1
  msgSvc().OutputLevel = 1
  return _application('NONE',evtsel='NONE',extsvc=[monSvc,mepMgr,evtloop,runable],runable=runable,evtloop=evtloop)

#------------------------------------------------------------------------------------------------
def dataSenderApp(partID, partName, target, buffer, partitionBuffers=True, decode=False,algs=[]):
  mepMgr               = mepManager(partID,partName,[buffer], partitionBuffers)
  runable              = evtRunable(mepMgr)
  evtSel               = mbmSelector(buffer,decode=decode)
  evtdata              = evtDataSvc()
  evtPers              = rawPersistencySvc()
  sender               = evtSender(target)
  algs                 = algs
  algs.append(sender)
  return _application('NONE',extsvc=[Configs.MonitorSvc(),mepMgr,evtSel],runable=runable,algs=algs)

#------------------------------------------------------------------------------------------------
def dataReceiverApp(partID, partName, buffer, partitionBuffers=True,algs=[]):
  mepMgr               = mepManager(partID,partName,[buffer],partitionBuffers)
  runable              = onlineRunable(3)
  receiver             = evtReceiver(buffer)
  extsvc               = [Configs.MonitorSvc(),mepMgr,receiver]
  return _application('NONE',evtsel='NONE',extsvc=extsvc,runable=runable,algs=algs)

#------------------------------------------------------------------------------------------------
def evtServerApp(partID, partName, buffer, partitionBuffers):
  "Event server application to distribute events over the network to clients."
  mepMgr               = mepManager(partID,partName,[buffer],partitionBuffers)
  runable              = evtServerRunable(mepMgr,buffer=buffer,request='USER')
  evtPers              = rawPersistencySvc()
  return _application('NONE','NONE',extsvc=[Configs.MonitorSvc(),mepMgr],runable=runable)

#------------------------------------------------------------------------------------------------
def netConsumerApp(partID, partName, source, algs=[]):
  "Simple network consumer picking events from the event server application."
  mepMgr               = mepManager(partID,partName,[])
  runable              = evtRunable(mepMgr)
  evtSel               = netSelector(source,'ALL')
  evtdata              = evtDataSvc()
  evtPers              = rawPersistencySvc()
  return _application('NONE',extsvc=[Configs.MonitorSvc(),mepMgr,evtSel],runable=runable,algs=algs)

#------------------------------------------------------------------------------------------------
def panoramixSimApp(partID, partName, source, algs=[]):
  "Simple network consumer picking events from the event server application."
  mepMgr               = mepManager(partID,partName,[])
  runable              = Configs.LHCb__PanoramixSim('Runable')
  runable.MEPManager   = mepMgr
  evtSel               = netSelector(source,'ALL')
  evtSel.CancelOnDeath = True
  evtdata              = evtDataSvc()
  evtPers              = rawPersistencySvc()
  return _application('NONE',extsvc=[Configs.MonitorSvc(),mepMgr,runable,evtSel],runable=runable,algs=algs)

#------------------------------------------------------------------------------------------------
def defaultFilterApp(partID, partName, percent, print_freq):
  mepMgr               = mepManager(partID,partName,['EVENT','RESULT'])
  mepMgr.HandleSignals = True
  runable              = evtRunable(mepMgr)
  evtSel               = mbmSelector('EVENT')
  evtdata              = evtDataSvc()
  evtPers              = rawPersistencySvc()
  seq                  = CFG.Sequencer('SendSequence')
  seq.Members          = [prescaler(percent=percent),Configs.LHCb__DecisionSetterAlg('DecisionSetter')]
  algs                 = [storeExplorer(load=1,freq=print_freq),seq]
  #delay                = Configs.LHCb__DelaySleepAlg('Delay')
  #delay.DelayTime      = 999999;
  #algs.append(delay)
  return _application('NONE',extsvc=[Configs.MonitorSvc(),mepMgr,evtSel],runable=runable,algs=algs)

#------------------------------------------------------------------------------------------------
def bufferCons(partID, partName, buffer, partitionBuffers, decode):
  "Simple disk writer dumping events from file to disk."
  mepMgr               = mepManager(partID,partName,[buffer],partitionBuffers=partitionBuffers)
  evtSel               = mbmSelector(input=buffer,decode=decode,type='ALL')
  evtPers              = rawPersistencySvc()
  algs                 = [storeExplorer(load=1,freq=0.001)]
  return _application('NONE',extsvc=[Configs.MonitorSvc(),mepMgr,evtSel],runable=evtRunable(mepMgr),algs=algs)

#------------------------------------------------------------------------------------------------
def bufferReformatter(partID, partName, input, output, partitionBuffers, decode, routing=0x1):
  "Simple disk writer dumping events from file to disk."
  mepMgr               = mepManager(partID,partName,[input,output],partitionBuffers=partitionBuffers)
  evtSel               = mbmSelector(input=input,decode=decode,type='ALL')
  evtPers              = rawPersistencySvc()
  algs                 = [storeExplorer(load=1,freq=0.001)]
  algs                 = [evtMerger(buffer=output,name='In2Out',location='DAQ/RawEvent',routing=routing)]
  return _application('NONE',extsvc=[Configs.MonitorSvc(),mepMgr,evtSel],runable=evtRunable(mepMgr),algs=algs)

#------------------------------------------------------------------------------------------------
def diskWRApp(partID, partName, buffer, partitionBuffers, decode, output):
  "Simple disk writer dumping events from file to disk."
  mepMgr               = mepManager(partID,partName,[buffer],partitionBuffers=partitionBuffers)
  evtSel               = mbmSelector(input=buffer,decode=decode,type='ALL')
  evtPers              = rawPersistencySvc()
  algs                 = [storeExplorer(load=1,freq=0.001),diskWriter(output=output,input=3,compress=0)]
  return _application('NONE',extsvc=[Configs.MonitorSvc(),mepMgr,evtSel],runable=evtRunable(mepMgr),algs=algs)

#------------------------------------------------------------------------------------------------
def mdf2mbmApp(partID, partName, buffers, input, partitionBuffers=True, routing=0x1):
  mepMgr               = mepManager(partID,partName,buffers,partitionBuffers=partitionBuffers)
  runable              = evtRunable(mepMgr)
  evtSel               = CFG.EventSelector()
  evtSel.PrintFreq     = 100
  evtSel.Input         = input
  evtdata              = evtDataSvc()  
  evtPers              = rawPersistencySvc()
  algs                 = [evtMerger(buffer=buffers[0],name='MDF2MBM',location='DAQ/RawEvent',routing=routing)]
  return _application('NONE',extsvc=[Configs.MonitorSvc(),mepMgr,evtSel],runable=runable,algs=algs)

#------------------------------------------------------------------------------------------------
def mdf2mbmReproApp(partID, partName, buffers, input, partitionBuffers=True, routing=0x1):
  mepMgr               = mepManager(partID,partName,buffers,partitionBuffers=partitionBuffers)
  runable              = evtRunable(mepMgr)
  evtSel               = CFG.EventSelector()
  evtSel.PrintFreq     = 100
  evtSel.Input         = input
  evtdata              = evtDataSvc()  
  evtPers              = rawPersistencySvc()
  merger               = evtMerger(buffer=buffers[0],name='MDF2MBM',location='DAQ/RawEvent',routing=routing,datatype=MDF_BANKS)
  # If the MDF record should be directly manipulated: Need to tweak FID algs and merger
  merger.DataType      = MDF_BANKS
  add                  = fidAddAlg(location='/Event',datatype=MDF_BANKS)
  prt                  = fidPrintAlg(location='/Event',datatype=MDF_BANKS)
  # If all starts from the rawEvent:
  #add                  = fidAddAlg()
  #prt                  = fidPrintAlg()  
  algs                 = [add, prt, merger]
  return _application('NONE',extsvc=[Configs.MonitorSvc(),mepMgr,evtSel],runable=runable,algs=algs)

#------------------------------------------------------------------------------------------------
def dimFileReaderApp(partID, partName, buffer, partitionBuffers=True, routing=0x1):
  mepMgr               = mepManager(partID,partName,[buffer],partitionBuffers=partitionBuffers)
  runable              = dimFileReaderRunable()
  evtSel               = CFG.EventSelector()
  evtSel.PrintFreq     = 1000
  evtdata              = evtDataSvc()  
  evtPers              = rawPersistencySvc()
  algs                 = [evtMerger(buffer=buffer,name='Writer',location='DAQ/RawEvent',routing=routing)]
  return _application('NONE',extsvc=[Configs.MonitorSvc(),mepMgr,evtSel],runable=runable,algs=algs)
