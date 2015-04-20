import os, sys, time, traceback
import Scanner
from HLT2 import *

# ------------------------------------------------------------------------------
class OfflineScanner(Scanner.BasicScanner):
  """
        \author  M.Frank
        \version 1.0
  """

  # ----------------------------------------------------------------------------
  def __init__(self, config, **keys):
    """ Initializing constructor

        \author  M.Frank
        \version 1.0
    """
    Scanner.BasicScanner.__init__(self,config,**keys)
    self.linkDirectory = '.'
    if keys.has_key('link_directory'):
      self.linkDirectory = keys['link_directory']
    self.farmClient = None
    self.minEvents  = 1

  # ----------------------------------------------------------------------------
  def selectRunDbRuns(self, partition='LHCb'):
    """
        \author  M.Frank
        \version 1.0
    """
    stmt =     """SELECT r.runid AS runid 
                  FROM   rundbruns r
                  WHERE  r.partitionname=:partitionname AND (SYSDATE-r.starttime)<5 AND r.state>1"""
    param= { 'partitionname': partition }
    cursor = self.execute(stmt,params=param,connection=self.rundb)
    info = cursor.fetchall()
    cursor.close()
    if info is not None:
      info = [i[0] for i in info]
      info.sort()
      return info
    return None

  # ----------------------------------------------------------------------------
  def selectMonDbRuns(self, partition='LHCb'):
    """
        \author  M.Frank
        \version 1.0
    """
    stmt =     """SELECT r.runid AS runid 
                  FROM   mondbruns r
                  WHERE  r.partitionname=:partitionname AND (SYSDATE-r.starttime)<2 AND r.state>1"""
    param= { 'partitionname': partition }
    cursor = self.execute(stmt,connection=self.mondb)
    info = cursor.fetchall()
    cursor.close()
    if info is not None:
      info = [i[0] for i in info]
      info.sort()
      print info
      return info
    return None

  # ----------------------------------------------------------------------------
  def rundbFiles(self, run, stream):
    """
        \author  M.Frank
        \version 1.0
    """
    stmt = """SELECT p.value, f.name, f.stream, f.events
              FROM rundbfiles f, rundbfileparams p
              WHERE f.fileid=p.fileid and f.runid=:runid
              AND p.name=:name
              AND f.stream=:stream"""
    param = {'runid': run, 'name': 'directory', 'stream': stream }
    cursor = self.execute(stmt, param, connection=self.rundb)
    files = cursor.fetchall()
    cursor.close()
    the_files = []
    for f in files:
      if len(files)>2:
        file = [i for i in f]  
        try:
          if file[3] is None:
            file[3] = int(os.stat(f[0]+os.sep+f[1])/self.config.avg_event_size)
          the_files.append(file)
        except:
          pass
    return the_files

  # ----------------------------------------------------------------------------
  def fullRunDbInfo(self,run):
    """
        \author  M.Frank
        \version 1.0
    """
    items = ['runid','starttime','endtime','partitionname','runtype','activity','tck','state']
    stmt = """SELECT r.runid AS runid,
                     TO_CHAR(r.starttime,'YYYY-MM-DD HH24:MI:SS') AS starttime,
                     TO_CHAR(r.endtime,  'YYYY-MM-DD HH24:MI:SS') AS endtime,
                     r.partitionname AS pname,
                     r.runtype AS runtype,
                     r.activity AS activity,
                     TO_NUMBER(r.tck) AS tck,
                     r.state AS state
              FROM   rundbruns r
              WHERE  r.runid=%s AND r.state>1"""%(run,)

    cursor = self.rundb.cursor()
    if not cursor.execute(stmt):
      raise 'Could not execute statement %s' %(stmt,)
    row = cursor.fetchone()
    if not row:
      return None

    res = {}
    for i in xrange(len(items)):
      res[items[i]] = row[i]
    r = res
    stmt = """SELECT name, value
              FROM rundbrunparams 
              WHERE runid=%s"""%(run,)
    
    if not cursor.execute(stmt):
      raise 'Could not execute statement %s' %stmt
    params = cursor.fetchall()
    res = {}
    for i in params:
      res[i[0]] = i[1]
    params = res
    files = self.rundbFiles(run=run,stream='FULL')
    if not len(files):
      return None

    # Build the final object
    r['Param'] = params
    r['Files'] = {}
    if len(files):
      count = 0
      for f in files:
        count = count + f[3]
      r['Files']['Entries'] = files
      r['Files']['NumEvent'] = count
    else:
      r['Files']['NumEvent'] = 0
      r['Files']['Entries'] = []
    return r

  # ----------------------------------------------------------------------------
  def use_run(self, run_no, files):
    """ Example implementation.
    """
    runs_pending = self.farmClient.getData()
    if not runs_pending.has_key(run_no):
      evt_count = 0
      for f in files:
        evt_count = evt_count + f[3]
      if evt_count > self.minEvents:
        return True
    else:
      files_pending = runs_pending[run_no]
      if files_pending < 2:
        return True
    return False

  # ----------------------------------------------------------------------------
  def verifyFiles(self, run_no, files):
    used_files = []
    num_evt = 0
    if self.use_run(run_no, files):
      num_file = 0
      for f in files:
        file_name = f[0]+os.sep+f[1]
        if os.access(file_name,os.R_OK):
          num_file  = num_file + 1
          num_evt   = num_evt + f[3]
          used_files.append(f)
    return (num_evt,used_files)

  # ----------------------------------------------------------------------------
  def make_links(self,caller,run_object):
    run = run_object[0]
    files = self.rundbFiles(run=run,stream='FULL')
    if len(files)>0 and self.use_run(run_object, files):
      status = True
      num_evt = 0
      num_file = 0
      for f in files:
        try:
          num_file  = num_file + 1
          num_evt   = num_evt + f[3]
          file_name = f[0]+os.sep+f[1]
          link_name = self.linkDirectory+os.sep+'Run_'+f[1]
          if os.access(file_name,os.R_OK):
            if os.access(link_name,os.R_OK):
              if os.readlink(link_name) == link_name:
                continue
              else:
                os.unlink(link_name)
            os.symlink(file_name, link_name)
            continue
          status = False
        except Exception,X:
          print str(X)
          status = False
      if not status:
        log(WARNING,'++ Run %d CANNOT be prepared for execution. [Internal error]'%(run,))
        return False
      log(INFO,'++ Run %d is prepared for execution: %d files %d events'%(run,num_file,num_evt,))
      return True
    log(WARNING,'++ Run %d CANNOT be prepared for execution. [No files]'%(run,))
    return False

  # ----------------------------------------------------------------------------
  def runsFinished(self,run_list):
    files = os.listdir(self.linkDirectory)
    still_processing = set()
    for file in files:
      for run in run_list:
        run_no = run[0]
        if file.find(str(run[0]))>=0:
          still_processing.add(run)
    result = run_list.difference(still_processing)
    return result


