import cgi, socket, threading, xmlrpclib, urllib, time, sys, os

import StringIO, SocketServer, BaseHTTPServer

logPrefix     = ''
logHeader     = '+----------------------------------------------------------------------------------------------------'
logTrailer    = logHeader
Version       = "1.0.0"

TODO          = 'ToDo'
EXECUTING     = 'Executing'
DONE          = 'Done'
ERROR         = 'Error'

OK            = 'OK'
STOPPED       = 'STOPPED'
RUNNING       = 'RUNNING'
DISABLED      = 'DISABLED'
ENABLED       = 'ENABLED'

jobProdState  = {
               TODO:      TODO,
               EXECUTING: EXECUTING,
               DONE:      DONE,
               ERROR:     ERROR
             }

def S_ERROR():
  return {'Status': ERROR, 'Log':'Error during processing'}
def S_OK():
  return {'Status': OK, 'Log': 'Request handled successfully.'}

def timeStamp():
  import time
  return time.ctime(time.time())

def log(msg, with_header=1, with_trailer=1):
  if ( with_header != 0 ):
    print logPrefix + ' ' + timeStamp() + ' ' + logHeader
  if msg.__class__ == str().__class__:
    print logPrefix + ' ' + timeStamp() + ' |  ' + msg
  else:
    for line in msg:
      print logPrefix + ' ' + timeStamp() + ' |  ' + line
  if ( with_trailer != 0 ):
    print logPrefix + ' ' + timeStamp() + ' ' + logTrailer

def _resolve_dotted_attribute(obj, attr):
    """Resolves a dotted attribute name to an object.  Raises
    an AttributeError if any attribute in the chain starts with a '_'.
    """
    for i in attr.split('.'):
        if i.startswith('_'):
            raise AttributeError(
                'attempt to access private attribute "%s"' % i
                )
        else:
            obj = getattr(obj,i)
    return obj


# =================================================================
#
# class BasicAuthenticator
#
# @author   M.Frank
# @version  1.0
# @date     30/06/2002
# =================================================================
class BasicAuthenticator:
  #===============================================================================
  def __init__(self, realm):
    """ Standard Constructor. """
    self.realm = realm

  #===============================================================================
  def type(self):
    """ Authentication type. 
        Needed to supply browsers with the HTTP WWW-Authenticate header
    """
    return 'Basic realm="'+self.realm+'"'

  #===============================================================================
  def encode(self, aut):
    """ Encode authentication string. """
    import base64
    return base64.encodestring(aut)

  #===============================================================================
  def ticketFromHeader(self, header):
    """ Retrieve authentication from standard header.
        Needed to interprete the HTTP Authorization header
    """
    return header[6:]+'\n'

# =================================================================
#
# class Service
#
# @author   M.Frank
# @version  1.0
# @date     30/06/2002
# =================================================================
class Service:
  """ class Service.
      Base class to encapulate all minimum behaviour
      of a web application/service

      @author   M.Frank
      @version  1.0
      @date     30/06/2002
  """
  #===============================================================================
  def __init__(self, name, require_authorization=0):
    """  Standard Constructor.
    
         @author   M.Frank
         @version  1.0
         @date     30/06/2002
    """
    import shutil
    import posixpath
    import mimetypes
    self.m_state           = STOPPED
    self.m_name            = name
    self.copyfile          = shutil.copyfile
    self.splitext          = posixpath.splitext
    self.allowedUsers      = {}
    self.extensions_map    = mimetypes.types_map.copy()
    self.needAuthorization = require_authorization
    self.setAuthentication(BasicAuthenticator)
    self.m_state           = RUNNING

  #===============================================================================
  def name(self):
    """ Access the name of the servlet """
    return self.m_name

  #===============================================================================
  def welcome(self):
    """  Print Service setup
    
         @author   M.Frank
         @version  1.0
         @date     30/06/2002
    """
    pass

  #===============================================================================
  def setState(self, state):
    self.m_state = state

  #===============================================================================
  def state(self):
    return self.m_state

  #===============================================================================
  def may_stop(self):
    return None

  #===============================================================================
  def setAuthentication(self, authenticator_class):
    """  Attach authentication mechanism.
    
         @author   M.Frank
         @version  1.0
         @date     30/06/2002
    """
    self.authenticatorClass   = authenticator_class
    self.authenticator        = self.authenticatorClass(self.m_name)
    self.authenticationType   = self.authenticator.type
    self.encodeTicket         = self.authenticator.encode
    self.ticketFromHeader     = self.authenticator.ticketFromHeader
    return S_OK()

  #===============================================================================
  def getMimeType(self, path):
    """  Get MIME file type from the file path/extension.
    
         @author   M.Frank
         @version  1.0
         @date     30/06/2002
    """
    base, ext = self.splitext(path)
    if self.extensions_map.has_key(ext):
      return self.extensions_map[ext]
    ext = ext.lower()
    if self.extensions_map.has_key(ext):
      return self.extensions_map[ext]
    else:
      return '' #self.extensions_map['']

  #===============================================================================
  def allowUser(self, handler):
    """  Check the authentication of a given user.

         @author   M.Frank
         @version  1.0
         @date     30/06/2002
    """
    result = S_ERROR()
    if ( self.state() == RUNNING):
      result = S_OK()
      has_auth = handler.headers.has_key('Authorization')
      # print str(handler.headers)
      if ( self.needAuthorization ):
        if ( not has_auth):
          handler.send_response(401, '')
          handler.send_header('WWW-Authenticate', self.authenticationType())
          handler.end_headers()
          result['Status'] = 'ALL_DONE'
        else:
          ticket = self.ticketFromHeader(handler.headers['Authorization'])
          if ( self.allowedUsers.has_key(ticket)):
            handler.ticket = self.allowedUsers[ticket][0]
          else:
            handler.send_response(401, '')
            handler.send_header('WWW-Authenticate', self.authenticationType())
            handler.end_headers()
            result['Status'] = 'ALL_DONE'
