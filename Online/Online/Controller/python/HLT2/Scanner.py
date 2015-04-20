"""

"""

import os, time
from HLT2 import *

FIELD_RUNID     = 0
FIELD_STATE     = 1
FIELD_TODO_TIME = 2
FIELD_DONE_TIME = 3
FIELD_CHECKED   = 4
FIELD_ARCHIVED  = 5
FIELD_PARTID    = 6
FIELD_PARTNAME  = 7
FIELD_ACTIVITY  = 8
FIELD_RUNTYPE   = 9
FIELD_PARAMS    = 10
FIELD_STARTTIME = 11

# ------------------------------------------------------------------------------
class Scanner:
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
    from OnlineKernel.DbCore import DbCore
    self.config = config
    self.mondb  = DbCore(keys['mondb']).connection()
    self.rundb  = DbCore(keys['rundb']).connection()
    self.table  = keys['table']

  # ----------------------------------------------------------------------------
  def cursor(self):
    """
        \author  M.Frank
        \version 1.0
    """
    return self.mondb.cursor()


  def prepareWork(self,run):
    return True

  def execute(self,stmt,params,cursor=None,connection=None,close=False):
    c = cursor
    try:
      if not c: 
        if connection:
          c = connection.cursor()
        else:
          c = self.cursor()
      c.execute(stmt,params)
      if close:
        c.close()
        return None
      return c
    except Exception,X:
      log(WARNING,'Failed to execute statement:%s'%(stmt,))
      log(WARNING,'   Parameters: %s'%(str(params),))
      log(WARNING,'   Exception: %s'%(str(X),))
      raise(X)

  # ----------------------------------------------------------------------------
  def lastRunReady(self):
    """ Retrieve the last run scanned in conditions area.

        \author  M.Frank
        \version 1.0
    """
    cursor = self.mondb.cursor()
    cursor.execute("SELECT runid FROM %s_LAST WHERE type='LAST_CREATED'"%(self.table,))
    ret = cursor.fetchone()
    cursor.close()
    if ret is None: 
      return None
    return ret[0]

  # ----------------------------------------------------------------------------
  def rundbInfo(self, run_no):
    """ Retrieve the basic run information from the run database
    
        \author  M.Frank
        \version 1.0
    """
    rcur = self.rundb.cursor()
    stmt = """SELECT r.partitionid AS pid,
                     r.partitionname AS pname,
                     r.runtype AS runtype,
                     r.activity AS activity,
                     TO_CHAR(r.starttime,'YYYY-MM-DD HH24:MI:SS') AS starttime
               FROM   rundbruns r
               WHERE  r.runid=%d """%(run_no,)
    rcur.execute(stmt)
    info = rcur.fetchone()
    rcur.close()
    if info is not None:
      result = [i for i in info]
      result[4] = time.mktime(time.strptime(info[4],'%Y-%m-%d %H:%M:%S'))
      return result
    return None

  # ----------------------------------------------------------------------------
  def mondbHasRun(self,run_no):
    """  Check if the run in question is already registered in the monitoring database
    
        \author  M.Frank
        \version 1.0
    """
    stmt = "SELECT runid, state FROM %s WHERE runid=:runid"%(self.table,)
    param = {'runid': run_no}
    cursor = self.execute(stmt,params=param)
    db_data = cursor.fetchone()
    ##print db_data, stmt, param
    cursor.close()
    return db_data;

  # ----------------------------------------------------------------------------
  def addTodo(self,run_no, params,check_directory=True):
    """ Insert a new record into the HLT2 monitroing database for work....

        \author  M.Frank
        \version 1.0
    """
    db_data = self.mondbHasRun(run_no)
    if db_data is None:
      info = self.rundbInfo(run_no)
      if info is not None:
        part_id,part_name,runtype,activity,starttime = info
        param = {'runid':         run_no, 
                 'state':         self.config.st_initial,
                 'todo':          int(time.time()), 
                 'done':          0, 
                 'checked':       0,
                 'archived':      0,
                 'partitionid':   part_id,
                 'partitionname': part_name, 
                 'activity':      activity, 
                 'runtype':       runtype, 
                 'params':        params,
                 'starttime':     starttime}
        # check if the data directory exists:
        if check_directory:
          path = self.config.daqarea_directory+os.sep+param['partitionname']+\
              os.sep+param['runtype']+os.sep+str(param['runid'])
          try:
            os.stat(path)
          except Exception,X:
            log(WARNING,'Skip directory %s [No data files:%s] '%(path,str(X)))
            return None

        stmt = ""
        # Now insert the record into the database
        try:
          stmt = """INSERT INTO %s 
                    VALUES (:runid, :state, :todo, :done, :checked, :archived, 
                            :partitionid, :partitionname, :activity, 
                            :runtype, :params, :starttime)"""%(self.table,)

          ##print "INSERT INTO ",self.table," VALUES ",str(param)
          cursor = self.execute(stmt,params=param)
          stmt = "UPDATE %s_LAST SET runid=:runid WHERE type='LAST_CREATED'"%(self.table,)
          param = {'runid': run_no}
          self.execute(stmt,params=param,cursor=cursor,close=True)
          self.mondb.commit()
        except Exception,X:
          return None
        return self
      log(WARNING,'Run %d does not exist in the run database [Inconsistent data]'%(run_no,))
    return None

  # ----------------------------------------------------------------------------
  def createWork(self):
    """ Insert a new record into the HLT2 monitroing database for work....

        \author  M.Frank
        \version 1.0
    """
    stmt  = "SELECT * FROM %s WHERE state=:state"%(self.table,)
    param = {'state': self.config.st_initial}
    cursor = self.execute(stmt, params=param)
    runs = cursor.fetchall()
    cursor.close()
    to_process = []
    for run in runs:
      if self.prepareWork(self,run):
         to_process.append(run)
    cursor = None
    for run in to_process:
      stmt = "UPDATE %s SET state=:state, todo=:todo WHERE runid=:runid"%(self.table,)
      params = {'state': self.config.st_todo, 'todo': int(time.time()), 'runid': run[0] }
      cursor = self.execute(stmt,params=params,cursor=cursor)
    if cursor:
      cursor.connection.commit()
      cursor.close()
    return self

  # ----------------------------------------------------------------------------
  def populateFromConditions(self):
    """ Populate the database from the content of the conditons directory

        \author  M.Frank
        \version 1.0
    """
    dir_name = self.config.conditions_directory
    runs = os.listdir(dir_name)
    runs = [int(i) for i in runs]
    runs.sort()
    last_run_added = self.lastRunReady()
    count = 0
    for run_no in runs:
      if run_no > last_run_added:
        sub_dir = dir_name + os.sep + str(run_no)
        files = os.listdir(sub_dir)
        for fname in files:
          if fname == "HLT2Params.py":
            if self.addTodo(run_no,sub_dir+os.sep+fname):
              count = count + 1
            break
    if count > 0:
      log(INFO,'Inserted %d runs to be worked on....'%(count,))

  # ----------------------------------------------------------------------------
  def checkDone(self):
    """ Check the 'Done' subdirectory for analysed runs and archive them by
        moving the run directory into the Archive area.

        \author  M.Frank
        \version 1.0
    """
    return self.updateRuns(self.config.done,self.config.archive.done,self.config.st_done,archive=False)

  # ----------------------------------------------------------------------------
  def checkFailed(self):
    """ Check the 'Failed' subdirectory for analysed runs and archive them by
        moving the run directory into the Archive area.

        \author  M.Frank
        \version 1.0
    """
    return self.updateRuns(self.config.failed,self.config.archive.failed,self.config.st_failed,archive=False)

  # ----------------------------------------------------------------------------
  def updateRuns(self,from_dir,to_dir,new_state,archive=False):
    """
    """
    runs = os.listdir(from_dir)
    if len(runs):
      update_cursor = self.mondb.cursor()
      for run in runs:
        if archive:
          from_dir_entry = from_dir + os.sep + run
          to_dir_entry   = to_dir   + os.sep + run
          log(INFO,'Move run %s from %s to %s'%(run,from_dir_entry,to_dir_entry,))
          try:
            os.rename(from_dir_entry,to_dir_entry)
          except OSError, e:
            if e.errno==39: # OSError: [Errno 39] Directory not empty
              os.system('rm -rf '+to_dir_entry)
            os.rename(from_dir_entry,to_dir_entry)
        update_cursor.execute("UPDATE %s SET state=?, done=?, archived=? WHERE runid=?"%(self.table,),(new_state,int(time.time()),archive,run,))
      update_cursor.connection.commit()
      update_cursor.close()
    return self

  # ----------------------------------------------------------------------------
  def setRunsDone(self, run_list):
    """
    """
    if len(run_list):
      update_cursor = self.mondb.cursor()
      time_done = int(time.time())
      time_arch = int(time.time())
      for runid in run_list:
        stmt  = "UPDATE %s SET state=:state, done=:done, archived=:archived WHERE runid=:runid"%(self.table,)
        param = {'state': self.config.st_done, 'done': time_done, 'archived': time_arch, 'runid': runid}
        update_cursor.execute(stmt, param)
        log(INFO,'Set run %d to state %s [Processing DONE]'%(runid,self.config.st_done,))
      self.mondb.commit()
      update_cursor.close()
    return self    

  # ----------------------------------------------------------------------------
  def dump(self):
    """
    """
    find_cursor = self.mondb.cursor()
    find_cursor.execute("SELECT * FROM %s WHERE runid<>0"%(self.table,))
    data = find_cursor.fetchone()
    while data is not None:
      path = self.config.daqarea_directory+os.sep+data[FIELD_PARTNAME]+\
          os.sep+data[FIELD_RUNTYPE]+os.sep+str(data[FIELD_RUNID])
      files = os.listdir(path)
      print 'DUMP:',str(data), len(files),' Files'      
      data = find_cursor.fetchone()
    find_cursor.close()
    return self


