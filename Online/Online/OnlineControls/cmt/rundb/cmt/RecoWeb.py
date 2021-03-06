import sys, os, time, DbCore, gaudiweb, xmlrpclib, datetime, pprint

basePath        = os.environ['ONLINEKERNELROOT']
default_users = [ ('administrator', 'admin_oder_was'), ('frankm', 'h')]
servicePort = 8081
startup = 'startAll'


import os, sys

g_simulate = True

RUN_RUNID=0
RUN_PARTITION=1
RUN_STARTTIME=2
RUN_RUNTYPE=3
RUN_ACTIVITY=4

FILE_FILEID=0
FILE_RUNID=1
FILE_NAME=2
FILE_STREAM=6
FILE_CREATION=7
FILE_SOURCE='/daqarea'

# ------------------------------------------------------------------------------
def _createConnection(username, password, database):
  import cx_Oracle
  db = cx_Oracle.Connection(username, password, database)
  return db

# ------------------------------------------------------------------------------
class Dirac:
  # ----------------------------------------------------------------------------
  def __init__(self,slice='Reco_Ctrl00'):
    import xmlrpclib
    self.slice = slice
    self.diracmanager = xmlrpclib.ServerProxy('http://storeio01.lbdaq.cern.ch:8890')
    self.jobIDs = {}
    
  # ----------------------------------------------------------------------------
  def removeFile(self, fid):
    global g_simulate
    print "diracmanager.getJobOutputb("+str(fid)+")"
    if g_simulate: return 1
    sc = self.diracmanager.getJobOutput(int(fid));
    print 'Dirac Simulator:Submitted job to DIRAC:', sc
    self.jobIDs[sc['Value']] = 'Submitted'
    return sc['Value']

  # ----------------------------------------------------------------------------
  def submit(self, guid, source_file, out_file):
    global g_simulate
    logfile = "/localdisk/repro/%s.log" %os.path.basename(os.path.splitext(source_file)[0])
    #sc = self.diracmanager.getState()
    #print sc
    #if not sc['ok'] or sc['params']['state'] != 'RUNNING':
    #  print 'Dirac Simulator: DIRAC Manager is not running!'
    #  return None
    print "diracmanager.submitJob('"+self.slice+"','"+source_file+"','"+out_file+"','"+logfile+"','"+str(guid)+"')"
    if g_simulate: return 1
    sc = self.diracmanager.submitJob(self.slice, source_file, out_file, logfile, guid)
    print 'Dirac Simulator:Submitted job to DIRAC:', sc
    self.jobIDs[sc['Value']] = 'Submitted'
    return sc['Value']

  # ----------------------------------------------------------------------------
  def run(self):
    import time
    print 'Dirac Simulator: Need to check ',len(self.jobIDs.keys()),'jobs'
    while(len(self.jobIDs.keys())>0):
      time.sleep(30)
      for id in self.jobIDs.keys():
        state = self.jobIDs[id]
        if state == 'Submitted':
          sc = self.diracmanager.jobStatus(id)
          status = sc['Value']
          if status.lower() in ['done', 'error']:
            sc = self.diracmanager.getJobOutput(id)
            print sc
            self.jobIDs[id] = status.lower()
      for id in self.jobIDs.keys():
        if self.jobIDs[id] in ['done', 'error']:
          del self.jobIDs[id]
    print 'Dirac Simulator: done'
    

# ------------------------------------------------------------------------------
RUN_RUNID=0
RUN_PARTITION=1
RUN_STARTTIME=2
RUN_RUNTYPE=3
RUN_ACTIVITY=4

FILE_FILEID=0
FILE_RUNID=1
FILE_NAME=2
FILE_STREAM=6
FILE_CREATION=7
FILE_SOURCE='/daqarea'
# ------------------------------------------------------------------------------
class RunDB:
  def __init__(self,db):
    self.db = db
    self.cursor = db.cursor()

  # ----------------------------------------------------------------------------
  def fetchOne(self,stmt):
    c = self.cursor
    if not c.execute(stmt):
      raise 'Could not execute statement %s' %stmt
    row = c.fetchone()
    return row

  # ----------------------------------------------------------------------------
  def fetchMany(self,stmt):
    c = self.cursor
    if not c.execute(stmt):
      raise 'Could not execute statement %s' %stmt
    result=[]
    row = c.fetchone()
    while row:
      result.append(row)
      row = c.fetchone()
    return result

  # ----------------------------------------------------------------------------
  def run(self,run_no,partition='LHCb'):
    stmt = """\
    SELECT runid, partitionname, starttime, runtype, activity
    FROM rundbruns
    WHERE destination = 'OFFLINE'
    AND runid=%d"""%(run_no,)
    if partition: stmt = stmt + " AND partitionname='%s'"%(partition,)
    return self.fetchOne(stmt)
  
  # ----------------------------------------------------------------------------
  def files(self,run_no,stream=None):
    stmt = """\
    SELECT f.fileid, f.runid, 
           f.name, f.state, f.bytes,
           f.events, f.stream, f.creationtime
    FROM rundbfiles f
    WHERE f.runid=%d"""%(run_no,)
    if stream: stmt = stmt + " AND f.stream='%s'"%(stream,)
    return self.fetchMany(stmt)

  # ----------------------------------------------------------------------------
  def fileParam(self,fid,name):
    stmt = """\
    SELECT p.name, p.value, p.type
    FROM rundbfileparams p
    WHERE p.fileid=%d and p.name='%s'"""%(fid,name,)
    return self.fetchOne(stmt)
  
  # ----------------------------------------------------------------------------
  def fileGUID(self,fid):
    return self.fileParam(fid,'guid')


#=================================================================================
class DbTable:
  #===============================================================================
  def __init__(self,cursor):
    self.cursor = cursor
    self.headers = None
   
  #===============================================================================
  def add2line(self,items,tag='td'):
    if items:
      line=''
      for i in items: line=line+'<'+tag+'>'+str(i)+'</'+tag+'>'
      return line
    return ''
  #===============================================================================
  def line(self,items,tag='td'):
    if items:
      line='<TR>'+self.add2line(items,tag)+'</TR>'
      return line+'</TR>'
    return ''

  #===============================================================================
  def heading(self):
    return self.line(self.headers,'th')

  #===============================================================================
  def build(self,name,stmt):
    res = self.cursor.select(stmt)
    if res[1]:
      b = '<TABLE name="'+name+'">'+self.heading()
      res = self.cursor.next()[1]
      while res is not None:
        b = b + self.line(res)
        res = self.cursor.next()[1]
      return b + '</TABLE>'
    return 'No Results found.'

