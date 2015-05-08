import os, sys, time, traceback

_debug = False

# ------------------------------------------------------------------------------
def _createConnection(username, password, database):
    import cx_Oracle
    db = cx_Oracle.Connection(username, password, database)
    return db

def _fetch(cursor):
    result = []
    row = cursor.fetchone()
    while row:
        res = [i for i in row]
        result.append(res)
        row = cursor.fetchone()
    return result

def make_directory(nam):
  try:
    os.mkdir(nam)
  except:
    pass

def makeXML(directory,tag,o):
  run_no = int(o[0])
  run_str = str(o[0]);
  make_directory(directory+os.sep+run_str)
  f = open(directory+os.sep+run_str+os.sep+run_str+'.run','w')
  print >>f, '<'+tag+' runid="'+str(o[0])+'" year="'+str(o[1])+'" month="'+str(o[2])+'">'
  print >>f, '  <param name="partitionname" value="'+str(o[5])+'"/>'
  print >>f, '  <param name="startdate" value="'+str(o[3])+'"/>'
  print >>f, '  <param name="enddate" value="'+str(o[4])+'"/>'
  print >>f, '  <param name="runtype" value="'+str(o[6])+'"/>'
  print >>f, '  <param name="activity" value="'+str(o[7])+'"/>'
  print >>f, '  <param name="tck" value="'+str(hex(0xFFFFFFFF&int(o[8])))+'"/>'
  for i in o[9]:
    print >>f, '  <param name="'+i[0]+'" value="'+str(i[1])+'"/>'
  print >>f, '</'+tag+'>'
  f.close()
 
# ===============================================================================
def run_init():
    make_directory('ToDo')
    make_directory('Done')
    make_directory('Failed')
    make_directory('Running')
    make_directory('Archive')
    make_directory('Archive/ToDo')
    make_directory('Archive/Done')
    make_directory('Archive/Fail')
    os.chdir('ToDo')

def dump_db_runs():
    run_init()
    db=_createConnection('rundb_admin','adm1n','lhcbonr_rundb')
    #db=_createConnection('rundb_admin','adm1n','rundb')
    cursor = db.cursor()
    stmt = """SELECT * FROM (
                 SELECT r.runid AS runid,
                        TO_CHAR(r.starttime,'YYYY') AS year,
                        TO_CHAR(r.starttime,'MM') AS month,
                        TO_CHAR(r.starttime,'YYYY-MM-DD HH24:MI:SS') AS starttime,
                        TO_CHAR(r.endtime,  'YYYY-MM-DD HH24:MI:SS') AS endtime,
                        r.partitionname AS pname,
                        r.runtype AS runtype,
                        r.activity AS activity,
                        TO_NUMBER(r.tck) AS tck,
                        p1.value AS program,
                        p2.value AS programVersion
                 FROM   rundbruns r, rundbrunparams p1, rundbrunparams p2
                        WHERE r.partitionname='LHCb' AND p1.name='program' AND p2.name='programVersion' AND
                              p1.runid=p2.runid AND r.runid=p1.runid AND p1.value<>'DEFAULT' AND
                              r.starttime>TO_DATE('2015/01/01','yyyy/mm/dd')
                        ORDER BY r.runid desc)
                 WHERE rownum<10"""
    if not cursor.execute(stmt):
        raise 'Could not execute statement %s' %stmt
    runs = _fetch(cursor)

    for r in runs:
      print r
      stmt = """SELECT name, value, type 
                FROM rundbrunparams 
                WHERE runid=%d"""%(r[0],)
      
      if not cursor.execute(stmt):
        raise 'Could not execute statement %s' %stmt
      params = _fetch(cursor)
      r.append(params)

    for r in runs:
      makeXML('.','run',r)


