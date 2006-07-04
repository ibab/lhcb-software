import time, traceback, DbCore

_debug  = None
FAILED  = 0
SUCCESS = 1
EXISTS  = 2

tablePrefix       = 'Rundb'
RunsTable         = tablePrefix+'Runs'
RunParamsTable    = tablePrefix+'RunParams'
FilesTable        = tablePrefix+'Files'
FileParamsTable   = tablePrefix+'FileParams'
SequencesTable    = tablePrefix+'Sequences'
RunNumberSequence = tablePrefix+'RunNumbers'
FileIDSequence    = tablePrefix+'FileIDs'

#==============================================================================
def db_time(t):
  stamp = time.strftime("%d-%m-%Y %H:%M.%S",time.gmtime(t))
  return stamp  

#==============================================================================
def strtime():
  stamp = time.strftime("%d-%m-%Y %H:%M.%S",time.gmtime())
  return stamp

#==============================================================================
def fail(*parm):
  s = strtime()+' '
  for p in parm:
    s = s + str(p)
  return (FAILED,s)

#==============================================================================
class RunDatabaseException(Exception):
  """ Database exception class.

      @author M.Frank
  """
  def __init__(self, msg):
    Exception.__init__(self, msg)
  def __str__(self):
    return 'RunDatabaseException: '+self.args[0]