# ----------------------------------------------------------------------------
def makeXML(self, record):
  """
    \author  M.Frank
    \version 1.0
  """
  run_no  = int(record[FIELD_RUNID])
  run_str = str(record[FIELD_RUNID]);
  dname   = self.config.todo + os.sep + run_str
  fname   = dname + os.sep + run_str+'.run'
  log(INFO,"Writing XML run file:%s tm:%s"%(fname,str(record[FIELD_STARTTIME]),))
  tm = time.localtime(record[FIELD_STARTTIME])
  make_directory(dname)
  f = open(fname,'w')
  print >>f, '<run runid="%d" year="%d" month="%d">'%(run_no,tm.tm_year,tm.tm_mon,)
  print >>f, '  <param name="partitionname"  value="%s"/>'%(record[FIELD_PARTNAME],)
  print >>f, '  <param name="partitionid"    value="%d"/>'%(record[FIELD_PARTID],)
  print >>f, '  <param name="runtype"        value="%s"/>'%(record[FIELD_RUNTYPE],)
  print >>f, '  <param name="activity"       value="%s"/>'%(record[FIELD_ACTIVITY],)
  print >>f, '  <param name="runstarttime"   value="%d"/>'%(record[FIELD_STARTTIME],)
  print >>f, '  <param name="params"         value="%s"/>'%(record[FIELD_PARAMS],)
  print >>f, '</run>'
  f.close()
  return True

# ------------------------------------------------------------------------------
def scan():
  from Config import Config
  from Setup  import Setup
  import Params as params
  table = 'hlt2_mon'
  config = Config(top=params.home,have_archive=True)
  Setup(config.config(),credentials=params.credentials,table=table) \
      .createDirectories().createTables().printSummary()
  
  hlt2 = Scanner(config.config(),mondb=params.mondb,rundb=params.rundb,table=table)
  hlt.prepareWork = makeXML

  log(INFO, 'last Run Ready: %d'%(hlt2.lastRunReady(),))
  while 1:
    hlt2.populateFromConditions()
    hlt2.createWork()
    hlt2.checkDone()
    hlt2.checkFailed()
    hlt2.dump()
    time.sleep(5)


BasicScanner = Scanner

if __name__ == "__main__":
  scan()

  
