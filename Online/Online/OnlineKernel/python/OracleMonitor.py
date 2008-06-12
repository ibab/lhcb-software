import gaudiweb, DbCore

INDX_TABLESPACE = 0
INDX_NAME       = 1
INDX_TABLENAME  = 2
INDX_TYPE       = 3
INDX_UNIQUENESS = 4
INDX_ROWS       = 5
INDX_BYTES      = 6

INDX_UNIQUENESS = 3
INDX_BYTES      = 4

TABL_TABLESPACE = 0
TABL_NAME       = 1
TABL_ROWS       = 2
TABL_BLOCKS     = 3
TABL_BYTES      = 4
datamgmt_dbs=['lhcb_bookkeeping','lhcb_bookkeeping_dev/lhcb_bookkeeping','lhcb_production','lhcb_jobconfig']
datamgmt_dbs=['lhcb_bookkeeping','lhcb_production','lhcb_jobconfig']

"""
    File catalog class
    
    @author M.Frank
"""
class Monitor:
  """
      File catalog interface for LHCb bookkeeping database
      
      @author M.Frank
  """
  def __init__(self, dbname='', load=None):
    self.login = dbname
    self.core = None
    self.tables = []
    self.indices = []
    self.table_space_bytes = {}
    self.index_bytes = 0
    self.table_bytes = 0
    if ( load is not None ):
      self.loadIndices()
      self.loadTables()

  def doc(self):
    s = """
     """
    return s

  def _core(self):
    if ( self.core is None ):
      self.core = DbCore.DbCore(self.login)
    return self.core
  
  def _tables(self):
    if ( len(self.tables) == 0 ):
      self.loadTables()
    return self.tables
  
  def _indices(self):
    if ( len(self.indices) == 0 ):
      self.loadIndices()
    return self.indices

  def loadIndices(self):
    """
      Load all index specific data:
        - index name
        - tablespace name
        - table name
        - index type
        - uniqueness constraint
        - number of rows
        - size in bytes

      @author  M.Frank
      @version 1.0
    """
    stmt  = """SELECT i.tablespace_name,
                      i.index_name,
                      i.table_name,
                      i.index_type,
                      i.uniqueness,
                      NVL(i.num_rows,0),
                      NVL(s.bytes,0)
               FROM user_indexes i, user_segments s
               WHERE s.segment_name=i.index_name
               ORDER BY i.tablespace_name, i.table_name
            """
    stmt  = """SELECT i.tablespace_name,
                      i.index_name,
                      i.table_name,
                      i.uniqueness,
                      NVL(s.bytes,0)
               FROM user_indexes i, user_segments s
               WHERE s.segment_name=i.index_name
               ORDER BY i.tablespace_name, i.index_name, i.table_name
            """
    ret = self._core().executeQuery(stmt)
    if ( ret is None ):
      self.indices = []
    else:
      self.indices = ret
    self.index_bytes = 0
    for i in self.indices:
      bytes = i[INDX_BYTES]
      space = i[INDX_TABLESPACE]
      self.index_bytes = self.index_bytes + bytes
      if ( self.table_space_bytes.has_key(space) ):
        self.table_space_bytes[space] = self.table_space_bytes[space]+bytes
      else:
        self.table_space_bytes[space] = bytes
    return self.indices

  def loadTables(self):
    """
      Load all table specific data:

      @author  M.Frank
      @version 1.0
    """
    stmt  = """SELECT t.tablespace_name,
                      t.table_name,
                      NVL(t.num_rows,0),
                      NVL(t.blocks,0),
                      NVL(s.bytes,0)
               FROM user_tables t, user_segments s
               WHERE t.table_name=s.segment_name
               ORDER BY t.tablespace_name, t.table_name
            """
    ret = self._core().executeQuery(stmt)
    if ( ret is None ):
      self.tables = []
    else:
      self.tables = ret
    self.table_bytes = 0
    for i in self.tables:
      bytes = i[TABL_BYTES]
      space = i[TABL_TABLESPACE]
      self.table_bytes = self.table_bytes + bytes
      if ( self.table_space_bytes.has_key(space) ):
        self.table_space_bytes[space] = self.table_space_bytes[space]+bytes
      else:
        self.table_space_bytes[space] = bytes
    return self.tables

  def printSpace(self, dsc, bytes):
    b = float(bytes)/1024.0
    gaudiweb.log("%-40s %12d Bytes / %8d KB / %6d MB / %6.2f GB"%(dsc, bytes, int(b), int(b/1024.0), b/1024.0/1024.0),0,0)

  def tableBytes(self):
    self._tables()
    return self.table_bytes
    
  def indexBytes(self):
    self._indices()
    return self.index_bytes
    
  def printIndices(self):
    gaudiweb.log('Index summary '+self.login[:self.login.find('/')],1,0)
    gaudiweb.log("%-16s%-31s%-30s%-10s%11s"%('TableSpace','Index','Table','Uniqueness','Bytes'),0,1)
    for i in self._indices():  
      #gaudiweb.log("%-16s%-32s%-32s%-10s%-12s %8d %12d"%i,0,0)
      gaudiweb.log("%-16s%-31s%-30s%-10s%11d"%i,0,0)

  def printTables(self):
    gaudiweb.log('Table summary '+self.login[:self.login.find('/')],1,0)
    gaudiweb.log("%-16s%-51s %8s %8s %12s"%('TableSpace','Table','Rows','Blocks','Bytes'),0,1)
    for i in self._tables():  
      gaudiweb.log("%-16s%-51s %8d %8d %12d"%i,0,0)

  def printSummary(self):
    self._printSummary(self.indexBytes(), self.tableBytes())
    keys = self.table_space_bytes.keys()
    for k in keys:
      self.printSpace(' -> tablespace '+k+':', self.table_space_bytes[k])
  def _printSummary(self, idx_bytes, tab_bytes):
    gaudiweb.log('Database summary '+self.login[:self.login.find('/')])
    self.printSpace('Total space occupied by tables:', tab_bytes)
    self.printSpace('Total space occupied by indices:', idx_bytes)
    self.printSpace('Total space occupied:', tab_bytes+idx_bytes)

  def add(self, monitor):
    for i in monitor._tables():
      self.tables.append(i)
    for i in monitor._indices():
      self.indices.append(i)
    for i in monitor.table_space_bytes.keys():
      if ( self.table_space_bytes.has_key(i) ):
        self.table_space_bytes[i] = self.table_space_bytes[i] + monitor.table_space_bytes[i]
      else:
        self.table_space_bytes[i] = monitor.table_space_bytes[i]
    self.index_bytes = self.index_bytes + monitor.index_bytes
    self.table_bytes = self.table_bytes + monitor.table_bytes
    
def getLogins(log=datamgmt_dbs):
  import getpass
  login = []
  last_password = ''
  for l in log:
    password = getpass.getpass(prompt='Password for '+l+':')
    if ( len(password)==0 ):
      password = last_password
    last_password = password
    db = l
    idx = db.find('/')
    if ( idx < 0 ):
      db = l + '/' + l
    login.append(db+'/'+password)
  return login

def printSummary(login):
  monitors = []
  summary = Monitor()
  for l in xrange(len(login)):
    try:
      print ' ... Loading information for ',login[l][:login[l].find('/')]
      m = Monitor(login[l], load=1)
      monitors.append(m)
      summary.add(m)
    except Exception, X:
      print 'Login failure:', login[l]
      print str(X)
    except:
      print 'Login failure:', login[l]
      

  for m in monitors:
    m.printTables()
    m.printIndices()

  for m in monitors:
    m.printSummary()
  summary.printSummary()

def summary(log):
  printSummary(getLogins(log))

if ( __name__ == '__main__' ):
  printSummary(getLogins())