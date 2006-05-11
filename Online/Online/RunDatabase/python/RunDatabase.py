import time, traceback, gaudiweb, DbCore

_debug  = None
def _timeStamp():
  stamp = time.strftime("%d-%m-%Y %H:%M.%S",time.localtime())
  return stamp

class RunDb:
    "  bla bla"
    FAILED  = 0
    SUCCESS = 1
    EXISTS  = 2
    def __init__(self):
      pass

tablePrefix       = 'Rundb'
RunsTable         = tablePrefix+'Runs'
RunParamsTable    = tablePrefix+'RunParams'
FilesTable        = tablePrefix+'Files'
FileParamsTable   = tablePrefix+'FileParams'
SequencesTable    = tablePrefix+'Sequences'
RunNumberSequence = tablePrefix+'RunNumbers'
FileIDSequence    = tablePrefix+'FileIDs'

known_run_keys = {'RunNumber'       : 0,
                  'FillNumber'      : 1,
                  'PartitionID'     : 2,
                  'StartDate'       : 3,
                  'EndDate'         : 4,
                  'StartLuminosity' : 5,
                  'EndLuminosity'   : 6,
                  'BeamEnergy'      : 7
                  }

def timetag():
  return time.strftime('%Y-%b-%d %H:%M Exception: ')

def RunDbStatus():
  pass

def getSQL(*parm,**options):
  i = 0
  stmt = ''
  if ( _debug ):
    print 'getSQL:',parm,options
  for o in options:
    i = i + 1
    if ( i == 1 ): stmt = o
    else:          stmt = stmt + " AND " + o
    opt = options[o]
    if type(opt) == type(tuple()):
      for q in opt:
        stmt = stmt + " " + str(q)
    elif type(opt) == type(str()):
      stmt = stmt + "='" + str(opt)+"'"
    else:
      stmt = stmt + "=" + str(opt)
  return stmt

def _getOption(name, default, pre=None, post=None, *parms, **opts):
  try:
    if ( _debug ):
      print parms, opts
    opt = opts['opts'][name]
    if ( pre and opt is not None ):
      opt = pre + str(opt)
    if ( post and opt is not None ):
      opt = str(opt) + post
    return opt
  except KeyError, X:
    return default

class RunDatabaseException(Exception):
  def __init__(self, msg):
    Exception.__init__(self, msg)
  def __str__(self):
    return 'RunDatabaseException: '+self.args[0]

