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
    self.db     = DbCore(keys['credentials']).connection()
    self.rundb  = DbCore(keys['rundb']).connection()

  # ----------------------------------------------------------------------------
  def cursor(self):
    """
        \author  M.Frank
        \version 1.0
    """
    return self.db.cursor()

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

  # ----------------------------------------------------------------------------
  def lastRunReady(self):
    """ Retrieve the last run scanned in conditions area.

        \author  M.Frank
        \version 1.0
    """
    cursor = self.db.cursor()
    cursor.execute("SELECT runid FROM hlt2_mon_last WHERE type='LAST_CREATED'")
    ret = cursor.fetchone()
    cursor.close()
    if ret is None: 
      return None
    return ret[0]

  def rundbInfo(self, run_no):
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
  def addTodo(self,run_no, params):
    """ Insert a new record into the HLT2 monitroing database for work....

        \author  M.Frank
        \version 1.0
    """
    find_cursor = self.db.cursor()
    find_cursor.execute("SELECT runid FROM hlt2_mon WHERE runid=?",(run_no,))
    db_data = find_cursor.fetchone()
    find_cursor.close()
    if db_data is None:
      info = self.rundbInfo(run_no)
      if info is not None:
        part_id,part_name,runtype,activity,starttime = info
        data = (run_no, self.config.st_initial, int(time.time()), 0, 0, 0, part_id, part_name, activity, runtype, params, starttime, )
        path = self.config.daqarea_directory+os.sep+data[FIELD_PARTNAME]+\
            os.sep+data[FIELD_RUNTYPE]+os.sep+str(data[FIELD_RUNID])
        try:
          os.stat(path)
          insert_cursor = self.db.cursor()
          print "INSERT INTO hlt2_mon VALUES ",str(data)
          insert_cursor.execute("INSERT INTO hlt2_mon VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)",data)
          insert_cursor.execute("UPDATE hlt2_mon_last SET runid=? WHERE type='LAST_CREATED'",(run_no,))
          insert_cursor.connection.commit()
          insert_cursor.close()
        except Exception,X:
          log(WARNING,'Skip directory %s [No data files:%s] '%(path,str(X)))
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
    find_cursor = self.db.cursor()
    find_cursor.execute("SELECT * FROM hlt2_mon WHERE state=?",(self.config.st_initial,))
    data = find_cursor.fetchone()
    runs = []
    while data:
      runs.append(data[0])
      self.makeXML(data)
      data = find_cursor.fetchone()
    find_cursor.close()
    update_cursor = self.db.cursor()
    for run in runs:
      update_cursor.execute("UPDATE hlt2_mon SET state=?, todo=? WHERE runid=?",(self.config.st_todo,int(time.time()),run,))
    update_cursor.connection.commit()
    update_cursor.close()
    return self

  # ----------------------------------------------------------------------------
  def populate(self):
    """ Populate the database from the content of the conditons directory

        \author  M.Frank
        \version 1.0
    """
    dir_name = self.config.conditions_directory
    runs = os.listdir(dir_name)
    runs.sort()
    last_run_added = self.lastRunReady()
    count = 0
    for run in runs:
      run_no = int(run)
      if run_no > last_run_added:
        sub_dir = dir_name + os.sep + str(run)
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
    runs = os.listdir(from_dir)
    if len(runs):
      update_cursor = self.db.cursor()
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
        update_cursor.execute("UPDATE hlt2_mon SET state=?, done=?, archived=? WHERE runid=?",(new_state,int(time.time()),archive,run,))
      update_cursor.connection.commit()
      update_cursor.close()
    return self

  # ----------------------------------------------------------------------------
  def dump(self):
    find_cursor = self.db.cursor()
    find_cursor.execute("SELECT * FROM hlt2_mon WHERE runid<>0")
    data = find_cursor.fetchone()
    while data is not None:
      path = self.config.daqarea_directory+os.sep+data[FIELD_PARTNAME]+\
          os.sep+data[FIELD_RUNTYPE]+os.sep+str(data[FIELD_RUNID])
      files = os.listdir(path)
      print 'DUMP:',str(data), len(files),' Files'      
      data = find_cursor.fetchone()
    find_cursor.close()
    return self

# ------------------------------------------------------------------------------
def scan():
  from Config import Config
  from Setup  import Setup
  import Params as params
  config = Config(top=params.home,have_archive=True)
  Setup(config.config(),credentials=params.credentials) \
      .createDirectories().createTables().printSummary()
  
  hlt2 = Scanner(config.config(),credentials=params.credentials,rundb=params.rundb)
  log(INFO, 'last Run Ready: %d'%(hlt2.lastRunReady(),))
  while 1:
    hlt2.populate()
    hlt2.createWork()
    hlt2.checkDone()
    hlt2.checkFailed()
    hlt2.dump()
    time.sleep(5)

if __name__ == "__main__":
  scan()

  
