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

  # ----------------------------------------------------------------------------
  def createTables(self):
    """ Create all tables to monitor the HLT2 monitoring facility

        \author  M.Frank
        \version 1.0
    """
    db = self.DbCore(self.credentials).connection()
    cursor = db.cursor()
    try:
      cursor.execute("""CREATE TABLE hlt2_mon (runid INTEGER PRIMARY KEY ASC, 
                                               state TEXT,
                                               todo INTEGER, 
                                               done INTEGER,
                                               checked INTEGER,
                                               archived INTEGER, 
                                               partitionid INTEGER,
                                               partitionname TEXT,
                                               activity TEXT,
                                               runtype TEXT,
                                               params TEXT,
                                               starttime INTEGER)""")
    except Exception, X:
      log(INFO,'Failed to create table:'+str(X))
    try:
      cursor.execute("CREATE TABLE hlt2_mon_last (runid INTEGER PRIMARY KEY ASC, type TEXT)")
      cursor.execute("INSERT INTO hlt2_mon_last VALUES (?, ?)",(0,"LAST_CREATED",))
    except Exception, X:
      log(INFO,'Failed to create table:'+str(X))
    cursor.connection.commit()
    cursor.close()
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
    db = self.DbCore(self.credentials).connection()
    cursor = db.cursor()
    try:
      cursor.execute("""SELECT COUNT(state) FROM hlt2_mon""")
    except Exception, X:
      log(INFO,'Failed to execute SQL:'+str(X))
    count = cursor.fetchone()
    cursor.close()
    log(INFO,'+--------------------------------------------------------------------------------+')
    log(INFO,'|       Working directory:   %-50s  |'%(os.getcwd(),))
    log(INFO,'|       Monitoring database: %-50s  |'%(self.credentials,))
    log(INFO,'|       dto. Number of entries: %-47s  |'%(str(count),))
    log(INFO,'+--------------------------------------------------------------------------------+')
    #sys.exit(0)
