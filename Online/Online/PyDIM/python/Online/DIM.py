import os, sys, string
import PyCintex as PyLCGDict
# PyLCGDict.gbl.Cintex.SetDebug(1)

PyLCGDict.loadDict('PyDIMDict')
DIM = PyLCGDict.makeNamespace('DIM')
gbl = PyLCGDict.makeNamespace('')
PyLCGDict.loadDict('STLRflx')

logPrefix     = ''
logHeader     = '+----------------------------------------------------------------------------------------------------'
logTrailer    = logHeader

#----enable tab completion---------------------------------------------------------------
try:
  import rlcompleter,readline    
  readline.parse_and_bind("tab: complete")
except:
  pass

def void_call(self): 
  return 1

def timeStamp():
  import time
  return time.ctime(time.time())

def log(msg, with_header=0, with_trailer=0):
  if ( with_header != 0 ):
    print logPrefix + ' ' + timeStamp() + ' ' + logHeader
  if msg.__class__ == str().__class__:
    print logPrefix + ' ' + timeStamp() + ' |  ' + msg
  else:
    for line in msg:
      print logPrefix + ' ' + timeStamp() + ' |  ' + line
  if ( with_trailer != 0 ):
    print logPrefix + ' ' + timeStamp() + ' ' + logTrailer

#CmdInfo    = gbl.CmndInfo
Info        = gbl.DimInfo
Info        = DIM.Info
Timer       = gbl.DimTimer
Client      = gbl.DimClient
Server      = gbl.DimServer
Service     = gbl.DimService
Command     = gbl.DimCommand
Browser     = DIM.Browser
Cmnd        = gbl.DimCmnd
UpdatedInfo = gbl.DimUpdatedInfo
StampedInfo = gbl.DimStampedInfo
RpcInfo     = gbl.DimRpcInfo
CurrentInfo = gbl.DimCurrentInfo

# DIM info handler
class InfoHandler(DIM.InfoHandler):
  def __init__(self):       DIM.InfoHandler.__init__(self,self)
  def infoHandler(self):    pass

# DIM command handler
class CommandHandler(DIM.CommandHandler):
  def __init__(self):           DIM.CommandHandler.__init__(self,self)
  def commandHandler(self):     pass

# DIM service handler
class ServiceHandler(DIM.ServiceHandler):
  def __init__(self):           DIM.ServiceHandler.__init__(self,self)
  def serviceHandler(self):     pass

# DIM ClientExitHandler
class ClientExitHandler(DIM.ClientExitHandler):
  def __init__(self):           DIM.ServiceHandler.__init__(self,self)
  def clientExitHandler(self):  pass

# DIM ExitHandler
class ExitHandler(DIM.ExitHandler):
  def __init__(self):           DIM.ExitHandler.__init__(self,self)
  def exitHandler(self):        pass

class TestServer(CommandHandler):
  def __init__(self):
    import time
    CommandHandler.__init__(self)
    self.command = Command('TEST/CMND','C',self)
    self.service = Service('TEST/STRVAL','initial '+time.asctime())
    Server.start('TEST')
    print time.asctime(), 'Dim Server started.'

  def commandHandler(self):
    import time
    cmd = self.command.getString()
    src = Server.getClientName()
    self.service.updateService(cmd+time.asctime())
    print time.asctime(), 'Command received:', '"'+cmd+'"', 'from', "'"+src+'"'

  def run(self):
    import time
    while(1):
      time.sleep(1)

class TestClient:
  def __init__(self):
    import time
    count = 0
    while(1):
      count = count + 1
      s = time.asctime()+' Command '+str(count)
      Client.sendCommand('TEST/CMND',s)
      time.sleep(2)