#            result['Log'] = 'Authentication ticket is not valid for the requested service.'
      return result
    result['Log'] = 'The service is currently diabled for maintenance reasons. Please try later.'
    return result

  #===============================================================================
  def users(self):
    """  Access allowed users.

         @author   M.Frank
         @version  1.0
         @date     30/06/2002
    """
    return self.allowedUsers

  #===============================================================================
  def addUser(self, user, pwd):
    """  Handle authorization request of a given user.

         @param user    User name
         @param pwd     Password for this user

         @author   M.Frank
         @version  1.0
         @date     30/06/2002
    """
    aut = user+':'+pwd
    key = self.encodeTicket(aut)
    self.allowedUsers[key] = (user, pwd, aut)

  #===============================================================================
  def addUsers(self, users):
    """  Handle authorization request of a list of users.
         A user is a tuple: ('user-name', 'password')

         @param users   Collection of users

         @author   M.Frank
         @version  1.0
         @date     30/06/2002
    """
    for us in users:
      if ( len(us) > 1 ):
        self.addUser(us[0], us[1])

  #===============================================================================
  def readFile(self, fname, req_path, mime_type=None):
    """ Open file and read content """
    path = self.mount() + fname
    obj = None
    try:
      if os.path.isdir(path):
        return None
      if ( mime_type is None ):
        mime_type = self.getMimeType(path)
      if mime_type.startswith('text/'):
        mode = 'r'
      else:
        mode = 'rb'
      fp  = open(path, mode)
      buf = fp.read()
      fp.close()
      obj = (req_path, path, mime_type, os.stat(path)[6], buf)
    except Exception, X:
      log(['Service: Error reading file:'+req_path,str(X)],0,0)
      raise X
    return obj

  #===============================================================================
  def readDirectory(self, fname, req_path):
    """Helper to produce a directory listing (absent index.html).
    
    Return value is either a file object, or None (indicating an
    error).  In either case, the headers are sent, making the
    interface the same as for send_head().
    
    """
    path = self.mount()+fname
    try:
      list = os.listdir(path)
    except os.error:
      self.send_error(404, "No permission to list directory")
      return None
    list.sort(key=lambda a: a.lower())
    f = StringIO.StringIO()
    f.write("<title>Directory listing for %s</title>\n" % req_path)
    f.write("<h2>Directory listing for %s</h2>\n" % req_path)
    f.write("<hr>\n<ul>\n")
    for name in list:
      fullname = os.path.join(path, name)
      displayname = name
      linkname = req_path+os.sep+name
      # Append / for directories or @ for symbolic links
      if os.path.isdir(fullname):
        displayname = name + "/"
        linkname = linkname
      if os.path.islink(fullname):
        displayname = name + "@"
        # Note: a link to a directory displays with @ and links with /
      f.write('<li><a href="%s">%s</a>\n'
              % (urllib.quote(linkname), cgi.escape(displayname)))
    f.write("</ul>\n<hr>\n")
    length = f.tell()
    f.seek(0)
    return (req_path, path, 'text/html', os.stat(path)[6], f.read())

  #===============================================================================
  def makeWebFile(self, obj):
    """ Create web file from cache. """
    result = S_OK()
    result['Type']    = obj[2]
    result['Length']  = obj[3]
    result['Content'] = obj[4]
    return result

  #===============================================================================
  def makeFile(self, fname, req_path='', mime_type=None,opt='r'):
    try:
      if ( mime_type is None ):
        mime_type = self.getMimeType(fname)
      if ( not mime_type.startswith('text/') ):
        opt = 'rb'
      file = open(fname, opt)
      buf = file.read()
      file.close()
      return self.makeWebFile((req_path, self.name(), mime_type, len(buf), buf))
    except Exception, X:
      log(['Error reading file:'+req_path,str(X)],0,0)
      raise X
    return S_ERROR()
  
  #===============================================================================
  def handle(self, handler, req_path, req, args, data):
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
    result = self.allowUser(handler)
    if ( result['Status'] == OK):
      return self.handleHTTP(handler, req_path, req, args, data)
    return result

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
    return S_ERROR()

# =================================================================
#
# class FileServlet
#
# @author   M.Frank
# @version  1.0
# @date     30/06/2002
# =================================================================
class FileServlet(Service):
  """ Small class to allow file access on a given mount point.

      @author   M.Frank
      @version  1.0
      @date     30/06/2002
  """
  #===============================================================================
  def __init__(self, name='', mount_point='', require_authorization=0):
    """  Standard Constructor.
    
         @author   M.Frank
         @version  1.0
         @date     30/06/2002
    """
    Service.__init__(self, name, require_authorization)
    self.m_mount = mount_point
    self.m_cachedFiles = {}
    self.m_cache = 0

  #===============================================================================
  def mount(self):
    """ Access the mountpoint of the servlet """
    return self.m_mount

  #===============================================================================
  def name(self):
    """ Access the name of the servlet """
    return self.m_mount

  #===============================================================================
  def useCache(self, opt):
    """ Change cache flag """
    if ( opt is None or not opt):
      self.m_cachedFiles = {}
    self.m_cache = opt
    return self.m_cache
  
  #===============================================================================
  def getFile(self, req_path, cache_only=None, refresh=None):
    """  Retrieve a file from the servlet.
         @param req_path   Full path name of the request
         @param cache_only Only look in the existing cache of files
         @param refresh    Read again and update the cached data

         @author   M.Frank
         @version  1.0
         @date     10/06/2004
    """
    if ( refresh is not None and self.m_cache ):
      if ( self.m_cachedFiles.has_key(req_path)):
        obj = self.m_cachedFiles[req_path]
        return obj

    # Unfortunately we have to create the object now
    if ( cache_only is None ):
      try:
        obj = self.readFile(req_path[len(self.m_name)+1:], req_path)
        if obj is None:
          obj = self.readDirectory(req_path[len(self.m_name)+1:], req_path)
        if ( self.m_cache ):
          self.m_cachedFiles[req_path] = obj
        return obj
      except IOError, X:
        log(['Error on reading file:'+req_path+' Exception='+str(X)],0,0)
    return None

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
    obj = self.getFile(req_path)
    if ( obj is not None ):
      return self.makeWebFile(obj)
    return S_ERROR()

