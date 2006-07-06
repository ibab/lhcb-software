import time, traceback
from RunDatabaseItems import *

_debug  = None
FAILED  = 0
SUCCESS = 1
EXISTS  = 2

#==============================================================================
def fail(*parm):
  s = strtime()+' '
  for p in parm:
    s = s + str(p)
  return (FAILED,s)


class RequestDB:
  def __init__(self):
    pass
  def migrateFile(self, fname):
    return 1
  
#==============================================================================
#
#
#==============================================================================
class FileMover:
  """ FileMover class.

      @author M.Frank
  """

  #============================================================================
  def __init__(self, RunDb, RequestDb, File):
    self.runDb = RunDb
    self.reqDb = RequestDb
    self.file = File
    pass

  #============================================================================
  def migrate(self):
    fs = f.file[FILE_STATUS]
    if ( fs == FILE_CLOSED ):
      ret = self.reqDb.migrateFile(self.file[FILE_NAME])
      if ( ret ):
        return (SUCCESS,)
      return fail('Cannot migrate file:',self.file[FILE_NAME],' to longterm storage.')
    elif ( fs == FILE_MIGRATING or fs == FILE_MIGRATED ):
      return (SUCCESS,)
    return fail('Cannot migrate file:',self.file[FILE_NAME],' from state:',fs)
      
  #============================================================================
  def delete(self):
    return (SUCCESS,)

  #============================================================================
  def junk(self):
    return (SUCCESS,)

  #============================================================================
  def unjunk(self):
    return (SUCCESS,)

  #============================================================================
  def keep(self):
    return (SUCCESS,)

