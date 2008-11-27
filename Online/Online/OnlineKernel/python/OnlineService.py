import sys, os, time, gaudiweb

# Standard global variables
gaudi_app       = None
basePath        = os.environ['ONLINEKERNELROOT']
default_users = [ ('administrator', 'admin_oder_was'), ('frankm', 'hallo_wer_bin_ich')
        ]
servicePort = 8081
startup = 'startAll'
useOracle = 0
allowedSites = {}

def rejectUnregistered(sitename):
  """Reject all sites, which are not explicitly REGISTERED."""
  if ( allowedSites.has_key(sitename) ):
    if ( allowedSites[sitename] == gaudiweb.DISABLED ):
      return 0
    return 1
  return 0

def rejectDisabled(sitename):
  """Reject only those sites, which are explicitly DISABLED."""
  if ( allowedSites.has_key(sitename) ):
    if ( allowedSites[sitename] == gaudiweb.DISABLED ):
      return 0
  return 1

sitePolicy = rejectDisabled


#=================================================================================
#
#  class ServerConfig
#
#=================================================================================
class ServerConfig(gaudiweb.ManagementServlet):
  """  class ManagementServlet.
    
       @author   M.Frank
       @version  1.0
       @date     30/06/2002
  """

  #===============================================================================
  def __init__(self, name, manager, require_authentication=1):
    """  Standard Constructor.
    
         @author   M.Frank
         @version  1.0
         @date     30/06/2002
    """
    self.manager = manager
    gaudiweb.ManagementServlet.__init__(self, name, require_authentication)

  #===============================================================================
  def services(self):
    s = {}
    """
    s['RPC/ProductionInfo']   = ('ProductionSvc',      'ProductionSvc',      'IProductionInfo',  'RPC/ProductionInfo')
    """
    return s

  #===============================================================================
  def servlets(self):
    s = {}
    """
    s['Production']        = ('Production::ProductionServlet','Production')
    """
    return s

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
    srv = handler.server
    object = None
    if ( args.has_key('-name')):
      object = args['-name']
      if ( args.has_key('-type')):
        typ = args['-type']
        if ( args.has_key('-action')):
          act = args['-action']
          if ( typ == 'servlet'):
            if ( act == 'start'):
              if ( args.has_key('-class')):
                cl = args['-class']
                self.manager.startDataMgmtServlet(cl, object)
                return self.makePage(handler, req_path, req, args, data)
            elif ( act == 'stop'):
              self.manager.startServer().unregisterServlet(object)
              return self.makePage(handler, req_path, req, args, data)
          if ( typ == 'service'):
            object = 'RPC/'+object
            if ( act == 'start'):
              if ( args.has_key('-class')):
                cl = args['-class']
                if ( args.has_key('-interface')):
                  iface = args['-interface']
                  if ( args.has_key('-service')):
                    svc = args['-service']
                    self.manager.startDataMgmtService(cl+'/'+svc, iface, object)
                    return self.makePage(handler, req_path, req, args, data)
            elif ( act == 'stop'):
              self.manager.startServer().unregisterService(object)
              return self.makePage(handler, req_path, req, args, data)

    return gaudiweb.ManagementServlet.handleHTTP(self, handler, req_path, req, args, data)

