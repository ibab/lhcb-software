import DbCore, DimRPC, RunDatabase

class Server(DimRPC.Server):
  """ DIM RCP Rundatabase server class

      @author  M. Frank
      @version 1.0
  """
  #============================================================================
  def __init__(self):
    """ Standard constructor

        @author  M. Frank
        @version 1.0
    """
    DbCore._debug = 1
    DbCore._ms_access = 1
    RunDatabase._debug  = 1
    db = RunDatabase.RunDatabase("RunDatabase")
    db.optionalWelcome = self.optionalWelcome
    DimRPC.Server.__init__(self,db,'RunDbSrv/RunDb',RunDatabase.fail) 
    db.welcome()
  #============================================================================
  def optionalWelcome(self,fmt,line):
    """ Optional welcome header information

        @author  M. Frank
        @version 1.0
    """
    print fmt%'     D I M - R P C   Server Version'

if __name__ == "__main__":
  Server().run()
  