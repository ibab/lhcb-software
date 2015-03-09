import os, sys, time

os.environ['ONLINEKERNELROOT'] = '../../OnlineKernel'

from HLT2 import *
from OnlineKernel import DbCore, OnlineService

from OnlineKernel.gaudiweb \
    import gaudiweb, \
    Service, \
    DataManagementServer, \
    FileServlet, \
    ManagementServlet, \
    FavIconServer

# =================================================================
#
# class FileServlet
#
# @author   M.Frank
# @version  1.0
# @date     30/06/2002
# =================================================================
class HLT2Servlet(Service):
  """ Small class to allow file access on a given mount point.

      @author   M.Frank
      @version  1.0
      @date     30/06/2002
  """
  #===============================================================================
  def __init__(self, name='HLT2', **keys):
    """  Standard Constructor.
    
         @author   M.Frank
         @version  1.0
         @date     30/06/2002
    """
    from OnlineKernel.DbCore import DbCore
    require_authorization = 0
    if keys.has_key('require_authorization'):
      require_authorization = keys['require_authorization']
    Service.__init__(self, name, require_authorization)
    self.credentials = keys['credentials']
    self.m_mount  = keys['mount']
    self.m_config = keys['config']
    self.m_dbCore = DbCore
    c = self.m_config
    self.states = [c.st_initial,c.st_todo, c.st_done,c.st_failed, c.st_running]

  def mount(self):
    return self.m_mount

  def state_info(self):
    result = {}
    db     = self.m_dbCore(self.credentials).connection()
    cursor = db.cursor()
    for s in self.states:
      cursor.execute("SELECT * FROM hlt2_mon WHERE state=?",(s,))
      result[s] = cursor.fetchall()
    cursor.close()
    return result
    
  #===============================================================================
  def summary_data(self):
    db     = self.m_dbCore(self.credentials).connection()
    cursor = db.cursor()
    data   = '[\n'
    for s in self.states:
      cursor.execute("""SELECT '%s' as State, COUNT(*) as count 
                        FROM hlt2_mon 
                        WHERE state=?
                        ORDER BY State ASC"""%(s,),(s,))
      record = cursor.fetchall()[0]
      state  = record[0]
      num    = record[1]
      data   = data + '\t{ text: "%s [%d runs]", id: "%s", state: "%s", cls: "folder"},\n'%(state,num,state,state,)
    cursor.close()
    data = data[:-2] + '\n]\n'
    return data

  def select_runs(self, **keys):
    db     = self.m_dbCore(self.credentials).connection()
    cursor = db.cursor()
    count  = 0
    where  = '    '
    args   = []
    for k,v in keys.items():
      where = where[:-4] + k + '=? AND '
      args.append(v)
    where = where[:-4]
    stmt = """SELECT runid as Run, 
                     state as State,
                     partitionname as PartitionName,
                     partitionid as PartitionID,
                     runtype as RunType,
                     activity as Activity,
                     starttime as Started,
                     todo as Todo,
                     done as Done,
                     archived as Archived,
                     checked as Checked
              FROM hlt2_mon WHERE %s ORDER BY runid DESC"""%(where,)

    #print stmt
    #print args
    cursor.execute(stmt, args)
    record = cursor.fetchone()
    data   = '{\truns:[  \n'  # Careful! significant space here
    
    while record:
      count = count + 1
      checked = 'No'
      tm_todo  = 'No'
      tm_done  = 'No'
      tm_archived = 'No'
      tm_start = time.strftime('%d.%m.%Y %H:%M:%S',time.localtime(record[6]))
      if record[7]>0: tm_todo = time.strftime('%d.%m.%Y %H:%M:%S',time.localtime(record[7]))
      if record[8]>0: tm_done = time.strftime('%d.%m.%Y %H:%M:%S',time.localtime(record[8]))
      if record[9]>0: tm_archived = time.strftime('%d.%m.%Y %H:%M:%S',time.localtime(record[9]))
      if record[10]>0: checked='Yes'
      data = data + "\t\t{ Run:%d, State:'%s', PartitionName:'%s', PartitionID: %d, RunType:'%s', Activity:'%s', StartTime: '%s', Todo: '%s', Done: '%s', Archived: '%s', Checked: '%s'},\n"%(record[0],record[1],record[2],record[3],record[4],record[5],tm_start,tm_todo,tm_done,tm_archived,checked, )
      record = cursor.fetchone()
    cursor.close()
    data = data[:-2] + '\n\t],\n\troot:\'runs\',\n\tsuccess:true,\n\tcount:%d,\n\ttime:%d\n}\n'%(count,int(time.time()))    
    return data

  def runs_in_state2(self,state):
    db     = self.m_dbCore(self.credentials).connection()
    cursor = db.cursor()
    stmt = """SELECT runid as Run, 
                     state as State
              FROM hlt2_mon WHERE state=? ORDER BY runid DESC"""
    cursor.execute(stmt, (state,))
    record = cursor.fetchone()
    data   = '[  \n'   # Careful! significant space here
    while record:
      data = data + '\t{ text: "%d (%s)", id: "Run_%d", run: %d, leaf: 1, cls: "file" },\n'%(record[0],record[1],record[0],record[0], )
      record = cursor.fetchone()
    cursor.close()
    data = data[:-2] + '\n]\n'
    return data

  #===============================================================================
  def existsRun(self, runid):
    db     = self.m_dbCore(self.credentials).connection()
    cursor = db.cursor()
    try:
      cursor.execute("SELECT runid, state, archived  FROM hlt2_mon WHERE runid=?", (int(runid),))
      result = cursor.fetchone()
    except:
      result = None
    cursor.close()
    return result

  #===============================================================================
  def err(self, msg):
    return '{ failure:  1, message: "'+msg+'", status: "Failed" }'

  #===============================================================================
  def ok(self, msg):
    return '{ success:  1, message: "'+msg+'", status: "Failed" }'

  #===============================================================================
  def mark_checked(self, runid):
    if self.existsRun(runid):
      db = self.m_dbCore(self.credentials).connection()
      cursor = db.cursor()
      cursor.execute("UPDATE hlt2_mon SET checked=? WHERE runid=?",(1,runid,))
      cursor.close()
      db.commit()
      return self.ok('Run %d was successfully marked as checked.'%(runid,))
    return self.err('Run '+str(runid)+' is not in the database!')

  #===============================================================================
  def mark_unchecked(self, runid):
    if self.existsRun(runid):
      db = self.m_dbCore(self.credentials).connection()
      cursor = db.cursor()
      cursor.execute("UPDATE hlt2_mon SET checked=? WHERE runid=?",(0,runid,))
      cursor.close()
      db.commit()
      return self.ok('Run %d was successfully marked as NOT checked.'%(runid,))
    return self.err('Run '+str(runid)+' is not in the database!')

  #===============================================================================
  def archive_run(self, runid):
    run = self.existsRun(runid)
    if run:
      if run[1] == 'DONE' or run[1] == 'FAILED':
        if run[2]>0:
          return self.ok('Run %d was already archived -- Nothing done.'%(runid,))
        if run[1] == 'DONE':
          move_run_dir(runid, self.m_config.done, self.m_config.archive.done)
        elif run[1] == 'FAILED':
          move_run_dir(runid, self.m_config.failed, self.m_config.archive.failed)

        db = self.m_dbCore(self.credentials).connection()
        cursor = db.cursor()
        cursor.execute("UPDATE hlt2_mon SET archived=? WHERE runid=?",(int(time.time()),runid,))
        cursor.close()
        db.commit()
        return self.ok('Run %d was successfully archived.'%(runid,))
      return self.err('Run '+str(runid)+' is in state '+run[1]+' and cannot be archived.')
    return self.err('Run '+str(runid)+' is not in the database!')

  #===============================================================================
  def resubmit_run(self, runid):
    run = self.existsRun(runid)
    if run:
      if run[1] == 'DONE' or run[1] == 'FAILED':
        if run[2] > 0 and run[1] == 'DONE':
          move_run_dir(runid, self.m_config.archive.done, self.m_config.todo)
        elif run[2] > 0 and run[1] == 'FAILED':
          move_run_dir(runid, self.m_config.archive.failed, self.m_config.todo)
        elif run[1] == 'DONE':
          move_run_dir(runid, self.m_config.done, self.m_config.todo)
        elif run[1] == 'FAILED':
          move_run_dir(runid, self.m_config.failed, self.m_config.todo)
        db = self.m_dbCore(self.credentials).connection()
        cursor = db.cursor()
        cursor.execute("UPDATE hlt2_mon SET state=?, todo=?, done=?, archived=?, checked=? WHERE runid=?",\
                         ('TODO',int(time.time()),0,0,0,runid,))
        cursor.close()
        db.commit()
        return self.ok('Run %d was successfully resubmitted to analysis.'%(runid,))
      return self.err('Run '+str(runid)+' is in state '+str(run[1])+' and cannot be resubmitted.')
    return self.err('Run '+str(runid)+' is not in the database!')

  #===============================================================================
  def handleHTTP(self, handler, req_path, req, args, data):
    """  Handle standard HTTP requests to access files.
         Normally the file to be accessed must be visible under the mount point.

         @param handler    Request handler
         @param req_path   Full path name of the request
         @param req        Request type (GET, PUT, etc)
         @param args       URL Parameters passed to access a given page
         @param data       Input data (optional)

         @author   M.Frank
         @version  1.0
         @date     30/06/2002
    """
    print req_path, req, args
    if len(req_path)>2+len(self.name()):
      try:
        path = req_path[2+len(self.name()):]
        idx = path.find('&')
        if idx>0: path = path[:idx]
        idx = path.find('?')
        if idx>0: path = path[:idx]

        if path == 'state_summary' and args.has_key('state'):
          data = self.runs_in_state2(args['state'])
        elif path == 'state_summary':
          data = self.summary_data()
        elif path == 'select_runs' and args.has_key('state'):
          data = self.select_runs(state=args['state'])
        elif path == 'select_runs' and args.has_key('run'):
          data = self.select_runs(runid = int(args['run']))
        elif path == 'mark_checked' and args.has_key('run'):
          data = self.mark_checked(runid = int(args['run']))
        elif path == 'uncheck' and args.has_key('run'):
          data = self.mark_unchecked(runid = int(args['run']))
        elif path == 'archive' and args.has_key('run'):
          data = self.archive_run(runid = int(args['run']))
        elif path == 'resubmit' and args.has_key('run'):
          data = self.resubmit_run(runid = int(args['run']))
        elif path == 'reload' and args.has_key('run'):
          data = self.select_runs(runid = int(args['run']))
        else:
          a = '{  '
          for k,v in args.items():
            a = a + str(k) + ':  "' + str(v) + '", '
          a = a[:-2] + '}'
          data = \
              '{ failure:  1,' + \
              '  message: "Function \''+path+'\' is not recognized!",' + \
              '  status:  "Failed",' + \
              '  args:    '+ str(a) + \
              '}'
          print data
      except Exception, X:
        import traceback
        traceback.print_tb(sys.exc_info()[2])
        traceback.print_stack()
        a = '{  '
        for k,v in args.items():
          a = a + str(k) + ':  "' + str(v) + '", '
        a = a[:-2] + '}'
        data = \
            '{ failure:   1,\n' + \
            '  message:   "Exception during processing function \''+path+'\'.",\n' + \
            '  status:    "Failed",\n' + \
            '  args:       '+ str(a) + ",\n"+ \
            '  exception: "'+ str(X) + '"\n}'
        print data
      obj = (req_path, req_path, 'html', len(data), data)
      if ( obj is not None ):
        return self.makeWebFile(obj)
    return S_ERROR()

def run_web():
  from Config import Config
  import Params as params
  opt = os.environ['ONLINEKERNELROOT']
  config  = Config(top=params.home,have_archive=True)
  server  = DataManagementServer(8081, logging=1)
  server.registerMountpoint('html',     opt+os.sep+'html')
  server.registerMountpoint('static',   '..'+os.sep+'static')
  server.registerMountpoint('images',   opt+os.sep+'images')
  server.registerServlet('Manager',     ManagementServlet('Manager', 0))
  server.registerServlet('favicon.ico', FavIconServer(opt+os.sep+'images'+os.sep+'favicon.ico'))
  servlet = HLT2Servlet(name='hlt2mon', mount=params.home,credentials=params.credentials,config=config.config())
  server.registerServlet   (servlet.name(),     servlet)
  server.welcome()
  server.serve_forever()

if ( __name__ == "__main__"):
  run_web()
