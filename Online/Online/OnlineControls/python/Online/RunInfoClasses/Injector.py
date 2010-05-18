import time
import Online.PVSS as PVSS
import Online.Utils as Utils
import Online.PVSSSystems as Systems
from Online.RunInfoClasses.General import General as General

log       = Utils.log
error     = Utils.error
std       = PVSS.gbl.std
DataPoint = PVSS.DataPoint

# =============================================================================
class Info(General):
  """ @class HLTFarm
  
      Connect to all storage related datapoints in the RunInfo object

      @author  M.Frank
      @version 1.0
  """
  # ===========================================================================
  def __init__(self, manager, name):
    """ Default constructor
        @param  manager       Reference to PVSS ControlManager
        @param  name          Detector/Partition name of the RunInfo datapoint

        @return reference to initialized object
    """
    General.__init__(self,manager,name)
    self.nodes           = self.dp('HLTFarm.nodeList')
    self.odinData        = self.dp('TFC.OdinData')
    self.odinRequest     = self.dp('TFC.OdinMEPRequest')
    self.inj_odinRequest = self.dp('Injector.OdinMEPRequest')
    self.inj_odinData    = self.dp('Injector.OdinData')
    self.inj_mepAddress  = self.dp('Injector.MEPRequestAddr')
    self.inj_dataAddress = self.dp('Injector.DataAddr')
    self.inj_tell1Boards = self.dp('Injector.FullTell1List')
    self.inj_nodeList    = self.dp('Injector.nodeList')
    self.inj_fileList    = self.dp('Injector.FileList')
    self.inj_fullPartId  = self.dp('Injector.FullPartId')
    self.inj_loopOnFiles = self.dp('Injector.LoopOnFiles')
    self.inj_readerNames = self.dp('Injector.ReaderNames')
    self.inj_fullFileList= self.dp('Injector.FullFileList')
    self.inj_readers = {}
    for i in xrange(7):
      n = 'Reader'+str(i+1)
      rdr = {}
      rdr['Name'] = self.dp('Injector.'+n+'.Name')
      rdr['FileList'] = self.dp('Injector.'+n+'.FileList')
      rdr['DataDirectory'] = self.dp('Injector.'+n+'.DataDirectory')
      self.reader.add(rdr['Name'])
      self.reader.add(rdr['FileList'])
      self.reader.add(rdr['DataDirectory'])
      self.inj_readers[n] = rdr

    self.reader.add(self.nodes)
    self.reader.add(self.odinData)
    self.reader.add(self.odinRequest)
    self.reader.add(self.inj_odinData)
    self.reader.add(self.inj_odinRequest)
    self.reader.add(self.inj_mepAddress)
    self.reader.add(self.inj_dataAddress)
    self.reader.add(self.inj_tell1Boards)
    self.reader.add(self.inj_nodeList)
    self.reader.add(self.inj_fileList)
    self.reader.add(self.inj_fullPartId)
    self.reader.add(self.inj_loopOnFiles)
    self.reader.add(self.inj_readerNames)
    self.reader.add(self.inj_fullFileList)

  # ===========================================================================
  def showSubfarms(self):
    """
    Show subfarm related information of the RunInfo datapoint.
    """
    farms = self.subFarms.data
    nSF = self.nSubFarm.data
    if len(farms) != nSF:
      error('Subfarm allocation inconsistency: nSubFarm(%d) != len(subFarms)(%d)'%(len(farms),nSF))
    log('Input from %d sub farms'%nSF)
    for i in nSF:
      log(' -> Subfarm:[SF%02d] %-24s '%(i,farms[i],))
    log(' -> TFC:    Data interface:%-12s MEP Request interface'%(self.odinData.data,self.odinRequest.data,))
  # ===========================================================================
  def show(self):
    """
    Show all information from the RunInfo structure concerning the HLT farm.
    """
    if self.load():
      self.showSubfarms()
      self.showGeneral()
      return
    error('Failed to load RunInfo for partition:'+self.name)    


# =============================================================================
class InfoCreator:
  def __init__(self):
    pass
  def create(self,rundp_name,partition):
    items = rundp_name.split(':')
    mgr = Systems.controlsMgr(items[0])
    return Info(mgr,partition)
