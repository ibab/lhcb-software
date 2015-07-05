import Gaudi.Configuration as CFG
import Configurables as Configs
try:
  import signal
  SIGSEGV = signal.SIGSEGV
except:
  SIGSEGV = 11

# Data type: banks from TES=1, 
#            compressed records data from address = 2
#            banks data from address (MDF bank first) = 3
MDF_NONE    = 1
MDF_RECORDS = 2
MDF_BANKS   = 3

mbm_requirements={}
mbm_requirements['TAE']   = "EvType=1;TriggerMask=0xffffffff,0xffffffff,0xffffffff,0xffffffff;VetoMask=0,0,0,0;MaskType=ANY;UserType=ONE;Frequency=PERC;Perc=100.0"

mbm_requirements['MEP']   = "EvType=1;TriggerMask=0xffffffff,0xffffffff,0xffffffff,0xffffffff;VetoMask=0,0,0,0;MaskType=ANY;UserType=ALL;Frequency=PERC;Perc=100.0"

mbm_requirements['EVENT'] = "EvType=2;TriggerMask=0xffffffff,0xffffffff,0xffffffff,0xffffffff;VetoMask=0,0,0,0;MaskType=ANY;UserType=ONE;Frequency=PERC;Perc=100.0"

mbm_requirements['RESULT']= "EvType=2;TriggerMask=0xffffffff,0xffffffff,0xffffffff,0xffffffff;VetoMask=0,0,0,0;MaskType=ANY;UserType=ALL;Frequency=PERC;Perc=100.0"

mbm_requirements['SEND']  = "EvType=2;TriggerMask=0xffffffff,0xffffffff,0xffffffff,0xffffffff;VetoMask=0,0,0,0;MaskType=ANY;UserType=ALL;Frequency=PERC;Perc=100.0"

mbm_requirements['ERROR'] = "EvType=3;TriggerMask=0xffffffff,0xffffffff,0xffffffff,0xffffffff;VetoMask=0,0,0,0;MaskType=ANY;UserType=ALL;Frequency=PERC;Perc=100.0"

mbm_requirements['OTHER'] = "EvType=%s;TriggerMask=0xffffffff,0xffffffff,0xffffffff,0xffffffff;VetoMask=0,0,0,0;MaskType=ANY;UserType=%s;Frequency=PERC;Perc=100.0"

ApplicationMgr = CFG.ApplicationMgr

#------------------------------------------------------------------------------------------------
def _mbmRequirement(input,type,TAE=False,event_type=2):
  if   input == 'MEP' and TAE: return mbm_requirements['TAE']
  elif input == 'MEP':         return mbm_requirements['MEP']
  elif input == 'EVENT':       return mbm_requirements['EVENT']
  elif input == 'RESULT':      return mbm_requirements['RESULT']
  elif input == 'SEND':        return mbm_requirements['SEND']
  elif type is not None:       return mbm_requirements['OTHER']%(str(event_type),str(type),)
  return 'Unknown requirement'

#------------------------------------------------------------------------------------------------
def patchExitHandler():
  "Remove the GaudiPython exithandler. It is only harmful in the online envoriment"
  import atexit
  handlers = []
  for i in atexit._exithandlers:
    ##print '[ERROR] Exit:',str(i[0])
    if str(i[0]).find('AppMgr.exit') == -1:
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
def signalAlg(signal=SIGSEGV,probability=0.0001,name='SEGV'):
  alg                      = Configs.LHCb__SignalAlg(name)
  alg.Probability          = probability
  alg.SignalNumber         = signal
  return alg

#------------------------------------------------------------------------------------------------
def timeoutAlg(timeout=1000,trace=False,name='Timeout'):
  alg                      = Configs.LHCb__TimeoutAlg(name)
  alg.Timeout              = timeout
  alg.PrintTrace           = trace
  return alg

#------------------------------------------------------------------------------------------------
def delayAlg(delay=1000,errmax=1,name='Delay'):
  alg                      = Configs.LHCb__DelaySleepAlg(name)
  alg.DelayTime            = delay
  alg.ErrorMax             = errmax
  return alg

