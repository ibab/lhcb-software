import sys, string, traceback
_debug = 0
_ms_access = 1
def S_ERROR():
  return {'Status': 0, 'Log': 'Error during processing'}
def S_OK():
  return {'Status': 1, 'Log': 'Request handled successfully.'}

def sqlOpts(*parm,**options):
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
    elif type(opt) == type(list()):
      for q in opt:
        stmt = stmt + " " + str(q)
    elif type(opt) == type(str()):
      stmt = stmt + "='" + str(opt)+"'"
    else:
      stmt = stmt + "=" + str(opt)
  return stmt

def make_result(core, stmt, return_result=1, multiplicity=1):
  result = core.executeQuery(stmt)
  if ( len(result) > 0 ):
    status = S_OK()
    if ( return_result ):
      if ( multiplicity == 1 ):
        res = result[0][0]
      else:
        res = result
    else:
      res = 'Data found'
    status['Result'] = res
    return status
  status = S_ERROR()
  status['Result'] = 'SQL statement "'+stmt+'" returned NULL.'
  return status

class DbException(Exception):
  def __init__(self):
    self.msg = 'Unknown database exception'
    info = sys.exc_info()
    lns  = traceback.format_exception(info[0],info[1],info[2])
    for line in lns:
      if ( line[:4] == 'dbi.' ):
        self.msg = line
        if ( self.msg[-1] == '\n' ):
          self.msg = self.msg[:-1]
        return
  def __str__(self):
    return 'DatabaseException: '+self.msg

class DbCore:
  """
      Database access class
      
      @author  M.Frank
      @version 1.0
  """
  def __init__(self,dbname):
    if type(dbname)==type(str()):
      self._name = dbname
      self._DB = self._db()
    else:
      self._DB = dbname._DB
      self._name = dbname._name
    self._cur = self._DB.cursor()
  
  def _db(self):
    if ( self._name[0:5]=='odbc:' ):
      import odbc
      return odbc.odbc(self._name[5:])
    else:
      import cx_Oracle
      return cx_Oracle.Connection(self._name)
  
  def executeQuery(self, sql, *parms, **kws):
    try:
      sql = string.replace(sql,'\n',' ')
      return self._executeQuery(sql, *parms, **kws)
    except Exception, X:
      #print 'Failure in DB access:', str(X)
      #print 'SQL statement was:', sql
      pass
    except:
      #traceback.print_exc()
      #print 'SQL statement was:',sql
      pass
    return None
    
  def execute(self, sql, *parms, **kws):
    try:
      sql = string.replace(sql,'\n',' ')
      if ( _ms_access ):  
        sql = string.replace(sql,'SUBSTRING','Mid$')
      if ( _debug ):
        print 'DbCore.execute:\n', sql, parms, kws
      return self._cur.execute(sql,*parms, **kws)
    except:
      #traceback.print_exc()
      #print 'SQL statement was:\n',sql
      #print 'Starting DBI error recovery ....'
      self._DB = self._db()
      self._cur = self._DB.cursor()
    try:
      return self._cur.execute(sql,*parms, **kws)
    except Exception, X:
      raise X
    except:
      raise DbException()
    return None

  def fetch(self, *parms, **kws):
    strip = 0
    many = 0
    one = 0
    try:
      strip = kws['strip']
    except KeyError:
      pass
    try:
      many = kws['many']
    except KeyError:
      pass
    try:
      one = kws['one']
    except KeyError:
      pass

    if one:
      # print 'fetch(1)'
      results = self._cur.fetchone()
    elif many:
      # print 'fetch(many)'
      results = self._cur.fetchmany(many)
    else:
      results = self._cur.fetchall()
    if strip:
      # Remove extraneous sequence nesting.
      if len(results) == 1 and len(results[0]) == 1:
        return results[0][0]
    return results
    
  def _executeQuery(self, sql, *parms, **kws):
    strip = 0
    try:
      strip = kws['strip']
    except KeyError:
      pass

    if ( _debug ):
      print 'DbCore._executeQuery:\n', sql, parms, kws
    try:
      if ( _ms_access ):  
        sql = string.replace(sql,'SUBSTRING','Mid$')
      self._cur.execute(sql,*parms)
    except:
      traceback.print_exc()
      print 'Starting DBI error recovery ....'
      self._DB = self._db()
      self._cur = self._DB.cursor()
      try:
        self._cur.execute(sql,*parms)
        print 'Automatic error recovery succeeded....'
      except Exception, X:
        raise X
      except:
        raise DbException()
    # Now get the results:
    try:
      results = self._cur.fetchall()
    except Exception, X:
      raise X
    except:
      raise DbException()
      
    if strip:
      # Remove extraneous sequence nesting.
      if len(results) == 1 and len(results[0]) == 1:
        return results[0][0]
    return results

class Result(list):
  def __init__(self, r1, r2):
    list.__init__(self)
    self.append(r1)
    self.append(r2)

  def isSuccess(self):
    return self[0] is not None
  
  def result(self):
    return self[1]

  def next(self):
    if ( _debug ):
      print 'Result.next():', self[0], self[1]
    if self[0] is not None:
      self[0], self[1] = self[0].next()
    else:
      self[0], self[1] = (None,None)
    return self
  
class Cursor:
  """
      Database cursor class
      
      @author  M.Frank
      @version 1.0
  """
  def __init__(self, core):
    """
        Standard constructor
    """
    self.core = core

  def commit(self):
    """
        Commit pending transactions
    """
    return self.core._DB.commit()
  
  def select(self, sql, *parms, **kws):
    """
        Selector
    """
    ret = self.core.execute(sql,*parms,**kws)
    if ( _debug ):
      print 'DbCore.select:\n', sql, parms, kws, ret
    if ret:
      return Result(self, ret)
    return Result(None, None)
  
  def next(self):
    ret = self.core.fetch(one=1)
    if ret is not None:
      return Result(self, ret)
    return Result(None, None)
  
