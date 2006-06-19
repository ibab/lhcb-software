import string, DimRunDb

class Server:
  #============================================================================
  def __init__(self):
    import RunDatabase
    import DbCore
    DbCore._ms_access = 1
    self.fail = RunDatabase.fail
    self.db=RunDatabase.RunDatabase("RunDatabase")
    self.srv=DimRunDb.PyDimRpcCommandServer(self,'_exec','RunDbSrv','RunDb')
    
  #============================================================================
  def _exec(self, s):
    try:
      q = 'self.db.'+s
      #print q
      res=eval(q)
      #print q , ' = ',res
      return str(res)
    except Exception, X:
      return self.fail('Processing Error:',X)
    except:
      return self.fail('Unknown Processing Error.')

  #============================================================================
  def run(self):
    import time
    while ( 1 ):
      time.sleep(10)


class Client:
  """
      Client class

      @author  M.Frank
      @version 1.0
  """

  #============================================================================
  def __init__(self, connection='RunDbSrv/RunDb'):
    """ Default constructor: Takes basic dim-URL"""
    self.object = DimRunDb.PyDimRpcCommandClient(connection)
    self.call = 'doc'
    
  #============================================================================
  def __getattr__(self, name):
    self.call = name
    return self.execute

  #============================================================================
  def execute(self, *parms, **opts):
    """ Magic method dispatcher """
    args = '('
    for a in parms:       args = args + str(a) + ', '
    for a in opts:        args = args + str(a) + '=' + str(opts[a]) + ', '
    if len(args)>1: args = args[:-2]
    args = args+')'
    call = self.call+args
    try:
      #print call
      result = self.object.executeCmd(call)
      if len(result) > 0:
        #print '"'+str(result)+'"'
        return eval(result)
      return (0,'Unknown Processing Error (Bad args ?)',)
    except Exception, X:
      return (0,'Processing Error:'+str(X),)
    except:
      return (0,'Unknown Processing Error.',)

def server():
  Server().run()

def client():
  c = Client()
  c.dump(RunNumber=1)
