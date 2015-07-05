import os, sys, time, traceback
from DbAccess import MonDB, RunDB
from DIM import IntegerService, RunFileClient, Controller, FSM
from DataQualityScan import *

# ------------------------------------------------------------------------------
class Setup:
  """
        Setup module for the data quality monitoring
        Member functions perform the creation 
        of the database and the working directories

        \author  M.Frank
        \version 1.0
  """
  # ----------------------------------------------------------------------------
  def __init__(self, config):
    """ Initializing constructor

        \author  M.Frank
        \version 1.0
    """
    from OnlineKernel.DbCore import DbCore
    self.config = config
    self.DbCore = DbCore

  # ----------------------------------------------------------------------------
  def createTables(self):
    """ Create all tables to monitor the data quality monitoring facility

        \author  M.Frank
        \version 1.0
    """
    table  = self.config.mondb_table.upper()
    db     = self.DbCore(self.config.mondb);
    cursor = db.connection().cursor()
    text = db.text_t(255)
    integer = db.int_t(12)
    stmt = ""
    try:
      stmt = """CREATE TABLE %s (runid INTEGER PRIMARY KEY, 
                                 state %s,
                                 todo INTEGER, 
                                 done INTEGER,
                                 checked INTEGER,
                                 archived INTEGER, 
                                 partitionid INTEGER,
                                 partitionname %s,
                                 activity %s,
                                 runtype %s,
                                 params %s,
                                 starttime INTEGER) """%(table,text,text,text,text,text,)
      cursor.execute(stmt)
      cursor.close()
      db.connection().commit()
    except Exception, X:
      log(INFO,'Failed to create table: :"%s" with statement:%s'%(table,stmt,))
      log(INFO,'Failure:  '+str(X))
    return self

  # ----------------------------------------------------------------------------
  def createDirectories(self):
    """ Create all working directories used by the data quality monitoring

        \author  M.Frank
        \version 1.0
    """
    make_directory(self.config.optsDirectory)
    make_directory(self.config.linkDirectory)
    return self

  # ----------------------------------------------------------------------------
  def printSummary(self):
    """ Print summary information about the facility environment

        \author  M.Frank
        \version 1.0
    """
    import sys
    table = self.config.mondb_table
    db = self.DbCore(self.config.mondb).connection()
    cursor = db.cursor()
    try:
      cursor.execute("""SELECT COUNT(state) FROM %s"""%(table,))
    except Exception, X:
      log(INFO,'Failed to execute SQL:'+str(X))
    count = cursor.fetchone()
    cursor.close()
    log(INFO,'+%s+'%(110*'-',))
    log(INFO,'|  Working directory:   %-85s  |'%(os.getcwd(),))
    log(INFO,'|  Monitoring database: %-85s  |'%(self.config.mondb,))
    log(INFO,'|  dto. Number of entries: %-82s  |'%(str(count),))
    log(INFO,'+%s+'%(110*'-',))
    #sys.exit(0)

