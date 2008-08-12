import pprint, atexit, OnlineEnv

def configure():
    """ Configuration of the job """
    import GaudiPython
    gaudi = GaudiPython.AppMgr()
    gaudi.HistogramPersistency = "NONE"
    gaudi.DLLs             += [ 'GaudiAlg', 'Gaucho']
    gaudi.ExtSvc           += [ 'MonitorSvc', 'LHCb::OnlineEvtSelector/EventSelector','LHCb::MEPManager/MEPManager']
    gaudi.Runable           = "LHCb::EventRunable/Runable";
    gaudi.SvcOptMapping    += [ "LHCb::MEPManager/MEPManager" ]
    msgSvc                  = gaudi.service('MessageSvc')
    msgSvc.OutputLevel      = 1

    MEPManager              = gaudi.service('MEPManager')
    MEPManager.PartitionID  = OnlineEnv.PartitionID
    MEPManager.Buffers      = ['EVENT','RESULT']
    
    runable = gaudi.service('Runable')
    runable.MEPManager      = "LHCb::MEPManager/MEPManager"

    evtdata = gaudi.service('EventDataSvc')
    evtdata.RootCLID        = 1
    evtdata.EnableFaultHandler = True
    
    evtPers = gaudi.service('EventPersistencySvc')
    evtPers.CnvServices     = ['LHCb::RawDataCnvSvc/RawDataCnvSvc' ]

    evtSel = gaudi.service('EventSelector')
    evtSel.Input            = 'EVENT'
    evtSel.REQ1             = "EvType=2;TriggerMask=0xffffffff,0xffffffff,0xffffffff,0xffffffff;VetoMask=0,0,0,0;MaskType=ANY;UserType=ONE;Frequency=PERC;Perc=100.0"

    gaudi.TopAlg            = ['StoreExplorerAlg','Sequencer/SendSequence']
    StoreExplorerAlg        = gaudi.algorithm('StoreExplorerAlg')
    StoreExplorerAlg.Load   = 1
    StoreExplorerAlg.PrintFreq = 0.5
    SendSequence            = gaudi.algorithm('SendSequence')
    SendSequence.Members    = ["Prescaler/Prescaler", "LHCb::DecisionSetterAlg/DecisionSetter"]
    Prescaler               = gaudi.algorithm('Prescaler')
    Prescaler.PercentPass   = 25
    Prescaler.OutputLevel   = 4
    
configure()

atexit._exithandlers = [atexit._exithandlers[0],atexit._exithandlers[1]]
print atexit._exithandlers