class RunDatabase(gaudiweb.Service):
  def __init__(self, login, name="RunDatabase"):
    self.login = login
    self.core  = DbCore.DbCore(self.login)
    gaudiweb.Service.__init__(self,name)

  def db(self):
    return DbCore.DbCore(self.core)

  def _exec(self, stmt, OnErrorContinue, Print=0):
    """ Simple wrapper to execute a SQL database statement.
    """
    try:
      result = self.db().execute(stmt)
    except Exception, X:
      if ( not OnErrorContinue ):
        raise RunDatabaseException('Failed to execute statement '+stmt+'\n'+str(X))
      if ( Print ):
        print _timeStamp(), 'Failed to execute statement '+stmt+'\n'+str(X)
      return (RunDb.FAILED,str(X))
    return (RunDb.SUCCESS,'')

  def createSequence(self, name, OnErrorContinue):
    try:
      stmt = "INSERT INTO "+SequencesTable+" (Name,Entry) VALUES ('"+name+"',0)"
      result = self._exec(stmt, OnErrorContinue)
      if ( result[0] != RunDb.SUCCESS ):
        return (RunDb.FAILED,'Cannot create sequence: '+name+'\n'+result[1])
      return (RunDb.SUCCESS,'')
    except KeyError, X:
      return (RunDb.FAILED,str(X))
    except Exception, X:
      return (RunDb.FAILED,str(X))
    return (RunDb.FAILED,'')
    
  def nextSequenceID(self, name):
    try:
      stmt = 'UPDATE '+SequencesTable+' s '\
             'SET s.Name=s.Name, s.Entry=s.Entry+1 '\
             'WHERE s.Name=\''+name+'\''
      result = self._exec(stmt,0)
      if ( result[0] != RunDb.SUCCESS ):
        return (RunDb.FAILED,'Cannot get next sequence ID: '+name+'\n'+result[1])
      stmt = "SELECT s.Entry FROM "+SequencesTable+" s WHERE s.Name='"+name+"'"
      cur = DbCore.Cursor(self.db()).select(stmt)
      if ( cur.next().isSuccess() ):
        return (RunDb.SUCCESS,cur.result()[0])
      return (RunDb.FAILED,'Cannot access sequence ID: '+name)
    except Exception, X:
      return (RunDb.FAILED,str(X))
    return (RunDb.FAILED,'')

  def nextFileID(self):
    return self.nextSequenceID(tablePrefix+'FileIDs')

  def nextRunNumber(self):
    return self.nextSequenceID(tablePrefix+'RunNumbers')
  
  #============================================================================
  def uninstall(self, OnErrorContinue=1):
    """ Uninstall the Run database. All basic tables and indices are dropped.

        @author M.Frank
    """
    self._exec('DROP TABLE ' + RunParamsTable,OnErrorContinue)
    self._exec('DROP TABLE ' + FileParamsTable,OnErrorContinue)
    self._exec('DROP TABLE ' + FilesTable,OnErrorContinue)
    self._exec('DROP TABLE ' + RunsTable,OnErrorContinue)
    self._exec('DROP TABLE ' + SequencesTable,OnErrorContinue)
    if ( not OnErrorContinue ):
      return (RunDb.SUCCESS,'Installation succeeded.')
    return (RunDb.SUCCESS,'Installation finished (need to check printout)')

  def _make(self, stmt,OnErrorContinue):
    self._exec(stmt,1,1)

  #============================================================================
  def install(self, OnErrorContinue=1):
    """ Install Run database from scratch. Only the basic tables and keys are defined.
        No attempt here is taken to optimize the data access in any way.

        @author M.Frank
    """
    make = self._make
    # Create Runs table
    make('CREATE TABLE '+SequencesTable+'(Name VARCHAR(32), Entry INTEGER)',OnErrorContinue)
    self.createSequence(FileIDSequence,OnErrorContinue)
    self.createSequence(RunNumberSequence,OnErrorContinue)
    
    make('ALTER TABLE  '+SequencesTable+' ADD PRIMARY KEY (Name)',OnErrorContinue)
    # Create Runs table
    make('CREATE TABLE '+RunsTable+"""
           (RunNumber   INTEGER,
            FillNumber  INTEGER,
            PartitionID INTEGER,
            StartDate   VARCHAR,
            EndDate     VARCHAR,
            StartLumi   REAL,
            EndLumi     REAL,
            BeamEnergy  REAL)""",OnErrorContinue)
    make('CREATE UNIQUE INDEX PK_Runs ON '+RunsTable+' (RunNumber) WITH PRIMARY',OnErrorContinue)
    #  Create run parameters table
    make('CREATE TABLE '+RunParamsTable+"""
           (RunNumber INTEGER     NOT NULL,
            Name      VARCHAR(32) NOT NULL,
            Val       VARCHAR(255),
            Typ       VARCHAR(32))""",OnErrorContinue)
    make('CREATE UNIQUE INDEX PK_RunParams ON '+RunParamsTable+' (RunNumber,Name) WITH PRIMARY',OnErrorContinue)
    make('ALTER TABLE  '+RunParamsTable+' ADD FOREIGN KEY (RunNumber) REFERENCES '+RunsTable+' (RunNumber)',OnErrorContinue)
    # Create Files table
    make('CREATE TABLE '+FilesTable+"""
           (FileID      INTEGER NOT NULL,
            RunNumber   INTEGER NOT NULL,
            Name        VARCHAR(255) NOT NULL,
            Stream      VARCHAR(128))""",OnErrorContinue)
    make('CREATE UNIQUE INDEX PK_Files ON '+FilesTable+' (FileID) WITH PRIMARY',OnErrorContinue)
    make('CREATE UNIQUE INDEX I_Files_RunFname ON '+FilesTable+' (RunNumber,Name)',OnErrorContinue)
    make('ALTER TABLE  '+FilesTable+' ADD FOREIGN KEY (RunNumber) REFERENCES '+RunsTable+' (RunNumber)',OnErrorContinue)
    #  Create file parameters table
    make('CREATE TABLE '+FileParamsTable+"""
           (FileID INTEGER   NOT NULL,
            Name VARCHAR(32) NOT NULL,
            Val VARCHAR(255),
            Typ VARCHAR(32))""",OnErrorContinue)
    make('CREATE UNIQUE INDEX PK_FileParams ON '+FileParamsTable+' (FileID,Name) WITH PRIMARY',OnErrorContinue)
    make('ALTER TABLE '+FileParamsTable+' ADD FOREIGN KEY (FileID) REFERENCES '+FilesTable+' (FileID)',OnErrorContinue)
    
    if ( not OnErrorContinue ):
      return (RunDb.SUCCESS,'Installation succeeded.')
    return (RunDb.SUCCESS,'Installation finished (need to check printout)')

  #============================================================================
  def runs(self, **options):
    try:
      opt = getSQL(**options)
      stmt = "SELECT RunNumber,FillNumber,PartitionID,StartDate,EndDate,StartLumi,EndLumi,BeamEnergy \
              FROM "+RunsTable+" WHERE "+opt
      #print stmt
      cur = DbCore.Cursor(self.db()).select(stmt)
      res = []
      while ( cur.next().isSuccess() ):
        res.append(cur.result())
      return (RunDb.SUCCESS,res)
    except KeyError, X:
      return (RunDb.FAILED,str(X))
    except Exception, X:
      return (RunDb.FAILED,str(X))
    return (RunDb.FAILED,'')

  #============================================================================
  def existsRun(self, **options):
    try:
      stmt = 'SELECT   r.RunNumber FROM '+RunsTable+' r WHERE '
      for o in options:
        stmt = stmt + str(o) + '=' + str(options[o]) + ' AND '
      stmt = stmt[:len(stmt)-5]
      cur = DbCore.Cursor(self.db()).select(stmt)
      if ( cur.next().isSuccess() ):
        return (RunDb.SUCCESS,)
      return (RunDb.FAILED,'Unknown run number')
    except KeyError, X:
      return (RunDb.FAILED,str(X))
    except Exception, X:
      return (RunDb.FAILED,str(X))
    return (RunDb.FAILED,'')

  def sqlCreateRunParam(self, run, name, value, typ='PARAM'):
    stmt = "INSERT INTO "+RunParamsTable+" (RunNumber, Name, Val, Typ) \
            VALUES ("+str(run)+",'"+name+"','"+str(value)+"','"+typ+"')"
    return self._exec(stmt,1)

  #============================================================================
  def deleteFiles(self,RunNumber):
    """ Delete all files corresponding to a given run number.

        @author M.Frank
    """
    stmt = "DELETE FROM "+FileParamsTable+" p \
            WHERE p.FileID IN \
              (SELECT f.FileID FROM "+FilesTable+" f \
               WHERE f.RunNumber="+str(RunNumber)+")"
    result = self._exec(stmt,1)
    if ( result[0] != RunDb.SUCCESS ):
      return (RunDb.FAILED,'Cannot delete file parameters for run: '+str(RunNumber)+'\n'+result[1])
    stmt = "DELETE FROM "+FilesTable+" f  WHERE f.RunNumber="+str(RunNumber)
    result = self._exec(stmt,1)
    if ( result[0] != RunDb.SUCCESS ):
      return (RunDb.FAILED,'Cannot delete file parameters for run: '+str(RunNumber)+'\n'+result[1])  
    return (RunDb.SUCCESS,)

  #============================================================================
  def deleteRun(self, RunNumber):
    """ Delete a complete run from the database including all dependent table entries.

        @author M.Frank
    """
    err    = 'Cannot delete run: '+str(RunNumber)+'\n'
    result = self.deleteFiles(RunNumber=RunNumber)
    if ( result[0] != RunDb.SUCCESS ):
      return (RunDb.FAILED,err+result[1])
    result = self.deleteRunParams(RunNumber=RunNumber)
    if ( result[0] != RunDb.SUCCESS ):
      return (RunDb.FAILED,err+result[1])
    stmt = "DELETE FROM "+RunsTable+" WHERE RunNumber="+str(RunNumber)
    result = self._exec(stmt,1)
    if ( result[0] != RunDb.SUCCESS ):
      return (RunDb.SUCCESS,err+result[1])
    return (RunDb.SUCCESS,)

  #============================================================================
  def createRun(self, **options):
    """ Add entry to the Runs table. The required arguments are:
        RunNumber
        FillNumber
        PartitionID
        StartDate
        StartLumi
        BeamEnergy
        The arguments must be passed in the keyword - value semantic of python:
        key=<value>,...
    """
    try:
      run_number = options['RunNumber']
      s = 'Run number '+str(run_number)
      result = self.existsRun(RunNumber=run_number)
      if(result[0] == RunDb.SUCCESS):
        return (RunDb.EXISTS, s+' exists already.')
      run   = options['RunNumber']
      fill  = options['FillNumber']
      pid   = options['PartitionID']
      start = options['StartDate']
      lumi  = options['StartLuminosity']
      ene   = options['BeamEnergy']
      vals  = "VALUES (%d, %d, %d, '%s', '%s', %f, %f, %f)"%(run,fill,pid,start,start,lumi,lumi,ene)
      stmt = "INSERT INTO "+RunsTable+" (RunNumber,FillNumber,PartitionID,StartDate,EndDate,StartLumi,EndLumi,BeamEnergy) "+vals
      result = self._exec(stmt,0)
      if ( result[0] != RunDb.SUCCESS ):
        return (0,'Failed to create Run '+str(run))
      # Cleanup parameters and save optional ones as RunParameters
      opts = options
      del opts['RunNumber']
      del opts['FillNumber']
      del opts['PartitionID']
      del opts['StartDate']
      if opts.has_key('EndDate'): del opts['EndDate']
      del opts['StartLuminosity']
      if opts.has_key('EndLumi'): del opts['EndLumi']
      del opts['BeamEnergy']
      for k in opts:
        result = self.sqlCreateRunParam(run, k, str(options[k]))
        if(result[0] != RunDb.SUCCESS):
          return (RunDb.FAILED,'Cannot insert run parameter:'+k+'='+str(options[k])+' for run '+str(run)+'\n'+result[1])
      return (RunDb.SUCCESS,)
    except KeyError, X:
      return (RunDb.FAILED,'[Insufficient arguments supplied] '+str(X))
    except Exception, X:
      return (RunDb.FAILED,str(X))
    return (RunDb.FAILED,'[Unknown Error]')

  #============================================================================
  def modifyRun(self, RunNumber, **options):
    """ Add a new parameter to an existing run.
        The run is identified by its run number.
        The parameter is identified by its name,
        the value and the type are the parameter's data

        @author M.Frank
    """
    stmt = "UPDATE "+RunsTable+" SET "
    for o in options:
      stmt = stmt + o + "='" + str(options[o]) + "', "
    stmt = stmt[:-2]
    stmt = stmt + " WHERE RunNumber="+str(RunNumber)
    result = self._exec(stmt,1)
    if(result[0] != RunDb.SUCCESS):
      return (RunDb.FAILED,'Cannot modify run:'+str(RunNumber)+'\n'+result[1])
    return (RunDb.SUCCESS,)

  #============================================================================
  def deleteRunParams(self,RunNumber):
    """ Delete all parameters of a given run.
        The run is identified by its run number.

        @author M.Frank
    """
    stmt = "DELETE FROM "+RunParamsTable+" WHERE RunNumber="+str(RunNumber)
    result = self._exec(stmt,1)
    if ( result[0] != RunDb.SUCCESS ):
      return (RunDb.FAILED,'Cannot delete run: '+str(RunNumber)+'\n'+result[1])
    return (RunDb.SUCCESS,)
      
  #============================================================================
  def runParams(self, RunNumber, **options):
    """ Retrieve all parameters of a given run.
        The run is identified by its run number.
        The parameters are returned in a tuple of
        tuples: (status, ((name,value,type),...))

        @author M.Frank
    """
    try:
      res = []
      opt = getSQL(**options)
      stmt = "SELECT RunNumber, Name, Val, Typ from "+RunParamsTable+" WHERE RunNumber="+str(RunNumber)
      if ( len(opt) > 0 ):
        stmt = stmt + " AND " + opt
      cur = DbCore.Cursor(self.db()).select(stmt)
      while ( cur.next().isSuccess() ):
        res.append(cur.result())
      if ( len(res) == 0 ):
        res = self.existsRun(RunNumber=RunNumber)
        if ( res[0] == RunDb.SUCCESS ):
          return (RunDb.SUCCESS,[])
        return (RunDb.FAILED,'[No such run]: Run '+str(RunNumber)+' does not exist!')
      return (RunDb.SUCCESS,res)
    except Exception, X:
      return (RunDb.FAILED,str(X))
    return (RunDb.FAILED,'')

  #============================================================================
  def addRunParam(self, RunNumber, Name, Val, Typ='PARAM'):
    """ Add a new parameter to an existing run.
        The run is identified by its run number.
        The parameter is identified by its name,
        the value and the type are the parameter's data

        @author M.Frank
    """
    result = self.sqlCreateRunParam(RunNumber, Name, Val, Typ)
    if(result[0] != RunDb.SUCCESS):
      return (RunDb.FAILED,'Cannot insert run parameter:'+Name+'='+str(Val)+\
                           ' for run '+str(RunNumber)+'\n'+result[1])
    return (RunDb.SUCCESS,)

  #============================================================================
  def modifyRunParam(self, RunNumber, Name, **options):
    """ Add a new parameter to an existing run.
        The run is identified by its run number.
        The parameter is identified by its name,
        the value and the type are the parameter's data

        @author M.Frank
    """
    stmt = "UPDATE "+RunParamsTable+" SET "
    for o in options:
      stmt = stmt + o + "='" + str(options[o]) + "', "
    stmt = stmt[:-2]
    stmt = stmt + " WHERE RunNumber="+str(RunNumber)+" AND Name='"+Name+"'"
    result = self._exec(stmt,1)
    if(result[0] != RunDb.SUCCESS):
      return (RunDb.FAILED,'Cannot modify run parameter:'+Name+' for run '+str(RunNumber)+'\n'+result[1])
    return (RunDb.SUCCESS,)

  #============================================================================
  def deleteRunParam(self, RunNumber, Name):
    """ Remove a parameter to an existing run.
        The run is identified by its run number.
        The parameter is identified by its name.
        No check is performed if this parameter actually
        exists in the context of the specified run.
        @author M.Frank
    """
    stmt = "DELETE FROM     "+RunParamsTable+" \
            WHERE RunNumber="+str(RunNumber)+" \
            AND   Name='"    +Name+"'"
    result = self._exec(stmt,1)
    if(result[0] != RunDb.SUCCESS):
      return (RunDb.FAILED,'Cannot remove run parameter:'+Name+' for run '+str(RunNumber)+'\n'+result[1])
    return (RunDb.SUCCESS,)

  #============================================================================
  def runParam(self, RunNumber, Name):
    """ Access a parameter of an existing run.
        The run is identified by its run number.
        The parameter is identified by its name.

        @author M.Frank
    """
    try:
      stmt = "SELECT Name,Val,Typ from "+RunParamsTable+" \
              WHERE RunNumber="+str(RunNumber)+" \
              AND Name='"+Name+"'"
      res = []
      cur = DbCore.Cursor(self.db()).select(stmt)
      while ( cur.next().isSuccess() ):
        res.append(cur.result())
      if ( len(res) == 1 ):
        return (RunDb.SUCCESS,res[0])
      elif ( len(res) == 0 ):
        return (RunDb.FAILED,'Run parameter:'+Name+' for run '+str(RunNumber)+' does not exist!')
      elif ( len(res) > 1 ):
        return (RunDb.FAILED,'[Internal Error] Multiple entries for run parameter:'+Name+\
                             ' [run:'+str(RunNumber)+'] found!')
    except Exception, X:
      return (RunDb.FAILED,str(X))
    return (RunDb.FAILED,'[Internal Error]')

  #============================================================================
  def files(self, **options):
    """ Access files according to some selection criteria.

        Calling sequence:
        files(<CriteriaName>=<Value>,[<CriteriaName>=<Value>]...)

        Returns:
        Tuple containing status code and result (or error description)
        
        @author M.Frank
    """
    try:
      stmt = "SELECT FileID, RunNumber, Name, Stream FROM "+FilesTable+" WHERE "+getSQL(**options)
      if ( len(options) > 0 ):
        res = []
        cur = DbCore.Cursor(self.db()).select(stmt)
        while ( cur.next().isSuccess() ):
          res.append(cur.result())
        return (RunDb.SUCCESS,res)
      return (RunDb.FAILED,'[Invalid arguments] You need to specify some selection criteria!')
    except Exception, X:
      return (RunDb.FAILED,str(X))
    return (RunDb.FAILED,'[Internal Error]')
    
  #============================================================================
  def addFile(self, RunNumber, Name, Stream, **options):
    """ Add a new file to an existing run.
        The run is identified by its run number.
        The file is identified by its name and is
        additionally characterized by its stream.
        Optionally a set of named options may be passed,
        which will be used to directly populate the
        file parameters table

        Calling sequence:
        addFile(<RunNumber>,<file-name>,<stream-name>,<opt-name>=<opt-value>,[<opt-name>=<opt-value>],...)

        Returns:
        Tuple containing status code and result (or error description)

        @author M.Frank
    """
    fid = self.nextFileID()
    run = str(RunNumber)
    if ( fid[0] == RunDb.SUCCESS ):
      fid = str(fid[1])
      stmt = "INSERT INTO "+FilesTable+" (FileID, RunNumber, Name, Stream) \
              VALUES ("+fid+","+run+",'"+Name+"','"+Stream+"')"
      result = self._exec(stmt,1)
      if ( result[0] != RunDb.SUCCESS ):
        return (RunDb.FAILED,'Cannot add file '+Name+' for run: '+run+'\n'+result[1])
      for k in options:
        result = self.addFileParam(FileID=fid, Name=k, Val=str(options[k]), Typ='PARAM')
        if(result[0] != RunDb.SUCCESS):
          return (RunDb.FAILED,'Cannot insert run parameter:'+k+'='+str(options[k])+\
                               ' for run '+run+'\n'+result[1])
      return (RunDb.SUCCESS,)
    return (RunDb.FAILED,'Cannot add file '+Name+' for run: '+str(RunNumber)+'\n'+fid[1])

  #============================================================================
  def removeFile(self, RunNumber, **options):
    """ Add a new file to an existing run.
        The run is identified by its run number.
        The file is identified by its name and is
        additionally characterized by its stream.
        Optionally a set of named options may be passed,
        which will be used to directly populate the
        file parameters table

        @author M.Frank
    """
    run = str(RunNumber)
    if ( options.has_key('Name') ):
      opt = getSQL(**options)
      stmt = "SELECT FileID FROM "+FilesTable+" WHERE RunNumber="+run
      if ( len(opt) > 0 ):
        stmt = stmt + " AND " + opt
      cur = DbCore.Cursor(self.db()).select(stmt)
      fids = ''
      while ( cur.next().isSuccess() ):
        fids = fids + str(cur.result()[0]) + ', '
      if ( len(fids) > 0 ):
        fids = fids[:-2]
        stm = "DELETE FROM "+FileParamsTable+" WHERE FileID IN ("+fids+")"
        result = self._exec(stmt,1)
        if(result[0] != RunDb.SUCCESS):
          return (RunDb.FAILED,'Cannot delete file parameters for file IDs='+fids)
        stmt = "DELETE FROM "+FilesTable+" WHERE RunNumber="+run
        result = self._exec(stmt,1)
        if(result[0] != RunDb.SUCCESS):
          return (RunDb.FAILED,'Cannot delete files with IDs='+fids)
      return (RunDb.SUCCESS,)
    return (RunDb.FAILED,'Cannot remove file. At least the file name must be specified.')

  #============================================================================
  def fileParams(self, **options):
    """ 
    
        @author M.Frank
    """
    try:
      fid = ''
      stmt = ''
      if ( options.has_key('FileID') ):
        opt = getSQL(FileID=options['FileID'])
        stmt = str(opt)
      elif ( options.has_key('File') ):        
        fileopts = options['File']
        if ( fileopts.has_key('FileID') ):
          fid = str(fileopts['FileID'])
        else:
          stmt = "SELECT DISTINCT FileID from "+FilesTable+" WHERE "+getSQL(**fileopts)
          cur = DbCore.Cursor(self.db()).select(stmt)
          while ( cur.next().isSuccess() ):
            fid = fid + str(cur.result()[0]) + ', '
          if ( len(fid) == 0 ):
            return (RunDb.FAILED,'Cannot access file parameters.\n')
          fid = fid[:-2]
        stmt = ' FileID IN ('+fid+')'
      if ( options.has_key('Param') ):
        paropt = options['Param']
        par = getSQL(**paropt)
        if ( len(par) > 0 ):
          if ( len(stmt) > 0 ): stmt = stmt + " AND " + par
          else:                 stmt = par
      stm = "SELECT FileID,Name,Val,Typ FROM "+FileParamsTable+" WHERE "+stmt
      #print stm
      res = []
      cur = DbCore.Cursor(self.db()).select(stm)
      while ( cur.next().isSuccess() ):
        res.append(cur.result())
      return (RunDb.SUCCESS,res)
    except KeyError, X:
      return (RunDb.FAILED,'[Insufficient arguments supplied]: '+str(options)+'\n'+str(X))
    except Exception, X:
      return (RunDb.FAILED,'[Internasl Error] '+str(X))
    
  #============================================================================
  def addFileParam(self, **options):
    """ Add a new parameter to the FileParameters table.
    
        @author M.Frank
    """
    stmt = ""
    try:
      name = options['Name']
      val  = str(options['Val'])
      typ  = options['Typ']
      fid = None
      if ( options.has_key('FileID') ):
        fid = options['FileID']
      else:
        stmt = "SELECT f.FileID from "+FilesTable+" f \
                WHERE f.Name='"+options['File']+"' \
                AND f.RunNumber="+str(options['RunNumber'])
        cur = DbCore.Cursor(self.db()).select(stmt)
        if ( cur.next().isSuccess() ):
          fid = str(cur.result()[0])
        else:
          return (RunDb.FAILED,'Cannot add file Parameter '+name+\
                               ' [File does not exist]\n'+result[1])
      stmt = "INSERT INTO "+FileParamsTable+" (FileID, Name, Val, Typ) \
              VALUES ("+fid+",'"+name+"','"+val+"','"+typ+"')"
      result = self._exec(stmt,1)
      if ( result[0] != RunDb.SUCCESS ):
        return (RunDb.FAILED,'Cannot add file Parameter '+name+'\n'+result[1])
      return (RunDb.SUCCESS,)
    except KeyError, X:
      return (RunDb.FAILED,'[Insufficient arguments supplied]: '+str(options)+'\n'+str(X))
    except Exception, X:
      return (RunDb.FAILED,'[Internasl Error] '+str(X))
    
  #============================================================================
  def removeFileParam(self, **options):
    """ Add a new parameter to the FileParameters table.
    
        @author M.Frank
    """
    stmt = ""
    try:
      name = options['Name']
      fid = None
      if ( options.has_key('FileID') ):
        fid = str(options['FileID'])
      else:
        stmt = "SELECT f.FileID from "+FilesTable+" f \
                WHERE f.Name='"+options['File']+"' \
                AND f.RunNumber="+str(options['RunNumber'])
        cur = DbCore.Cursor(self.db()).select(stmt)
        if ( cur.next().isSuccess() ):
          fid = str(cur.result()[0])
        else:
          return (RunDb.FAILED,'Cannot add file Parameter '+name+\
                               ' [File does not exist]\n'+result[1])
      stmt = "DELETE FROM "+FileParamsTable+" WHERE Name='"+Name+" AND FileID="+fid
      result = self._exec(stmt,1)
      if ( result[0] != RunDb.SUCCESS ):
        return (RunDb.FAILED,'Cannot add file Parameter '+name+'\n'+result[1])
      return (RunDb.SUCCESS,)
    except KeyError, X:
      return (RunDb.FAILED,'[Insufficient arguments supplied]: '+str(options)+'\n'+str(X))
    except Exception, X:
      return (RunDb.FAILED,'[Internasl Error] '+str(X))

  #============================================================================
  def dump(self,**options):
    """ Dump database content.

        @author M.Frank
    """    
    try:
      result = ''
      opt = getSQL(**options)
      if ( len(opt) == 0 ): opt = "RunNumber>0"
      stmt = "SELECT RunNumber,FillNumber,PartitionID,StartDate,EndDate,StartLumi,EndLumi,BeamEnergy \
              FROM "+RunsTable+" WHERE "+opt
      cur = DbCore.Cursor(self.db()).select(stmt)
      result = result + '\n\n\n%-8s %-8s %-4s %-24s %-24s %11s %11s %11s\n'%('Run','Fill','PID','Start Date','End Date','Lumi(Start)','Lumi(End)','Energy')
      while ( cur.next().isSuccess() ):
        (RunNumber,FillNumber,PartitionID,StartDate,EndDate,StartLumi,EndLumi,BeamEnergy) = cur.result()
        result = result + '%-8d %-8d %-4d %-24s %-24s %11.2f %11.2f %11.2f\n'%(RunNumber,FillNumber,PartitionID,StartDate,EndDate,StartLumi,EndLumi,BeamEnergy)
        pars = self.runParams(RunNumber)
        if ( pars[0] == RunDb.SUCCESS ):
          context = 'RunParameters:'
          pars = pars[1]
          for p in pars:
            (ID,Name,Value,Type) = p
            result = result + '   %-19s %-24s %-s\n'%(context,Name+'['+Type+']',Value)
            context = ''
        files = self.files(RunNumber=RunNumber,Name=("LIKE","'%'",))
        if ( files[0] != RunDb.SUCCESS ):
          result = result + 'Failed to retrieve files for run:'+str(RunNumber)+' Err='+str(files[1])+'\n'
        else:
          files = files[1]
          context = 'Files:'
          for f in files:
            (ID,Run,Name,Stream) = f
            result = result + '   %-19s %-8d %-50s %-15s\n'%(context, ID, '"'+Name+'"','"'+Stream+'"')
            pars = self.fileParams(File={'RunNumber':RunNumber,'Name':Name},Param={'Name':('LIKE',"'%'")})
            if ( pars[0] != RunDb.SUCCESS ):
              result = result + 'Failed to retrieve fileparameters for run:'+str(RunNumber)+' File:'+Name+' Err='+str(pars[1])+'\n'
            else:
              pars = pars[1]
              context = 'Parameters:'
              for p in pars:
                (FID,Name,Value,Type) = p
                result = result + '   %25s %-16s %-24s = %-15s\n'%('',context,Name+'['+Type+']',Value)
                context = ''
          if ( context == '' ): result = result + '\n'
          context = ''
      result = result + '\n\n\n'
      print result
      return (RunDb.SUCCESS,result)
    except Exception, X:
      result = result + 'Exception:\n'%str(X)
      return (RunDb.FAILED,str(X))
    return (RunDb.FAILED,'')

def install():
  res = RunDatabase("RunDatabase").install(1)
  print 'db.install() returned:', str(res)
  
def uninstall():
  res = RunDatabase("RunDatabase").uninstall(1)
  print 'db.uninstall() returned:', str(res)