#=================================================================================
#
#  class SiteEditor
#
#=================================================================================
class SiteEditor(gaudiweb.ManagementServlet):
  """  class SiteEditor.
    
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
    global allowedSites
    gaudiweb.ManagementServlet.__init__(self, name, require_authentication)
    allowedSites = {}
    self.port = port
    self.readDb()
    
  #===============================================================================
  def getPageTitle(self):
    return 'Welcome to the Production Site editor'


  #===============================================================================
  def readDb(self):
    try:
      global allowedSites
      allowedSites = {}
      f = open(self.name()+'.'+str(self.port)+'.dat', "r")
      lines = f.readlines()
      for l in lines:
        seq = l.replace('\n','').split(' ')
        allowedSites[seq[0]] = seq[1]
      f.close()
    except Exception, X:
      print str(X)
    except:
      print 'Unknown Exception while reading sites file'
      pass
  
  #===============================================================================
  def updateDb(self):
    global allowedSites
    f = open(self.name()+'.'+str(self.port)+'.dat', "w")
    for s in allowedSites.keys():
      data = s + ' ' + allowedSites[s] + '\n'
      f.write(data)
    f.close()       
  
  #===============================================================================
  def disableallsites(self):
    global allowedSites
    for s in allowedSites.keys():
      allowedSites[s] = gaudiweb.DISABLED
    self.updateDb()

  #===============================================================================
  def enableallsites(self):
    global allowedSites
    for s in allowedSites.keys():
      allowedSites[s] = gaudiweb.ENABLED
    self.updateDb()

  #===============================================================================
  def disablesite(self, site):
    global allowedSites
    allowedSites[site] = gaudiweb.DISABLED
    self.updateDb()

  #===============================================================================
  def enablesite(self, site):
    global allowedSites
    allowedSites[site] = gaudiweb.ENABLED
    self.updateDb()

  #===============================================================================
  def createsite(self, site):
    global allowedSites
    allowedSites[site] = gaudiweb.DISABLED
    self.updateDb()

  #===============================================================================
  def deletesite(self, site):
    global allowedSites
    del(allowedSites[site])
    self.updateDb()
    
  #===============================================================================
  def pageBody(self, handler):
    global allowedSites
    global sitePolicy
    srv = handler.server
    url = 'http://'+srv.nodeName()+':'+str(srv.port)+'/'+self.name()+'?'
    buf = '<H2>Site policy:'+sitePolicy.__doc__+'</H2>'
    buf = buf + '<TABLE border="1" cellspacing="0" cellpadding="6" width="80%">'
    buf = buf + '<TR><TD colspan="2" bgcolor="red"><div class="TITLES">Production Sites</div></TD>'
    buf = buf +     '<TD colspan="2" bgcolor="lightgreen"><div class="TITLES"><A href="'+url+'-enableall=true">Enable ALL</A></div></TD>'
    buf = buf +     '<TD colspan="2" bgcolor="lightgreen"><div class="TITLES"><A href="'+url+'-disableall=true">Disable ALL</A></div></TD>'
    buf = buf + '</TR>'
    for s in allowedSites.keys():
      buf = buf + '<TR><TD colspan="2"><div class="bullets">'+s+'</div></TD>'
      buf = buf +     '<TD bgcolor="yellow"><B>'+allowedSites[s]+'</B></TD>'
      buf = buf +     '<TD bgcolor="lightgreen"><B><A href="'+url+'-enable='+s+'">Enable</A></B></TD>'
      buf = buf +     '<TD bgcolor="lightgreen"><B><A href="'+url+'-disable='+s+'">Disable</A></B></TD>'
      buf = buf +     '<TD bgcolor="lightgreen"><B><A href="'+url+'-delete='+s+'">Delete</A></B></TD>'
      buf = buf + '</TR>'

    buf = buf + '<TR><TD colspan="2"><div class="bullets">Create new SITE (Enter name)</div></TD>'
    buf = buf +     '<TD colspan="4"><FORM action="'+url+'"><TABLE cellpadding="0" cellspacing="0"><TR>'
    buf = buf +     '<TD><INPUT type="text" name="-create" value="ANY"/></TD>'
    buf = buf +     '<TD><INPUT type="submit" value="Submit"/></TD>'
    buf = buf +     '</TR></TABLE></FORM></TD></TR>'
    #buf = buf + '<TR><TD colspan="2"><div class="bullets">Delete a SITE (Enter name)</div></TD>'
    #buf = buf +     '<TD colspan="4"><FORM action="'+url+'"><TABLE cellpadding="0" cellspacing="0"><TR>'
    #buf = buf +     '<TD><INPUT type="text" name="-delete" value="ANY"/></TD>'
    #buf = buf +     '<TD><INPUT type="submit" value="Submit"/></TD>'
    #buf = buf +     '</TR></TABLE></FORM></TD></TR>'
    buf = buf + self.serverSetup(handler, 6)
    buf = buf + '</TABLE>'
    return buf

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
      node = None
      if ( args.has_key('-disable')):
        site = args['-disable']
        self.disablesite(site)
      elif ( args.has_key('-enable')):
        site = args['-enable']
        self.enablesite(site)
      elif ( args.has_key('-create')):
        site = args['-create']
        self.createsite(site)
      elif ( args.has_key('-delete')):
        site = args['-delete']
        self.deletesite(site)
      elif ( args.has_key('-disableall')):
        self.disableallsites()
      elif ( args.has_key('-enableall')):
        self.enableallsites()
      return self.makePage(handler, req_path, req, args, data)
    return S_ERROR()

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
      #manager_servlet = gaudiweb.ManagementServlet('Manager')
      #manager_servlet.addUsers(self.users)
      #self.server.registerServlet('Manager', manager_servlet)
      #
      # ServerConfig servlet
      #
      cfg_servlet = ServerConfig('ServerConfig', self)
      cfg_servlet.addUsers(self.users)
      self.server.registerServlet('ServerConfig', cfg_servlet)
      #
      # Site editor servlet
      #
      site_servlet = SiteEditor('SiteEditor', self.port, self)
      site_servlet.addUsers(self.users)
      self.server.registerServlet('SiteEditor', site_servlet)
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

# =================================================================
if ( __name__ == "__main__x"):
  import time
  cmd_fmt = '"import ProductionService;ProductionService.%s(\'%s\', %d)"'
  log_fmt = '%s'+os.sep+'%s.%d.started_at.%s.log'
  startup_processes = []
  seperate_window = 1
  logDir = '.'
  now = time.strftime("%Y-%m-%d.%H-%M-%S",time.localtime())
  for i in xrange(len(sys.argv)):
    if ( sys.argv[i][1:3]=='p='):
      servicePort = int(sys.argv[i][3:])
    elif ( sys.argv[i][1:3]=='o='):
      options = sys.argv[i][3:]
    elif ( sys.argv[i][1:3]=='l='):
      logDir = sys.argv[i][3:]
    elif ( sys.argv[i][1:3]=='s='):
      startup_processes.append({'Cmd':sys.argv[i][3:], 'Opts':options, 'Port':servicePort})
      servicePort = servicePort + 1

  if ( len(startup_processes) == 0 ):
    startup_processes.append({'Cmd':startup, 'Opts':options, 'Port':servicePort})

  if ( len(startup_processes) > 1 ):
    for i in xrange(len(startup_processes)):
      proc = startup_processes[i]
      log = (log_fmt % (logDir + os.sep, proc['Cmd'], proc['Port'], now))
      cmd = (cmd_fmt % (proc['Cmd'], proc['Opts'], proc['Port']))
      print 'Spawn off:'+cmd
      print ' Log file redirected to:'+log
      if ( seperate_window):
        os.system('start /i python -c "'+cmd+'>'+log+'"')
      else:
        os.system('start /b /i python -c "'+cmd+'>'+log+'"')
        time.sleep(30)

    if ( seperate_window == 0):
      while(1):
        time.sleep(100000)
  else:
    proc = startup_processes[0]
    log = (log_fmt % (logDir, proc['Cmd'], proc['Port'], now))
    cmd = (cmd_fmt % (proc['Cmd'], proc['Opts'], proc['Port']))
    print 'Running:'+cmd
