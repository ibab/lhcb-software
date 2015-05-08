"""
     HLT2 monitoring initialization: 
     Creation of the database and the working directories

     \author  M.Frank
     \version 1.0
"""
import os
from HLT2 import *

# ------------------------------------------------------------------------------
class Setup:
  """
        Setup module for the HLT2 monitoring
        Member functions perform the creation 
        of the database and the working directories

        \author  M.Frank
        \version 1.0
  """
  # ----------------------------------------------------------------------------
  def __init__(self, config, **keys):
    """ Initializing constructor

        \author  M.Frank
        \version 1.0
    """
    from OnlineKernel.DbCore import DbCore
    self.config = config
    self.DbCore = DbCore
    self.credentials = keys['credentials']
    self.table_name = keys['table']

  # ----------------------------------------------------------------------------
  def createTables(self):
    """ Create all tables to monitor the HLT2 monitoring facility

        \author  M.Frank
        \version 1.0
    """
    table_name = self.table_name.upper()
    db     = self.DbCore(self.credentials);
    cursor = db.connection().cursor()
    text = db.text_t(255)
    integer = db.int_t(12)
    stmt = ""
    try:
      stmt = """CREATE TABLE %s (runid INTEGER PRIMARY KEY, 
                                 state %s,
                                 todo INTEGER, 
                                 done INTEGER,
                                 checked INTEGER,
                                 archived INTEGER, 
                                 partitionid INTEGER,
                                 partitionname %s,
                                 activity %s,
                                 runtype %s,
                                 params %s,
                                 starttime INTEGER) """%(table_name,text,text,text,text,text,)
      cursor.execute(stmt)
      cursor.close()
      db.connection().commit()
    except Exception, X:
      log(INFO,'Failed to create table: :"%s" with statement:%s'%(table_name,stmt,))
      log(INFO,'Failure:  '+str(X))

    try:
      stmt = "CREATE TABLE %s_LAST (runid %s PRIMARY KEY, type %s) "%(table_name,integer,text,)
      cursor = db.connection().cursor()
      cursor.execute(stmt)
      cursor.close()
      db.connection().commit()
    except Exception, X:
      log(INFO,'Failed to create table: :"%s_LAST" with statement:"%s"'%(table_name,stmt,))
      log(INFO,'Failure:  '+str(X))

    try:
      stmt = "INSERT INTO %s_LAST VALUES (:runid, :last)"%(table_name,)
      cursor = db.connection().cursor()
      cursor.execute(stmt, {'runid':0, 'last':'LAST_CREATED'})
    except Exception, X:
      log(INFO,'Failed to execute statement:"%s"'%(stmt,))
      log(INFO,'Failure:  '+str(X))

    cursor.close()
    db.connection().commit()
    return self

  # ----------------------------------------------------------------------------
  def createDirectories(self):
    """ Create all working directories used by the HLT2 monitoring

        \author  M.Frank
        \version 1.0
    """
    make_directory(self.config.top_working_directory)
    make_directory(self.config.todo)
    make_directory(self.config.done)
    make_directory(self.config.failed)
    make_directory(self.config.running)
    make_directory(self.config.archive.top_working_directory)
    make_directory(self.config.archive.todo)
    make_directory(self.config.archive.failed)
    make_directory(self.config.archive.done)
    os.chdir(self.config.todo)
    log(INFO,'Working directory:'+os.getcwd())
    return self

  # ----------------------------------------------------------------------------
  def printSummary(self):
    """ Print summary information about the facility environment

        \author  M.Frank
        \version 1.0
    """
    import sys
    table_name = self.table_name
    db = self.DbCore(self.credentials).connection()
    cursor = db.cursor()
    try:
      cursor.execute("""SELECT COUNT(state) FROM %s"""%(table_name,))
    except Exception, X:
      log(INFO,'Failed to execute SQL:'+str(X))
    count = cursor.fetchone()
    cursor.close()
    cursor = db.cursor()
    try:
      cursor.execute("""SELECT COUNT(runid) FROM %s_LAST"""%(table_name,))
    except Exception, X:
      log(INFO,'Failed to execute SQL:'+str(X))
    count_last = cursor.fetchone()
    cursor.close()
    log(INFO,'+--------------------------------------------------------------------------------+')
    log(INFO,'|       Working directory:   %-50s  |'%(os.getcwd(),))
    log(INFO,'|       Monitoring database: %-50s  |'%(self.credentials,))
    log(INFO,'|       dto. Number of entries: %-8s %-38s  |'%(str(count_last),str(count),))
    log(INFO,'+--------------------------------------------------------------------------------+')
    #sys.exit(0)