#------------------------------------------------------------------------------------------------
def prescaler(percent=25,name='Prescaler'):
  alg                      = CFG.Prescaler(name)
  alg.PercentPass          = percent
  alg.OutputLevel          = 4
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
def evtSender(target,name='Sender',input_type=MDF_NONE):
  sender                   = Configs.LHCb__SocketDataSender(name)
  sender.DataSink          = target
  sender.Compress          = 0
  sender.InputDataType     = input_type
  sender.DataType          = MDF_BANKS
  return sender
  
#------------------------------------------------------------------------------------------------
def evtMerger(buffer='Events',name='Writer',location='/Event/DAQ/RawEvent',routing=0x1,datatype=MDF_NONE,silent=False):
  merger                  = Configs.LHCb__RawEvent2MBMMergerAlg(name)
  merger.Buffer           = buffer
  merger.Compress         = 0
  #merger.DataType        = MDF_RECORDS
  merger.InputDataType    = datatype
  merger.BankLocation     = location
  merger.RoutingBits      = routing
  merger.Silent           = silent
  return merger

#------------------------------------------------------------------------------------------------
def fidManip(name,action,datatype,location):
  manip                   = Configs.LHCb__FIDManipAlg(name)
  manip.Action            = action
  manip.DataType          = datatype
  manip.BankLocation      = location
  return manip
#------------------------------------------------------------------------------------------------
def dstAddressUpdate(name='FidAdder',location='/Event/DAQ/RawEvent',datatype=MDF_BANKS):
  alg = fidManip(name,6,datatype,location)
  alg.OutputLocation = '/Event/DAQ/DstEvent'
  return alg
#------------------------------------------------------------------------------------------------
def fidAddAlg(name='FidAdder',location='/Event/DAQ/RawEvent',datatype=MDF_NONE):
  return fidManip(name,1,datatype,location)
#------------------------------------------------------------------------------------------------
def fidAdd2Alg(name='FidAdder',location='/Event/DAQ/RawEvent',datatype=MDF_NONE):
  return fidManip(name,5,datatype,location)
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
  ser    = Configs.Gaudi__SerializeCnvSvc('Gaudi::SerializeCnvSvc')
  serial = CFG.OutputStream(name)
  ser.Banks = location
  svc.CnvServices.append('Gaudi::SerializeCnvSvc/SerializeCnvSvc')
  serial.Output = "DATAFILE='root.buffers' SVC='Gaudi::SerializeCnvSvc/SerializeCnvSvc' OPT='RECREATE'"
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
    svc.REQ = _mbmRequirement(request,request,False)
  svc.MEPManager        = mepMgr
  svc.Input             = buffer
  return svc  
  
#------------------------------------------------------------------------------------------------
def evtRunable(mepMgr=None,name='Runable'):
  svc = Configs.LHCb__EventRunable(name)
  if mepMgr is not None:
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
  svc                    = CFG.EventDataSvc()
  svc.RootCLID           = 1
  svc.EnableFaultHandler = True
  return svc

#------------------------------------------------------------------------------------------------
def monSvc(name='MonitorSvc'):
  return Configs.MonitorSvc(name)
  
#------------------------------------------------------------------------------------------------
def mbmSelector(input=None,type=None,decode=True,TAE=False,event_type=2):
  svc = Configs.LHCb__OnlineEvtSelector('EventSelector')
  if decode is not None:
    svc.Decode = decode
  else:
    import OnlineEnv as _o
    svc.IntelligentSetup = True
    svc.TAE = _o.TAE

  if input is not None:
    svc.Input  = input
    svc.REQ1   = _mbmRequirement(input=input,type=type,TAE=TAE,event_type=event_type)
  return svc

#------------------------------------------------------------------------------------------------
def netSelector(input=None,type=None,event_type=2):
  svc = Configs.LHCb__NetworkEvtSelector('EventSelector')
  if input is not None: svc.Input = input
  if type is not None:  svc.REQ1  = _mbmRequirement(input='Other',type=type,event_type=event_type)
  return svc

#------------------------------------------------------------------------------------------------
def dummy_exit_handler(*args,**kwd):
  ###import GaudiPython.Bindings
  #print '[INFO] *** GaudiPython.Bindings.AppMgr: Executing dummy exithandler.....'
  return 1 ###GaudiPython.Bindings.SUCCESS