# =================================================================
#
# class FavIconServer
#
# @author   M.Frank
# @version  1.0
# @date     30/06/2002
# =================================================================
class FavIconServer(FileServlet):
  """ Small class to allow file access on a given mount point.

      @author   M.Frank
      @version  1.0
      @date     30/06/2002
  """
  #===============================================================================
  def __init__(self, path):
    """  Standard Constructor.
    
         @author   M.Frank
         @version  1.0
         @date     30/06/2002
    """
    FileServlet.__init__(self, 'FavIcon', path, 0)

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
    if ( self.m_cachedFiles.has_key(self.mount())):
      obj = self.m_cachedFiles[self.mount()]
      return self.makeWebFile(obj)

    # Unfortunately we have to create the object now
    try:
      print 'Reading:' + req_path+ '  -- ' + self.mount()
      obj = self.readFile('', req_path)
      if ( self.m_cache ):
        self.m_cachedFiles[self.mount()] = obj
      return self.makeWebFile(obj)
    except IOError:
      return S_ERROR()

#=================================================================================
#
#  class ManagementServlet
#
#=================================================================================
class ManagementServlet(Service):
  """  class ManagementServlet.
    
       @author   M.Frank
       @version  1.0
       @date     30/06/2002
  """

  #===============================================================================
  def __init__(self, name, require_authentication=1):
    """  Standard Constructor.
    
         @author   M.Frank
         @version  1.0
         @date     30/06/2002
    """
    Service.__init__(self, name, require_authentication)

  #===============================================================================
  def setState(self, state):
    pass

  #===============================================================================
  def state(self):
    return RUNNING

  #===============================================================================
  def services(self):
    return {}

  #===============================================================================
  def servlets(self):
    return {}

  #===============================================================================
  def getPageTitle(self):
    return 'Welcome to the Gaudi RPC/Web Portal Manager'
    
  #===============================================================================
  def getWebPage(self, url):
    try:
      print 'URL=', url
      doc = urllib.URLopener().open(url).read()
      return doc
    except:
      log(['Web page for URL '+url+' does not exist.'],0,0)      
    return None

  #===============================================================================
  def showState(self, ref, object):
    buf = ''
    has_state = hasattr(object,'state')
    may_stop  = hasattr(object,'may_stop')
    if ( may_stop ):
      may_stop = object.may_stop()
    else:
      may_stop = 1

    if ( has_state):
      buf = buf +     '<TD bgcolor="yellow"><B>'+object.state()+'</B></TD>'
      buf = buf +     '<TD bgcolor="lightgreen"><B><A href="'+ref+'enable">Enable</A></B></TD>'
      buf = buf +     '<TD bgcolor="orange"><B><A HREF="'+ref+'disable">Disable</A></B></TD>'
    else:
      buf = buf +     '<TD colspan="3">State cannot be changed.</TD>'

    if ( may_stop ):
      buf = buf +     '<TD bgcolor="red"><B><A HREF="'+ref+'stop">Stop</A></B></TD>'
    else:
      buf = buf +     '<TD bgcolor="red"><FONT color="red"><B>Stop</B></FONT></TD>'
    return buf
      
  #===============================================================================
  def makePage(self, handler, req_path, req, args, data):
    buf = '<html><head><title>Gaudi RPC/Web Portal</title>'
    buf = buf + '<link rel="stylesheet" href="/html/DataMgmt.css"/>'
    buf = buf + '</head><body>'
    buf = buf + self.pageHeader(handler)
    buf = buf + self.pageBody(handler)
    buf = buf + self.pageTrailer(handler)
    buf = buf + '</body></html>'
    return self.makeWebFile((req_path, self.name(), 'text/html', len(buf), buf))

  #===============================================================================
  def serverSetup(self, handler, cols=5):
    srv = handler.server
    usr = ''
    for u in self.users().keys():
      usr = usr + ' ' + self.users()[u][0]
    buf = ''
    buf = buf + '<TR><TD colspan="'+str(cols)+'" bgcolor="red"><div class="TITLES">Server setup</div></TD></TR>'
    buf = buf + '<TR><TD><DIV class="bullets"> Allowed Users:  </DIV></TD><TD colspan="'+str(cols-1)+'"><DIV class="bullets">'+usr+'</DIV></TD></TR>'
    if os.environ.has_key('POWERUSERS'):
      buf = buf + '<TR><TD><DIV class="bullets"> Power users:  </DIV></TD><TD colspan="'+str(cols-1)+'"><DIV class="bullets">'+os.environ['POWERUSERS']+'</DIV></TD></TR>'
    buf = buf + '<TR><TD><DIV class="bullets"> TCP/IP address: </DIV></TD><TD colspan="'+str(cols-1)+'"><DIV class="bullets">'+srv.node[2][0]+'</DIV></TD></TR>'
    buf = buf + '<TR><TD><DIV class="bullets"> Host name:      </DIV></TD><TD colspan="'+str(cols-1)+'"><DIV class="bullets">'+srv.nodeName()+'</DIV></TD></TR>'
    buf = buf + '<TR><TD><DIV class="bullets"> Port number:    </DIV></TD><TD colspan="'+str(cols-1)+'"><DIV class="bullets">'+str(srv.port)+'</DIV></TD></TR>'
    return buf

  #===============================================================================
  def pageHeader(self, handler):
    srv = handler.server
    tim = time.strftime('%Y-%m-%d  %H:%M:%S',time.gmtime())
    buf = '<H1>'+self.getPageTitle()+'</H1>'
    buf = buf + '<H2>'+srv.nodeName()+':'+str(srv.port)+' Snapshot time:' + tim + '</H2>'
    if os.environ.has_key('DBSERVER'):
      buf = buf + '<H2> Database server:'+os.environ['DBSERVER']+'</H2>'
    buf = buf + '<H2> Process ID:'+str(os.getpid())+'</H2>'
    return buf

  #===============================================================================
  def pageTrailer(self, handler):
    srv = handler.server
    buf =       '<TABLE cellspacing="0" cellpadding="10">'
    buf = buf + '  <TR>'
    buf = buf +     '<TD colspan="4">'
    buf = buf +       '<FORM action="http://'+srv.nodeName()+':'+str(srv.port)+'/'+self.name()+'?-action=exit" method="post">'
    buf = buf +       '<BR><input type="submit" value="Stop Server"/></BR>'
    buf = buf +       '</FORM>'
    buf = buf +     '</TD>'
    buf = buf +     '<TD><IMG border=0 src="/images/lhcblogo.gif"></TD>'
    buf = buf +     '<TD>Featured by</TD>'
    buf = buf +     '<TD><IMG border=0 src="/images/Gaudi.gif" height="160"></TD>'
    buf = buf +   '</TR>'
    buf = buf + '</TABLE>'
    buf = buf + '<P><img src="/images/e-mail.gif" height="25" width="72"></img>'
    buf = buf + 'If you have problems or suggestions send mail to '
    buf = buf + '<I>M.Frank CERN/LHCb</I>'
    return buf

  #===============================================================================
  def pageBody(self, handler):
    obj = None
    srv = handler.server
    url = 'http://'+srv.nodeName()+':'+str(srv.port)+'/'+self.name()+'?-name='
    buf = '<TABLE border="1" cellspacing="0" cellpadding="6" width="100%">'

    services = self.services()
    servlets = self.servlets()
    for s in services.keys():
      if ( srv.services.has_key(s) ):
        del(services[s])

    for s in servlets.keys():
      if ( srv.servlets.has_key(s) ):
        del(servlets[s])

    if ( (len(services.keys()) + len(srv.services.keys())) > 0 ):
      buf = buf + '<TR><TD colspan="6" bgcolor="red"><div class="TITLES">Services</div></TD></TR>'
      for s in services.keys():
        nam = s
        ref = url+s[4:]+'&-type=service&-service='+services[s][0]+'&-class='+services[s][1]+'&-interface='+services[s][2]+'&-action='
        buf = buf + '<TR><TD><div class="redbullets">http://'+srv.nodeName()+':'+str(srv.port)+'/'+s+'</div></TD>'
        buf = buf +     '<TD><div class="bullets">'+nam+'</div></TD>'
        buf = buf +     '<TD bgcolor="yellow"><B>OFFLINE</B></TD>'
        buf = buf +     '<TD colspan="3" bgcolor="lightgreen"><B><A href="'+ref+'start">Start</A></B></TD>'
        buf = buf + '</TR>'
      for s in srv.services.keys():
        n = srv.services[s]
        nam = s
        if ( hasattr(n,'name')):
          nam = n.name()
        ref = url+s[4:]+'&-type=service&-action='
        buf = buf + '<TR><TD><div class="redbullets">http://'+srv.nodeName()+':'+str(srv.port)+'/'+s+'</div></TD>'
        buf = buf +     '<TD><div class="bullets">'+nam+'</div></TD>'
        buf = buf +     self.showState(ref, n)
        buf = buf + '</TR>'

    if ( (len(srv.servlets.keys()) + len(srv.services.keys())) > 0 ):
      buf = buf + '<TR><TD colspan="6" bgcolor="red"><div class="TITLES">Servlets</div></TD></TR>'
      for s in servlets.keys():
        nam = s
        ref = url+s+'&-type=servlet&-class='+servlets[s][0]+'&-action='
        buf = buf + '<TR><TD><div class="redbullets">http://'+srv.nodeName()+':'+str(srv.port)+'/'+s+'</div></TD>'
        buf = buf +     '<TD><div class="bullets">'+nam+'</div></TD>'
        buf = buf +     '<TD bgcolor="yellow"><B>OFFLINE</B></TD>'
        buf = buf +     '<TD colspan="3" bgcolor="lightgreen"><B><A href="'+ref+'start">Start</A></B></TD>'
        buf = buf + '</TR>'
      for s in srv.servlets.keys():
        n = srv.servlets[s]
        nam = s
        if ( hasattr(n,'name')):
          nam = n.name()
        ref = url+s+'&-type=servlet&-action='
        svc_url = 'http://'+srv.nodeName()+':'+str(srv.port)+'/'+s
        buf = buf + '<TR><TD><DIV class="redbullets"><A href="'+svc_url+'">'+svc_url+'</A></DIV></TD>'
        buf = buf +     '<TD><DIV class="bullets">'+nam+'</DIV></TD>'
        buf = buf +     self.showState(ref, n)
        buf = buf + '</TR>'

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
    #print 'Handle Manager request....'
    srv = handler.server    
    if ( handler.mount == self.name() ):
      cnt = None
      nam = None
      action = None
      if ( args.has_key('-action')):
        action = args['-action']
        if ( action=='exit'):
          handler.server.stopServing()
          buf = '<html><head><title>Gaudi RPC/Web Portal</title>'
          buf = buf + '</head><body>'
          buf = buf + '<H1>The server process exited gracefully.</H1>'
          #buf = buf + '<IMG border=0 src=\"/images/lhcblogo.gif\">'
          buf = buf + '<FORM action="http://'+srv.nodeName()+':'+str(srv.port)+'/'+self.name()+'?-action=exit" method="post">'
          buf = buf + '<BR><input type="submit" value="Retry: Stop Server"/></BR>'
          buf = buf + '</FORM>'
          buf = buf + '</body></html>'
          return self.makeWebFile((req_path, self.name(), 'text/html', len(buf), buf))
        elif ( args.has_key('-type')):
          if ( args.has_key('-name')):
            nam = args['-name']
            if ( args['-type']=='servlet'):
              cnt = srv.servlets
            elif ( args['-type']=='service'):
              cnt = srv.services
              nam = 'RPC/'+nam
            if ( cnt is not None and nam is not None):
              self.treatObject(cnt, nam, action)
              return self.makePage(handler, req_path, req, args, data)

          return S_ERROR()
      return self.makePage(handler, req_path, req, args, data)
    return S_ERROR()

  def treatObject(self, cnt, nam, action):
    if ( cnt.has_key(nam)):
      obj = cnt[nam]
      if ( action=='stop'):
        obj.setState(STOPPED)
      elif ( action=='start'):
        obj.setState(RUNNING)
      elif ( action=='enable'):
        obj.setState(RUNNING)
      elif ( action=='disable'):
        obj.setState(DISABLED)

