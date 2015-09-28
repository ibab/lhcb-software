import traceback
import Online.PVSS as PVSS
import Online.Utils as Utils
std       = PVSS.gbl.std
log       = Utils.log
DataPoint = PVSS.DataPoint

_debug = False

# ------------------------------------------------------------------------------
def _createConnection(username, password, database):
    import cx_Oracle
    db = cx_Oracle.Connection(username, password, database)
    return db

# ------------------------------------------------------------------------------
def _mgr(name):
  "Access PVSS controls manager by name"
  import Online.PVSSSystems as Systems
  return Systems.controlsMgr(name)

# ------------------------------------------------------------------------------
class RunDB:
    def __init__(self,db):
        self.db = db
        self.cursor = db.cursor()
        self.state = ''
        self.mgr       = _mgr('STORAGE')
        self.mgrLHC    = _mgr('LHC')
        self.mgrECS    = _mgr('ECS')
        self.devMgr        = self.mgr.deviceMgr()
        self.writer        = self.mgr.devWriter()
        self.reader        = self.mgr.devReader()
        self.readerECS     = self.mgrECS.devReader()
        self.readerLHC     = self.mgrLHC.devReader()
        self.fillHistory   = std.vector('std::string')()
        self.fillHistoryDp = self.dp0('LHCbPerformance.History')
        self.fillLuminosity= self.dp0('LHCbPerformance.CurrentLuminosities')

        self.fillCurrent   = self.dp0('LHCbPerformance.CurrentFill')
        self.fillInjection = self.dp0('LHCbPerformance.CurrentFillStart')
        self.fillRamped    = self.dp0('LHCbPerformance.CurrentFillRamped')
        self.fillSqueeze   = self.dp0('LHCbPerformance.CurrentFillSqueezed')
        self.fillStable    = self.dp0('LHCbPerformance.CurrentFillStable')
        self.fillEnded     = self.dp0('LHCbPerformance.CurrentFillEnded')

        self.lhcState      = self.dp1('LHCCOM/LHC.LHC.RunControl.BeamMode')
        self.b1Intensity   = self.dp1('LHCCOM/LHC.LHC.Beam.IntensityPerBunch.Beam1.wholeBeamIntensity');
        self.b2Intensity   = self.dp1('LHCCOM/LHC.LHC.Beam.IntensityPerBunch.Beam2.wholeBeamIntensity');
        self.energy        = self.dp1('LHCCOM/LHC.LHC.Beam.Energy')
        self.readerLHC.add(self.lhcState)
        self.readerLHC.add(self.b1Intensity)
        self.readerLHC.add(self.b2Intensity)
        self.readerLHC.add(self.energy)

        self.effi          = []
        self.effi.append(self.dp2('LHCbEfficiency.ResultsLumi.HV'))
        self.effi.append(self.dp2('LHCbEfficiency.ResultsLumi.VELO'))
        self.effi.append(self.dp2('LHCbEfficiency.ResultsLumi.DAQ'))
        self.effi.append(self.dp2('LHCbEfficiency.ResultsLumi.DAQLiveTime'))
        self.effi.append(self.dp2('LHCbEfficiency.LumiTotal'))
        self.fillNo        = self.dp2('LHCbEfficiency.Sources.FillNumber')
        self.readerECS.add(self.fillNo)
        for i in self.effi: self.readerECS.add(i)

        self.lumi = []
        self.lumi.append(self.dp1('LHCCOM/LHC.LHCb.Internal.Luminosity.LumiInst_calo'))
        self.lumi.append(self.dp1('LHCCOM/LHC.LHCb.Internal.Luminosity.LumiInst_BLS_Aside'))
        self.lumi.append(self.dp1('LHCCOM/LHC.LHCb.Internal.Luminosity.LumiInst_BLS_Cside'))
        self.lumi.append(self.dp1('LHCCOM/LHC.LHCb.Internal.Luminosity.LumiInst_BRAN_4L8'))
        self.lumi.append(self.dp1('LHCCOM/LHC.LHCb.Internal.Luminosity.LumiInst_BRAN_4R8'))
        self.lumi.append(self.dp1('LHCCOM/LHC.LHCb.Internal.Luminosity.LumiInst_Ideal'))
        self.lumi.append(self.dp1('LHCCOM/LHC.LHCb.Internal.Luminosity.LumiInst_GP'))
        self.lumi.append(self.dp1('LHCCOM/LHC.LHCb.Internal.Luminosity.LumiIntRec_annual_4000'))
        self.lumi.append(self.dp1('LHCCOM/LHC.LHCb.Internal.Luminosity.LumiIntDel_annual_4000'))
        self.lumi.append(self.dp1('LHCCOM/LHC.LHCb.Internal.Luminosity.LumiIntRec_GP'))
        self.lumi.append(self.dp1('LHCCOM/LHC.LHCb.Internal.Luminosity.LumiIntDel_GP'))
        self.lumi.append(self.dp1('LHCCOM/LHC.LHCb.Internal.TriggerRates.TrgRateLumi_GP'))
        for i in self.lumi: self.readerLHC.add(i)
        
    # ===========================================================================
    def dp0(self,name):
        "Access internal datapoint of the datapoint structure."
        return DataPoint(self.mgr,DataPoint.original(name))
    
    # ===========================================================================
    def dp1(self,name):
        "Access internal datapoint of the datapoint structure."
        return DataPoint(self.mgrLHC,DataPoint.original(name))
    
    # ===========================================================================
    def dp2(self,name):
        "Access internal datapoint of the datapoint structure."
        return DataPoint(self.mgrECS,DataPoint.original(name))
    
    # ===========================================================================
    def initialize(self):
        self.reader.add(self.fillHistoryDp)
        self.reader.add(self.fillInjection)
        self.reader.add(self.fillSqueeze)
        self.reader.add(self.fillRamped)
        self.reader.add(self.fillStable)
        self.reader.add(self.fillEnded)
        self.reader.add(self.fillLuminosity)
        self.reader.execute(0,1)
        self.readerECS.execute(1,1)
        self.readerLHC.execute(1,1)
        self.state = ''
        exe = False
        if not len(self.fillInjection.data): self.fillInjection.data='0'
        if not len(self.fillRamped.data):    self.fillRamped.data='0'
        if not len(self.fillSqueeze.data):   self.fillSqueeze.data='0'
        if not len(self.fillStable.data):    self.fillStable.data='0'
        if not len(self.fillEnded.data):     self.fillEnded.data='0'
        if len(self.fillInjection.data)==0 or int(self.fillInjection.data.split('/')[0]) != self.fillNo.data:
            self.fillInjection.data = str(self.fillNo.data)
            self.fillRamped.data    = str(self.fillNo.data)
            self.fillSqueeze.data   = str(self.fillNo.data)
            self.fillStable.data    = str(self.fillNo.data)
            self.fillEnded.data     = str(self.fillNo.data)
            self.writer.add(self.fillInjection)
            self.writer.add(self.fillRamped)
            self.writer.add(self.fillSqueeze)
            self.writer.add(self.fillStable)
            self.writer.add(self.fillEnded)
            exe = True
        elif len(self.fillRamped.data)==0 or int(self.fillRamped.data.split('/')[0]) != self.fillNo.data:
            self.fillRamped.data    = str(self.fillNo.data)
            self.fillSqueeze.data   = str(self.fillNo.data)
            self.fillStable.data    = str(self.fillNo.data)
            self.fillEnded.data     = str(self.fillNo.data)
            self.writer.add(self.fillSqueeze)
            self.writer.add(self.fillRamped)
            self.writer.add(self.fillStable)
            self.writer.add(self.fillEnded)
            exe = True
        elif len(self.fillSqueeze.data)==0 or int(self.fillSqueeze.data.split('/')[0]) != self.fillNo.data:
            self.fillSqueeze.data   = str(self.fillNo.data)
            self.fillStable.data    = str(self.fillNo.data)
            self.fillEnded.data     = str(self.fillNo.data)
            self.writer.add(self.fillSqueeze)
            self.writer.add(self.fillStable)
            self.writer.add(self.fillEnded)
            exe = True
        elif len(self.fillStable.data)==0 or int(self.fillStable.data.split('/')[0]) != self.fillNo.data:
            self.fillStable.data    = str(self.fillNo.data)
            self.fillEnded.data     = str(self.fillNo.data)
            self.writer.add(self.fillStable)
            self.writer.add(self.fillEnded)
            exe = True
        elif len(self.fillEnded.data)==0 or int(self.fillEnded.data.split('/')[0]) != self.fillNo.data:
            self.fillEnded.data    = str(self.fillNo.data)
            self.writer.add(self.fillEnded)
            exe = True
        if exe:
            self.writer.execute(0,1)
        
    # ===========================================================================
    def fillRecord(self):
        import time
        #s = '%d/%f/%f/%f/%f/%f/%f/%f/%f/%f/%f'%(...)
        ##stmt = "SELECT * FROM (SELECT fill_id, timestamp, lumi_total, lumi_hvon, lumi_veloin, lumi_running, lumi_logged FROM rundbfills ORDER BY fill_id desc) WHERE ROWNUM<10"

        l = self.lumi[10].data;
        s = str(self.fillNo.data)+'/'+time.strftime('%Y-%m-%d %H:%M:%S',time.localtime())+'/'+str(l)+'/'
        for i in self.effi: s = s + str((1.-(i.data/100.))*l)+'/'
        s = s[:-1]+'/'+\
            str(self.lhcState.data)+'/'+str(self.b1Intensity.data)+'/'+\
            str(self.b2Intensity.data)+'/'+str(self.energy.data)+'/'+\
            str(self.lumi[9].data)+'/'+str(l)
        if _debug:
          print 'Current fill:',s
        return s
    
    # ===========================================================================
    def updateCurrentLumi(self):
        try:
            s = str(self.fillNo.data)+'/'+str(self.lhcState.data)+'/'
            for i in self.lumi: s = s + '%9.5e'%(i.data,) + '/'
            s = s[:-1]
            if _debug: log('Lumi: '+s)
            if s != self.fillLuminosity.data:
                self.fillLuminosity.data = s
                self.writer.add(self.fillLuminosity)
                return True
        except Exception,X:
            log('Exception:'+str(X))
            traceback.print_stack()
        return False
    
    # ===========================================================================
    def updateCurrentFill(self):
      try:
        st = self.lhcState.data
        rec = self.fillRecord()
        self.fillCurrent.data = rec
        self.writer.add(self.fillCurrent)
        if (st.find("INJECT")>=0 or st == "MD" or st == "SETUP" or st == "NO BEAM" ) and self.state != st:
            self.state = st
            self.fillInjection.data = rec
            self.fillRamped.data    = str(self.fillNo.data)
            self.fillSqueeze.data   = str(self.fillNo.data)
            self.fillStable.data    = str(self.fillNo.data)
            self.fillEnded.data     = str(self.fillNo.data)
            self.writer.add(self.fillInjection)
            self.writer.add(self.fillRamped)
            self.writer.add(self.fillSqueeze)
            self.writer.add(self.fillStable)
            self.writer.add(self.fillEnded)
        elif (st == "PHYS_ADJUST" or st == "FLAT TOP" or st == "ADJUST") and self.state != st:
            self.state = st
            self.fillRamped.data = rec
            self.writer.add(self.fillRamped)
        elif (st == "SQUEEZE") and self.state != st:
            self.state = st
            self.fillSqueeze.data = rec
            self.writer.add(self.fillSqueeze)
        elif (st == "PHYSICS" or st == "STABLE BEAMS") and self.state != st:
            self.state = st
            self.fillStable.data = rec
            self.writer.add(self.fillStable)
        elif st == "DUMP" or st == "EOF" or st == "NO_BEAM" or st == "RAMP DOWN":
            if self.state != 'EOF':
                self.state = 'EOF'
                self.fillEnded.data = rec
                self.writer.add(self.fillEnded)
        if _debug: log('Current: '+str(rec))
        return True
      except Exception,X:
        log('Execption:'+str(X))
        traceback.print_stack()
      return False
  
    # ===========================================================================
    def updateFillSummary(self):
        stmt = "SELECT * FROM (SELECT fill_id, timestamp, lumi_total, lumi_hvon, lumi_veloin, lumi_running, lumi_logged FROM rundbfills ORDER BY fill_id desc) WHERE ROWNUM<10"
        c = self.cursor
        if not c.execute(stmt):
            raise 'Could not execute statement %s' %stmt
        
        tmp = std.vector('std::string')()
        row = c.fetchone()
        while row:
          res = ''
          for r in row:
            res = res + str(r) + '/'
          tmp.push_back(res[:-1])
          if _debug: log(' History:'+str(res[:-1]))
          #log(' History:'+str(res[:-1]))
          row = c.fetchone()

        if self.fillHistory.size() != tmp.size():
          #log('Saving history...')
          self.fillHistory = tmp
          self.fillHistoryDp.data = tmp
          self.writer.add(self.fillHistoryDp)
          return True
        else:
          for i in xrange(tmp.size()):
            if self.fillHistory[i] != tmp[i]:
              #log('Saving history...')
              self.fillHistory = tmp
              self.fillHistoryDp.data = tmp
              self.writer.add(self.fillHistoryDp)
              return True

    # ===========================================================================
    def loadData(self):
      try:
        if self.readerECS.execute(1,1) and self.readerLHC.execute(1,1):
          st = self.lhcState.data
          if    st.find("INJECTION")>=0      \
             or st == "MD"                   \
             or st == "NO BEAM"              \
             or st == "EOF"                  \
             or st == "DUMP"                 \
             or st == "SETUP"                \
             or st == "RECOVERY"             \
             or st == "CYCLING"              \
             or st == "PHYS_ADJUST"          \
             or st == "ADJUST"               \
             or st == "SQUEEZE"              \
             or st == "RAMP"                 \
             or st == "RAMP DOWN"            \
             or st == "PREPARE RAMP":
            #log('Reset lumi to 0 for state '+st)
            self.lumi[-2].data = 0.
            self.lumi[-1].data = 0.
            #pass
          return True
      except Exception,X:
        log('Failed to load data: Exception:'+str(X))
        traceback.print_stack()
      return False

# ===============================================================================
def run():
    import time

    db=_createConnection('rundb_admin','adm1n','lhcbonr_rundb')
    #db=_createConnection('rundb_admin','adm1n','rundb')
    rundb = RunDB(db)
    rundb.initialize()
    while(1):
        time.sleep(5)
        try:
            b0 = rundb.loadData()
            if b0:
              b1 = rundb.updateCurrentFill()
              b2 = rundb.updateCurrentLumi()
              b3 = rundb.updateFillSummary()
              if b1 or b2 or b3:
                if _debug:
                  log('Updating STORAGE datapoints...')
                  log('===============================================================================')
                rundb.writer.execute(0,1)
            else:
              log('Failed to load data...')
        except Exception,X:
            log('Exception:'+str(X))
            traceback.print_stack()


# ===============================================================================
if __name__=="__main__":
  run()