#------------------------------------------------------------------------------------------------
def end_config_normal(print_config=True):
  import sys, traceback
  try:
    import GaudiPython.Bindings
    setattr(GaudiPython.Bindings.AppMgr,'exit',dummy_exit_handler)
  except:
    pass
  try:
    import GaudiPython
    gaudi = GaudiPython.AppMgr()
    patchExitHandler()
    if print_config: printConfiguration()
    return gaudi
  except Exception,X:
    print '[ERROR] Exception:',str(X)
    info = sys.exc_info()
    if info is not None and info[0] is not None:
      lns=traceback.format_exception(info[0],info[1],info[2])
      for line in lns:
        print '[ERROR] Exception:',str(line)
  return None

#------------------------------------------------------------------------------------------------
def end_config_checkpoint(print_config, checkpoint):
  try:
    import GaudiPython.Bindings
    setattr(GaudiPython.Bindings.AppMgr,'exit',dummy_exit_handler)
  except:
    pass
  import os, sys, GaudiPython
  forker = Configs.LHCb__CheckpointSvc("CheckpointSvc")
  forker.NumberOfInstances   = 0
  #forker.NumberOfInstances   = 5
  forker.UseCores            = False  # -> N(core-1) procs!
  forker.ChildSessions       = False
  forker.DumpFiles           = False
  forker.Checkpoint          = checkpoint
  forker.PrintLevel          = 3  # 1=MTCP_DEBUG 2=MTCP_INFO 3=MTCP_WARNING 4=MTCP_ERROR
  forker.OutputLevel         = 2  # 1=VERBOSE 2=DEBUG 3=INFO 4=WARNING 5=ERROR 6=FATAL
  forker.ExitAfterCheckpoint = 2
  forker.CheckpointLibs      = "/dev/shm/checkpoint";
  forker.CheckpointSaveFlags = 2;
  ApplicationMgr().ExtSvc.append(forker)
  if CFG.allConfigurables.has_key('MEPManager'):
    mep = CFG.allConfigurables['MEPManager']
    mep.ConnectWhen = 'start'
    print 'Modified MEPManager to connect at start'    
  printConfiguration()
  print 132*'='
  print "== Starting checkpoint generation."
  print 132*'='
  sys.stdout.flush()
  gaudi = GaudiPython.AppMgr()
  gaudi.service('MEPManager').ConnectWhen = 'start'
  patchExitHandler()
  return gaudi

#------------------------------------------------------------------------------------------------
def end_config(print_config=True):
  import os
  if os.environ.has_key('CHECKPOINT_FILE'):
    return end_config_checkpoint(print_config,os.environ['CHECKPOINT_FILE'])
  return end_config_normal(print_config)

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
    evtloop.Warnings = False
  app.EventLoop = evtloop
  if app.HistogramPersistency=="NONE":
    pers = Configs.HistogramPersistencySvc('HistogramPersistencySvc')
    pers.Warnings = False
  return app

#------------------------------------------------------------------------------------------------
#
#  Some default applications used for dataflow
#
#------------------------------------------------------------------------------------------------
def mbmInitApp(partID, partName, flags,partitionBuffers=False):
  "MBM manager applications to initialize buffer managers."
  mepMgr = mepManager(partID,partName,[],partitionBuffers=partitionBuffers,flags=flags)
  return _application('NONE','NONE',extsvc=[monSvc(),mepMgr],runable=onlineRunable(1))

#------------------------------------------------------------------------------------------------
def mepHolderApp(partID, partName,errBuffer=None,partitionBuffers=False,routing=0x400):
  "MEP Holder application for usage of multi event packet buffers."
  runable              = Configs.LHCb__MEPHolderSvc('Runable')
  if errBuffer is None:
    mepMgr               = mepManager(partID,partName,['MEP'])
  else:
    mepMgr               = mepManager(partID,partName,['MEP',errBuffer],partitionBuffers)
    runable.HandleErrors = True
    runable.ErrorBuffer  = errBuffer
    runable.RoutingBits  = routing
    runable.HandleErrors = 2
  extsvc               = [monSvc(), mepMgr]
  runable.Requirements = [mbm_requirements['MEP']]
  evtloop              = Configs.LHCb__OnlineRunable('EmptyEventLoop')
  evtloop.Wait         = 3
  return _application('NONE',evtsel='NONE',extsvc=extsvc,runable=runable,evtloop=evtloop)