#==============================================================================
#
#
#==============================================================================
class RunMover:
  """ RunMover class.

      @author M.Frank
  """

  #============================================================================
  def __init__(self, RunDb, RequestDb, RunNumber):
    self.runDb = RunDb
    self.reqDb = RequestDb
    self.files = []
    self.runNumber = RunNumber;
    self.run = self._run();
    if ( self.loadFiles() != SUCCESS ):
      fail('Failed to access run:',self.RunNumber)
      
  #============================================================================
  def _run(self, force=None):
    if ( self.run is None ):
      r = self.runDb.run(RunNumber=self.runNumber)
      if ( r[0] == SUCCESS ):
        if ( len(r[1])>0 ):
          self.run = r[1][0]
          return self.run
    return self.run
  
  #============================================================================
  def _setRunState(self, State):
    return (SUCCESS,)

  #============================================================================
  def _setFilesState(self, State):
    unjunked = []
    try:
      for f in self.files:
        f.unjunk()
        unjunked.append(f)
    except Exception,X:
      pass

  #============================================================================
  def loadFiles(self):
    self.files = []
    f = self.runDb.filesByRun(RunNumber=self.runNumber)
    if ( f[0] == SUCCESS ):
      files = f[1]
      for f in files:
        self.files.append(FileMover(self.runDb, self.reqDb, f))

  #============================================================================
  def terminate(self):
    return self._setRunState(RUN_TERMINATED)
  
  #============================================================================
  def migrate(self):
    r = self.runNumber
    if ( self.run ):
      if ( self.run[RUN_STATUS] == RUN_CLOSED ):
        if ( self.run[RUN_MIGRATE] == 1 ):
          for f in self.files:
            if ( f.migrate()[0] != SUCCESS ):
                fail('Failed to migrate file ',f.file[FILE_NAME])
          self.loadFiles()
          missing = 0
          for f in self.files:
            if ( f.file[FILE_STATUS] != FILE_WAITDELETE ):
              missing = missing + 1
          if ( missing == 0 ):
            self._setRunState(RUN_WAITDELETE)
            if ( self.run[RUN_KEEP] == 0 ):
              return self.terminate()
            else:
              return (SUCCESS,)
          elif ( self.run[RUN_STATE] != RUN_MIGRATING ):
            return self._setRunState(RUN_MIGRATING)
          return (SUCCESS,)
        return fail('RunMover:migrate>    Run ',r,' is not supposed to be migrated')
      return   fail('RunMover:migrate>    Run ',r,' is not (yet) closed')
    return     fail('RunMover:migrate>    Run ',r,' is not accessible')

  #============================================================================
  def terminate(self):
    r = self.runNumber
    if ( self._run() ):
      if ( self.run[RUN_STATUS] == RUN_WAITDELETE ):
        if ( self.run[RUN_KEEP] != 1 ):
          fail('RunMover:terminate> Inconsistency: run state is ',self.run[RUN_STATUS],\
               ' though the run should not be migrated!')
          self._run()
        self.loadFiles()
        missing = 0
        deleted = 0
        for f in self.files:
          fs = f.file[FILE_STATUS]
          if ( fs == FILE_WAITDELETE ):
            if ( f.delete()[0] != SUCCESS ):
              fail('Failed to migrate file ',f.file[FILE_NAME])
          if ( fs == FILE_DELETED ):
            deleted = deleted + 1
          if ( not(fs == FILE_WAITDELETE or fs == FILE_DELETED) ):
            missing = missing + 1
        if ( deleted != 0 ):
          fail('RunMover:terminate> Inconsistency: Some files were already deleted for run:',r)
        if ( missing == 0 ):
          return self._setRunState(RUN_TERMINATED)
        return fail('RunMover:terminate> Inconsistency: Cannot terminate run ',r,\
                    'before all files were migrated.')
      return   fail('RunMover:terminate>  Run ',r,' is not (yet) migrated.')
    return     fail('RunMover:terminate>  Run ',r,' is not accessible')

  #============================================================================
  def junk(self, force=None):
    r = self.runNumber
    if ( self._run() ):
      rs = self.run[RUN_STATUS]
      if ( self.run[RUN_KEEP] == 0 and self.run[RUN_MIGRATE] == 0 ):
        return (SUCCESS,)
      elif ( rs == RUN_OPENED ):
        self.runDb.setMigrateRun(r,0)
        return self.runDb.setKeepRun(r,0)
      elif ( rs == RUN_CLOSED ):
        self._setFilesState(FILE_BLOCKED)
        self.runDb.setMigrateRun(r,0)
        return self.runDb.setKeepRun(r,0)
      elif ( rs == RUN_WAITDELETE or rs == RUN_MIGRATING or rs == RUN_TERMINATED ):
        return fail('RunMover:junk> Run ',r,' is ',rs,'. Junking is impossible - it\'s too late.')
    return fail('RunMover:junk>       Run ',r,' is not accessible')

  #============================================================================
  def unjunk(self, force=None):
    r = self.runNumber
    if ( self._run() ):
      rs = self.run[RUN_STATUS]
      if ( self.run[RUN_KEEP] != 0 and self.run[RUN_MIGRATE] != 0 ):
        return (SUCCESS,)
      elif ( rs == RUN_OPENED ):
        self.runDb.setMigrateRun(r,1)
        return self.runDb.setKeepRun(r,1)
      elif ( rs == RUN_CLOSED ):
        self.runDb.setMigrateRun(r,1)
        return self.runDb.setKeepRun(r,1)
      elif ( rs == RUN_WAITDELETE ):
        self._setFilesState(FILE_CLOSED)
        self.runDb.setMigrateRun(r,1)
        return self.runDb.setKeepRun(r,1)
      else:
        return fail('RunMover:unjunk> Run ',r,' is ',rs,'. Unjunking is impossible - it\'s too late.')
    return fail('RunMover:unjunk>     Run ',r,' is not accessible')

#==============================================================================
#
#
#==============================================================================
class DataMover:

  #============================================================================
  def __init__(self, RunDb=None):
    self.runDb = RunDb

  #============================================================================
  def checkRuns(self):
    pass
    
  #============================================================================
  def retrieveFiles(self):
    pass

  #============================================================================
  def sendTransferRequest(self):
    pass

  #============================================================================
  def run(self):
    print 'Data mover is now running.....'
  

#==============================================================================
if __name__ == "__main__":
  mover = DataMover()
  mover.run()
