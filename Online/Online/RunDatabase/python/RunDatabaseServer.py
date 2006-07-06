import os, sys, math, time, string, xmlrpclib, tempfile
import gaudiweb, DbCore, RunDatabase
from RunDatabaseItems import *

DbCore._ms_access = 1
db_time = RunDatabase.db_time
def _dbError(s):
  print 'Dbatabase access error:',s

def errstr(*parm):
  s = '<BR></BR><BR></BR><B>'
  for p in parm:
    s = s + str(p)
  return s+'</B>'
  
class RunTable:
  def __init__(self):
    pass
  # RunNumber,FillNumber,Partition,Activity,StartDate,EndDate,ProgramName,ProgramVersion,IntegratedLumi
  def row(self, r):
    ref = '/RunDbSummary?-show=fill&-fill='+str(r[RUN_FILLNUMBER])
    tmp =       '<TR><TD align="left"><A href="'+ref+'" target="">'+str(r[RUN_FILLNUMBER])+'</A></TD>\n'
    ref = '/RunDbSummary?-show=run&-run='+str(r[RUN_RUNNUMBER])
    tmp = tmp + '<TD align="left"><A href="'+ref+'" target="">'+str(r[RUN_RUNNUMBER])+'</A></TD>\
                <TD align="left">'+str(r[RUN_PARTITION])+'</TD> \
                <TD align="left">'+str(r[RUN_ACTIVITY])+'</TD> \
                <TD align="left">'+db_time(r[RUN_STARTDATE])+'</TD> \
                <TD align="left">'+db_time(r[RUN_ENDDATE])+'</TD> \
                <TD align="left">'+str(r[RUN_PROGRAMNAME])+'</TD> \
                <TD align="left">'+str(r[RUN_PROGRAMVERSION])+'</TD> \
                <TD align="left">'+str(r[RUN_INTEGRATEDLUMI])+'</TD></TR>\n'
    return tmp;
  def build(self, recordset):
    tmp = """               
          <TABLE border="1" cellpadding="5" cellspacing="0">                    \n
          <TR><TD align="left" bgcolor="#9DFF9D"><B>Fill</B></TD>               \n
              <TD align="left" bgcolor="#9DFF9D"><B>Run</B></TD>                \n
              <TD align="left" bgcolor="#9DFF9D"><B>Partition</B></TD>          \n
              <TD align="left" bgcolor="#9DFF9D"><B>Activity</B></TD>           \n
              <TD align="left" bgcolor="#9DFF9D"><B>Start date <sup>(1)</sup></B></TD> \n
              <TD align="left" bgcolor="#9DFF9D"><B>End date <sup>(1)</sup></B></TD>   \n
              <TD align="left" bgcolor="#9DFF9D"><B>Program</B></TD>            \n
              <TD align="left" bgcolor="#9DFF9D"><B>dto.Version</B></TD>        \n
              <TD align="left" bgcolor="#9DFF9D"><B>Integrated Lumi</B></TD>    \n
          </TR>
          """
    for r in recordset:
      tmp = tmp + self.row(r)
    tmp = tmp + """</TABLE>

            <sup>(1)</sup> Date format: yyyy-mm-dd H24:MM:SS
            """
    return tmp
  
class ParametersTable:
  def __init__(self):
    pass
  def row(self, r):
    tmp =       '<TR><TD align="left">&nbsp;&nbsp;&nbsp;'+str(r[1])+'&nbsp;&nbsp;</TD>\n'
    tmp = tmp + '<TD align="left">&nbsp;&nbsp;'+str(r[3])+'&nbsp;&nbsp;</TD>\n'
    tmp = tmp + '<TD align="left">&nbsp;&nbsp;'+str(r[2])+'&nbsp;&nbsp;</TD></TR>\n'
    return tmp;
  def build(self, recordset,header=1, width=None, border="1", cellpadding="5", cellspacing="0"):
    tmp = '<TABLE '
    if ( border ):
      tmp = tmp + ' border="'+str(border)+'"'
    else:
      tmp = tmp + ' border="0"'
    if ( cellspacing ):
      tmp = tmp + ' cellspacing="'+str(cellspacing)+'"'
    else:
      tmp = tmp + ' cellspacing="0"'
    if ( cellpadding ):
      tmp = tmp + ' cellpadding="'+str(cellpadding)+'"'
    else:
      tmp = tmp + ' cellpadding="0"'
    if ( width ):
      tmp = tmp + ' width="'+width+'">'
    else:
      tmp = tmp + '>'
    if ( header ):
      tmp = tmp + """
          <TR><TD align="left" bgcolor="#9DFF9D"><B>Name</B></TD>\n
              <TD align="left" bgcolor="#9DFF9D"><B>Type</B></TD>\n
              <TD align="left" bgcolor="#9DFF9D"><B>Value</B></TD>\n
          </TR>\n
          """
    for r in recordset:
      tmp = tmp + self.row(r)
    tmp = tmp + "</TABLE>\n"
    return tmp