def dump_disk_runs():
  run_init()
  dir_name='/daqarea/lhcb/data/2015/RAW/FULL/FEST/FEST'
  runs = os.listdir(dir_name)
  runs.sort()
  runs = sys.argv[1:]
  
  db=_createConnection('rundb_admin','adm1n','lhcbonr_rundb')
  for run in runs:
    stmt =     """SELECT r.runid AS runid,
                        TO_CHAR(r.starttime,'YYYY') AS year,
                        TO_CHAR(r.starttime,'MM') AS month,
                        TO_CHAR(r.starttime,'YYYY-MM-DD HH24:MI:SS') AS starttime,
                        TO_CHAR(r.endtime,  'YYYY-MM-DD HH24:MI:SS') AS endtime,
                        r.partitionname AS pname,
                        r.runtype AS runtype,
                        r.activity AS activity,
                        TO_NUMBER(r.tck) AS tck
                 FROM   rundbruns r
                 WHERE  r.runid=%s """%(run,)

    cursor = db.cursor()
    if not cursor.execute(stmt):
        raise 'Could not execute statement %s' %stmt
    row = cursor.fetchone()
    res = [i for i in row]
    r = res
    stmt = """SELECT name, value
              FROM rundbrunparams 
              WHERE runid=%s"""%(run,)
      
    if not cursor.execute(stmt):
      raise 'Could not execute statement %s' %stmt
    params = _fetch(cursor)
    r.append(params)
    makeXML('.','run',r)
    print 'Run:',r

def run():
  dump_disk_runs()

#===========================================================================================
class Config:
  def __init__(self):    pass

config = Config();
curr_dir = os.getcwd()
config.archive =  curr_dir + os.sep + 'Archive'

config.todo = curr_dir + os.sep + 'ToDo'
config.archive_todo = config.archive+os.sep+'ToDo'

config.done = curr_dir + os.sep + 'Done'
config.archive_done = config.archive+os.sep+'Done'

config.fail = curr_dir + os.sep + 'Fail'
config.archive_fail = config.archive+os.sep+'Fail'

config.running = curr_dir + os.sep + 'Running'

def exists_run(place, run):
  name = place + os.sep + str(run)
  try:
    os.stat(name)
    return True
  except Exception,X:
    return False

def remove_dir(nam):
  files = os.listdir(nam);
  for f in files:
    remove(nam+os.sep+f)
  remove(nam)

def move_run(from_dir , to_dir, run):
  fr = from_dir + os.sep + str(run)
  to = to_dir + os.sep + str(run)
  print '+++ Archiving run ',run,' from ',fr,' to ',to,'....'
  if exists_run(to_dir, run):
    remove_dir(to)
  os.rename(fr, to)
  return True

def prepare_run(run):
  runs = [str(run)]
  db=_createConnection('rundb_admin','adm1n','lhcbonr_rundb')
  for run in runs:
    stmt = """SELECT r.runid AS runid,
                     TO_CHAR(r.starttime,'YYYY') AS year,
                     TO_CHAR(r.starttime,'MM') AS month,
                     TO_CHAR(r.starttime,'YYYY-MM-DD HH24:MI:SS') AS starttime,
                     TO_CHAR(r.endtime,  'YYYY-MM-DD HH24:MI:SS') AS endtime,
                     r.partitionname AS pname,
                     r.runtype AS runtype,
                     r.activity AS activity,
                     TO_NUMBER(r.tck) AS tck
              FROM   rundbruns r
              WHERE  r.runid=%s """%(run,)

    cursor = db.cursor()
    if not cursor.execute(stmt):
        raise 'Could not execute statement %s' %stmt
    row = cursor.fetchone()
    res = [i for i in row]
    r = res
    stmt = """SELECT name, value
              FROM rundbrunparams 
              WHERE runid=%s"""%(run,)
      
    if not cursor.execute(stmt):
      raise 'Could not execute statement %s' %stmt
    params = _fetch(cursor)
    r.append(params)
    makeXML(config.todo,'run',r)
    return True

