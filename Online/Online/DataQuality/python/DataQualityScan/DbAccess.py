"""

Database access module for the DataQuality monitoring business....

"""
import os, time
from DataQualityScan import *

# ------------------------------------------------------------------------------
class DbAccess:
  """
        \author  M.Frank
        \version 1.0
  """

  # ----------------------------------------------------------------------------
  def __init__(self, config, logon):
    """ Initializing constructor

        \author  M.Frank
        \version 1.0
    """
    from OnlineKernel.DbCore import DbCore
    self.config = config
    self.db  = DbCore(logon).connection()

  # ----------------------------------------------------------------------------
  def commit(self):
    """
        \author  M.Frank
        \version 1.0
    """
    self.db.commit()
  # ----------------------------------------------------------------------------
  def execute(self,stmt,params,cursor=None,connection=None,close=False):
    """
        \author  M.Frank
        \version 1.0
    """
    c = cursor
    try:
      if not c: 
        if connection:
          c = connection.cursor()
        else:
          c = self.db.cursor()
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

# ------------------------------------------------------------------------------
class RunDB(DbAccess):
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
    DbAccess.__init__(self,config,keys['db'])

  # ----------------------------------------------------------------------------
  def runInfo(self, run_no):
    """ Retrieve the basic run information from the run database
    
        \author  M.Frank
        \version 1.0
    """
    cur = self.db.cursor()
    stmt = """SELECT r.partitionid AS pid,
                     r.partitionname AS pname,
                     r.runtype AS runtype,
                     r.activity AS activity,
                     TO_CHAR(r.starttime,'YYYY-MM-DD HH24:MI:SS') AS starttime
               FROM   rundbruns r
               WHERE  r.runid=%d """%(run_no,)
    cur.execute(stmt)
    info = cur.fetchone()
    cur.close()
    if info is not None:
      result = [i for i in info]
      result[4] = time.mktime(time.strptime(info[4],'%Y-%m-%d %H:%M:%S'))
      return result
    return None

  # ----------------------------------------------------------------------------
  def runFiles(self, run, stream):
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
    cursor = self.execute(stmt, param)
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

# ------------------------------------------------------------------------------
class MonDB(DbAccess):
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
    DbAccess.__init__(self,config,keys['db'])
    self.table  = keys['table']

  # ----------------------------------------------------------------------------
  def hasRun(self,run_no,state=None):
    """  Check if the run in question is already registered in the monitoring database
    
        \author  M.Frank
        \version 1.0
    """
    if state:
      stmt = "SELECT runid, state FROM %s WHERE runid=:runid AND state=:state"%(self.table,)
      param = {'runid': run_no, 'state': state}
    else:
      stmt = "SELECT runid, state FROM %s WHERE runid=:runid"%(self.table,)
      param = {'runid': run_no}
    cursor = self.execute(stmt,params=param)
    db_data = cursor.fetchone()
    cursor.close()
    return db_data

  # ----------------------------------------------------------------------------
  def runsByState(self,state):
    """  Retrieve all runs according to a given state
    
        \author  M.Frank
        \version 1.0
    """
    stmt = "SELECT runid FROM %s WHERE state=:state"%(self.table,)
    param = {'state': state}
    cursor = self.execute(stmt,params=param)
    db_data = cursor.fetchall()
    cursor.close()
    if db_data:
      return [i[0] for i in db_data]
    return []

  # ----------------------------------------------------------------------------
  def nextRun(self,state):
    """  Get next run in ascending order with the required state

        \author  M.Frank
        \version 1.0
    """
    stmt = "SELECT runid FROM %s WHERE state=:state ORDER BY runid ASC"%(self.table,)
    param = {'state': state}
    cursor = self.execute(stmt,params=param)
    db_data = cursor.fetchone()
    cursor.close()
    if db_data and len(db_data):
      return db_data[0]
    return None

  # ----------------------------------------------------------------------------
  def setRunState(self, stmt, param):
    """
    \author  M.Frank
    \version 1.0
    """
    cursor = self.db.cursor()
    cursor.execute(stmt, param)
    self.db.commit()
    cursor.close()
    return self

  # ----------------------------------------------------------------------------
  def setRunTodo(self, run):
    """
    \author  M.Frank
    \version 1.0
    """
    stmt  = "UPDATE %s SET state=:state, todo=:todo WHERE runid=:runid"%(self.table,)
    param = {'state': self.config.st_todo, 'todo': int(time.time()), 'runid': run}
    log(INFO,'Set run %d to state %s [Processing start]'%(run,self.config.st_todo,))
    return self.setRunState(stmt,param)

  # ----------------------------------------------------------------------------
  def setRunRunning(self, run):
    """
    \author  M.Frank
    \version 1.0
    """
    stmt  = "UPDATE %s SET state=:state, todo=:todo WHERE runid=:runid"%(self.table,)
    param = {'state': self.config.st_running, 'todo': int(time.time()), 'runid': run}
    log(INFO,'Set run %d to state %s [Processing start]'%(run,self.config.st_running,))
    return self.setRunState(stmt,param)

  # ----------------------------------------------------------------------------
  def setRunDone(self, run):
    """
    \author  M.Frank
    \version 1.0
    """
    stmt  = "UPDATE %s SET state=:state, done=:done WHERE runid=:runid"%(self.table,)
    param = {'state': self.config.st_done, 'done': int(time.time()), 'runid': run}
    log(INFO,'Set run %d to state %s [Processing DONE]'%(run,self.config.st_done,))
    return self.setRunState(stmt,param)
