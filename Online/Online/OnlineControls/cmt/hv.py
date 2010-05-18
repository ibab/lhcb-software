import sys, time
import Online.PVSS as PVSS
import Online.Utils as Utils
import Online.PVSSSystems as Systems
DP = PVSS.DataPoint
s_debug = True #False

def i__attr(obj,nam,data):
  n = nam
  idx = 1
  itm = obj
  while(idx>0):
    idx = n.find('.')
    if idx < 0:
      try:
        setattr(itm,n,data)
      except Exception,X:
        print 'Exception:',nam,X
      return
    itm = getattr(itm,n[:idx])
    n = n[idx+1:]
  print 'Bad return !!!!!!!!!!!!!!!!!!!!!!!!!!'

class LHCbStatus:
  def __init__(self,manager,partition):
    Utils.load('LHCbStatusDict')
    self.manager = manager
    self.partition = partition
    self.rdr  = self.manager.devReader()
    self.typ  = self.manager.typeMgr().type('_FwFsmObject')
    self.srv  = Utils.gbl.LHCbStatus.Server()
    self.runInfo = {}

  def publish(self):
    self.srv.publish()
    return self
  
  def swapBuffers(self):
    self.srv.swapBuffers()
    return self
  
  def _connectItem(self,itm,pf=''):
    match='*'
    actor = PVSS.DpVectorActor(mgr)
    lookup = self.partition+itm+match+itm+match+itm[:-1]+pf+'.fsm.currentState'
    actor.lookup(DP.original(lookup),self.typ)
    if s_debug: print 'Size:',lookup,actor.container.size()
    return actor

  def connectItem(self,itm,add=False,pf=''):
    actor = self._connectItem(itm='_'+itm+'|',pf=pf)
    if add:
      self.rdr.add(actor.container)
    return actor

  def addRunInfo(self, nam, partition=None):
    if partition is None:
      partition = self.partition
    if ( self.runInfo.has_key(nam) ):
      print 'Key ',nam,' already present in runinfo!'
      return
    dpn = partition+'_RunInfo.'+nam
    if s_debug: print 'Connecting to:',dpn
    self.runInfo[nam] = DP(self.manager,DP.original(dpn))
    self.rdr.add(self.runInfo[nam])
      
  def addRunCond(self, nam, partition=None):
    if partition is None:
      partition = self.partition
    if ( self.runInfo.has_key(nam) ):
      print 'Key ',nam,' already present in runinfo!'
      return
    dpn = partition+'_RunInfoCond.'+nam
    if s_debug: print 'Connecting to:',dpn
    self.runInfo[nam] = DP(self.manager,DP.original(dpn))
    self.rdr.add(self.runInfo[nam])
      
  def getFsmDp(self, nam):
    dpn = nam+'.fsm.currentState'
    if s_debug: print 'Connecting to:',dpn
    return DP(self.manager,DP.original(dpn))
    
  def connect(self):
    self.hvStatus  = self.connectItem('HV', add=True)
    self.dets = {}
    for i in xrange(self.hvStatus.container.size()):
      n = self.hvStatus.container.at(i).name()
      n = n[n.find('|')+1:n.find('.fsm.currentState:')]
      n = n[:n.find('_HV')]
      ## hv:    ECS:LHCb_HV|PRS_HV|PRS_HV.fsm.currentState:_original.._value
      ## dcs:   ECS:LHCb_DCS|PRS_DCS|PRS_DCS.fsm.currentState:_original.._value
      ## daq:   ECS:LHCb_DAQ|PRS_DAQ|PRS_DAQ.fsm.currentState:_original.._value
      ## dai:   ECS:LHCb_DAI|PRS_DAI|PRS_DAI.fsm.currentState:_original.._value

      ## lock:  ECS:LHCb|PRS_FWM.fsm.currentState:_original.._value
      ## status:ECS:LHCb|PRS|PRS.fsm.currentState:_original.._value
      ## included:ECS:LHCb|PRS|PRS_FWM.fsm.currentState:_original.._value
      ## complete:ECS:LHCb|PRS|PRS_FWCNM.fsm.currentState:_original.._value
      ##
      ##  LHCb|OTA_FWM        --> Locked out
      ##  LHCb|OTA|OTA        --> State
      ##  LHCb|OTA|OTA_FWM    --> INCLUDED
      ##  LHCb|OTA|OTA_FWCNM  --> INCOMPLETE
      ##
      self.dets[n] = []
      self.dets[n].append(self.getFsmDp(self.partition+'|'+n+'_FWM'))
      self.dets[n].append(self.getFsmDp(self.partition+'|'+n+'|'+n))
      self.dets[n].append(self.getFsmDp(self.partition+'|'+n+'|'+n+'_FWM'))
      self.dets[n].append(self.getFsmDp(self.partition+'|'+n+'|'+n+'_FWCNM'))
      self.dets[n].append(self.getFsmDp(self.partition+'_DAI|'+n+'_DAI|'+n+'_DAI'))
      self.dets[n].append(self.getFsmDp(self.partition+'_DAQ|'+n+'_DAQ|'+n+'_DAQ'))
      self.dets[n].append(self.getFsmDp(self.partition+'_DCS|'+n+'_DCS|'+n+'_DCS'))
      for i in self.dets[n]: self.rdr.add(i)
      
    self.addRunCond('LHC.FillNumber',partition='LHCb')
    self.addRunCond('LHC.Mode',partition='LHCb')
    #self.addRunCond('LHC.State',partition='LHCb')
    self.addRunCond('LHC.Energy',partition='LHCb')
    self.addRunCond('Magnet.Set.Current',partition='LHCb')
    self.addRunCond('Magnet.Set.Polarity',partition='LHCb')
    self.addRunCond('Magnet.Measured.Current',partition='LHCb')
    self.addRunCond('Magnet.Measured.Polarity',partition='LHCb')
    self.addRunCond('VELO.MotionSystem.ResolPosRC',partition='LHCb')
    self.addRunCond('VELO.MotionSystem.ResolPosLA',partition='LHCb')
    self.addRunCond('VELO.MotionSystem.ResolPosY',partition='LHCb')

    self.addRunInfo('HLTFarm.nSubFarms')
    self.addRunInfo('HLTFarm.subFarms')
    self.addRunInfo('HLTFarm.nodeList')
    self.addRunInfo('HLTFarm.requestedSubFarms')
    self.addRunInfo('HLTFarm.hltRate')
    self.addRunInfo('HLTFarm.runHltRate')
    self.addRunInfo('HLTFarm.hltRateWidget')
    self.addRunInfo('HLTFarm.runHltRateWidget')
    self.addRunInfo('HLTFarm.hltNTriggers')
    #self.addRunInfo('SubDetectors.tell1List')
    #self.addRunInfo('SubDetectors.ethMTU')
    #self.addRunInfo('SubDetectors.recalculate')
    #self.addRunInfo('SubDetectors.tell1Info')
    #self.addRunInfo('SubDetectors.tell1Domains')
    self.addRunInfo('TFC.nTriggers')
    self.addRunInfo('TFC.triggerRate')
    self.addRunInfo('TFC.runTriggerRate')
    self.addRunInfo('TFC.liveTime')
    self.addRunInfo('TFC.runLiveTime')
    self.addRunInfo('TFC.OdinData')
    self.addRunInfo('TFC.OdinMEPRequest')
    self.addRunInfo('TFC.deadTime')
    self.addRunInfo('TFC.runDeadTime')
    self.addRunInfo('TFC.triggerRateWidget')
    self.addRunInfo('TFC.runTriggerRateWidget')
    self.addRunInfo('TFC.nTriggersU')
    self.addRunInfo('TFC.lostTriggers')
    self.addRunInfo('Trigger.TCK')
    self.addRunInfo('Trigger.TCKLabel')
    self.addRunInfo('Trigger.onlineVersion')
    self.addRunInfo('Trigger.gaudiVersion')
    self.addRunInfo('Trigger.mooreVersion')
    self.addRunInfo('Trigger.HLTType')
    self.addRunInfo('Trigger.swPath')
    self.addRunInfo('Trigger.dataflowVersion')
    self.addRunInfo('Trigger.L0Type')
    self.addRunInfo('general.TAE')
    self.addRunInfo('general.L0Gap')
    self.addRunInfo('general.activePartId')
    self.addRunInfo('general.dataType')
    self.addRunInfo('general.fillNumber')
    self.addRunInfo('general.partName')
    self.addRunInfo('general.runNMinutes')
    self.addRunInfo('general.runNTriggers')
    self.addRunInfo('general.runNumber')
    self.addRunInfo('general.runType')
    self.addRunInfo('general.runStartTime')
    self.addRunInfo('general.runStopTime')
    self.addRunInfo('general.runInitialStartTime')
    self.addRunInfo('status')
    return self

  def fill(self):
    if self.rdr.execute(1,1):
      cnt = self.hvStatus.container
      status = self.srv.info()
      status.partition = self.partition
      status.numDetectors = cnt.size()
      for i in xrange(cnt.size()):
        n = cnt.at(i).name()
        n = n[n.find('|')+1:n.find('.fsm.currentState:')]
        n = n[:n.find('_HV')]
        det = status.detector(i)
        det.name = n
        det.hvStatus   = cnt.at(i).data
        det.hwLock     = self.dets[n][0].data
        det.hwStatus   = self.dets[n][1].data
        det.hwIncluded = self.dets[n][2].data
        det.hwComplete = self.dets[n][3].data
        det.daiStatus  = self.dets[n][4].data
        det.daqStatus  = self.dets[n][5].data
        det.dcsStatus  = self.dets[n][6].data
        """
        print 'hv: %s\ndcs:%s\ndaq:%s\ndai:%s\nlock:%s\nstatus:%s\nincluded:%s\ncomplete:%s\n'%(cnt.at(i).name(),
                                                       self.dets[n][5].name(),
                                                       self.dets[n][6].name(),
                                                       self.dets[n][4].name(),
                                                       self.dets[n][0].name(),
                                                       self.dets[n][1].name(),
                                                       self.dets[n][2].name(),
                                                       self.dets[n][3].name()
                                                              )
                                                              """
      for i in self.runInfo.keys():
        attr = i__attr(status,i,self.runInfo[i].data)
    return self

  def dump(self):
    self.srv.info().dump()
    
#PVSS.setDebug(1)

def cycle(status):
  status.fill()
  status.dump()
  status.publish()
  
time.sleep(10)
partition = 'LHCb'
if len(sys.argv)>1: partition=sys.argv[1]
print 'Analysing partition',partition
mgr=Systems.controlsMgr('ECS')
status = LHCbStatus(mgr,partition)
status.connect()

cycle(status)
status.srv.start()
#time.sleep(3)
#cycle(status)

for i in xrange(5000):
  time.sleep(3)
  status.swapBuffers()
  cycle(status)
  
