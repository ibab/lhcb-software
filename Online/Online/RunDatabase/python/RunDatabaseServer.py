import string,xmlrpclib,gaudiweb,DbCore,RunDatabase

import os, sys, math, time, tempfile

DbCore._ms_access = 1

class RunTable:
  def __init__(self):
    pass
  def row(self, r):
    ref = '/RunDbSummary?-show=fill&-fill='+str(r[1])
    tmp =       '<TR><TD align="left"><A href="'+ref+'" target="">'+str(r[1])+'</A></TD>\n'
    ref = '/RunDbSummary?-show=run&-run='+str(r[0])
    tmp = tmp + '<TD align="left"><A href="'+ref+'" target="">'+str(r[0])+'</A></TD>\n'
    tmp = tmp + '<TD align="left">'+str(r[2])+'</TD>\n'
    tmp = tmp + '<TD align="left">'+str(r[3])+'</TD>\n'
    tmp = tmp + '<TD align="left">'+str(r[4])+'</TD>\n'
    tmp = tmp + '<TD align="left">'+str(r[5])+'</TD>\n'
    tmp = tmp + '<TD align="left">'+str(r[6])+'</TD>\n'
    tmp = tmp + '<TD align="left">'+str(r[7])+'</TD></TR>\n'
    return tmp;
  def build(self, recordset):
    tmp = """               
          <TABLE border="1" cellpadding="5" cellspacing="0">                    \n
          <TR><TD align="left" bgcolor="#9DFF9D"><B>Fill</B></TD>               \n
              <TD align="left" bgcolor="#9DFF9D"><B>Run</B></TD>                \n
              <TD align="left" bgcolor="#9DFF9D"><B>Partition</B></TD>          \n
              <TD align="left" bgcolor="#9DFF9D"><B>Start date <sup>(1)</sup></B></TD>         \n
              <TD align="left" bgcolor="#9DFF9D"><B>End date <sup>(1)</sup></B></TD>           \n
              <TD align="left" bgcolor="#9DFF9D"><B>Start luminosity</B></TD>   \n
              <TD align="left" bgcolor="#9DFF9D"><B>End luminosity</B></TD>     \n
              <TD align="left" bgcolor="#9DFF9D"><B>Energy</B></TD>             \n
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
  def row(self, svc, r):
    run = r[1]
    tmp =       '<TR><TD align="left">'+str(r[0])+'</TD>'
    tmp = tmp + '<TD align="left">'+str(r[2])+'</TD>'
    tmp = tmp + '<TD align="left">'+str(r[3])+'</TD><TD align="left">'
    pars = svc.fileParams(FileID=('=',r[0]))
    if ( pars[0] != RunDatabase.RunDb.SUCCESS ):
      tmp = tmp + 'Failed to retrieve fileparameters for run:'+str(run)+' File:'+r[2]+' Err='+str(pars[1])+'\n'
    else:
      tmp = tmp + ParametersTable().build(pars[1],header=None,width="100%",border=None,cellpadding=None)
    tmp = tmp + '</TD></TR>'
    return tmp;
  def build(self, svc, recordset):
    tmp = """               
        <TABLE border="1" cellpadding="5" cellspacing="0">
        <TR><TD align="left" bgcolor="#9DFF9D"><B>ID</B></TD>
            <TD align="left" bgcolor="#9DFF9D"><B>Name</B></TD>
            <TD align="left" bgcolor="#9DFF9D"><B>Stream</B></TD>
            <TD align="left" bgcolor="#9DFF9D"><B>Parameters</B></TD>
        </TR>
        """
    for r in recordset:
      tmp = tmp + self.row(svc, r)
    tmp = tmp + "</TABLE>"
    return tmp
  
""" Run database servlet class
    
    @author M.Frank
"""
class Servlet(gaudiweb.FileServlet):
  """
      WWW interface for LHCb online run database
      
      @author M.Frank
  """
  #===============================================================================
  def __init__(self, name, mount, service):
    import DbCore
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
    result = []
    while ( cur.next().isSuccess() ):
      r = cur.result();
      fill = str(r[1])
      ref = '/RunDbSummary?-show=fill&-fill='+fill
      tmp = tmp + '<TR><TD align="left"><A href="'+ref+'" target="">'+fill+'</A></TD>'+\
                  '<TD align="left">'+str(r[0])+'</TD>'+\
                  '<TD align="left">'+str(r[2])+'</TD>'+\
                  '<TD align="left">'+str(r[3])+'</TD>'+\
                  '<TD align="left">'+str(r[4])+'</TD>'+\
                  '<TD align="left">'+str(r[5])+'</TD></TR>'
    b = b + tmp
    b = b + """</TABLE>

            <sup>(1)</sup> Date format: yyyy-mm-dd H24:MM:SS
            """
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
    if ( res[0] == RunDatabase.RunDb.SUCCESS ):
      return "<H2>LHCb Run Database: Year "+year+"</H2>" + RunTable().build(res[1])
    return "<H2>LHCb Run Database: Year "+year+"</H2>" + str(res[1])

  #===============================================================================
  def fillInfo(self, fill_no):    
    res = self.service.runs(FillNumber=int(fill_no))
    if ( res[0] == RunDatabase.RunDb.SUCCESS ):
      return "<H2>LHCb Run Database: Fill "+str(fill_no)+"</H2>" + RunTable().build(res[1])
    return "<H2>LHCb Run Database: Fill "+str(fill_no)+"</H2>" + str(res[1])

  #===============================================================================
  def runInfo(self, run_no):
    svc = self.service
    b = "<H2>LHCb Run Database: Run "+str(run_no)+"</H2>"
    res = svc.runs(RunNumber=run_no)
    if ( res[0] == RunDatabase.RunDb.SUCCESS ):
      b = b + RunTable().build(res[1])
    else:
      b = b + str(res[1])
    # add run parameters
    pars = svc.runParams(run_no)
    if ( pars[0] == RunDatabase.RunDb.SUCCESS ):
      b = b + '<H3>Run Parameters:</H3>' + ParametersTable().build(pars[1])
    else:
      b = b + str(pars[1])
    # add files          
    files = svc.files(RunNumber=run_no)
    if ( files[0] != RunDatabase.RunDb.SUCCESS ):
      b = b + 'Failed to retrieve files for run:'+str(RunNumber)+' Err='+str(files[1])+'\n'
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
    xmlrpclib.ExpatParser = None
    print '==========================================================='
    print '===     running with SLOW XML parser (Expat error)     ===='
    print '==========================================================='
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
    self.server = gaudiweb.DataManagementServer(port=self.port, nam='pclhcb102.cern.ch', lg=0)
    manager_servlet = gaudiweb.ManagementServlet('ServerConfig')
    manager_servlet.addUsers(self.users)
    self.server.registerServlet('ServerConfig', manager_servlet)

    service = RunDatabase.RunDatabase(login)
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
  