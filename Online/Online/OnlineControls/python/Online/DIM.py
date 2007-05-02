import os, sys, string, platform
import Online.Utils as Utils
import PyCintex as PyLCGDict

lib_prefix = ''
if platform.system()=='Linux': lib_prefix = 'lib'
PyLCGDict.loadDict(lib_prefix+'DIMDict')
  
DIM = PyLCGDict.makeNamespace('DIM')
gbl = PyLCGDict.makeNamespace('')

#----enable tab completion---------------------------------------------------------------
try:
  import rlcompleter,readline    
  readline.parse_and_bind("tab: complete")
except:
  pass

log = Utils.log
error = Utils.error

def void_call(self): 
  return 1

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
      Client.sendCommandNB('TEST/CMND',s)
      time.sleep(2)