#------------------------------------------------------------------------------------------------
def mepConverterApp(partID, partName, bursts=True, freq=0.,errors=None):
  "MEP to event converter application for usage of multi event packet buffers."
  print 'Running event producer with error flag:',errors
  buffs                = ['MEP','EVENT']
  if errors: buffs.append('SEND')
  mepMgr               = mepManager(partID,partName,buffs)
  runable              = Configs.LHCb__MEPConverterSvc('Runable')
  runable.BurstMode    = False # bursts
  runable.PrintFreq    = freq
  runable.Requirements = [mbm_requirements['MEP']]
  if errors:
    runable.HandleErrors = errors
    runable.ErrorBuffer  = 'SEND'
    runable.RoutingBits  = 0x400
    runable.Requirements.append(mbm_requirements['ERROR'])

  evtloop              = Configs.LHCb__OnlineRunable('EmptyEventLoop')
  evtloop.Wait         = 1
  msgSvc().OutputLevel = 1
  return _application('NONE',evtsel='NONE',extsvc=[monSvc(),mepMgr,evtloop,runable],runable=runable,evtloop=evtloop)

#------------------------------------------------------------------------------------------------
def mepFeederApp(partID, partName, buffer, directory='./data', partitionBuffers=False):
  "MEP file reader app feeding HLT tasks from disk."
  mep                  = mepManager(partID,partName,[buffer], partitionBuffers)
  mep.ConnectWhen      = 'initialize'
  runable              = Configs.LHCb__HltBufferedIOReader('Runable')
  runable.Buffer       = buffer
  runable.Directory    = directory
  evtloop              = Configs.LHCb__OnlineRunable('EmptyEventLoop')
  evtloop.Wait         = 1
  msgSvc().OutputLevel = 1
  return _application('NONE',evtsel='NONE',extsvc=[monSvc(),mep,evtloop,runable],runable=runable,evtloop=evtloop)

#------------------------------------------------------------------------------------------------
def dataSenderApp(partID, partName, target, buffer, partitionBuffers=True, decode=False,request=None,algs=[],input_type=MDF_NONE):
  mepMgr               = mepManager(partID,partName,[buffer], partitionBuffers)
  runable              = evtRunable(mepMgr)
  evtSel               = mbmSelector(buffer,type=request,decode=decode)
  evtdata              = evtDataSvc()
  evtPers              = rawPersistencySvc()
  algs                 = algs
  if target is not None:
    sender             = evtSender(target=target,input_type=input_type)
    algs.append(sender)
  return _application('NONE',extsvc=[monSvc(),mepMgr,evtSel],runable=runable,algs=algs)

#------------------------------------------------------------------------------------------------
def dataReceiverApp(partID, partName, buffer, partitionBuffers=True,algs=[]):
  mepMgr               = mepManager(partID,partName,[buffer],partitionBuffers)
  runable              = onlineRunable(3)
  receiver             = evtReceiver(buffer)
  extsvc               = [monSvc(),mepMgr,receiver]
  return _application('NONE',evtsel='NONE',extsvc=extsvc,runable=runable,algs=algs)

#------------------------------------------------------------------------------------------------
def evtServerApp(partID, partName, buffer, partitionBuffers,request='USER'):
  "Event server application to distribute events over the network to clients."
  mepMgr               = mepManager(partID,partName,[buffer],partitionBuffers)
  runable              = evtServerRunable(mepMgr,buffer=buffer,request=request)
  evtPers              = rawPersistencySvc()
  return _application('NONE','NONE',extsvc=[monSvc(),mepMgr],runable=runable)