class FilesTable:
  def __init__(self):
    pass
  # ID, RunNumber, FileName, FileStatus, StartDate, EndDate, Stream, MD5Sum, LogicalName, LogicalStatus, EventStat, FileSize
  def row(self, svc, r):
    run = r[1]
    tmp = '<TR><TD align="left" bgcolor="#FFFF00"><B>'+str(r[FILE_FILEID])+'&nbsp;:&nbsp;'+str(r[FILE_NAME])+'</B></TD> \
           <TD align="left" bgcolor="#FFDD00"><B>'+str(r[FILE_STATUS])+'</B></TD> \
           <TD align="left" bgcolor="#FFDD00"><B>'+str(r[FILE_STREAM])+'</B></TD>'
    tmp = tmp + '<TD align="left" colspan="3" rowspan="3" bgcolor="#FFDD00">'
    pars = svc.fileParams(FileID=('=',r[FILE_FILEID]))
    if ( pars[0] != RunDatabase.SUCCESS ):
      tmp = tmp + errstr('Failed to retrieve fileparameters for run:',run,' File:',r[FILE_NAME],' Err=',pars[1]+'\n')
      _dbError(str(pars[1]))
    else:
      tmp = tmp + ParametersTable().build(pars[1],header=None,width="100%",border=None,cellpadding=None)
    tmp = tmp + '</TD></TR>                                     \
       <TR><TD align="left">'+str(r[FILE_LOGNAME])+'</TD>                  \
           <TD align="left">'+str(r[FILE_LOGSTATUS])+'</TD>                  \
           <TD align="left">'+str(r[FILE_EVENTSTAT])+'&nbsp; / &nbsp;'+str(r[FILE_SIZE])+'</TD></TR> \
       <TR><TD align="left">'+db_time(r[FILE_STARTDATE])+'<BR>'+db_time(r[FILE_ENDDATE])+'</TD> \
           <TD align="left" colspan="2">'+str(r[FILE_MD5])+'</TD></TR> '
    return tmp;
  def build(self, svc, recordset):
    tmp = """               
        <TABLE border="1" cellpadding="5" cellspacing="0">
        <TR><TD align="left" bgcolor="#FDFF9D"><B>ID/Name</B></TD>
            <TD align="left" bgcolor="#FDFF9D"><B>Status</B></TD>
            <TD align="left" bgcolor="#FDFF9D"><B>Stream</B></TD>
            <TD align="left" bgcolor="#FDFF9D" colspan="3" rowspan="3"><B>Parameters</B></TD></TR>
        <TR><TD align="left" bgcolor="#9DFF9D"><B>LogicalName</B></TD>
            <TD align="left" bgcolor="#9DFF9D"><B>Status</B></TD>
            <TD align="left" bgcolor="#9DFF9D"><B>Events/Size</B></TD>
        <TR><TD align="left" bgcolor="#9DFF9D"><B>Start/End Date<sup>(1)</sup></B></TD>
            <TD align="left" bgcolor="#9DFF9D" colspan="2"><B>MD5</B></TD></TR>
        </TR>
        """
    for r in recordset:
      tmp = tmp + self.row(svc, r)
    tmp = tmp + "</TABLE>            <sup>(1)</sup> Date format: yyyy-mm-dd H24:MM:SS"
    return tmp

#=================================================================================
class Service(gaudiweb.Service, RunDatabase.RunDatabase):
  """ Run database servlet class
      Exports the functionality of the run database python class as a
      python XML-RPC server
      
      @author M.Frank
  """
  #===============================================================================
  def __init__(self, login='RunDatabase', name='RunDatabase'):
    """ Constructor
      
        @author M.Frank
    """
    RunDatabase.RunDatabase.__init__(self, login)
    gaudiweb.Service.__init__(self, name)
  