#=================================================================================
#
#  class ReprocesssingManager
#
#=================================================================================
class ReprocesssingManager(gaudiweb.FileServlet):
  """  class ReprocesssingManager.
    
       @author   M.Frank
       @version  1.0
       @date     30/06/2002
  """

  #===============================================================================
  def __init__(self, name, port, require_authentication=1):
    """  Standard Constructor.
    
         @author   M.Frank
         @version  1.0
         @date     30/06/2002
    """
    gaudiweb.FileServlet.__init__(self, name, name, require_authentication)
    self.rundb  = DbCore.DbCore('rundb_admin/adm1n@rundb')
    self.recodb = DbCore.DbCore('RECODB/lbreco09@lhcbonr_recodb')
    self.dirac  = xmlrpclib.ServerProxy('http://storeio01.lbdaq.cern.ch:8890')
    self.slice  = 'Reco_Ctrl00'
    self.port = port
    
  #===============================================================================
  def fetchOne(self,db,stmt):
    c = db._cur
    if not c.execute(stmt):
      raise 'Could not execute statement %s' %stmt
    row = c.fetchone()
    return row

  #===============================================================================
  def fetchMany(self,db,stmt):
    c = db._cur
    if not c.execute(stmt):
      raise 'Could not execute statement %s' %stmt
    result=[]
    row = c.fetchone()
    while row:
      result.append(row)
      row = c.fetchone()
    return result
    
  #===============================================================================
  def configSelectionBox(self):
    configs = []
    stmt = "SELECT slice FROM reco_slices ORDER BY slice DESC"
    cur = DbCore.Cursor(self.recodb)
    res = cur.select(stmt)
    b = '<SELECT id="recodb_slice_selector">'
    if res:
      res = cur.next()[1]
      while res is not None:
        s = ''
        if res[0]==self.slice:s=' SELECTED="true"'
        b = b + '<OPTION'+s+' value="'+res[0]+'">'+res[0]+'</OPTION>'
        res = cur.next()[1]
    b = b + '</SELECT>'
    return b
  #===============================================================================
  def rundbRun(self,run_no,partition='LHCb'):
    stmt = """\
    SELECT runid, partitionname, starttime, runtype, activity
    FROM rundbruns
    WHERE destination = 'OFFLINE'
    AND runid=%d"""%(run_no,)
    if partition: stmt = stmt + " AND partitionname='%s'"%(partition,)
    return self.fetchOne(self.rundb,stmt)

  #===============================================================================
  def rundbFiles(self,run_no=None,fid=None,stream=None):
    stmt = """\
    SELECT f.fileid, f.runid, 
           f.name, f.state, f.bytes,
           f.events, f.stream, f.creationtime
    FROM rundbfiles f """
    o = " WHERE "
    if stream:
      stmt = stmt + o + " f.stream='%s'"%(stream,)
      o = "AND"
    if run_no:
      stmt = stmt + o + " f.runid=%s"%(str(run_no),)
      o = "AND"    
    if fid:
      stmt = stmt + o + " f.fileid=%s"%(str(fid),)
      o = "AND"

    return self.fetchMany(self.rundb,stmt)

  #===============================================================================
  def rundbFileParam(self,fid,name):
    stmt = """\
    SELECT p.name, p.value, p.type
    FROM rundbfileparams p
    WHERE p.fileid=%d and p.name='%s'"""%(fid,name,)
    return self.fetchOne(self.rundb,stmt)
  
  #===============================================================================
  def rundbFileGUID(self,fid):
    return self.rundbFileParam(fid,'guid')

  #===============================================================================
  def submitJob(self, run, f):
    in_fmt  = '/lhcb/data/%d/RAW/%s/%s/%s/%d/%s'
    out_fmt = '/lhcb/data/%d/RDST/%08d/0000/%s'
    guid = self.rundbFileGUID(f[FILE_FILEID])
    out_path = '/lhcb/data/'+str(run[RUN_STARTTIME].year)+'/RAW/'+f[FILE_STREAM]+'/'+run[RUN_PARTITION]+'/'+run[RUN_RUNTYPE]+'/'+str(f[FILE_RUNID])+'/'+f[FILE_NAME]
    in_path = in_fmt%(run[RUN_STARTTIME].year,f[FILE_STREAM],run[RUN_PARTITION],run[RUN_RUNTYPE],f[FILE_RUNID],f[FILE_NAME])
    guid = self.rundbFileGUID(f[FILE_FILEID])
    if not guid:
      return 'ALARM: No GUID for file:',in_path
    fout = "%s.mdf" %os.path.splitext(f[FILE_NAME])[0]
    out_path = out_fmt%(run[RUN_STARTTIME].year,f[FILE_RUNID],fout,)
    ###r = dirac.submit(guid[1], in_path, out_path)
    log_path = "/localdisk/repro/%s.log" %os.path.basename(os.path.splitext(in_path)[0])
    sc = self.dirac.submitJob(self.slice, in_path, out_path, log_path, guid[1])

    return '<TABLE class="RecoLog"><TR><TD>Result:</TD><TD>'+str(sc)+\
           '</TD></TR><TR><TD>GUID:</TD><TD>'+guid[1]+\
           '</TD></TR><TR><TD>Input:</TD><TD>'+in_path+\
           '</TD></TR><TR><TD>Output:</TD><TD>'+out_path+'</TD></TR></TABLE>'

  #===============================================================================
  def pageTitle(self):
    return 'LHCb Reprocessing web'

  #===============================================================================
  def pageTrailer(self):
    return '</body></html>'

  #===============================================================================
  def pageHeader(self, url, home_page,with_header=True):
    buf = """<html><head><title>"""+self.pageTitle()+"""</title>
               <meta content="text/html; charset=iso-8859-1" http-equiv=Content-Type></meta>
               <meta HTTP-EQUIV="Pragma"         CONTENT="no-cache"></meta>
               <meta HTTP-EQUIV="CACHE-CONTROL"  CONTENT="no-cache"></meta>
               <meta HTTP-EQUIV="Expires"        CONTENT="0"></meta>
               <meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1"></meta>

               <base href="""+url[:-1]+'"'+"""></base>
               <style type="text/css">
                 .OVL  { text-decoration: overline; }
                 .FEW  { text-decoration: none;     }
                 .SOME { text-decoration: none;     }
                 .MANY { text-decoration: none;     }
                 .VERYMANY { text-decoration: none; }
               </style>
               <link rel="stylesheet" href="/html/DataMgmt.css"/>
               <link rel="stylesheet" href="/style/Reco.css"/>
               <script lang="javascript" src="/style/Reco.cpp"></script>
               <script lang="javascript" src="/html/DataMgmt.js"></script>
               """
    if with_header:
      buf = buf + """
               <script lang="javascript">
                   make_header();
                   make_toolbar('http://lhcb.cern.ch','"""+home_page+"""',null); </script>
                   """
    buf = buf + '</head><body>'
    return buf

  #===============================================================================
  def pageBody(self, handler, req_path, req, args, data, home_page='/ReprocesssingManager'):
    srv = handler.server
    url = 'http://'+srv.nodeName()+':'+str(srv.port)+'/'+self.name()
    buf = self.pageHeader(url,home_page)
    buf = buf + self.pageContent(url, handler, req_path, req, args, data)
    buf = buf + self.pageTrailer()
    return self.makeWebFile((req_path, self.name(), 'text/html', len(buf), buf))

  #===============================================================================
  def defaultPageContent(self, url, handler, req_path, req, args, data): 
    buf =       '<TABLE cellspacing="0" cellpadding="6" width="100%">'
    buf = buf + '<TR><TD colspan="3" bgcolor="red"><div class="TITLES">Commands</div></TD></TR>'
    buf = buf + self.addItem(url, 'rundb_recent_runs', 'Show recent runs')
    buf = buf + self.addItem(url, 'recodb_show_status','Reprocessing summary')
    buf = buf + '</TABLE>'
    return buf

  #===============================================================================
  def addItem(self,url,tag,title):
    buf =       '<TR><FORM action="'+url+'">'
    buf = buf + '<TD colspan="2" class="bullets">'+title+'</TD>'
    buf = buf + '<TD><INPUT type="hidden" name="-'+tag+'" value="1"/><INPUT type="submit" value="Submit"/></TD>'
    buf = buf + '</FORM></TR>'
    return buf

  #===============================================================================
  def showRunDbFills(self, url, handler, req_path, req, args, data):
    srv = handler.server
    fill = ''
    if args.has_key('-fill_selector'):
      fill = args['-fill_selector']

    f = 'fill_id='
    if len(fill): f = fill
    buf = """<H2>Recent LHCb Fills:</H2>
             Select fills by number:<INPUT type="text" id="rundb_fill_selector" width="200" value="%s"/>
             <INPUT type="button" value="Show" onclick="javascript:rundb_show_fills()"/><BR>
    """%(f,)

    fill_sel = ''
    if len(fill): fill_sel = " WHERE " + fill
    stmt= """\
    SELECT * FROM (
    SELECT   fill_id,
    TO_CHAR(timestamp,'YYYY-MM-DD HH24:MI:SS'),
    time_total,
    time_hvon,
    time_veloin,
    time_running,
    time_logged,
    FLOOR(lumi_total),
    FLOOR(lumi_hvon),
    FLOOR(lumi_veloin),
    FLOOR(lumi_running),
    FLOOR(lumi_logged)
    FROM     rundbfills %s
    ORDER BY fill_id DESC)
    WHERE ROWNUM<100
    """%(fill_sel,)
    cur = DbCore.Cursor(self.rundb)
    tab = DbTable(cur)
    tab.headers = ['Fill', 'Time:Start', 'Fill Length','HV On','Velo IN','Running','Logged','Lumi:Total','HV On','Velo IN','Running','Logged']
    res = cur.select(stmt)
    if res[1]:
      b = '<TABLE class="RecoTable">'+tab.heading()
      res = cur.next()[1]
      while res is not None:
        r = str(res[0])
        b = b + """<TR><TD onclick="javascript:rundb_show_runs_by_fill('fillid=%s')">%s</TD>"""%(r,r,)
        b = b + '<TD>%s</TD><TD>%s</TD><TD>%s</TD><TD>%s</TD><TD>%s</TD><TD>%s</TD>'%\
            (res[1],
             datetime.datetime.fromtimestamp(res[2]).strftime('%H:%M:%S'),
             datetime.datetime.fromtimestamp(res[3]).strftime('%H:%M:%S'),
             datetime.datetime.fromtimestamp(res[4]).strftime('%H:%M:%S'),
             datetime.datetime.fromtimestamp(res[5]).strftime('%H:%M:%S'),
             datetime.datetime.fromtimestamp(res[6]).strftime('%H:%M:%S'),
             )
        b = b + tab.add2line(res[7:])+'</TR>'
        res = cur.next()[1]
      return buf + b + '</TABLE>'
    return buf

  #===============================================================================
  def showRunDbRuns(self, url, handler, req_path, req, args, data):
    srv = handler.server
    fill = '';
    if args.has_key('-fill'):
      fill = ' AND '+args['-fill']

    f = 'fillID='
    if len(fill): f = args['-fill']
    buf = """<H2>Recent LHCb runs:</H2>
             Select runs by fill number:<INPUT type="text" id="fill_no" width="200" value="%s"/>
             <INPUT type="button" value="Show" onclick="javascript:rundb_show_fill()"/><BR>
    """%(f,)

    stmt= """\
    SELECT * FROM (
    SELECT   runid, fillid, partitionname, state, starttime, endtime, runtype, activity, destination
    FROM     rundbruns
    WHERE    partitionname='LHCb'
    AND      destination='OFFLINE' %s
    ORDER BY runid DESC)
    WHERE ROWNUM<30
    """%(fill,)
    cur = DbCore.Cursor(self.rundb)
    tab = DbTable(cur)
    tab.headers = ['Run', 'Fill', 'Partition','State','Start','End','Runtype','Activity','Destination']
    ###buf = tab.build('rundbruns',stmt)
    res = cur.select(stmt)
    if res[1]:
      b = '<TABLE class="RecoTable">'+tab.heading()
      res = cur.next()[1]
      while res is not None:
        r = str(res[0])
        b = b + '<TR><TD onclick="javascript:rundb_show_run('+r+')">'+r+'</TD>'
        b = b + tab.add2line(res[1:])+'</TR>'
        res = cur.next()[1]
      return buf + b + '</TABLE>'
    return buf

  #===============================================================================
  def showRunDbRun(self, url, handler, req_path, req, args, data):
    srv = handler.server
    run = args['-rundb_show_run']
    buf = '<H2>Files for run '+str(run)+':</H2>'

    stmt= """\
    SELECT   runid, fileid, name, stream, creationtime, events, bytes
    FROM     rundbfiles
    WHERE    runid=%s
    ORDER BY fileid DESC
    """%(str(run),)
    cur = DbCore.Cursor(self.rundb)
    tab = DbTable(cur)
    tab.headers = ['Run', 'FID', 'Name','Stream','Created','Events','Size']
    res = cur.select(stmt)
    if res[1]:
      b = '<TABLE class="RecoTable">'+tab.heading()
      res = cur.next()[1]
      while res is not None:
        fid = str(res[1])
        b = b + '<TR><TD onclick="javascript:rundb_show_file('+fid+')">'+str(res[0])+'</TD>'
        b = b + tab.add2line(res[1:])+'</TR>'
        res = cur.next()[1]

      cmd = """
        <TABLE class="RecoActions">
        <TR><TD>
        <INPUT type="button" value="Show files in recodb" onclick="javascript:recodb_show_files()"></INPUT>
        </TD><TD>
        <INPUT type="button" value="Goto Run Selector" onclick="javascript:rundb_show_runs()"></INPUT>
        </TD><TD>
        Select Configuration:%s
        </TD><TD>
        <INPUT type="button" value="Submit FULL Stream" onclick="javascript:rundb_submit_run('%s','FULL')"></INPUT>
        </TD><TD>
        <INPUT type="button" value="Submit EXPRESS Stream" onclick="javascript:rundb_submit_run('%s','EXPRESS')"></INPUT>
        </TD></TR></TABLE>
        """%(self.configSelectionBox(),run,run,)
      return buf + cmd + b + '</TABLE>'
    return buf

  #===============================================================================
  def showRunDbFileCounters(self, fid):
    stmt = """\
    SELECT fileid, type, value
    FROM rundbfilecounters
    WHERE fileid=%s
    ORDER BY type ASC"""%(fid,)

    buf = '<H2>File Counters:</H2>'
    curP = DbCore.Cursor(self.rundb)
    tab = DbTable(curP)
    tab.headers = ['Counter ID/Mnemonic','Number of events']
    res = curP.select(stmt)
    if res[1]:
      buf = buf + '<TABLE class="RecoTable">'+tab.heading()
      res = curP.next()[1]
      while res is not None:
        typ = 'Unknown'
        if   res[1]==0:  typ = 'Physics'
        elif res[1]==1:  typ = 'BeamGas'
        elif res[1]==2:  typ = 'Lumi'
        elif res[1]==3:  typ = 'Technical'
        elif res[1]==4:  typ = 'Other'
        elif res[1]==8:  typ = 'Inclusive Physics'
        elif res[1]==10: typ = 'Inclusive MinBias'
        elif res[1]==12: typ = 'Inclusive Lumi'
        elif res[1]==14: typ = 'Inclusive BeamGas'
        elif res[1]==16: typ = 'Inclusive Other'
        elif res[1]==9:  typ = 'Exclusive Physics'
        elif res[1]==11: typ = 'Exclusive MinBias'
        elif res[1]==13: typ = 'Exclusive Lumi'
        elif res[1]==15: typ = 'Exclusive BeamGas'
        elif res[1]==17: typ = 'Exclusive Other'
        elif res[1]==18: typ = 'High Lumi'
        elif res[1]==19: typ = 'Low Lumi'

        buf = buf + '<TD>'+str(res[1])+'/'+typ+'</TD><TD>'+str(res[2])+'</TD></TR>'
        res = curP.next()[1]
      buf = buf + '</TABLE>'
    return buf

  #===============================================================================
  def showRunDbFileParams(self, fid):
    stmt = """\
    SELECT fileid, name, value
    FROM rundbfileparams
    WHERE fileid=%s"""%(fid,)

    buf = '<H2>File Parameters:</H2>'
    curP = DbCore.Cursor(self.rundb)
    tab = DbTable(curP)
    tab.headers = ['FID', 'Name','Value']
    res = curP.select(stmt)
    if res[1]:
      buf = buf + '<TABLE class="RecoTable">'+tab.heading()
      res = curP.next()[1]
      while res is not None:
        buf = buf + tab.line(res)+'</TR>'
        res = curP.next()[1]
      buf = buf + '</TABLE>'
    return buf

  #===============================================================================
  def showRunDbFile(self, url, handler, req_path, req, args, data):
    srv = handler.server
    fid = args['-rundb_show_file']
    stmt= """\
    SELECT   runid, fileid, name, stream, creationtime, events, bytes
    FROM     rundbfiles
    WHERE    fileid=%s
    ORDER BY fileid DESC
    """%(str(fid),)
    cur = DbCore.Cursor(self.rundb)
    tab = DbTable(cur)
    tab.headers = ['Run', 'FID', 'Name','Stream','Created','Events','Size']
    res = cur.select(stmt)
    if res[1]:
      b = '<TABLE class="RecoTable">'+tab.heading()
      res = cur.next()[1]
      run = str(res[0])
      buf = '<H2>File '+str(res[2])+' for run '+run+':</H2>'
      while res is not None:
        fid = str(res[1])
        b = buf + b + tab.line(res)+'</TR></TABLE>'
        b = b + """
        <TABLE class="RecoActions">
        <TR><TD>
        <INPUT type="button" value="Goto Run" onclick="javascript:rundb_show_run(%s)"></INPUT>
        <INPUT type="button" value="Goto Run Selector" onclick="javascript:rundb_show_runs()"></INPUT>
        <INPUT type="button" value="File in RecoDB" onclick="javascript:recodb_show_files()"></INPUT>
        Select Configuration:%s
        <INPUT type="button" value="Submit File" onclick="javascript:rundb_submit_file(%s)"></INPUT>
        </TD></TR></TABLE>
        """%(run,self.configSelectionBox(),fid)
        b = b + self.showRunDbFileParams(fid)
        b = b + self.showRunDbFileCounters(fid)
        return b
    else:
      buf = buf + '<H1>No file found with this FID!</H1>'
    return buf

  #===============================================================================
  def showRunDbSubmitRun(self, url, handler, req_path, req, args, data):
    run    = args['-rundb_submit_run']
    stream = args['-stream']
    self.slice = args['-recodb_slice']
    buf = "<H2>Submitted from run:"+run+" all files of stream:"+stream+"</H2>"
    files = self.rundbFiles(run,stream=stream)
    if len(files)>0:
      f = files[0]
      r = self.rundbRun(f[FILE_RUNID])
      log = '<TABLE class="RecoLog"><TR><TH>FID</TH><TH>Result</TH></TR>'
      for f in files: log = log + '<TR><TD>' + str(f[FILE_FILEID]) + '</TD><TD>' + self.submitJob(r,f) + '</TD></TR>'
      ##for f in files: log = log + '<TR><TD>' + str(f[FILE_FILEID]) + '</TD><TD>' + str(f)+'  '+str(r)+'  </TD></TR>'
      log = log + '</TABLE>'
    else:
      log = '<H2>No files found....</H2>'
    return buf + log + self.showRecoDbFiles(url, handler, req_path, req, args, data)

  #===============================================================================
  def showRunDbSubmitFile(self, url, handler, req_path, req, args, data):
    fid    = args['-rundb_submit_file']
    self.slice = args['-recodb_slice']
    files = self.rundbFiles(fid=fid)
    if len(files) > 0:
      f = files[0]
      r = self.rundbRun(f[FILE_RUNID])
      buf = "<H2>Submitted file:"+fid+"</H2>"
      log = '<TABLE class="RecoLog"><TR><TH>FID</TH><TH>Result</TH></TR>'
      log = log + '<TR><TD>' + str(f[FILE_FILEID]) + '</TD><TD>' + self.submitJob(r,f) + '</TD></TR>'
      ##log = log + '<TR><TD>' + str(f[FILE_FILEID]) + '</TD><TD>' + self.slice + '</TD></TR>'
      log = log + '</TABLE>'
    else:
      log = '<H2>No files found....</H2>'
    return buf + log + self.showRecoDbFiles(url, handler, req_path, req, args, data)

  #===============================================================================
  def showReprocessing(self, url, handler, req_path, req, args, data):
    buf =       '<TABLE cellspacing="0" cellpadding="6" width="100%">'
    buf = buf + '<TR><TD colspan="3" bgcolor="red"><div class="TITLES">Reprocessing Actions &amp; Commands</div></TD></TR>'
    buf = buf + self.addItem(url, 'recodb_show_configs', 'Show known configurations')
    buf = buf + self.addItem(url, 'recodb_show_summary', 'Show Reconstruction activity summary')
    buf = buf + self.addItem(url, 'recodb_show_files',   'Show all files present in the system')
    buf = buf + self.addItem(url, 'recodb_clean',        'Clean reprocessing database')
    buf = buf + '</TABLE>'
    return buf;

  #===============================================================================
  def showRecoDbClean(self, url, handler, req_path, req, args, data):
    buf = "<H1>Cleaning reconstruction database</H1>"
    stmt = "DELETE FROM reco_main"
    cur = DbCore.Cursor(self.recodb)
    res = cur.select(stmt)
    buf = buf + '<BR>'+stmt+'<BR>SQL result:'+str(res)+'<BR>'
    stmt = "DELETE FROM reco_files"
    cur = DbCore.Cursor(self.recodb)
    res = cur.select(stmt)
    buf = buf + '<BR>'+stmt+'<BR>SQL result:'+str(res)+'<BR>'
    cur.commit()
    return buf + self.showRecoDbFiles(url, handler, req_path, req, args, data)

  #===============================================================================
  def showRecoDbRemoveFile(self, url, handler, req_path, req, args, data):
    fid = args['-recodb_remove_file']
    buf = "<H1>Remove file "+fid+"</H1>"
    """
    #  ==> Do it with the database:
    stmt = "DELETE FROM reco_main WHERE fileid=%s"%(fid,)
    cur = DbCore.Cursor(self.recodb)
    res = cur.select(stmt)
    buf = buf + '<BR>'+stmt+'<BR>SQL result:'+str(res)+'<BR>'
    stmt = "DELETE FROM reco_files WHERE fileid=%s"%(fid,)
    cur = DbCore.Cursor(self.recodb)
    res = cur.select(stmt)
    buf = buf + '<BR>'+stmt+'<BR>SQL result:'+str(res)+'<BR>'
    cur.commit()
    """
    sc = self.dirac.getJobOutput(int(fid));
    res = pprint.pformat(sc).replace('\n','</TD></TR><TR><TD>')
    buf = buf + '<BR>' + '<TABLE class="RecoLog"><TR><TD><B>Result of removing FID:'+fid+'</B></TD></TR><TR><TD>'+res+'</TD></TR></TABLE>'
    return buf + self.showRecoDbFiles(url, handler, req_path, req, args, data)

  #===============================================================================
  def showRecoDbRescheduleFile(self, url, handler, req_path, req, args, data):
    fid = args['-recodb_reschedule_file']
    buf = "<H1>Reschedule file with fid:"+fid+"</H1>"
    sc = self.dirac.rescheduleJob(int(fid))
    res = pprint.pformat(sc).replace('\n','</TD></TR><TR><TD>')
    buf = buf + '<BR>' + '<TABLE class="RecoLog"><TR><TD><B>Result of rescheduling FID:'+fid+'</B></TD></TR><TR><TD>'+res+'</TD></TR></TABLE>'
    args['-recodb_show_file'] = fid
    return buf + self.showRecoDbFile(url, handler, req_path, req, args, data)

  #===============================================================================
  def showRecoDbRescheduleFileSelection(self, url, handler, req_path, req, args, data):
    prev  = ''
    state = args['-state']
    mgr   = args['-manager']

    if args.has_key('-prev_state'): prev = " AND prev_state='%s'"%(args['-prev_state'],)

    buf = "<H1>Reschedule files for manager:"+mgr+" which are in state:"+state+"</H1>"

    stmt= """\
    SELECT   m.fileid
    FROM     reco_main m
    WHERE    m.slice='%s' AND m.now_state='%s' %s
    ORDER BY m.fileid ASC
    """%(mgr,state,prev,)
    cur = DbCore.Cursor(self.recodb)
    res = cur.select(stmt)
    fids=[]
    if res[1]:
      res = cur.next()[1]
      while res is not None:
        fids.append(res[0])
        res = cur.next()[1]
      for fid in fids:
        sc = self.dirac.rescheduleJob(fid)
        fid = str(fid)
        res = pprint.pformat(sc).replace('\n','</TD></TR><TR><TD>')
        buf = buf + '<BR>' + '<TABLE class="RecoLog"><TR><TD><B>Result of rescheduling FID:'+fid+'</B></TD></TR><TR><TD>'+res+'</TD></TR></TABLE>'
    else:
      buf = buf + 'No files found, which satisfy the selection.'
    return buf + self.showRecoDbSummary(url, handler, req_path, req, args, data)

  #===============================================================================
  def showRecoDbSetFileState(self, url, handler, req_path, req, args, data):
    st  = args['-state']
    fid = args['-recodb_set_filestate']
    buf = "<H1>Change of file state fid:"+fid+" new state:"+st+"</H1>"
    stmt = "UPDATE reco_main SET prev_state='%s', now_state='%s',  WHERE fileid=%s"%(st,st,fid,)
    cur = DbCore.Cursor(self.recodb)
    res = cur.select(stmt)
    buf = buf + '<BR>'+stmt+'<BR>SQL result:'+str(res)+'<BR>'
    args['-recodb_show_file'] = fid
    cur.commit()
    return buf + self.showRecoDbFile(url, handler, req_path, req, args, data)

  #===============================================================================
  def showRecoDbFile(self, url, handler, req_path, req, args, data):
    fid = args['-recodb_show_file']
    stmt= """\
    SELECT   m.fileid,
             NVL(m.slice,'---'),
             NVL(m.prev_state,'Unknown'),
             NVL(m.now_state,'Unknown'),
             NVL(m.updated_time,0.0),
             NVL(f.raw_file,'---'),
             NVL(f.reco_file,'---'),
             NVL(f.log_file,'---'),
             NVL(f.guid,'---'),
             NVL(f.read_evts,-1),
             NVL(f.written_evts,-1),
             NVL(f.file_size,-1),
             NVL(f.md5,'---')
    FROM     reco_main m, reco_files f
    WHERE    m.fileid=f.fileid
    AND      m.fileid=%s
    ORDER BY m.fileid ASC
    """%(fid,)
    cur = DbCore.Cursor(self.recodb)
    res = cur.select(stmt)
    buf = '<H2>Reconstruction file ID:%s</H2>'%(fid,)
    if res[1]:
      buf = buf + """<TABLE class="RecoTable">"""
      res = cur.next()[1]
      while res is not None:
        buf = buf + """
        <TR><TD>FID</TD><TD>%d</TD></TR>
        <TR><TD>Raw file:</TD><TD>%s</TD></TR>
        <TR><TD>Reconstructed file:</TD><TD>%s</TD></TR>
        <TR><TD>Log file:</TD><TD>%s</TD></TR>
        <TR><TD>Previous State:</TD><TD>%s</TD></TR>
        <TR><TD>Current State:</TD><TD>%s</TD></TR>
        <TR><TD>Timestamp:</TD><TD>%s</TD></TR>
        <TR><TD>GUID:</TD><TD>%s</TD></TR>
        <TR><TD>Events read:</TD><TD>%d</TD></TR>
        <TR><TD>Events written:</TD><TD>%d</TD></TR>
        <TR><TD>Output file size:</TD><TD>%d</TD></TR>
        <TR><TD>MD5 checksum:</TD><TD>%s</TD></TR></TABLE>
        """%(res[0],res[5],res[6],res[7],res[2],res[3],
             datetime.datetime.fromtimestamp(res[4]).strftime('%Y-%m-%d %H:%M:%S'),
             res[8],res[9],res[10],res[11],res[12])
        res = cur.next()[1]
      buf = buf + """
        </TABLE><BR></BR>
        <TABLE class="RecoActions">
        <TR><TD>
        <INPUT type="button" value="Remove File" onclick="javascript:recodb_remove_file('%s')"></INPUT>
        </TD><TD>
        <INPUT type="button" value="Reschedule File" onclick="javascript:recodb_reschedule_file('%s','TODO')"></INPUT>
        </TD><TD>
        <INPUT type="button" value="Set to Error" onclick="javascript:recodb_setstate_file('%s','ERROR')"></INPUT>
        </TD><TD>
        <INPUT type="button" value="Show files" onclick="javascript:recodb_show_files()"></INPUT>
        </TD><TD>
        <INPUT type="button" value="Show configs" onclick="javascript:recodb_show_configs()"></INPUT>
        </TD></TR></TABLE>
        """%(fid,fid,fid,)
    else:
      buf = buf + '<H1>No file found with this FID!</H1>'
    return buf

  #===============================================================================
  def showRecoDbSummary(self, url, handler, req_path, req, args, data):
    buf = '<H2>Reconstruction Activity Summary</H2>'
    stmt= """\
    SELECT   m.fileid as fid,
             NVL(f.raw_file,'---'),
             NVL(m.slice,'---') as slice,
             NVL(m.prev_state,'Unknown'),
             NVL(m.now_state,'Unknown'),
             NVL(m.updated_time,0.0)
    FROM     reco_main m, reco_files f
    WHERE    m.fileid=f.fileid
    ORDER BY slice, m.fileid ASC
    """
    cur = DbCore.Cursor(self.recodb)
    res = cur.select(stmt)
    if res[1]:
      res = cur.next()[1]
      evts={}
      states = {}
      while res is not None:
        key = str([res[2],res[3],res[4]])
        curr = str(res[4])
        if not states.has_key(key): states[key]=[0,str(res[2]),str(res[3]),curr]
        if not evts.has_key(curr): evts[curr] = 0
        states[key][0] = states[key][0]+1
        evts[curr] = evts[curr] + 1
        res = cur.next()[1]

      b2 = """<TABLE class="RecoTable">
      <TR><TH>Slice</TH><TH>Previous State</TH><TH>Current State</TH><TH>Number of files</TH><TH></TH></TR>
      """
      sl = ''
      count = 0;
      for k in sorted(states):
        v=states[k]
        count = count + v[0]
        if sl != v[1]:
          if len(sl):
            b2 = b2 + '<TR><TD colspan="3"></TD><TD>Total:'+str(count)+'</TD>' + \
                 '<TD><INPUT style="{width:50%}" type=\"button\" value=\"Show\" onclick=\"javascript:recodb_show_files_selection(\''+sl+'\',null)"></INPUT></TD></TR>'
          count = 0
          sl = v[1]
        reschedule = "<INPUT style=\"{width:50%}\" type=\"button\" value=\"Reschedule\" onclick=\"javascript:recodb_reschedule_files_selection('"+v[1]+"','"+v[2]+"','"+v[3]+"')\"></INPUT>"
        b2 = b2 + '<TR><TD>'+v[1]+'</TD><TD>'+v[2]+'</TD><TD>'+v[3]+'</TD><TD>'+str(v[0])+'</TD>' + \
             '<TD><INPUT style="{width:50%}" type="button" value="Show" onclick="javascript:recodb_show_files_selection(\''+v[1]+'\',\''+v[3]+'\')"></INPUT>' + \
             reschedule + \
             '</TD></TR>'
      if count > 0:
        b2 = b2 + '<TR><TD colspan="3"></TD><TD>Total:'+str(count)+'</TD>' + \
             '<TD><INPUT style="{width:50%}" type=\"button\" value=\"Show\" onclick="javascript:recodb_show_files_selection(\''+sl+'\',null)"></INPUT></TD></TR>'
      b2 = b2 + "</TABLE><BR>"

      b3 = '<TABLE class="RecoTable"><TR><TH>Current State</TH><TH>Number of files</TH><TH></TH></TR>'
      for k in sorted(evts):
        b3 = b3 + "<TR><TD>"+k+"</TD><TD>"+str(evts[k])+"</TD>" + \
                  "<TD><INPUT type=\"button\" value=\"Show\" onclick=\"javascript:recodb_show_files_selection(null,'"+k+"')\"></INPUT></TD></TR>"
      b3 = b3 + "</TABLE><BR>"

      buf = buf + b3 + b2 + """
        </TABLE><BR></BR>
        <TABLE class="RecoActions">
        <TR><TD>
        <INPUT type="button" value="Back" onclick="javascript:goto_history_back()"></INPUT>
        </TD><TD>
        <INPUT type="button" value="Show files" onclick="javascript:recodb_show_files()"></INPUT>
        </TD><TD>
        <INPUT type="button" value="Show recent runs" onclick="javascript:rundb_show_runs()"></INPUT>
        </TD><TD>
        <INPUT type="button" value="Show configs" onclick="javascript:recodb_show_configs()"></INPUT>
        </TD></TR></TABLE>
      """
    return buf

  #===============================================================================
  def showRecoDbFiles(self, url, handler, req_path, req, args, data):
    buf = '<H2>Reconstruction Database Dump</H2>'
    st = ''
    if args.has_key('-state'):
      st = ' AND m.now_state=\''+args['-state']+'\''
    slice = ''
    if args.has_key('-slice'):
      slice = ' AND m.slice=\''+args['-slice']+'\''

    stmt= """\
    SELECT   m.fileid as fid,
             NVL(f.raw_file,'---'),
             NVL(m.slice,'---') as slice,
             NVL(m.prev_state,'Unknown'),
             NVL(m.now_state,'Unknown'),
             NVL(m.updated_time,0.0)
    FROM     reco_main m, reco_files f
    WHERE    m.fileid=f.fileid %s %s
    ORDER BY slice, m.fileid ASC
    """%(st,slice)
    cur = DbCore.Cursor(self.recodb)
    res = cur.select(stmt)
    if res[1]:
      b1 = """<H3>File list</H3>
      <TABLE class="RecoTable">
      <TR><TH>FID</TH><TH colspan="4">File name</TH></TR>
      <TR><TH></TH><TH>Slice</TH><TH>Previous State</TH><TH>Current State</TH><TH>Timestamp</TH></TR>
      """
      res = cur.next()[1]
      states = {}
      evts={}
      while res is not None:
        b1 = b1 + """
        <TR><TD><A href="/ReprocesssingManager?-recodb_show_file=%d">%d</A></TD>
        <TD colspan="4"><A href="/ReprocesssingManager?-recodb_show_file=%d">%s</A></TD></TR>
        <TR><TD></TD><TD>%s</TD><TD>%s</TD><TD>%s</TD><TD>%s</TD></TR>
        """%(res[0],res[0],res[0],res[1],res[2],res[3],res[4],datetime.datetime.fromtimestamp(res[5]).strftime('%Y-%m-%d %H:%M:%S'),)
        curr = str(res[4])
        key = str([res[2],res[3],res[4]])
        if not states.has_key(key): states[key]=[0,str(res[2]),str(res[3]),curr]
        if not evts.has_key(curr): evts[curr] = 0
        states[key][0] = states[key][0]+1
        evts[curr] = evts[curr] + 1
        res = cur.next()[1]
      b1 = b1 + "</TABLE>"

      b2 = """<TABLE class="RecoTable">
      <TR><TH>Slice</TH><TH>Previous State</TH><TH>Current State</TH><TH>Number of files</TH><TH></TH></TR>
      """
      sl = ''
      for k in sorted(states):
        v=states[k]
        if sl != v[1]:
          if len(sl): b2 = b2 + '<TR><TD colspan="5">&nbsp;</TD></TR>'
          sl = v[1]
        reschedule = "<INPUT style=\"{width:50%}\" type=\"button\" value=\"Reschedule\" onclick=\"javascript:recodb_reschedule_files_selection('"+v[1]+"','"+v[2]+"','"+v[3]+"')\"></INPUT>"
        b2 = b2 + "<TR><TD>"+v[1]+"</TD><TD>"+v[2]+"</TD><TD>"+v[3]+"</TD><TD>"+str(v[0])+"</TD><TD>" + \
             "<INPUT style=\"{width:50%}\" type=\"button\" value=\"Show\" onclick=\"javascript:recodb_show_files_selection('"+v[1]+"','"+v[3]+"')\"></INPUT>" + \
             reschedule + \
             "</TD></TR>"
      b2 = b2 + "</TABLE><BR>"

      b3 = '<TABLE class="RecoTable"><TR><TH>Current State</TH><TH>Number of files</TH><TH></TH></TR>'
      for k in sorted(evts):
        b3 = b3 + "<TR><TD>"+k+"</TD><TD>"+str(evts[k])+"</TD>" + \
                  "<TD><INPUT style=\"{width:50%}\" type=\"button\" value=\"Show\" onclick=\"javascript:recodb_show_files_selection(null,null,'"+k+"')\"></INPUT></TD></TR>"
      b3 = b3 + "</TABLE><BR>"

      cmds = """
        </TABLE><BR></BR>
        <TABLE class="RecoActions">
        <TR><TD>
        <INPUT type="button" value="Back" onclick="javascript:goto_history_back()"></INPUT>
        </TD><TD>
        <INPUT type="button" value="Show files" onclick="javascript:recodb_show_files()"></INPUT>
        </TD><TD>
        <INPUT type="button" value="Show recent runs" onclick="javascript:rundb_show_runs()"></INPUT>
        </TD><TD>
        <INPUT type="button" value="Show configs" onclick="javascript:recodb_show_configs()"></INPUT>
        </TD><TD>
        <INPUT type="button" value="Clean reco DB" onclick="javascript:recodb_cleandb()"></INPUT>
        </TD></TR></TABLE>
      """
        
      buf = buf +  """<H3>Summary</H3>"""+ b3 + b2 + b1 + cmds
    return buf

  #===============================================================================
  def showRecoDbConfigurations(self, url, handler, req_path, req, args, data):
    buf = '<H2>Known configurations:</H2>'
    srv = handler.server
    stmt= """\
    SELECT   configname, applicationname, applicationversion, extrapackages, dddb, conddb, optionfiles
    FROM     reco_configuration
    ORDER BY configname DESC
    """
    cur = DbCore.Cursor(self.recodb)
    res = cur.select(stmt)
    if res[1]:
      tab = DbTable(cur)
      tab.headers = ['Name', 'Application', 'Version','Extra','DDDB','CondDB','Options']
      buf = buf + '<TABLE class="RecoTable">'+tab.heading()
      res = cur.next()[1]
      while res is not None:
        buf = buf + tab.line(res)+'</TR>'
        res = cur.next()[1]
      buf = buf + '</TABLE>'

    buf = buf + '<H2>Slice Status:</H2>'
    stmt= """\
    SELECT   slice, NVL(partitionname,''), NVL(configname,''), NVL(status,''), NVL(nsubfarms,0)
    FROM     reco_slices
    ORDER BY slice DESC
    """
    cur = DbCore.Cursor(self.recodb)
    res = cur.select(stmt)
    if res[1]:
      tab = DbTable(cur)
      tab.headers = ['Slice Name', 'Partition', 'Application', 'Status','No.of Subfarms']
      buf = buf + '<TABLE class="RecoTable">'+tab.heading()
      res = cur.next()[1]
      while res is not None:
        buf = buf + tab.line(res)+'</TR>'
        res = cur.next()[1]
      buf = buf + '</TABLE>'

    return buf

  #===============================================================================
  def showRecoDbManagers(self, url, handler, req_path, req, args, data):
    buf = '<H2>Processing Manager Setup:</H2>'
    srv = handler.server
    stmt= """\
    SELECT   pid, name, start_time, src_file
    FROM     system_managers
    ORDER BY name DESC
    """
    cur = DbCore.Cursor(self.recodb)
    res = cur.select(stmt)
    if res[1]:
      tab = DbTable(cur)
      tab.headers = ['PID','Name', 'Start time','File']
      buf = buf + '<TABLE class="RecoTable">'+tab.heading()
      res = cur.next()[1]
      while res is not None:
        pid = str(res[0])
        buf = buf + '<TR><TD>'+pid+'</TD><TD>'+str(res[1])+'</TD><TD>'+\
              datetime.datetime.fromtimestamp(res[2]).strftime('%Y-%m-%d %H:%M:%S')+'</TD><TD>'+\
              '<INPUT type="button" style="{width:30%}" value="Start" onclick="javascript:recodb_manip_manager('+pid+',\''+res[1]+'\',\'start\')"></INPUT>'+\
              '<INPUT type="button" style="{width:30%}" value="Stop"  onclick="javascript:recodb_manip_manager('+pid+',\''+res[1]+'\',\'stop\')"></INPUT>'+\
              '<INPUT type="button" style="{width:30%}" value="Kill"  onclick="javascript:recodb_manip_manager('+pid+',\''+res[1]+'\',\'kill\')"></INPUT>'+\
              '</TD></TR><TR><TD style="{text-align:right}" colspan="4">'+str(res[3])+'</TD></TR><TR><TD colspan="4">&nbsp;</TD></TR>'
        res = cur.next()[1]
      buf = buf + '</TABLE>'
    return buf
  
  #===============================================================================
  def showRecoDbManipManager(self, url, handler, req_path, req, args, data):
    buf    = '<H2>Manipulating Reprocessing manager</H2>'
    pid    = args['-pid']
    name   = args['-name']
    action = args['-action']
    buf    = buf + 'Set manager %s [PID:%s] to %s'%(name,pid,action,)
    return buf + self.showRecoDbManagers( url, handler, req_path, req, args, data)

  #===============================================================================
  def showMain(self, url, handler, req_path, req, args, data):
    buff = """
    <META HTTP-EQUIV="Content-Type" CONTENT="text/html; CHARSET=iso-8859-1">
    <HTML>
    <TITLE>LHCb Online status displays</TITLE>
    <HEAD id="html_head_tag">
    <META HTTP-EQUIV="Pragma"  CONTENT="no-cache"></META>
    <META HTTP-EQUIV="Expires" CONTENT="0"></META>
    </HEAD>
    <FRAMESET OnUnload="WarnOnLogOff()" framespacing="1" cols="150,*">
    <FRAME name="navigation" src="/ReprocesssingManager?-left_pane=1" marginheight="0" marginwidth="0" scrolling="auto" border="1"/>
    <FRAME name="viewer" src="/ReprocesssingManager?-right_pane=1" scrolling="auto"/>
    <NOFRAMES>
    <BODY>
    <P>This page uses frames, but your browser doesn't support them.</P>
    <P>Sorry, you cannot access this page.</P>
    </BODY>
    </NOFRAMES>
    </FRAMESET>     
    </HTML>
    """
    return buff

  #===============================================================================
  def showNavbar(self, url, handler, req_path, req, args, home):
    srv = handler.server
    nam = self.name()
    uri = 'http://'+srv.nodeName()+':'+str(srv.port)+'/'+nam
    buff = self.pageHeader(uri,home,False) + """
    <H2>Action<BR>Master</H2>
    <BR><BR>
    <TABLE class="RecoNavbar">
    <TR><TH>Run Database:</TH></TR>
    <TR><TD><INPUT style="{width:100%;text-align:left}" type="button" value="Show Fills"          onclick="javascript:rundb_show_fills()"></INPUT></TD></TR>
    <TR><TD><INPUT style="{width:100%;text-align:left}" type="button" value="Show Runs"           onclick="javascript:rundb_show_runs()"></INPUT></TD></TR>
    <TR><TH>Reprocessing:</TH></TR>
    <TR><TD><INPUT style="{width:100%;text-align:left}" type="button" value="Show Configs"        onclick="javascript:recodb_show_configs()"></INPUT>
    <TR><TD><INPUT style="{width:100%;text-align:left}" type="button" value="Show Summary"        onclick="javascript:recodb_show_summary()"></INPUT></TD></TR>
    <TR><TD><INPUT style="{width:100%;text-align:left}" type="button" value="Show Files"          onclick="javascript:recodb_show_files()"></INPUT></TD></TR>
    <TR><TH>For experts:</TH></TR>
    <TR><TD><INPUT style="{width:100%;text-align:left}" type="button" value="Clean Reco DB"       onclick="javascript:recodb_cleandb()"></INPUT></TD></TR>
    <TR><TD><INPUT style="{width:100%;text-align:left}" type="button" value="Reco Managers"       onclick="javascript:recodb_show_managers()"></INPUT></TD></TR>
    </TABLE>
    """
    return buff

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
    if ( handler.mount == self.name() ):
      home_page = '/ReprocesssingManager'
      if ( args.has_key('-main')):
        buf = self.showMain(req_path, handler, req, args, data, home_page)
        return self.makeWebFile((req_path, self.name(), 'text/html', len(buf), buf))
      elif ( args.has_key('-left_pane')):
        buf = self.showNavbar(req_path, handler, req, args, data, home_page)
        return self.makeWebFile((req_path, self.name(), 'text/html', len(buf), buf))
      elif ( args.has_key('-recodb_show_status')):
        self.pageContent = self.showReprocessing
      elif ( args.has_key('-recodb_show_configs')):
        home_page = '/ReprocesssingManager?-recodb_show_status=1'
        self.pageContent = self.showRecoDbConfigurations
      elif ( args.has_key('-recodb_show_managers')):
        home_page = '/ReprocesssingManager?-recodb_show_status=1'
        self.pageContent = self.showRecoDbManagers
      elif ( args.has_key('-recodb_manip_manager')):
        home_page = '/ReprocesssingManager?-recodb_show_status=1'
        self.pageContent = self.showRecoDbManipManager
      elif ( args.has_key('-recodb_show_files')):
        home_page = '/ReprocesssingManager?-recodb_show_status=1'
        self.pageContent = self.showRecoDbFiles
      elif ( args.has_key('-recodb_show_summary')):
        home_page = '/ReprocesssingManager?-recodb_show_status=1'
        self.pageContent = self.showRecoDbSummary
      elif ( args.has_key('-recodb_show_file')):
        home_page = '/ReprocesssingManager?-recodb_show_status=1'
        self.pageContent = self.showRecoDbFile
      elif ( args.has_key('-recodb_clean')):
        home_page = '/ReprocesssingManager?-recodb_show_status=1'
        self.pageContent = self.showRecoDbClean
      elif ( args.has_key('-recodb_remove_file')):
        home_page = '/ReprocesssingManager?-recodb_show_status=1'
        self.pageContent = self.showRecoDbRemoveFile
      elif ( args.has_key('-recodb_reschedule_file')):
        home_page = '/ReprocesssingManager?-recodb_show_status=1'
        self.pageContent = self.showRecoDbRescheduleFile
      elif ( args.has_key('-recodb_reschedule_file_selection')):
        home_page = '/ReprocesssingManager?-recodb_show_status=1'
        self.pageContent = self.showRecoDbRescheduleFileSelection
      elif ( args.has_key('-recodb_set_filestate')):
        home_page = '/ReprocesssingManager?-recodb_show_status=1'
        self.pageContent = self.showRecoDbSetFileState
      elif ( args.has_key('-rundb_recent_fills')):
        self.pageContent = self.showRunDbFills
      elif ( args.has_key('-rundb_recent_runs')):
        self.pageContent = self.showRunDbRuns
      elif ( args.has_key('-rundb_show_run')):
        home_page = '/ReprocesssingManager?-rundb_recent_runs=1'
        self.pageContent = self.showRunDbRun
      elif ( args.has_key('-rundb_show_file')):
        home_page = '/ReprocesssingManager?-rundb_recent_runs=1'
        self.pageContent = self.showRunDbFile
      elif ( args.has_key('-rundb_submit_run')):
        home_page = '/ReprocesssingManager?-rundb_recent_runs=1'
        self.pageContent = self.showRunDbSubmitRun
      elif ( args.has_key('-rundb_submit_file')):
        home_page = '/ReprocesssingManager?-rundb_recent_runs=1'
        self.pageContent = self.showRunDbSubmitFile
      else:
        self.pageContent = self.defaultPageContent
      return self.pageBody(handler,req_path, req, args, data, home_page)
    return gaudiweb.S_ERROR()