# ------------------------------------------------------------------------------
class DataQualityScanner:
  """
  Data file scanner to steer the data quality processing after HLT2.
  - This class scan the run database for recent runs of the LHCb partition
    --> self.scanDatabase(<partition>)

  \author  M.Frank
  \version 1.0
  """

  # ----------------------------------------------------------------------------
  def __init__(self, config, client):
    """ Initializing constructor of the file scanner

    \author  M.Frank
    \version 1.0
    """
    self.config = config
    self.mondb  = MonDB(config=self.config,
                        db=self.config.mondb,
                        table=self.config.mondb_table)
    self.rundb  = RunDB(config=self.config,
                        db=self.config.rundb)
    self.farmClient          = client
    self.runs_rejected       = {}
    self.run_modules         = {}

  # ----------------------------------------------------------------------------
  def runModule(self, run):
    """
    Create a python module containing the run parameters for the 
    data quality file processing

    \author  M.Frank
    \version 1.0
    """
    import sys, imp
    mod_name = 'Run_%d'%(run,)
    if not self.run_modules.has_key(mod_name):
      if not sys.modules.has_key(mod_name):
        source_path = self.config.condDirectory+'/%d/HLT2Params.py'%(run,)
        mod = imp.load_source(mod_name,source_path)
        self.run_modules[mod_name] = mod
    return self.run_modules[mod_name]
    

  # ----------------------------------------------------------------------------
  def selectRecentRuns(self, partition='LHCb', num_of_days=5):
    """
    Select all runs taken by partition <partition> during the last num_of_days days.

    \author  M.Frank
    \version 1.0
    """
    stmt = """SELECT r.runid AS runid 
              FROM   rundbruns r
              WHERE  r.partitionname=:partitionname 
                AND (SYSDATE-r.starttime)<:starttime AND r.state>1
              ORDER BY r.runid ASC"""
    param= { 'partitionname': partition, 'starttime': num_of_days }
    cursor = self.rundb.execute(stmt,params=param)
    info = cursor.fetchall()
    cursor.close()
    if info is not None:
      info = [i[0] for i in info]
      info.sort()
      return info
    return None

  # ----------------------------------------------------------------------------
  def addTodo(self, run_no, directory):
    """
    Insert a new record into the data quality monitoring database 
    for work....

        \author  M.Frank
        \version 1.0
    """
    info = self.rundb.runInfo(run_no)
    if info is not None:
      part_id, part_name, runtype, activity, starttime = info
      param = {'runid':              run_no, 
               'state':              self.config.st_todo,
               'todo':               int(time.time()), 
               'done':               0, 
               'checked':            0,
               'archived':           0,
               'partitionid':        part_id,
               'partitionname':      part_name, 
               'activity':           activity, 
               'runtype':            runtype, 
               'params':             directory,
               'starttime':          starttime}
      stmt = ""
      # Now insert the record into the database
      try:
        stmt = """INSERT INTO %s 
                  VALUES (:runid, :state, :todo, :done, :checked, :archived, 
                          :partitionid, :partitionname, :activity, 
                          :runtype, :params, :starttime)"""%(self.mondb.table,)
        ##print "INSERT INTO ",self.mondb.table," VALUES ",str(param)
        cursor = self.mondb.execute(stmt, params=param)
        self.mondb.commit()
      except Exception,X:
        log(WARNING,'Skip directory %s [No data files:%s] '%(path,str(X)))
        return None
      return self
    log(WARNING,'Run %d does not exist in the run database [Inconsistent data]'%(run_no,))
    return None

  # ----------------------------------------------------------------------------
  def dump(self):
    """
    \author  M.Frank
    \version 1.0
    """
    cursor = self.mondb.cursor()
    cursor.execute("SELECT * FROM %s WHERE runid<>0"%(self.mondb.table,))
    data = cursor.fetchone()
    while data is not None:
      path = self.config.daqarea_directory+os.sep+data[FIELD_PARTNAME]+\
          os.sep+data[FIELD_RUNTYPE]+os.sep+str(data[FIELD_RUNID])
      files = os.listdir(path)
      print 'DUMP:',str(data), len(files),' Files'      
      data = cursor.fetchone()
    cursor.close()
    return self

  # -----------------------------------------------------------------------------
  def scanDatabase(self, partition='LHCb', num_of_days=35):
    """
    Scan the run database and match all necessary information to populate
    the monitoring database

    \author  M.Frank
    \version 1.0
    """
    recent_runs  = self.selectRecentRuns(partition, num_of_days)
    runs_pending = self.farmClient.getData()

    for run in recent_runs:

      # If this run was rejected before, save us the work.
      if self.runs_rejected.has_key(run):
        continue

      srun = str(run)
      params = self.config.condDirectory+'/'+srun+'/HLT2Params.py'
      # First check if the params file is present:
      hlt2_params = os.access(params,os.R_OK)
      if not hlt2_params:
        #print 'Run ',run,' has no file ',params
        continue

      # If the run is already in the monitoring DB, ignore it:
      in_mondb = self.mondb.hasRun(run)
      if in_mondb:
        continue
      
      # Now check if a minimal number of files is present overall
      mod = self.runModule(run)
      num_files_total = float(mod.RunNFiles)
      if not num_files_total > 0:
        # This run will never be processed. There are simply no data
        self.runs_rejected[run] = 1
        #print 'Run ',run,' is rejected [No-files]'
        continue

      # Now check if a sufficient amount of files was processed:
      if not runs_pending.has_key(srun):
        num_files_waiting = 0.0
      elif runs_pending.has_key(srun):
        num_files_waiting = float(int(runs_pending[srun]))
      
      if num_files_waiting/num_files_total > self.config.min_files_processed:
        #print 'Run ',run,' is rejected [Not-enough-files]'
        continue

      # This is a new run to be processed:
      files = self.rundb.runFiles(run, self.config.file_stream_name)

      # Check if the number of output files is OK:
      num_events = 0
      num_files = 0
      usable_files = []
      for f in files:
        file_name = f[0]+os.sep+f[1]
        if os.access(file_name,os.R_OK):
          num_files  = num_files + 1
          num_events  = num_events + f[3]
          usable_files.append(f)

      # Check if the number of events is above threshold
      if not num_events > self.config.min_event_count:
        #print 'Run ',run,' is rejected [Not-enough events]'
        continue
      msg = 'Run:%d In MonDb:%s Files waiting:%4d total:%4d daq-area:%3d Events:%10d'%\
          (run,str(in_mondb),num_files_waiting,num_files_total,len(usable_files),num_events,)
      if len(usable_files):
        log(ALWAYS,'+++ Add todo '+msg)
        self.addTodo(run,directory=files[0][0])
      elif not self.runs_rejected.has_key(run):
        self.runs_rejected[run] = 1
        log(INFO,'+++ REJECTED '+msg)
    return self

  # -----------------------------------------------------------------------------
  def restart(self):
    """
    Check if there is old work left after a restart. If yes, restart these runs
    Update the monitoring database corrspondingly.

    \author  M.Frank
    \version 1.0
    """
    running_runs = self.mondb.runsByState(state=self.config.st_running)
    for run in running_runs:
      log(INFO,'+++ Reschedule run %d after restart.'%(run,))
      self.mondb.setRunTodo(run)
    return len(running_runs)>0

  # -----------------------------------------------------------------------------
  def _prepareRun(self, run, with_options=False):
    """
    Prepare a single run for execution.
    Update the monitoring database corrspondingly.

    \author  M.Frank
    \version 1.0
    """
    if run:
      files = self.rundb.runFiles(run=run,stream=self.config.file_stream_name)
      # Check if there are any files to be processed
      if not len(files):
        return None
      # Create option file
      if with_options:
        num_event_per_file = int(float(self.config.req_event_count)/float(len(files)) + 1.0)
        self.writeOpts(num_event_per_file,run)

      num_evt = 0
      num_file = 0
      for f in files:
        try:
          num_file  = num_file + 1
          num_evt   = num_evt + f[3]
          file_name = f[0]+os.sep+f[1]
          link_name = self.config.linkDirectory+os.sep+'Run_'+f[1]
          if os.access(file_name,os.R_OK):
            if os.access(link_name,os.R_OK):
              if os.readlink(link_name) == link_name:
                continue
              else:
                os.unlink(link_name)
            os.symlink(file_name, link_name)
            continue
          return None
        except Exception,X:
          log(ERROR,'++ Run %d cannot be prepared for execution: %s'%(run,str(X),))
          return None
      self.mondb.setRunRunning(run)
      log(INFO,'++ Run %d is prepared for execution: %d files %d events'%(run,num_file,num_evt,))
      return run
    log(WARNING,'++ CANNOT prepare work. [No work to do]')
    return None

  # -----------------------------------------------------------------------------
  def prepareRun(self, with_options=False):
    """
    Prepare the next run in the todo list for execution.
    Update the monitoring database corrspondingly.

    \author  M.Frank
    \version 1.0
    """
    run = self.mondb.nextRun(state=self.config.st_todo)
    if run:
      return [self._prepareRun(run, with_options)]
    return None

  # -----------------------------------------------------------------------------
  def prepareRuns(self):
    """
    Prepare the a set of compatible runs in the todo list for execution.
    Update the monitoring database corrspondingly.

    \author  M.Frank
    \version 1.0
    """
    prepared_runs = []
    runs = self.mondb.runsByState(state=self.config.st_todo)
    if runs and len(runs):
      module = None
      for run in runs:
        mod = self.runModule(run)
        if module:
          # Test for compatible runs:
          b1 = mod.MooreOnlineVersion != module.MooreOnlineVersion
          b2 = mod.HLTType != module.HLTType
          b3 = False # mod.InitialTCK != module.InitialTCK
          b4 = mod.CondDBTag != module.CondDBTag
          b5 = mod.DDDBTag != module.DDDBTag
          b6 = mod.ConditionsMapping != module.ConditionsMapping
          #print 'mod:', b1, b2, b3, b4, b5, b6,mod.CondDBTag,module.CondDBTag
          if b1 or b2 or b3 or b4 or b5 or b6:
            continue
        else:
          module = mod
        result = self._prepareRun(run)
        if result:
          prepared_runs.append(result)
      return prepared_runs
    return None

  # -----------------------------------------------------------------------------
  def checkRun(self,run,files=None):
    """
    Check if a run finished processing (no more data files).
    Update the monitoring database corrspondingly if the processing finished.

    \author  M.Frank
    \version 1.0
    """
    if run:
      if not files:
        files = os.listdir(self.config.linkDirectory)
      prefix = 'Run_%6d_'%(run,)
      found = False
      for f in files:
        if f.find(prefix) == 0:
          found = True
          break
      if not found:
        log(INFO,'++ Run %s finished.... setting state to DONE.'%(str(run),))
        self.mondb.setRunDone(run)
        return None
      log(DEBUG,'++ Run %s NOT finished.... still %d files to process.'%(str(run),len(files),))
      return run
    return None

  # -----------------------------------------------------------------------------
  def checkRuns(self,runs):
    """
    Check if a list of runs finished processing (no more data files).
    Update the monitoring database corrspondingly if the processing finished.

    \author  M.Frank
    \version 1.0
    """
    if runs:
      running_runs = []
      files = os.listdir(self.config.linkDirectory)
      for run in runs:
        result = self.checkRun(run=run, files=files)
        if result:
          running_runs.append(result)
      if len(running_runs):
        return running_runs
    return None

  def writeOpts(self, num_evts, run, overWrite=True):
    o = 'OnlineEnv.DataDirectory = "'+self.config.linkDirectory+'";\n'
    o = o + 'OnlineEnv.EventsPerFile = %d;\n'%(num_evts,)
    if run:
      o = o + 'OnlineEnv.AllowedRuns = ["%d"];\n'%(run,)
    else:
      o = o + 'OnlineEnv.AllowedRuns = [];\n'
    fname = self.config.optsDirectory+os.sep+'ReaderInput.opts'
    acc = os.access(fname,os.O_RDWR)
    if not acc or (acc and overWrite):
      opts = open(fname,'w')
      print >>opts, o
      opts.close()
    o = o.replace(';','').replace('OnlineEnv.','')
    fname = self.config.optsDirectory+os.sep+'ReaderInput.py'
    acc = os.access(fname,os.O_RDWR)
    if not acc or (acc and overWrite):
      opts = open(fname,'w')
      print >>opts, o
      opts.close()