#=================================================================================
class Servlet(gaudiweb.FileServlet):
  """ Run database servlet class
      WWW interface for LHCb online run database
      
      @author M.Frank
  """
  #===============================================================================
  def __init__(self, name, mount, service):
    """ Constructor
      
        @author M.Frank
    """
    self.service = service
    gaudiweb.FileServlet.__init__(self,name,mount)

  #===============================================================================
  def pageTrailer(self):
    return '<script lang="javascript">make_featured_by(\'Online\');make_footer();</script>'

  #===============================================================================
  def navigationInfo(self):
    buf = open('../html/TreeHeader.htm','r').read() + \
          open('../html/FoldersTree.js','r').read() + """
    function initTreeDoc()    {
      foldersTree = new Folder('<div class=headline><b>Summary</b></div>','/RunDbSummary?-show=News');
      foldersTree.addChild(textLnk('News','/RunDbSummary?-show=News'));
      foldersTree.addChild(textLnk('Select by fill','/RunDbSummary?-show=selectFill'));
      foldersTree.addChild(textLnk('Select by year','/RunDbSummary?-show=selectYear'));
      initializeDocument(foldersTree);
    }
    initTreeDoc();
    </script>
    </TD>
    </TR>
    </table>
    </body>
    </html>
    """
    return buf
    
  #===============================================================================
  def selectByFillInfo(self, restriction=None):
    b = """<H2>LHCb Run Database: Access runs by fill</H2>
            <TABLE border="1" cellpadding="5" cellspacing="0">
            <TR><TD align="left" bgcolor="#9DFF9D"><B>Select fill</B></TD>
                <TD align="left" bgcolor="#9DFF9D"><B>Number of runs</B></TD>
                <TD align="left" bgcolor="#9DFF9D"><B>First run</B></TD>
                <TD align="left" bgcolor="#9DFF9D"><B>Last run</B></TD>
                <TD align="left" bgcolor="#9DFF9D"><B>Start date<sup>(1)</sup></B></TD>
                <TD align="left" bgcolor="#9DFF9D"><B>End date<sup>(1)</sup></B></TD>
            </TR>
        """
    tmp = ''
    stmt = """SELECT COUNT(*), 
                     FillNumber, 
                     MIN(RunNumber), 
                     MAX(RunNumber),
                     MIN(StartDate),
                     MAX(EndDate) 
              FROM RunDbRuns """
    if ( restriction ):
      stmt = stmt + restriction
    stmt = stmt + """
              GROUP BY FillNumber
              ORDER BY MIN(StartDate)"""
    cur = DbCore.Cursor(self.service.db()).select(stmt)
    cnt = 0
    result = []
    while ( cur.next().isSuccess() ):
      cnt = cnt + 1
      r = cur.result();
      fill = str(r[RUN_FILLNUMBER])
      ref = '/RunDbSummary?-show=fill&-fill='+fill
      tmp = tmp + '<TR><TD align="left"><A href="'+ref+'" target="">'+fill+'</A></TD>'+\
                  '<TD align="left">'+str(r[0])+'</TD>'+\
                  '<TD align="left">'+str(r[2])+'</TD>'+\
                  '<TD align="left">'+str(r[3])+'</TD>'+\
                  '<TD align="left">'+db_time(r[4])+'</TD>'+\
                  '<TD align="left">'+db_time(r[5])+'</TD></TR>'
    b = b + tmp
    b = b + """</TABLE>

            <sup>(1)</sup> Date format: yyyy-mm-dd H24:MM:SS
            """
    if ( cnt == 0 ):
      b = b + errstr('[No fills found] for condition:',restriction)
    return b

  #===============================================================================
  def selectByYearInfo(self):
    b = """<H2>LHCb Run Database: Access runs by year</H2>
            <TABLE border="1" cellpadding="5" cellspacing="0">
            <TR><TD align="left" bgcolor="#9DFF9D"><B>Year</B></TD>
                <TD align="left" bgcolor="#9DFF9D">&nbsp;</TD>
                <TD align="left" bgcolor="#9DFF9D">&nbsp;</TD>
                <TD align="left" bgcolor="#9DFF9D"><B>Number of runs</B></TD>
                <TD align="left" bgcolor="#9DFF9D"><B>First run</B></TD>
                <TD align="left" bgcolor="#9DFF9D"><B>Last run</B></TD>
            </TR>
        """
    tmp = ''
    stmt = "SELECT COUNT(*), SUBSTRING(StartDate,1,4), MIN(RunNumber), MAX(RunNumber) FROM RunDbruns GROUP BY SUBSTRING(StartDate,1,4)"
    cur = DbCore.Cursor(self.service.db()).select(stmt)
    result = []
    while ( cur.next().isSuccess() ):
      r = cur.result();
      year = str(r[1])
      ref = '/RunDbSummary?-show=year&-year='+year
      tmp = tmp + '<TR><TD align="left">'+year+'</TD>'+\
                  '<TD align="left"><A href="'+ref+'&-fillsonly=YES" target="">Show fills</A></TD>'+\
                  '<TD align="left"><A href="'+ref+'" target="">Show runs</A><sup>(1)</sup></TD>'+\
                  '<TD align="left">'+str(r[0])+'</TD>'+\
                  '<TD align="left">'+str(r[2])+'</TD>'+\
                  '<TD align="left">'+str(r[3])+'</TD></TR>'
    b = b + tmp
    b = b + """</TABLE>
    
            <sup>(1)</sup> This operation may require some time.
            """
    return b

  #===============================================================================
  def yearInfo(self, year):    
    res = self.service.runs(StartDate=("LIKE","'"+year+"%'",))
    if ( res[0] == RunDatabase.SUCCESS ):
      if ( len(res[1]) > 0 ):
        return "<H2>LHCb Run Database: Year "+year+"</H2>" + RunTable().build(res[1])
      err = errstr('[No runs found] for year:',year)
      return "<H2>LHCb Run Database: Year "+year+"</H2>"+err
    _dbError(str(res[1]))
    return "<H2>LHCb Run Database: Year "+year+"</H2>" + str(res[1])

  #===============================================================================
  def fillInfo(self, fill_no):    
    res = self.service.runs(FillNumber=int(fill_no))
    if ( res[0] == RunDatabase.SUCCESS ):
      if ( len(res[1]) > 0 ):
        return "<H2>LHCb Run Database: Fill "+str(fill_no)+"</H2>" + RunTable().build(res[1])
      err = errstr('[No runs found] for fill:',fill_no)
      return "<H2>LHCb Run Database: Fill "+str(fill_no)+"</H2>"+err
    _dbError(str(res[1]))
    return "<H2>LHCb Run Database: Fill "+str(fill_no)+"</H2>" + str(res[1])

  #===============================================================================
  def runInfo(self, run_no):
    svc = self.service
    b = "<H2>LHCb Run Database: Run "+str(run_no)+"</H2>"
    res = svc.runs(RunNumber=run_no)
    if ( res[0] == RunDatabase.SUCCESS ):
      b = b + RunTable().build(res[1])
    else:
      _dbError(str(res[1]))
      return "<H2>LHCb Run Database: Run "+str(run_no)+"</H2>" + errstr(pars[1])
    # add run parameters
    pars = svc.runParams(run_no)
    if ( pars[0] == RunDatabase.SUCCESS ):
      b = b + '<H3>Run Parameters:</H3>' + ParametersTable().build(pars[1])
    else:
      _dbError(str(pars[1]))
      return "<H2>LHCb Run Database: Run "+str(run_no)+"</H2>" + errstr(pars[1])
    # add files
    files = svc.files(RunNumber=run_no)
    if ( files[0] != RunDatabase.SUCCESS ):
      b = b + errstr('Failed to retrieve files for run:',RunNumber,' Err=',files[1],'\n')
      _dbError(str(files[1]))
    elif ( len(files[1])> 0 ):
      b = b + '<H3>Output files:</H3>' + FilesTable().build(svc, files[1])
    else:
      b = b + '<H3>No output files known.</H3>'
    return b;

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
    import string
    if ( len(args) == 0 ):
      return self.makeFile('../html/RunDbSummary.htm', mime_type='text/html')

    directory = self.mount()
    address = '"http://'+handler.server.nodeName()+':'+str(handler.server.port)+'/'
    buf = """<html><head><title>LHCb run database</title>
               <META content="text/html; charset=iso-8859-1" http-equiv=Content-Type>
               <BASE href="""+address+'"'+"""/>
               <style type="text/css">
                 .OVL  { text-decoration: overline; }
                 .FEW  { text-decoration: none;     }
                 .SOME { text-decoration: none;     }
                 .MANY { text-decoration: none;     }
                 .VERYMANY { text-decoration: none; }
               </style>
               <link rel="stylesheet" href="/html/DataMgmt.css"/>
               <script lang="javascript" src="/html/DataMgmt.js"></script>
               <script lang="javascript">make_header();make_toolbar('/RunDbSummary?-show=News','/RunDbSummary?-show=News#Help',null);</script>
               </head>
               <body>
            """
    if ( args.has_key('-show') ):
      val = args['-show']
      if ( val == 'Navigation' ):
        buf = self.navigationInfo()
        return self.makeWebFile((req_path, self.name(), 'text/html', len(buf), buf))
      elif val == 'News':
        buf = buf + open('../html/RunDbNews.htm','r').read()
      elif val == 'selectYear':
        buf = buf + self.selectByYearInfo()
      elif val == 'selectFill':
        buf = buf + self.selectByFillInfo()
      elif val == 'year':
        if args.has_key('-fillsonly'):
          buf = buf + self.selectByFillInfo("WHERE StartDate LIKE '"+args['-year']+"%'")
        else:
          buf = buf + self.yearInfo(args['-year'])
      elif val == 'fill':
        buf = buf + self.fillInfo(args['-fill'])
      elif val == 'run':
        run_no = int(args['-run'])
        buf = buf + self.runInfo(run_no)
    else:
      buf = buf + '<H1>Unknown request!!!</H1>'
    buf = buf + self.pageTrailer()
    buf = buf + '</body></html>'
    return self.makeWebFile((req_path, self.name(), 'text/html', len(buf), buf))
  