# =================================================================
#
# class DataManagementServices
#
# @author   M.Frank
# @version  1.0
# @date     30/06/2002
# =================================================================
class DataManagementServices:

  # ===============================================================
  def __init__(self, port, users):
    self.port = port
    self.users = users
    self.server = None
    self.fileServer = 0
    gaudiweb.logPrefix = str(port)
    self.startServer()

  # ===============================================================
  def startServer(self):
    if ( self.server is None):
      self.server = gaudiweb.DataManagementServer(self.port, None, 0)
      #
      # Default management servlet
      #
      manager_servlet = gaudiweb.ManagementServlet('Manager')
      manager_servlet.addUsers(self.users)
      self.server.registerServlet('Manager', manager_servlet)
      #
      # ServerConfig servlet
      #
      ##cfg_servlet = ServerConfig('ServerConfig', self)
      ##cfg_servlet.addUsers(self.users)
      ##self.server.registerServlet('ServerConfig', cfg_servlet)
      #
      # ReprocesssingManager servlet
      #
      servlet = ReprocesssingManager('ReprocesssingManager', self.port, self)
      servlet.addUsers(self.users)
      self.server.registerServlet('ReprocesssingManager', servlet)
    return self.server

  # ===============================================================
  def run(self):
    #import xmlrpclib
    self.server.welcome()
    #gaudiweb.log('===     running with SLOW XML parser (Expat error)')
    #xmlrpclib.ExpatParser = None
    self.server.serve_forever()
    sys.exit(0)

  # ===============================================================
  def startMountpoints(self):
    if ( 0 == self.fileServer):
      self.fileServer = 1
      server = self.startServer()
      server.registerMountpoint('html',   basePath+os.sep+'html')
      server.registerMountpoint('style',   '../style')
      server.registerMountpoint('images', basePath+os.sep+'images')
      server.servlets['html'].useCache(1)
      server.servlets['images'].useCache(1)
      favicon_servlet = gaudiweb.FavIconServer(basePath+os.sep+'images'+os.sep+'favicon.ico')
      self.server.registerServlet('favicon.ico', favicon_servlet)
    return self

  # ===============================================================
  def startAll(self):
    server = self.startServer()
    self.startMountpoints()
    return self

# =================================================================
def startAll(port=servicePort, users=default_users):
  DataManagementServices(port, users).startAll().run()

startAll()