#==============================================================================
class RunDatabase:
  """ Implementation of the run database object.
      All run database member functions return a tuple with the following meaning:

      result[0]: Status of the action (1=SUCCESS, 0=FAILURE)
      result[1]: On Error:   optional error message
                 On Success: result from the database query.

      @author M.Frank
  """

  #============================================================================
  def __init__(self, login):
    """ Constructor

        @author M.Frank
    """
    self.login = login
    self.core  = DbCore.DbCore(self.login)

  #============================================================================
  def db(self):
    return DbCore.DbCore(self.core)

  #============================================================================
  def _make(self, stmt,OnErrorContinue):
    self._exec(stmt,1,1)

  #============================================================================
  def _exec(self, stmt, OnErrorContinue, Print=0):
    """ Simple wrapper to execute a SQL database statement.

        @author M.Frank
    """
    try:
      result = self.db().execute(stmt)
    except Exception, X:
      if ( not OnErrorContinue ):
        raise RunDatabaseException('Failed to execute statement '+stmt,X)
      if ( Print ):
        print fail('Failed to execute statement ',stmt,'\n',str(X))
      return fail(X)
    return (SUCCESS,'')

  #============================================================================
  def createSequence(self, name, OnErrorContinue):
    try:
      stmt = "INSERT INTO "+SequencesTable+" (Name,Entry) VALUES ('"+name+"',0)"
      result = self._exec(stmt, OnErrorContinue)
      if ( result[0] != SUCCESS ):
        return fail('Cannot create sequence: ',name,'\n',result[1])
      return (SUCCESS,'')
    except KeyError, X:
      return fail(X)
    except Exception, X:
      return fail(X)
    return fail('Unknown error')
    
  #============================================================================
  def nextSequenceID(self, name):
    """ Retrieve the next available identifier of a named sequence. 
        The sequence is identified by its name.        

        @author M.Frank
    """
    try:
      stmt = 'UPDATE '+SequencesTable+' s '\
             'SET s.Name=s.Name, s.Entry=s.Entry+1 '\
             'WHERE s.Name=\''+name+'\''
      result = self._exec(stmt,0)
      if ( result[0] != SUCCESS ):
        return fail('Cannot get next sequence ID: '+name+'\n'+result[1])
      stmt = "SELECT s.Entry FROM "+SequencesTable+" s WHERE s.Name='"+name+"'"
      cur = DbCore.Cursor(self.db()).select(stmt)
      if ( cur.next().isSuccess() ):
        return (SUCCESS,cur.result()[0])
      return fail(strtime()+' Cannot access sequence ID: '+name)
    except Exception, X:
      return fail(X)
    return fail('Unknown error')

  #============================================================================
  def nextFileID(self):
    """ Retrieve the next available file identifier. 

        @author M.Frank
    """
    return self.nextSequenceID(FileIDSequence)

  #============================================================================
  def nextRunNumber(self):
    """ Retrieve the next available run identifier. 

        @author M.Frank
    """
    return self.nextSequenceID(RunNumberSequence)
  
  #============================================================================
  def runs(self, **options):
    """ Retrieve all runs according to SQL restrictions defined in the 
        options dictionary.

        @author M.Frank
    """
    try:
      opt = DbCore.sqlOpts(**options)
      stmt = "SELECT RunNumber,FillNumber,Partition,Activity,StartDate,EndDate,ProgramName,ProgramVersion,IntegratedLumi \
              FROM "+RunsTable+" WHERE "+opt
      cur = DbCore.Cursor(self.db()).select(stmt)
      res = []
      while ( cur.next().isSuccess() ):
        res.append(cur.result())
      return (SUCCESS,res)
    except KeyError, X:
      return fail(X)
    except Exception, X:
      return fail(X)
    return fail('Unknown error')

  #============================================================================
  def existsRun(self, **options):
    """ Check the existence of a given run according to SQL restrictions 
        defined in the options dictionary.

        @author M.Frank
    """
    try:
      stmt = 'SELECT   r.RunNumber FROM '+RunsTable+' r WHERE '
      for o in options:
        stmt = stmt + str(o) + '=' + str(options[o]) + ' AND '
      stmt = stmt[:len(stmt)-5]
      cur = DbCore.Cursor(self.db()).select(stmt)
      if ( cur.next().isSuccess() ):
        return (SUCCESS,)
      return fail('Unknown run number')
    except KeyError, X:
      return fail(X)
    except Exception, X:
      return fail(X)
    return fail('Unknown error')

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
    if ( result[0] != SUCCESS ):
      return fail('Cannot delete file parameters for run: ',RunNumber,'\n',result[1])
    stmt = "DELETE FROM "+FilesTable+" f  WHERE f.RunNumber="+str(RunNumber)
    result = self._exec(stmt,1)
    if ( result[0] != SUCCESS ):
      return fail('Cannot delete file parameters for run: ',RunNumber,'\n',result[1])  
    return (SUCCESS,)

  #============================================================================
  def deleteRun(self, RunNumber):
    """ Delete a complete run from the database including all dependent table entries.

        @author M.Frank
    """
    err    = 'Cannot delete run: '+str(RunNumber)+'\n'
    result = self.deleteFiles(RunNumber=RunNumber)
    if ( result[0] != SUCCESS ):
      return fail(err,result[1])
    result = self.deleteRunParams(RunNumber=RunNumber)
    if ( result[0] != SUCCESS ):
      return fail(err,result[1])
    stmt = "DELETE FROM "+RunsTable+" WHERE RunNumber="+str(RunNumber)
    result = self._exec(stmt,1)
    if ( result[0] != SUCCESS ):
      return (SUCCESS,err+result[1])
    return (SUCCESS,)

  #============================================================================
  def createRun(self, **options):
    """ Add entry to the Runs table. The required arguments are:
        FillNumber
        Partition
        Activity
        StartDate
        ProgramName
        ProgramVersion

        The arguments must be passed in the keyword - value semantic of python:
        key=<value>,...
    """
    try:
      run   = self.nextRunNumber()
      if ( run[0] != SUCCESS ):
        return fail('Failed to allocate new run-number.')
      run = run[1]
      fill  = str(options['FillNumber'])
      pid   = int(options['Partition'])
      act   = str(options['Activity'])
      pgm   = str(options['ProgramName'])
      vsn   = str(options['ProgramVersion'])
      start = int(time.time())
      if ( options.has_key('StartDate')      ): start = int(options['StartDate'])
      end   = start
      if ( options.has_key('EndDate')        ): end   = int(options['EndDate'])
      lumi  = str(0.0)
      if ( options.has_key('IntegratedLumi') ): lumi  = str(options['IntegratedLumi'])

      vals  = "VALUES (%d, %s, %d, '%s', %d, %d, '%s', '%s', %s)"%(run,fill,pid,act,start,end,pgm,vsn,lumi)
      stmt  = "INSERT INTO "+RunsTable+" (RunNumber,FillNumber,Partition,Activity,StartDate,EndDate,ProgramName,ProgramVersion,IntegratedLumi) "+vals
      result = self._exec(stmt,0)
      if ( result[0] != SUCCESS ):
        return (0,'Failed to create Run '+str(run))
      # Cleanup parameters and save optional ones as RunParameters
      opts = options
      del opts['FillNumber']
      del opts['Partition']
      del opts['Activity']
      del opts['StartDate']
      del opts['ProgramName']
      del opts['ProgramVersion']
      if opts.has_key('EndDate'): del opts['EndDate']
      if opts.has_key('IntegratedLumi'): del opts['IntegratedLumi']
      for k in opts:
        result = self.sqlCreateRunParam(run, k, str(options[k]))
        if(result[0] != SUCCESS):
          return fail('Cannot insert run parameter:',k,'=',options[k],' for run ',run,'\n',result[1])      
      return (SUCCESS,run,)
    except KeyError, X:
      return fail('[Insufficient arguments supplied] ',X)
    except Exception, X:
      return fail(X)
    return fail('[Unknown Error]')

  #============================================================================
  def finalizeRun(self, RunNumber, EndDate, IntegratedLumi):
    """ Finalize run: update EndDate and IntegratedLumi item in the Runs table.

        @author M.Frank
    """
    return self.modifyRun(RunNumber, EndDate=EndDate, IntegratedLumi=IntegratedLumi)

  #============================================================================
  def modifyRun(self, RunNumber, **options):
    """ Modify existing items in the runs table.
        The parameter is identified by its name; 
        Hence, every item __must__ correspond to a table row!

        @author M.Frank
    """
    stmt = "UPDATE "+RunsTable+" SET "
    try:
      for o in options:
        stmt = stmt + o + "='" + str(options[o]) + "', "
      stmt = stmt[:-2]
      stmt = stmt + " WHERE RunNumber="+str(RunNumber)
      result = self._exec(stmt,1)
      if(result[0] != SUCCESS):
        return fail('Cannot modify run:',RunNumber,'\n',result[1],'\n Statement=',stmt)
      return (SUCCESS,)
    except Exception, X:
      return fail('[Internal Error (modifyRun)] ',X,' Statement=',stmt)

  #============================================================================
  def deleteRunParams(self, RunNumber):
    """ Delete all parameters of a given run.
        The run is identified by its run number.

        @author M.Frank
    """
    stmt = "DELETE FROM "+RunParamsTable+" WHERE RunNumber="+str(RunNumber)
    try:
      result = self._exec(stmt,1)
      if ( result[0] != SUCCESS ):
        return fail('Cannot delete run: ',RunNumber,'\n',result[1])
      return (SUCCESS,)
    except Exception, X:
      return fail('[Internal Error (deleteRunParams)] ',X,' Statement=',stmt)
      
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
      opt = DbCore.sqlOpts(**options)
      stmt = "SELECT RunNumber, Name, Val, Typ from "+RunParamsTable+" WHERE RunNumber="+str(RunNumber)
      if ( len(opt) > 0 ):
        stmt = stmt + " AND " + opt
      # print 'runParams:',stmt
      cur = DbCore.Cursor(self.db()).select(stmt)
      while ( cur.next().isSuccess() ):
        res.append(cur.result())
      if ( len(res) == 0 ):
        res = self.existsRun(RunNumber=RunNumber)
        if ( res[0] == SUCCESS ):
          return (SUCCESS,[])
        return fail('[No such run]: Run ',RunNumber,' does not exist!')
      return (SUCCESS,res)
    except Exception, X:
      return fail('[Internal Error (runParams)] ',X,' Statement=',stmt)
    return fail('[Unknown error (runParams)] Statement=',stmt)

  #============================================================================
  def addRunParam(self, RunNumber, Name, Val, Typ='PARAM'):
    """ Add a new parameter to an existing run.
        The run is identified by its run number.
        The parameter is identified by its name,
        the value and the type are the parameter's data

        @author M.Frank
    """
    result = self.sqlCreateRunParam(RunNumber, Name, Val, Typ)
    if(result[0] != SUCCESS):
      return fail('Cannot insert run parameter:',Name,'=',Val,' for run ',RunNumber,'\n',result[1])
    return (SUCCESS,)

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
    if(result[0] != SUCCESS):
      return fail('Cannot modify run parameter:',Name,' for run ',RunNumber,'\n',result[1])
    return (SUCCESS,)

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
    if(result[0] != SUCCESS):
      return fail('Cannot remove run parameter:',Name,' for run ',RunNumber,'\n',result[1])
    return (SUCCESS,)

  #============================================================================
  def runParam(self, RunNumber, Name):
    """ Access a parameter of an existing run.
        The run is identified by its run number.
        The parameter is identified by its name.

        @author M.Frank
    """
    try:
      stmt = "SELECT RunNumber,Name,Val,Typ from "+RunParamsTable+" \
              WHERE RunNumber="+str(RunNumber)+" \
              AND Name='"+Name+"'"
      res = []
      cur = DbCore.Cursor(self.db()).select(stmt)
      while ( cur.next().isSuccess() ):
        res.append(cur.result())
      if ( len(res) == 1 ):
        return (SUCCESS,res[0])
      elif ( len(res) == 0 ):
        return fail('Run parameter:',Name,' for run ',RunNumber,' does not exist!')
      elif ( len(res) > 1 ):
        return fail('[Internal Error] Multiple entries for run parameter:',Name,' [run:',RunNumber,'] found!')
    except Exception, X:
      return fail(X)
    return fail('[Internal Error]')

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
      stmt = "SELECT FileID, RunNumber, FileName, FileStatus, StartDate, EndDate, Stream,\
                    MD5Sum, LogicalName, LogicalStatus, EventStat, FileSize\
              FROM "+FilesTable+" WHERE "+DbCore.sqlOpts(**options)
      if ( len(options) > 0 ):
        res = []
        cur = DbCore.Cursor(self.db()).select(stmt)
        while ( cur.next().isSuccess() ):
          res.append(cur.result())
        return (SUCCESS,res)
      return fail('[Invalid arguments] You need to specify some selection criteria!')
    except Exception, X:
      return fail(X)
    return fail('[Internal Error]')
    
  #============================================================================
  def addFile(self, RunNumber, FileName, Stream, **options):
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
    if ( fid[0] == SUCCESS ):
      fid = str(fid[1])
      fstat = 'Opened'
      if ( options.has_key('FileStatus') ):     fstat = str(options['FileStatus'])
      start = str(int(time.time()))
      if ( options.has_key('StartDate') ):      start = str(int(options['StartDate']))
      end   = start
      if ( options.has_key('EndDate') ):        end = str(int(options['EndDate']))
      md5 = '000000000000000000000000000000'
      if ( options.has_key('MD5Sum') ):         md5 = str(options['MD5Sum'])
      logName = 'None'
      if ( options.has_key('LogicalName') ):    logName = str(options['LogicalName'])
      logStat = 'None'
      if ( options.has_key('LogicalStatus') ):  logStat = str(options['LogicalStatus'])
      evtStat = str(0)
      if ( options.has_key('EventStat') ):      evtStat = str(options['EventStat'])      
      fSize   = str(0)      
      if ( options.has_key('FileSize') ):       fSize = str(options['FileSize'])      

      vals = " VALUES ("+fid+","+run+",'"+FileName+"','"+fstat+"',"+start+","+end+",'"+Stream+"','"+md5+"','"+logName+"','"+logStat+"',"+evtStat+","+fSize+")"
      stmt = "INSERT INTO "+FilesTable+" (FileID, RunNumber, FileName, FileStatus, StartDate, EndDate, Stream,\
                                          MD5Sum, LogicalName, LogicalStatus, EventStat, FileSize)"+vals
      result = self._exec(stmt,1)
      if ( result[0] != SUCCESS ):
        #print stmt
        return fail('Cannot add file ',FileName,' for run: ',run,'\n',result[1])
      opts = options
      if ( opts.has_key('FileStatus') ):    del opts['FileStatus']
      if ( opts.has_key('StartDate') ):     del opts['StartDate']
      if ( opts.has_key('EndDate') ):       del opts['EndDate']
      if ( opts.has_key('MD5Sum') ):        del opts['MD5Sum']
      if ( opts.has_key('LogicalName') ):   del opts['LogicalName']
      if ( opts.has_key('LogicalStatus') ): del opts['LogicalStatus']
      if ( opts.has_key('EventStat') ):     del opts['EventStat']   
      if ( opts.has_key('FileSize') ):      del options['FileSize']      
      for k in opts:
        result = self.addFileParam(FileID=fid, Name=k, Val=str(opts[k]), Typ='PARAM')
        if(result[0] != SUCCESS):
          return fail('Cannot insert file parameter:',k,'=',opts[k],' for file ',fid,'\n',result[1])
      return (SUCCESS,)
    return fail('Cannot add file ',FileName,' for run: ',RunNumber,'\n',fid[1])

  #============================================================================
  def modifyFile(self, RunNumber, FileName, **options):
    """ Update the information of an existing file.
        The run is identified by its run number.
        The file is identified by its name.
        Optionally a set of named options may be passed,
        which will be used to directly populate the
        file parameters table

        Calling sequence:
        modifyFile(<RunNumber>,<file-name>,<opt-name>=<opt-value>,[<opt-name>=<opt-value>],...)

        Returns:
        Tuple containing status code and result (or error description)

        @author M.Frank
    """
    stmt = ''
    try:
      run = str(RunNumber)
      opts = {}
      mods = []
      print 'modifyFile:',RunNumber,FileName
      fid = self.files(RunNumber=RunNumber,FileName=FileName)
      print fid
      if ( fid[0] == SUCCESS ):
        # print 'FID=',fid
        fid = fid[1]
        if ( len(fid) == 1):
          fid = fid[0][0]
          for o in options:
            if ( o == 'FileStatus' ):
              mods.append("FileStatus='"+str(options[o])+"'")
            elif ( o == 'StartDate' ):
              mods.append("StartDate="+str(options[o]))
            elif ( o == 'EndDate' ):
              mods.append("EndDate="+str(options[o]))
            elif ( o == 'MD5Sum' ):
              mods.append("MD5Sum='"+str(options[o])+"'")
            elif ( o == 'LogicalName' ):
              mods.append("LogicalName='"+str(options[o])+"'")
            elif ( o == 'LogicalStatus' ):
              mods.append("LogicalStatus='"+str(options[o])+"'")
            elif ( o == 'EventStat' ):
              mods.append("EventStat="+str(options[o]))
            elif ( o == 'FileSize' ):
              mods.append("FileSize="+str(options[o]))
            else:
              opts[o] = options[o]
          stmt = "UPDATE "+FilesTable+" SET "
          for m in mods:
            stmt = stmt + m + ","
          if (len(mods)>0):
            stmt = stmt[:-1]
          stmt = stmt + " WHERE FileID="+str(fid)
          print 'modifyFile:',stmt
          result = self._exec(stmt,1)
          if ( result[0] != SUCCESS ):
            #print stmt
            return fail('Cannot add file ',FileName,' for run: ',run,'\n',result[1])
          for k in opts:
            result = self.addFileParam(FileID=fid, Name=k, Val=str(opts[k]), Typ='PARAM')
            if(result[0] != SUCCESS):
              return fail('Cannot insert file parameter:',k,'=',opts[k],' for file ',FileName,'\n',result[1])
          return (SUCCESS,)
        return fail('Cannot modify file ',FileName,' for run: ',RunNumber,'[',len(fid),' File(s) found]\n',fid[1])
      return fail('Cannot modify file ',FileName,' for run: ',RunNumber,'\n',fid[1])
    except KeyError, X:
      return fail('modifyFile: [Insufficient arguments supplied]: ',options,'\n',X)
    except Exception, X:
      traceback.print_exc()
      return fail('modifyFile: [Internal Error] ',str(X),' Statement=',stmt)

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
      opt = DbCore.sqlOpts(**options)
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
        if(result[0] != SUCCESS):
          return fail('Cannot delete file parameters for file IDs=',fids)
        stmt = "DELETE FROM "+FilesTable+" WHERE RunNumber="+run
        result = self._exec(stmt,1)
        if(result[0] != SUCCESS):
          return fail('Cannot delete files with IDs=',fids)
      return (SUCCESS,)
    return fail('Cannot remove file. At least the file name must be specified.')

  #============================================================================
  def fileParams(self, **options):
    """ 
    
        @author M.Frank
    """
    stmt = ''
    try:
      fid = ''
      if ( options.has_key('FileID') ):
        opt = DbCore.sqlOpts(FileID=options['FileID'])
        stmt = str(opt)
      elif ( options.has_key('FileName') ):
        stmt = "SELECT DISTINCT FileID from "+FilesTable+" WHERE "+DbCore.sqlOpts(FileName=options['FileName'])
        # print '0 - fileParams:', stmt
        cur = DbCore.Cursor(self.db()).select(stmt)
        while ( cur.next().isSuccess() ):
          fid = fid + str(cur.result()[0]) + ', '
        if ( len(fid) == 0 ):
          return fail('Cannot access file parameters.\n')
        fid = fid[:-2]
        stmt = ' FileID IN ('+fid+')'
      elif ( options.has_key('File') ):        
        fileopts = options['File']
        if ( fileopts.has_key('FileID') ):
          fid = str(fileopts['FileID'])
        else:
          stmt = "SELECT DISTINCT FileID from "+FilesTable+" WHERE "+DbCore.sqlOpts(**fileopts)
          # print '1 - fileParams:', stmt
          cur = DbCore.Cursor(self.db()).select(stmt)
          while ( cur.next().isSuccess() ):
            fid = fid + str(cur.result()[0]) + ', '
          if ( len(fid) == 0 ):
            return fail('Cannot access file parameters.\n')
          fid = fid[:-2]
        stmt = ' FileID IN ('+fid+')'
      if ( options.has_key('Param') ):
        paropt = options['Param']
        par = DbCore.sqlOpts(**paropt)
        if ( len(par) > 0 ):
          if ( len(stmt) > 0 ): stmt = stmt + " AND " + par
          else:                 stmt = par
      stm = "SELECT FileID,Name,Val,Typ FROM "+FileParamsTable+" WHERE "+stmt
      # print 'fileParams:', stm
      res = []
      cur = DbCore.Cursor(self.db()).select(stm)
      while ( cur.next().isSuccess() ):
        res.append(cur.result())
      return (SUCCESS,res)
    except KeyError, X:
      return fail('[Insufficient arguments supplied]: ',options,'\n',X)
    except Exception, X:
      return fail('[Internal Error] ',str(X),' Statement=',stmt)
    
  #============================================================================
  def addFileParam(self, Name, Val, Typ, **options):
    """ Add a new parameter to the FileParameters table.
    
        @author M.Frank
    """
    stmt = ''
    try:
      name = Name # options['Name']
      val  = str(Val)  # str(options['Val'])
      typ  = str(Typ)  # options['Typ']
      fid = None
      if ( options.has_key('FileID') ):
        fid = str(options['FileID'])
      else:
        stmt = "SELECT FileID from "+FilesTable+"\
                WHERE FileName='"+options['FileName']+"' \
                AND RunNumber="+str(options['RunNumber'])
        cur = DbCore.Cursor(self.db()).select(stmt)
        if ( cur.next().isSuccess() ):
          fid = str(cur.result()[0])
          if ( cur.next().isSuccess() ):
            return fail('Cannot add file Parameter ',name,' [Ambiguous file ',options['FileName'],']')
        else:
          return fail('Cannot add file Parameter '+name+\
                      ' [File does not exist]\n'+result[1])
      stmt = "INSERT INTO "+FileParamsTable+" (FileID, Name, Val, Typ) \
              VALUES ("+fid+",'"+name+"','"+val+"','"+typ+"')"
      result = self._exec(stmt,1)
      if ( result[0] != SUCCESS ):
        return fail('Cannot add file Parameter ',name,'\n',result[1])
      return (SUCCESS,)
    except KeyError, X:
      return fail('[Insufficient arguments supplied]: ',options,'\n',X)
    except Exception, X:
      return fail('[Internal Error] ',X,' Statement=',stmt)
    
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
          return fail('Cannot add file Parameter '+name+\
                               ' [File does not exist]\n'+result[1])
      stmt = "DELETE FROM "+FileParamsTable+" WHERE Name='"+Name+" AND FileID="+fid
      result = self._exec(stmt,1)
      if ( result[0] != SUCCESS ):
        return fail('Cannot add file Parameter ',name,'\n',result[1])
      return (SUCCESS,)
    except KeyError, X:
      return fail('[Insufficient arguments supplied]: ',options,'\n',X)
    except Exception, X:
      return fail('[Internal Error] ',X)

  #============================================================================
  def dump(self,**options):
    """ Dump database content.

        @author M.Frank
    """    
    try:
      result = ''
      opt = DbCore.sqlOpts(**options)
      if ( len(opt) == 0 ): opt = "RunNumber>0"
      stmt = "SELECT RunNumber,FillNumber,Partition,Activity,StartDate,EndDate,ProgramName,ProgramVersion,IntegratedLumi \
              FROM "+RunsTable+" WHERE "+opt
      cur = DbCore.Cursor(self.db()).select(stmt)
      result = result + '\n\n\n%-8s %-8s %-8s %-12s %-24s %-24s %8s %8s %11s\n'%('Run','Fill','PID','Activity','Start Date','End Date','Pgm', 'Vsn', 'Lumi')
      while ( cur.next().isSuccess() ):
        (RunNumber,FillNumber,Partition,Activity,StartDate,EndDate,ProgramName,ProgramVersion,IntegratedLumi) = cur.result()
        result = result + '%-8d %-8d %-8s %-12s %-24s %-24s %8s %8s %11.2f\n'%(RunNumber,FillNumber,str(Partition),Activity,db_time(StartDate),db_time(EndDate),ProgramName,ProgramVersion,IntegratedLumi)
        pars = self.runParams(RunNumber)
        if ( pars[0] == SUCCESS ):
          context = 'RunParameters:'
          pars = pars[1]
          for p in pars:
            (ID,Name,Value,Type) = p
            result = result + '   %-19s %-24s %-s\n'%(context,Name+'['+Type+']',Value)
            context = ''
        files = self.files(RunNumber=RunNumber,FileName=("LIKE","'%'",))
        if ( files[0] != SUCCESS ):
          result = result + 'Failed to retrieve files for run:'+str(RunNumber)+' Err='+str(files[1])+'\n'
        else:
          files = files[1]
          context = 'Files:'
          for f in files:
            (ID, RunNumber, FileName, FileStatus, StartDate, EndDate, Stream, MD5Sum, LogicalName, LogicalStatus, EventStat, FileSize) = f
            result = result + '   %-19s %-8d %-50s %-15s %-8s %d %d Bytes\n'%(context, ID, '"'+FileName+'"','"'+Stream+'"',FileStatus,EventStat,FileSize)
            result = result + '   %-19s %-8s %-50s %-12s %-32s\n'%('', '', '"'+LogicalName+'"','"'+LogicalStatus+'"',MD5Sum)
            pars = self.fileParams(File={'RunNumber':RunNumber,'FileName':FileName},Param={'Name':('LIKE',"'%'")})
            if ( pars[0] != SUCCESS ):
              result = result + 'Failed to retrieve fileparameters for run:'+str(RunNumber)+' File:'+FileName+' Err='+str(pars[1])+'\n'
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
      return (SUCCESS,result)
    except Exception, X:
      import sys, traceback
      result = result + 'Exception:\n'+str(X)
      info = sys.exc_info()
      lns  = traceback.format_exception(info[0],info[1],info[2])
      for line in lns:
        print 'Error:', line
      return fail(X)
    return fail('Unknown error')