class RunDatabaseServer:

  def __init__(self, login, port=8081):
    """
        @author M.Frank
    """
    import os, xmlrpclib
    #xmlrpclib.ExpatParser = None
    #print '==========================================================='
    #print '===     running with SLOW XML parser (Expat error)     ===='
    #print '==========================================================='
    basePath = '..'
    if os.environ.has_key('DATAMGMTSVCROOT'):
      basePath = os.environ['DATAMGMTSVCROOT']
    if not os.environ.has_key('DBSERVER'):
      os.environ['DBSERVER'] = string.split(login,'/')[0]
    if os.environ.has_key('TMP'):
      directory = os.environ['TMP']+os.sep+'DataMgmtTables'
    elif os.environ.has_key('TEMP'):
      directory = os.environ['TEMP']+os.sep+'DataMgmtTables'
    else:
      directory = '..'+os.sep+'tmp'
    try:
      os.mkdir(directory)
    except:
      pass
    directory = directory+os.sep
    self.port = port
    self.users = [('administrator','admin'),('frankm','hallo'),('LHCb','CKM-best')]
    self.server = gaudiweb.DataManagementServer(port=self.port, name=None, lg=0)
    manager_servlet = gaudiweb.ManagementServlet('ServerConfig')
    manager_servlet.addUsers(self.users)
    self.server.registerServlet('ServerConfig', manager_servlet)

    service = Service(login, 'RunDatabase')
    self.server.registerService('RPC/RunDatabase', service)

    summary_servlet = Servlet('RunDbSummary',directory,service)
    summary_servlet.addUsers(self.users)
    self.server.registerServlet('RunDbSummary', summary_servlet)

    self.server.registerMountpoint('html',   basePath+os.sep+'html')
    self.server.registerMountpoint('images', basePath+os.sep+'images')
    self.server.servlets['html'].useCache(1)
    self.server.servlets['images'].useCache(1)
    favicon_servlet = gaudiweb.FavIconServer(basePath+os.sep+'images'+os.sep+'favicon.ico')
    self.server.registerServlet('favicon.ico', favicon_servlet)

  def run(self):
    """
        @author M.Frank
    """
    self.server.welcome()
    self.server.serve_forever()

if __name__ == "__main__":
  RunDatabaseServer('RunDatabase').run()