#------------------------------------------------------------------------------------------------
def netConsumerApp(partID, partName, source, algs=[]):
  "Simple network consumer picking events from the event server application."
  mepMgr               = mepManager(partID,partName,[])
  runable              = evtRunable(mepMgr)
  evtSel               = netSelector(source,'ALL')
  evtdata              = evtDataSvc()
  evtPers              = rawPersistencySvc()
  return _application('NONE',extsvc=[monSvc(),mepMgr,evtSel],runable=runable,algs=algs)

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
  return _application('NONE',extsvc=[monSvc(),mepMgr,runable,evtSel],runable=runable,algs=algs)

#------------------------------------------------------------------------------------------------
def defaultFilterApp(partID, partName, percent, print_freq):
  mepMgr               = mepManager(partID,partName,['Events','Send'])
  mepMgr.HandleSignals = True
  runable              = evtRunable(mepMgr)
  evtSel               = mbmSelector('Events')
  evtdata              = evtDataSvc()
  evtPers              = rawPersistencySvc()
  seq                  = CFG.Sequencer('SendSequence')
  seq.Members          = [prescaler(percent=percent),Configs.LHCb__DecisionSetterAlg('DecisionSetter')]
  ###seq.Members          = [prescaler(percent=percent)]
  algs                 = [storeExplorer(load=1,freq=print_freq),seq]
  #delay                = Configs.LHCb__DelaySleepAlg('Delay')
  #delay.DelayTime      = 999999;
  #algs.append(delay)
  return _application('NONE',extsvc=[monSvc(),mepMgr,evtSel],runable=runable,algs=algs)

#------------------------------------------------------------------------------------------------
def hltApp(partID, partName, percent, print_freq, delay=None, buffers=['IN','OUT'], decode=False, type=None, event_type=None, partitionBuffers=False):
  mepMgr               = mepManager(partID,partName,buffers,partitionBuffers)
  mepMgr.HandleSignals = True
  runable              = evtRunable(mepMgr)
  if event_type is None:
    evtSel             = mbmSelector(input=buffers[0],type=type,decode=decode)
  else:
    evtSel             = mbmSelector(input=buffers[0],type=type,decode=decode,event_type=event_type)
  evtdata              = evtDataSvc()
  evtPers              = rawPersistencySvc()
  algs                 = [storeExplorer(load=1,freq=print_freq)]
  if delay:
    delay_alg          = Configs.LHCb__DelaySleepAlg('Delay')
    delay_alg.DelayTime = 0
    delay_alg.MicroDelayTime = int(delay)
    algs.append(delay_alg)
  seq                  = CFG.Sequencer('SendSequence')
  seq.Members          = [prescaler(percent=percent),evtMerger(buffer=buffers[1])]
  algs.append(seq)
  return _application('NONE',extsvc=[monSvc(),mepMgr,evtSel],runable=runable,algs=algs)

#------------------------------------------------------------------------------------------------
def deferApp(partID, partName, buffers=None, type=None, event_type=None):
  mepMgr               = mepManager(partID,partName,buffers)
  mepMgr.HandleSignals = True
  runable              = evtRunable(mepMgr)
  if event_type is None:
    evtSel             = mbmSelector(input=buffers[0],type=type,decode=False)
  else:
    evtSel             = mbmSelector(input=buffers[0],type=type,decode=False,event_type=event_type)
  evtdata              = evtDataSvc()
  evtPers              = rawPersistencySvc()
  seq                  = CFG.GaudiSequencer('SendSequence')
  a1                   = evtMerger(name="Hlt2Supply", buffer=buffers[1],datatype=MDF_BANKS,silent=True)
  a1.DataType          = MDF_BANKS
  a2                   = evtMerger(name="DeferSupply",buffer=buffers[2],datatype=MDF_BANKS)
  a2.DataType          = MDF_BANKS
  seq.Members          = [a1,a2]
  seq.ModeOR           = True
  return _application('NONE',extsvc=[monSvc(),mepMgr,evtSel],runable=runable,algs=[seq])

#------------------------------------------------------------------------------------------------
def simpleFilterApp(partID, partName, percent, print_freq,buffers=['EVENT','OUT']):
  mepMgr               = mepManager(partID,partName,buffers,True)
  mepMgr.HandleSignals = True
  runable              = evtRunable(mepMgr)
  evtSel               = mbmSelector(buffers[0])
  evtdata              = evtDataSvc()
  evtPers              = rawPersistencySvc()
  algs                 = [storeExplorer(load=1,freq=print_freq),prescaler(percent=percent)]
  return _application('NONE',extsvc=[monSvc(),mepMgr,evtSel],runable=runable,algs=algs)

