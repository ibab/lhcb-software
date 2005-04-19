import PyLCGDict
PyLCGDict.loadDict('DIMDict')
DIM = PyLCGDict.makeNamespace('DIM')
gbl = PyLCGDict.makeNamespace('')

logPrefix     = ''
logHeader     = '+----------------------------------------------------------------------------------------------------'
logTrailer    = logHeader

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

#CmdInfo = gbl.CmndInfo
Info    = gbl.DimInfo

Timer   = gbl.DimTimer
Client  = gbl.DimClient
Server  = gbl.DimServer
Service = gbl.DimService
Command = gbl.DimCommand
Browser = gbl.DimBrowser
Cmnd    = gbl.DimCmnd
UpdatedInfo = gbl.DimUpdatedInfo
StampedInfo = gbl.DimStampedInfo
RpcInfo     = gbl.DimRpcInfo
CurrentInfo = gbl.DimCurrentInfo

# DIM info handler
InfoHandler = DIM.InfoHandler
InfoHandler_init_orig = InfoHandler.__init__
def InfoHandler_init(self):
  InfoHandler_init_orig(self,self)
InfoHandler.__init__          = InfoHandler_init
InfoHandler.InfoHandler       = void_call

# DIM command handler
CommandHandler = DIM.CommandHandler
CommandHandler_init_orig = CommandHandler.__init__
def CommandHandler_init(self):
  self.command = None
  CommandHandler_init_orig(self,self)
CommandHandler.__init__       = CommandHandler_init
CommandHandler.commandHandler = void_call

# DIM service handler
ServiceHandler = DIM.ServiceHandler
ServiceHandler_init_orig = ServiceHandler.__init__
def ServiceHandler_init(self):
  ServiceHandler_init_orig(self,self)
ServiceHandler.__init__       = ServiceHandler_init
ServiceHandler.serviceHandler = void_call

# DIM ClientExithandler
ClientExitHandler = DIM.ClientExitHandler
ClientExitHandler_init_orig = ClientExitHandler.__init__
def ClientExitHandler_init(self):
  ClientExitHandler_init_orig(self,self)
ClientExitHandler.__init__          = ClientExitHandler_init
ClientExitHandler.clientExitHandler = void_call

# DIM Exithandler
ExitHandler = DIM.ExitHandler
ExitHandler_init_orig = ExitHandler.__init__
def ExitHandler_init(self):
  ExitHandler_init_orig(self,self)
ExitHandler.__init__    = ExitHandler_init
ExitHandler.exitHandler = void_call


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
      