#=================================================================================
#
#  class DataManagementHandler
#
#=================================================================================
class DataManagementHandler(BaseHTTPServer.BaseHTTPRequestHandler):
  """  class DataManagementHandler.
    
       @author   M.Frank
       @version  1.0
       @date     30/06/2002
  """

  #===============================================================================
  def __init__(self, request, client_address, server):
    """  Standard Constructor.
    
         @author   M.Frank
         @version  1.0
         @date     30/06/2002
    """
    import traceback
    self.ticket = 'Unknown'
    self.raw_requestline = None
    self.base = BaseHTTPServer.BaseHTTPRequestHandler
    self.format_exception = traceback.format_exception
    self.log = server.log
    self.base.__init__(self,request,client_address, server)

  #===============================================================================
  def formatException(self, msg):  
    """  Properly format exception for sending in an HTML page.
    
         @author   M.Frank
         @version  1.0
         @date     30/06/2002
    """
    info = sys.exc_info()
    result = '<H1>'+msg+'</H1><UL>'
    if info is not None and info[0] is not None:
      lns=self.format_exception(info[0],info[1],info[2])
      for line in lns:
        result = result + '<LI>'+line+'</LI>'
    result = result + '</UL>'
    return result

  #===============================================================================
  def timeout_handle_handler(self):
    try:
      self.raw_requestline  = self.rfile.readline()
    except socket.error, X:
      if ( X[0] != 10054 ):  # skip: Connection reset by peer
        log(['Socket Exception during request processing from '+str(self.client_address)+': '+str(X)],0,0)
    except TypeError, X:
      log(['TypeError during request processing from '+str(self.client_address)+': '+str(X)],0,0)
    except Exception, X:
      log(['IO Exception during request processing from '+str(self.client_address)+': '+str(X.__class__)+' '+str(X)],0,0)
      
  #===============================================================================
  def handle(self):
    """Handle a single HTTP request.

    Overloaded method from BaseHTTPHandler.
    Need to set a timeout in order to escape broken connections
    while receiving data (avoid blocking recv)

    """

    # Set the timeout handler and a 10-second alarm
    # since this request may hang indefinitely
    try:
      t = threading.Thread(target=self.timeout_handle_handler)
      t.start()
      t.join(10.0)
      if ( t.isAlive() ):
        raise IOError, "Couldn't read request: Timout signal from HTTP handler"
    except socket.error, X:
      if ( X[0] != 10054 ):  # skip: Connection reset by peer
        log(['Socket Exception during request processing from '+str(self.client_address)+': '+str(X)],0,0)
      return
    except TypeError, X:
      log(['TypeError during request processing from '+str(self.client_address)+': '+str(X)],0,0)
      return
    except IOError, X:
      log(['IOError during request processing from '+str(self.client_address)+': '+str(X)],0,0)
      return
    except Exception, X:
      log(['Exception during request processing from '+str(self.client_address)+': '+str(X.__class__)+' '+str(X)],0,0)
      return
    except:
      log(['Unknown exception during request processing from '+str(self.client_address)],0,0)
      return

    try:
      if not self.parse_request(): # An error code has been sent, just exit
        return
      mname = 'do_' + self.command
      if not hasattr(self, mname):
        self.send_error(501, "Unsupported method (%s)" % `self.command`)
        return
      method = getattr(self, mname)
      method()
    except socket.error, X:
      if ( X[0] != 10054 ):  # skip: Connection reset by peer
        log(['Socket Exception during request processing from '+str(self.client_address)+': '+str(X)],0,0)
      return
    except TypeError, X:
      log(['3-TypeError during request processing from '+str(self.client_address)+': '+str(X)],0,0)
      return
    except Exception, X:
      log(['Exception during request processing from '+str(self.client_address)+': '+str(X.__class__)+' '+str(X)],0,0)
      return
    except:
      log(['Unknown exception during request processing from '+str(self.client_address)],0,0)
      return
  #===============================================================================
  def handleArgs(self):
    """  Interprete the arguments from the network request.
    
         @author   M.Frank
         @version  1.0
         @date     30/06/2002
    """
    self.args = {}
    self.target = '<Unknown>'
    self.rpc = 0
    qry = urllib.splitquery(self.path)
    if ( self.path[:4]=='/RPC'):
      self.target = qry[0][1:]
      self.rpc = 1
    else:
      self.target = qry[0][1:]
      self.rpc = 0
    self.mount = self.target
    idx = self.target.find('/',1)
    if ( idx > 0):
      self.mount = self.target[:idx]
    if ( qry[1] is not None ):
      args = qry[1].split('&')
      for a in args:
        arg = urllib.splitvalue(a)
        a0 = urllib.unquote_plus(arg[0])
        a1 = urllib.unquote_plus(arg[1])
        self.args[a0] = a1
    
  #===============================================================================
  def handleRPC(self):
    """  Handle RPC request.
    
         @author   M.Frank
         @version  1.0
         @date     30/06/2002
    """
    if ( self.server.services.has_key(self.target)):
      target = self.server.services[self.target]
      # get arguments
      data = self.rfile.read(int(self.headers['content-length']))
      params, method = xmlrpclib.loads(data)
      # generate response
      try:
        response = self._dispatch(target, method, params)
        # wrap response in a singleton tuple
        response = (response,)
      except:
        # report exception back to server
        fault = xmlrpclib.Fault(1, "%s:%s \n\n%s" % (sys.exc_type, sys.exc_value, sys.exc_info))
        response = xmlrpclib.dumps(fault)
      else:
        response = xmlrpclib.dumps(response, methodresponse=1)
        
      # got a valid XML RPC response
      self.send_response(200)
      self.send_header('Content-type',   'text/xml')
      self.send_header('Content-length', str(len(response)))
      self.end_headers()
      self.wfile.write(response)
      # shut down the connection
      self.wfile.flush()
      self.connection.shutdown(1)
      return 1
    msg = 'Unsupported RPC method ('+self.command+') on target:'+self.target
    self.base.send_error(self, 501, msg)
    return 0

  #===============================================================================
  def _dispatch(self, object, method, params):
    """  Dispatches the XML-RPC method.

         If the registered instance does not have a _dispatch method
         then the instance will be searched to find a matching method
         and, if found, will be called.

         @author   M.Frank
         @version  1.0
         @date     30/06/2002
    """
    func = None
    if ( object is not None):
      # call instance method
      try:
        func = _resolve_dotted_attribute(object, method)
      except AttributeError:
        pass
    if ( func is not None):
      return apply(func, params)
    else:
      raise Exception('method "%s" is not supported' % method)

  #===============================================================================
  def handleServlet(self):
    """  Handle Servlet request.
    
         @author   M.Frank
         @version  1.0
         @date     30/06/2002
    """
    import rfc822
    type = 'text/html'
    if ( self.server.servlets.has_key(self.mount)):
      target = self.server.servlets[self.mount]
      # get arguments
      data   = None
      if ( self.headers.has_key('content-length')):
        data = self.rfile.read(int(self.headers['content-length']))
      # generate response
      try:
        response    = ''
        respons_len = ''
        result = self._dispatch(target, 'handle', (self, self.path, self.command, self.args, data))
        if ( result['Status'] == 'ALL_DONE'):
          return 1
        elif ( result['Status'] != OK):
          self.send_error(500, self.formatException(result['Log']))
          return
        if ( result.has_key('Type')):
          type     = result['Type']
        if ( result.has_key('Content')):
          response = result['Content']
        if ( result.has_key('Length')):
          response_len = str(result['Length'])
        else:
          response_len = str(len(response))
      except:
        # report exception back to server
        msg = self.formatException('Error handling http servlet request')
        self.send_error(500, msg)
        return 0

      # got a valid response
      self.send_response(200)
      #expiration = rfc822.formatdate(time.time()+15)
      #self.send_header('Expires', expiration)
      self.send_header('Cache-Control','max-age=15')
      self.send_header('Content-type', type)
      self.send_header('Content-length', response_len)
      self.end_headers()
      self.wfile.write(response)
      # shut down the connection
      self.wfile.flush()
      self.connection.shutdown(1)
      return 1
    msg = 'Unsupported Servlet request ('+self.command+') on target:'+self.target
    self.send_error(501, msg)
    return 0

  #===============================================================================
  def do_GET(self):
    """  Handle the HTTP GET request.
         The request is forwarded to the proper servlet and interpreted
         if the requested mount point matches the declared servlet mount.
    
         @author   M.Frank
         @version  1.0
         @date     30/06/2002
    """
    try:
      self.handleArgs()
      self.handleServlet()

    except Exception, X:
      # internal error, report as HTTP server error
      msg = 'Exception during HTTP POST processing:'+str(X)
      self.send_response(500, msg)
      self.end_headers()
    else:
      pass
  
  #===============================================================================
  def do_POST(self):
    """  Handle the HTTP POST request.

         Attempts to interpret HTTP POST requests as XML-RPC calls,
         which are forwarded to the _dispatch method for handling,
         if the requested address matches an RPC mount point. 
         Otherwise the request is handled by a Servlet.

         @author   M.Frank
         @version  1.0
         @date     30/06/2002
    """
    try:
      self.handleArgs()
      if ( self.rpc ):
        self.handleRPC()
      else:
        self.handleServlet()
    except Exception, X:
      # internal error, report as HTTP server error
      msg = 'Exception during HTTP POST processing:'+str(X)
      self.send_response(500, msg)
      self.end_headers()
    else:
      pass

  #===============================================================================
  def do_PUT(self):
    """  Handle the HTTP PUT request.

         Attempts to interpret HTTP PUT requests.
         The request is forwarded to the proper servlet and interpreted
         if the requested mount point matches the declared servlet mount.

         @author   M.Frank
         @version  1.0
         @date     30/06/2002
    """
    try:
      self.handleArgs()
      self.handleServlet()
    except Exception, X:
      # internal error, report as HTTP server error
      msg = 'Exception during HTTP POST processing:'+str(X)
      self.send_response(500, msg)
      self.end_headers()
    else:
      pass

  #===============================================================================
  def send_error(self, code, message=None):
    """Send and log an error reply.
       Arguments are the error code, and a detailed message.
       The detailed message defaults to the short entry matching the
       response code.

       This sends an error response (so it must be called before any
       output has been generated), logs the error, and finally sends
       a piece of HTML explaining the error to the user.
       """
    header = """\
    <head>
    <title>Error response</title>
    </head>
    <body>
    <BR><IMG border=0 src=\"/images/lhcblogo.gif\"><BR>
    <H1>Invalid Request</h1>
    """

    trailer = """\
    <p>Unfortunately this is not a valid question to ask.
       If you do not know how to proceed, you are kindly invited
       to send a mail to
    <p><I><A HREF=\"mailto:Markus.Frank@cern.ch\">
       Markus Frank </A></I>.
    """
    
    try:
      short, long = self.responses[code]
    except KeyError:
      short, long = '???', '???'
    if not message:
      message = short
    explain = long
    msg = header + '<h1>Error response</h1>' + timeStamp()
    msg = msg + '<p>' + message
    msg = msg + '<p>Error code ' + str(code)
    msg = msg + '<p>Error code explanation:' + explain
    msg = msg + '<H1>Request:</H1>'+self.requestline
    if ( hasattr(self, 'target')):
      msg = msg + '<H1>Target:</H1>'
      msg = msg + self.target
    if ( hasattr(self, 'args')):
      msg = msg + '<H1>Arguments:</H1><table cellspacing="10" sellpadding="3"><tr><th>Name</th><th>Value</th></tr>'
      for a in self.args.keys():
        msg = msg + '<TR><TD>'+str(a)+'</TD><TD>'+str(self.args[a])+'</TD></TR>'
      msg = msg + '</table>'
    msg = msg + trailer
    msg = msg + '<H1>Python setup:</H1>'
    for a in sys.modules:
      msg = msg + '  ' + str(a)
    #msg = msg + '<H1>Environment:</H1><table cellspacing="10" sellpadding="3"><tr><th>Name</th><th>Value</th></tr>'
    #for a in os.environ.keys():
    #  msg = msg + '<TR><TD>'+str(a)+'</TD><TD>'+str(os.environ[a])+'</TD></TR>'
    #msg = msg + '</table>'
    msg = msg + '</body></html>'

    self.log_error("code %d, message %s", code, message)
    self.send_response(code, message)
    self.send_header("Content-Type", "text/html")
    self.end_headers()
    self.wfile.write(msg)
    return
  
  #===============================================================================
  def log_request(self, code='-', size='-'):
    """Selectively log an accepted request."""
    if ( self.server.logRequests):
      self.log_message('%s:%15s %3s "%s" %s',
                      self.ticket, 
                      self.client_address[0], 
                      str(code),
                      self.requestline,
                      str(size))
      
  #===============================================================================
  def log_message(self, format, *args):
    """Log an arbitrary message."""
    self.log([format%args], 0, 0)
        