#------------------------------------------------------------------------------------------------
def bufferCons(partID, partName, buffer, partitionBuffers, decode):
  "Simple disk writer dumping events from file to disk."
  mepMgr               = mepManager(partID,partName,[buffer],partitionBuffers=partitionBuffers)
  evtSel               = mbmSelector(input=buffer,decode=decode,type='ALL')
  evtPers              = rawPersistencySvc()
  algs                 = [storeExplorer(load=1,freq=0.001)]
  return _application('NONE',extsvc=[monSvc(),mepMgr,evtSel],runable=evtRunable(mepMgr),algs=algs)

#------------------------------------------------------------------------------------------------
def bufferReformatter(partID, partName, input, output, partitionBuffers, decode, routing=0x1):
  "Simple disk writer dumping events from file to disk."
  mepMgr               = mepManager(partID,partName,[input,output],partitionBuffers=partitionBuffers)
  evtSel               = mbmSelector(input=input,decode=decode,type='ALL')
  evtPers              = rawPersistencySvc()
  algs                 = [storeExplorer(load=1,freq=0.001)]
  algs                 = [evtMerger(buffer=output,name='In2Out',location='DAQ/RawEvent',routing=routing)]
  return _application('NONE',extsvc=[monSvc(),mepMgr,evtSel],runable=evtRunable(mepMgr),algs=algs)

#------------------------------------------------------------------------------------------------
def diskWRApp(partID, partName, buffer, partitionBuffers, decode, output):
  "Simple disk writer dumping events from file to disk."
  mepMgr               = mepManager(partID,partName,[buffer],partitionBuffers=partitionBuffers)
  evtSel               = mbmSelector(input=buffer,decode=decode,type='ALL')
  evtPers              = rawPersistencySvc()
  algs                 = [storeExplorer(load=1,freq=0.001),diskWriter(output=output,input=3,compress=0)]
  return _application('NONE',extsvc=[monSvc(),mepMgr,evtSel],runable=evtRunable(mepMgr),algs=algs)

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
  return _application('NONE',extsvc=[monSvc(),mepMgr,evtSel],runable=runable,algs=algs)

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
  add                  = fidAdd2Alg(location='/Event',datatype=MDF_BANKS)
  prt                  = fidPrintAlg(location='/Event',datatype=MDF_BANKS)
  # If all starts from the rawEvent:
  #add                  = fidAddAlg()
  #prt                  = fidPrintAlg()  
  algs                 = [add, prt, merger]
  return _application('NONE',extsvc=[monSvc(),mepMgr,evtSel],runable=runable,algs=algs)

#------------------------------------------------------------------------------------------------
def dimFileReaderApp(partID, partName, buffer, partitionBuffers=True, routing=0x1):
  mepMgr               = mepManager(partID,partName,[buffer],partitionBuffers=partitionBuffers)
  runable              = dimFileReaderRunable()
  evtSel               = CFG.EventSelector()
  evtSel.PrintFreq     = 1000
  evtdata              = evtDataSvc()  
  evtPers              = rawPersistencySvc()
  algs                 = [evtMerger(buffer=buffer,name='Writer',location='DAQ/RawEvent',routing=routing)]
  return _application('NONE',extsvc=[monSvc(),mepMgr,evtSel],runable=runable,algs=algs)

#------------------------------------------------------------------------------------------------
def eventMerger(partID,partName,input='EVENT',output='SEND',partitionBuffers=False,location='/Event/DAQ/RawEvent',routing=0x800):
  buffers = [input,output]
  mepMgr   = mepManager(partID,partName,buffers,partitionBuffers)
  evtdata  = evtDataSvc()
  evtPers  = rawPersistencySvc()
  merger   = evtMerger(buffer=output,name='Writer',location=location,routing=routing,datatype=MDF_NONE)
  return _application('NONE',extsvc=[monSvc(),mepMgr,mbmSelector(input)],runable=evtRunable(mepMgr),algs=[merger])