def isActive(fsm):
  st = fsm.state()
  if st == FSM.ST_RUNNING:
    return True
  elif st == FSM.ST_READY:
    return True
  elif st == FSM.ST_STOPPED:
    return True
  return False

# ===============================================================================
def run():
  ### prepare_run()
  ##dump_disk_runs()
  import Config, Setup
  import Params as params
  from DimInterface import RunFileClient, FSM

  print 'PID:',os.getpid()
  ##time.sleep(60)

  config = Config.Config(top=params.home,have_archive=True)
  Setup.Setup(config=config,credentials=params.mondb,table='offl_mon').createTables().printSummary()

  scanner = OfflineScanner(config,rundb=params.rundb,mondb=params.mondb,table='offl_mon')
  scanner.prepareWork   = scanner.make_links
  scanner.linkDirectory = 'OFFLINE_MON'
  runFiles = RunFileClient('/HLT/HLT1/Runs','C')
  scanner.farmClient = runFiles
  scanner.minEvents  = 2000

  try:
    os.mkdir(scanner.linkDirectory)
  except:
    pass

  while not runFiles.lastData:
    time.sleep(1)

  auto = (len(sys.argv)>1 and sys.argv[1] == '-a')
  fsm = FSM(os.environ['UTGID'],auto=auto)

  loop = 0
  runs_rejected = {}
  active_states = [FSM.ST_RUNNING,FSM.ST_READY,FSM.ST_STOPPED]

  fsm.start()

  while(1):
    if fsm.inState(active_states):
      recent_runs = scanner.selectRunDbRuns()
      loop = loop + 1
      runs_processing = set()
      for run in recent_runs:
        in_mondb = scanner.mondbHasRun(run)
        if not in_mondb:
          files = scanner.rundbFiles(run, 'FULL')
          num_events,files = scanner.verifyFiles(run,files)
          if len(files):
            print 'Add todo Run: ', run, in_mondb, 'Files:', len(files), 'Events:', num_events
            scanner.addTodo(run,files[0][0])
          elif not runs_rejected.has_key(run):
            runs_rejected[run] = 1
            print 'REJECTED Run: ', run, in_mondb, 'Files:', len(files), 'Events:', num_events
        else:
          runs_processing.add(in_mondb)
        if not fsm.inState(active_states): break

      if fsm.inState(active_states):
        if loop == 1: scanner.dump()
        scanner.createWork()
        if loop == 1: scanner.dump()
        runs_finished = scanner.runsFinished(runs_processing)
        print '===    Checking WORK   =============================================================='
        done = set()
        for r in runs_finished:
          if r[1] != 'DONE': done.add(r[0])
        scanner.setRunsDone(done)
    for i in xrange(30):
      if not fsm.inState(active_states): break      
      time.sleep(1)

    if fsm.state() == FSM.ST_UNKNOWN:
      log(INFO,'My work is done. Task exit requested....')
      sys.exit(0)

if __name__=="__main__":
  run()
