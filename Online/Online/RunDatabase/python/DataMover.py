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

class File(dict):
  def __init__(self):
    dict.__init__(self)
  def status(self):
    return self[FILE_STATUS]
  def name(self):
    return self[FILE_NAME]
  def path(self):
    return self[FILE_PATH]
  def unlink(self):
    return 1

class Run(dict):
  def __init__(self):
    dict.__init__(self)
  def runNumber(self):
    return int(self['PARTITIONID'])
  def fillNumber(self):
    return int(self['PARTITIONID'])
  def partition(self):
    return int(self['PARTITIONID'])
  
class RequestDB:
  def __init__(self):
    pass
  def migrateFile(self, logname, path):
    return 1
  def status(self):
    return self[FILE_STATUS]

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
    """ Standard constructor.
    """
    self.runDb = RunDb
    self.reqDb = RequestDb

  #============================================================================
  def _setFileState(self, file, State):
    """ Internal helper to update the state of a file.
    """
    try:      
      return (SUCCESS,)
    except Exception,X:
      return fail('Cannot change file state:'+str(X))

  #============================================================================
  def migrate(self,f):
    """ Migrate file to persistent long term storage.
        This may only be done if the file is CLOSED.
        Files still OPEN or DELETED cannot be migrated.
    """
    fs = f.status()
    if ( fs == FILE_CLOSED ):
      ret = self.reqDb.migrateFile(f.name(),f.path())
      if ( ret ):
        return self._setFileState(f, FILE_MIGRATING)
      return fail('Cannot migrate file:',f.name(),' to longterm storage.')
    elif ( fs == FILE_MIGRATING or fs == FILE_MIGRATED ):
      return (SUCCESS,)
    return fail('Cannot migrate file:',f.name(),' from state:',fs)
      
  #============================================================================
  def delete(self,f):
    """ Delete file from disk.
        This is only possible if the file is in the state WAITDELETE.
        Otherwise it is not allowed to delete the file.
    """
    fs = f.status()
    if ( fs == FILE_WAITDELETE ):
      ret = f.unlink()
      if ( ret ):
        return self._setFileState(f,FILE_DELETED)
      return fail('Cannot delete file:',f.name(),' from disk.')
    elif ( fs == FILE_DELETED ):
      return (SUCCESS,)
    return fail('Cannot delete file:',f.name(),' in state:',fs)

  #============================================================================
  def junk(self,f):
    """ Junk the file.
        As long as the file is not migrating it may be junked.
        The call does nothing if the file is already migrated.
    """
    fs = f.status()
    if ( fs != FILE_MIGRATING ):
      return self._setFileState(f,FILE_WAITDELETE)
    return fail('Cannot junk:',f.name(),' in state:',fs)

  #============================================================================
  def unjunk(self):
    """ Unjunk the file.
        As long as the file is not already deleted (ie. it must exist!)
        it may be unjunked.
    """
    fs = f.status()
    if ( fs != FILE_DELETED ):
      return self._setFileState(f,FILE_CLOSED)
    return fail('Cannot unjunk:',f.name(),' in state:',fs)

  #============================================================================
  def keep(self):
    """ Block file from being deleted.
        As long as the file is not already deleted (ie. it must exist!)
        it may be unjunked.
    """
    fs = f.status()
    if ( fs == FILE_DELETED ):
      return fail('Cannot undelete file:',f.name(),' from state:',fs)
    else:
      return self._setFileState(f, FILE_BLOCKED)

  #============================================================================
  def unblock(self):
    """ Unblock file and allow it to be deleted.
    """
    fs = f.status()
    if ( fs == FILE_DELETED ):
      return fail('Cannot unblock file:'+f.name()+' which is already deleted.')
    elif ( fs == FILE_OPENED ):
      return fail('Cannot unblock file:'+f.name()+' which is not yet closed.')
    elif ( fs == FILE_MIGRATING ):
      return fail('Cannot unblock file:'+f.name()+' which is not yet fully migrating.')
    else:
      return self._setFileState(f, FILE_BLOCKED)
    
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
  def loadFiles(self):
    self.files = []
    f = self.runDb.filesByRun(RunNumber=self.runNumber)
    if ( f[0] == SUCCESS ):
      files = f[1]
      for f in files:
        self.files.append(FileMover(self.runDb, self.reqDb, File(f)))
        
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
    try:
      for f in self.files:
        f.unjunk()
        unjunked.append(f)
      return (SUCCESS,)
    except Exception,X:
      pass
    return fail()

  #============================================================================
  def _setFilesState(self, State):
    try:
      for f in self.files:
        f.setState(State)
      return (SUCCESS,)
    except Exception,X:
      pass

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
              fail('Failed to migrate file ',f.name())
          self.loadFiles()
          missing = 0
          for f in self.files:
            if ( f.status() != FILE_WAITDELETE ):
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
          fs = f.status()
          if ( fs == FILE_WAITDELETE ):
            if ( f.delete()[0] != SUCCESS ):
              fail('Failed to migrate file ',f.name())
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