#=================================================================================
#
#  class DataManagementServer
#
#=================================================================================
class DataManagementServer(SocketServer.ThreadingTCPServer):
  """Data Management XML-RPC server.
     Simple XML-RPC server that allows functions and a single instance
     to be installed to handle requests.

     @author   M.Frank
     @version  1.0
     @date     30/06/2002
  """

  allow_reuse_address = 1    # Seems to make sense in testing environment

  #===============================================================================
  def __init__(self, port, name=None, lg=1):
    """  Standard Constructor.
         port number of the local server
         lg   Flag wether or not to log requests

         @author   M.Frank
         @version  1.0
         @date     30/06/2002
    """
    import socket
    nam = name
    self.log = log
    self.port = port
    self.servlets = {}
    self.services = {}
    self.logRequests  = lg
    self.continue_handling = 1
    if ( nam is None ): 
      nam = socket.gethostname()
    self.node = socket.gethostbyaddr(nam)
    if self.node[0].find('.') < 0:
      self.node = (self.node[0]+'.lbdaq.cern.ch',self.node[1],self.node[2])
    #BaseHTTPServer.HTTPServer.__init__(self, ('', port), DataManagementHandler)
    SocketServer.ThreadingTCPServer.__init__(self, ('', port), DataManagementHandler)

  def server_bind(self):
      """Override server_bind to store the server name."""
      SocketServer.ThreadingTCPServer.server_bind(self)
      host, port = self.socket.getsockname()[:2]
      self.server_name = socket.getfqdn(host)
      self.server_port = port
        
  def nodeName(self):
    return self.node[0]

  #===============================================================================
  def welcome(self):
    """ Print welcome message at server startup.

      @author   M.Frank
      @version  1.0
      @date     30/06/2002
    """
    log (['','','Welcome to the Gaudi Web/RPC Portal at '+self.nodeName(),'',''])
    servlet_info = []
    msg = ['',
           'Server setup:                           ',
           '   TCP/IP:                               ',
           '     TCP/IP address:                     '+self.node[2][0],
           '     Host name:                          '+self.node[0],
           '     Port number:                        '+str(self.port),
           '     Process ID:                         '+str(os.getpid()),
           '',
           'Services setup:',
          ]
    for s in self.services.keys():
      n = s
      if ( hasattr(self.services[s], 'name')):
        n = self.services[s].name()
      msg.append(' %-55s %s'%('http://'+self.nodeName()+':'+str(self.port)+'/'+s+':',n))
      if ( hasattr(self.services[s], 'welcome')):
        pass
    msg.append('Servlet setup:')
    for s in self.servlets.keys():
      n = s
      if ( hasattr(self.servlets[s], 'name')):
        n = self.servlets[s].name()
      msg.append(' %-55s %s'%('http://'+self.nodeName()+':'+str(self.port)+'/'+s+':',n))
      if ( hasattr(self.servlets[s], 'welcome')):
        pass
    if os.environ.has_key('DBSERVER'):
      msg.append('Database setup:')
      msg.append(' %-55s %s'%('Database server:',os.environ['DBSERVER']))

    msg.append('')
    msg.append('  Author: M.Frank CERN/LHCb')
    msg.append('')
    log (msg)

  #===============================================================================
  def registerServlet(self, name, instance):
    """Registers a servlet instance to respond to HTTP requests.

      @param   name       Name of the WEB service to be registered
      @param   instance   Object to be made availible as web service
      @return  Status     Status code indicating success or failure

      @author   M.Frank
      @version  1.0
      @date     30/06/2002
    """
    n = name
    if ( hasattr(instance, 'name')):
      n = instance.name()
    self.servlets[name] = instance
    self.log(['Servlet instance '+n+' ['+name+'] registered....'],1,0)
    return S_OK()

  #===============================================================================
  def unregisterServlet(self, name):
    """Registers a servlet instance to respond to HTTP requests.

      @param   name       Name of the WEB service to be registered
      @return  Status     Status code indicating success or failure

      @author   M.Frank
      @version  1.0
      @date     30/06/2002
    """
    if ( self.servlets.has_key(name) ):
      del self.servlets[name]
      self.log(['Servlet instance ['+name+'] de-registered....'],1,0)
      return S_OK()
    self.log(['Servlet instance ['+name+'] not present....'],1,0)
    return S_ERROR()

  #===============================================================================
  def registerService(self, name, instance):
    """Registers an instance to respond to XML-RPC requests.
    
      @param name       Name of the WEB service to be registered
      @param instance   Object to be made availible as web service
      @return  Status     Status code indicating success or failure
      
      @author   M.Frank
      @version  1.0
      @date     30/06/2002
    """
    n = name
    if ( hasattr(instance, 'name')):
      n = instance.name()
    self.services[name] = instance
    self.log(['Service instance '+n+' ['+name+'] registered....'],1,0)
    return S_OK()

  #===============================================================================
  def unregisterService(self, name):
    """Unregisters an instance to respond to XML-RPC requests.
    
      @param name       Name of the WEB service to be registered
      @return  Status     Status code indicating success or failure
      
      @author   M.Frank
      @version  1.0
      @date     30/06/2002
    """
    if ( self.services.has_key(name) ):
      del(self.services[name])
      self.log(['Service instance ['+name+'] unregistered....'],1,0)
      return S_OK()
    self.log(['Service instance ['+name+'] not present....'],1,0)
    return S_ERROR()

  #===============================================================================
  def registerMountpoint(self, name, mnt):
    """Registers an instance to respond to HTTP file requests.
    
      @param name         Name of the mount point to be registered
      @param mnt          Local mountpoint as absolute path corresponding
                          to name
      @return  Status     Status code indicating success or failure
      
      @author   M.Frank
      @version  1.0
      @date     30/06/2002
    """
    return self.registerServlet(name, FileServlet(name, mnt))

  def stopServing(self):
    log(['Server was requested to stop execution....'])
    self.continue_handling = 0
    return self

  def serve_forever(self):
    """Handle one request at a time until doomsday."""
    #log(['Starting message pump.....'])
    val = self.continue_handling
    while (val == 1):
      self.handle_request()
      val = self.continue_handling
    self.server_close()
    log(['Stopped message pump.....'])

  def handle_error(self, request, client_address):
    """Handle an error gracefully.  May be overridden.

       The default is to print a traceback and continue.

    """
    import traceback
    log(['Exception happened during processing of request from '+str(client_address)],1,0)
    etype, value, tb = sys.exc_info()
    if etype is not None:
      lines = traceback.format_exception_only(etype, value)
      for line in lines[:-1]:
        log([line], 0, 0)

if ( __name__ == "__main__"):
  opt = '..'
  if ( len(sys.argv) > 1):
    opt = sys.argv[1]
  server  = DataManagementServer(8081, 1)
  server.registerServlet   ('html',   FileServlet('html', opt+os.sep+'html'))
  server.registerMountpoint('images', opt+os.sep+'images')
  server.registerServlet('Manager', ManagementServlet('Manager', 0))
  server.registerServlet('favicon.ico', FavIconServer(opt+os.sep+'images'+os.sep+'favicon.ico'))
  server.welcome()
  server.serve_forever()