class Installer:

  #============================================================================
  def __init__(self, login):
    self.db = RunDatabase(login)

  #============================================================================
  def install(self, OnErrorContinue=1):
    """ Install Run database from scratch. Only the basic tables and keys are defined.
        No attempt here is taken to optimize the data access in any way.

        @author M.Frank
    """
    db = self.db
    make = db._make
    # Create Runs table
    make('CREATE TABLE '+SequencesTable+'(Name VARCHAR(32), Entry INTEGER)',OnErrorContinue)
    db.createSequence(FileIDSequence,OnErrorContinue)
    db.createSequence(RunNumberSequence,OnErrorContinue)
    
    make('ALTER TABLE  '+SequencesTable+' ADD PRIMARY KEY (Name)',OnErrorContinue)
    # Create Runs table
    make('CREATE TABLE '+RunsTable+"""
           (RunNumber        INTEGER,
            FillNumber       INTEGER,
            Partition        INTEGER,
            Activity         VARCHAR(32),
            StartDate        INTEGER,
            EndDate          INTEGER,
            ProgramName      VARCHAR(16),
            ProgramVersion   VARCHAR(9),
            IntegratedLumi   REAL)""",OnErrorContinue)
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
           (FileID        INTEGER NOT NULL,
            RunNumber     INTEGER NOT NULL,
            FileName      VARCHAR(255) NOT NULL,
            FileStatus    VARCHAR(32) NOT NULL,
            StartDate     INTEGER,
            EndDate       INTEGER,
            Stream        VARCHAR(128),
            MD5Sum        CHAR(32),
            LogicalName   VARCHAR(255),
            LogicalStatus VARCHAR(32),
            EventStat     INTEGER,
            FileSize      INTEGER
            )""",OnErrorContinue)
    make('CREATE UNIQUE INDEX PK_Files ON '+FilesTable+' (FileID) WITH PRIMARY',OnErrorContinue)
    make('CREATE UNIQUE INDEX I_Files_RunFname ON '+FilesTable+' (RunNumber,FileName)',OnErrorContinue)
    make('ALTER TABLE  '+FilesTable+' ADD FOREIGN KEY (RunNumber) REFERENCES '+RunsTable+' (RunNumber)',OnErrorContinue)
    #  Create file parameters table
    make('CREATE TABLE '+FileParamsTable+"""
           (FileID INTEGER   NOT NULL,
            Name VARCHAR(32) NOT NULL,
            Val VARCHAR(255),
            Typ VARCHAR(32))""",OnErrorContinue)
    make('CREATE UNIQUE INDEX PK_FileParams ON '+FileParamsTable+' (FileID,Name) WITH PRIMARY',OnErrorContinue)
    make('ALTER TABLE '+FileParamsTable+' ADD FOREIGN KEY (FileID) REFERENCES '+FilesTable+' (FileID)',OnErrorContinue)
    
    res = (SUCCESS,strtime()+' Installation finished (need to check printout)')
    if ( not OnErrorContinue ):
      res = (SUCCESS,strtime()+' Installation succeeded.')
    print res
    return res

  #============================================================================
  def uninstall(self, OnErrorContinue=1):
    """ Uninstall the Run database. All basic tables and indices are dropped.

        @author M.Frank
    """
    exe = self.db._exec
    exe('DROP TABLE ' + RunParamsTable,OnErrorContinue)
    exe('DROP TABLE ' + FileParamsTable,OnErrorContinue)
    exe('DROP TABLE ' + FilesTable,OnErrorContinue)
    exe('DROP TABLE ' + RunsTable,OnErrorContinue)
    exe('DROP TABLE ' + SequencesTable,OnErrorContinue)
    res = (SUCCESS,strtime()+' Deinstallation finished (need to check printout)')
    if ( not OnErrorContinue ):
      res = (SUCCESS,strtime()+' Deinstallation succeeded.')
    print res
    return res


def install():
  res = Installer('RunDatabase').install(1)
  print 'db.install() returned:', str(res)
  
def uninstall():
  res = Installer('RunDatabase').uninstall(1)
  print 'db.uninstall() returned:', str(res)