# -------------------------------------------------------------------------------
class Steer(FSM):
  # -----------------------------------------------------------------------------
  def __init__(self, name, controller, scanner, auto=False):
    FSM.__init__(self, name, partitionid=0, auto=auto)
    self.__active   = False
    self.__working  = True
    self.scanner    = scanner
    self.controller = Controller(controller,'C')
    self.controller.register(FSM.ST_UNKNOWN, self.onCtrlOffline)
    self.controller.register(FSM.ST_OFFLINE, self.onCtrlOffline)
    self.controller.register(FSM.ST_PAUSED,  self.onCtrlPaused)
    self.controller.register(FSM.ST_RUNNING, self.onCtrlRunning)
    self.controller.register(FSM.ST_READY,   self.onCtrlReady)
    self.controller.register(FSM.ST_ERROR,   self.onCtrlError)
    self.work = IntegerService(scanner.config.announceService,0)
  # -----------------------------------------------------------------------------
  def printCtrl(self):
    log(INFO,'Controller in state: %s [Active:%s Working:%s]'%\
        (self.controller.state(),str(self.__active),str(self.__working),))
    return self
  # -----------------------------------------------------------------------------
  def setW(self,value):
    self.__working = value
    return self
  # -----------------------------------------------------------------------------
  def setA(self,value):
    self.__active = value
    return self
  # -----------------------------------------------------------------------------
  def isActive(self):
    return self.__active
  # -----------------------------------------------------------------------------
  def onRUNNING(self):          return self.setW(True).setA(True).printCtrl()
  # -----------------------------------------------------------------------------
  def onREADY(self):            return self.setA(False).printCtrl()
  # -----------------------------------------------------------------------------
  def onUNKNOWN(self):          return self.setA(False).printCtrl()
  # -----------------------------------------------------------------------------
  def onCtrlOffline(self):      return self.printCtrl()
  # -----------------------------------------------------------------------------
  def onCtrlReady(self):        return self.printCtrl()
  # -----------------------------------------------------------------------------
  def onCtrlRunning(self):      return self.printCtrl()
  # -----------------------------------------------------------------------------
  def onCtrlPaused(self):       return self.setW(False).setA(True).printCtrl()
  # -----------------------------------------------------------------------------
  def onCtrlError(self):        return self.setA(False).printCtrl()
  # -----------------------------------------------------------------------------
  def signalWork(self, runs):
    log(INFO,'Signal new work..... work:%s'%(str(runs),))
    if runs:
      self.work.update(1)
    else:
      self.work.update(0)

  # -----------------------------------------------------------------------------
  def run(self):
    loop = 0
    debug = True
    active_states = [FSM.ST_RUNNING,FSM.ST_READY,FSM.ST_STOPPED]
    scan = self.scanner

    log(ERROR,"Starting FSM...")
    self.start()

    scan.writeOpts(0,run=None,overWrite=False)

    current_runs = None
    ##if debug: scan.scanDatabase()

    # Check if there is old work left. If yes, restart this run
    if not scan.restart():
      self.__working = False
    
    while(1):
      #log(INFO,'*** Prepare  %s %s %s'%(str(self.__working), str(self.isActive()), str(current_runs),))
      #if self.isActive():
      if self.controller.state() == FSM.ST_PAUSED:
        if not self.__working:
          loop = loop + 1
          if current_runs is None:
            #current_runs = scan.prepareRuns()
            current_runs = scan.prepareRun(with_options=True)
            if current_runs is None:
              scan.scanDatabase()
              #current_runs = scan.prepareRuns()
              current_runs = scan.prepareRun(with_options=True)
            if current_runs:
              self.signalWork(current_runs)

          # Check run. If done 'current_runs' it changes state:
          run = current_runs
          current_runs = scan.checkRuns(current_runs)
          # On state change signal no work
          if run and not current_runs:
            self.signalWork(None)
          #log(INFO,'*** Checkrun:  %s %s %s'%(str(self.__working), str(self.isActive()), str(current_runs),))

      time.sleep(3)
      if self.state() == FSM.ST_UNKNOWN:
        log(INFO,'My work is done. Task exit requested....')
        return

# -------------------------------------------------------------------------------
def run(run_auto=False):
  import Config, errno

  auto = (len(sys.argv)>1 and sys.argv[1] == '-a') or run_auto
  log(INFO,'%s PID: %-6d  auto: %-5s %s'%(43*'*',os.getpid(), str(auto),43*'*',))
  config = Config.Config()

  try:
    os.makedirs(os.path.abspath(config.linkDirectory))
  except:
    pass
    #print 'Exception: ',str(X)
    #sys.exit(errno.EACCES)

  try:
    os.makedirs(os.path.abspath(config.optsDirectory))
  except Exception,X:
    pass
    #print 'Exception: ',str(X)
    #sys.exit(errno.EACCES)

  os.chdir(os.path.dirname(os.path.abspath(config.linkDirectory)))

  Setup(config).createTables().printSummary()
  run_client = RunFileClient('/HLT/HLT1/Runs','C')
  scanner    = DataQualityScanner(config,run_client)

  log(INFO,"Started scanner...")

  fsm = Steer(os.environ['UTGID'],
              controller=scanner.config.statusService,
              scanner=scanner,auto=auto)

  fsm.run()
  sys.exit(0)

if __name__=="__main__":
  run()