def remove_run_ex(directory, run, comment=None):
  if exists_run(directory, run):
    remove_dir(directory+os.sep+str(run))
    if comment is None:
      print '+++ Run ',run,' will be removed from '+directory
    else:
      print '+++ Run ',run,comment+', and will be removed from '+directory
    return True
  return False

def archive_run_ex(directory_from, directory_to, run, comment=None):
  if exists_run(directory_from, run):
    move_run(directory_from, directory_to, run)
    if comment is None:
      print '+++ Run ',run,' will be archived to '+directory_to
    else:
      print '+++ Run ',run,comment+', and will be archived to '+directory_to
    return True
  if exists_run(directory_to, run):
    if comment is None:
      print '+++ Run ',run,' is already achived.... nothing done'
    else:
      print '+++ Run ',run,comment+', and is already achived.... nothing done'
  return True


def remove_run(run):
  if remove_run_ex(config.todo, run): return True
  if remove_run_ex(config.done, run, 'has analysed data'): return True
  if remove_run_ex(config.fail, run, 'has analysed data, failed'): return True
  if remove_run_ex(config.archive_done, run, 'has analysed data, is archived'): return True
  if remove_run_ex(config.archive_fail, run, 'has analysed data, failed, is archived'): return True
  if remove_run_ex(config.archive_todo, run, 'has not yet analysed data, is archived'): return True
  print '+++ Run ',run,' is not known.... nothing done'
  return False

def archive_run(run):
  if move_run_ex(config.todo, config.archive_todo, run): return True
  if move_run_ex(config.done, config.archive_done, run, 'has analysed data'): return True
  if move_run_ex(config.fail, config.archive_fail, run, 'has analysed data, failed'): return True
  if move_run_ex(config.archive_done, run, 'has analysed data, is archived'): return True
  if move_run_ex(config.archive_fail, run, 'has analysed data, failed, is archived'): return True
  if move_run_ex(config.archive_todo, run, 'has not yet analysed data, is archived'): return True
  print '+++ Run ',run,' is not known.... nothing done'
  return False

def resubmit_run(run):
  print '+++ Re-submitting run ',run
  if exists_run(config.todo, run):
    print '+++ Run ',run,' has nothing yet analysed and is queued. Nothing to do.'
    return True
  if exists_run(config.done, run):
    print '+++ Run ',run,' has already analysed data with success, and will be resubmitted now'
    move_run(config.done, config.todo, run)
    return True
  if exists_run(config.fail, run):
    print '+++ Run ',run,' has already analysed data, failed, and will be resubmitted now'
    move_run(config.fail, config.todo, run)
    return True
  if exists_run(config.archive_todo, run):
    print '+++ Run ',run,' has nothing yet analysed, but is already achived. Resubmitted now....'
    move_run(config.archive_todo, config.todo, run)
    return True
  if exists_run(config.archive_done, run):
    print '+++ Run ',run,' has successfully analysed data, is archived and will be resubmitted now.'
    move_run(config.archive_done, config.todo, run)
    return True
  if exists_run(config.archive_fail, run):
    print '+++ Run ',run,' has failed to analysed data, is archived and will be resubmitted now.'
    move_run(config.archive_todo, config.todo, run)
    return True
  if exists_run(config.running, run):
    print '+++ Run ',run,' is currently executing. You have to wait until finished.'
    return True
  print '+++ Run ',run,' is not known.... nothing done'
  return False

def archive_runs():
  for r in sys.argv[1:]:
    archive_run(int(r))
  return 0

def resubmit_runs():
  for r in sys.argv[1:]:
    resubmit_run(int(r))
  return 0


def make_aliases():
  print 'alias hlt2mon_archive_runs=\'python -c "import runmonitor; runmonitor.archive_runs()"\''
  print 'alias hlt2mon_resubmit_runs=\'python -c "import runmonitor; runmonitor.resubmit_runs()"\''



# ===============================================================================
if __name__=="__main__":
  #run()
  make_aliases()
