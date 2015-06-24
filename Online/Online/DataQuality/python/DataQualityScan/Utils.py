import os, sys, time, traceback
import Config

configuration = Config.Config()

run_hdr  = '| %-5s | %6s | %-8s | %-15s | %-15s | %-20s | %-8s | %-8s |'%\
          ('Part','Run','State','Activity','Run Type','Start Time', 'Todo', 'Done')
run_line = ('| %-5s | %6s | %-8s | %-15s | %-15s | %-20s | %-8s | %-8s |'%\
            ('','','','','','', '', '')).replace(' ','-').replace('|','+')
run_fmt  = '| %-5s | %6d | %-8s | %-15s | %-15s | %-20s | %-8s | %-8s |'


# ------------------------------------------------------------------------------
class Utils:
  """
  Data file scanner to steer the data quality processing after HLT2.
  - This class scan the run database for recent runs of the LHCb partition
    --> self.scanDatabase(<partition>)

  \author  M.Frank
  \version 1.0
  """

  # ----------------------------------------------------------------------------
  def __init__(self):
    """ Initializing constructor of the file scanner

    \author  M.Frank
    \version 1.0
    """
    self.config = configuration
    self.__mondb = None
    self.__rundb = None
    self.output  = sys.stdout
    
  # ----------------------------------------------------------------------------
  def rund(self):
    import DbAccess
    if not self.__rundb:
      self.__rundb  = DbAccess.RunDB(config=self.config,
                                     db=self.config.rundb)
    return self.__rundb

  # ----------------------------------------------------------------------------
  def mondb(self):
    import DbAccess
    if not self.__mondb:
      self.__mondb  = DbAccess.MonDB(config=self.config,
                            db=self.config.mondb,
                            table=self.config.mondb_table)
    return self.__mondb

  def printRun(self, run):
    if run:
      tm = time.localtime(run[11])
      start = time.strftime('%Y-%m-%d %H-%M-%S',tm)
      tm = time.localtime(run[2])
      todo = time.strftime('%H-%M-%S',tm)
      done = ' n/a '
      if run[3]>0:
        tm = time.localtime(run[3])
        done = time.strftime('%H-%M-%S',tm)
      print >> self.output, run_fmt%(run[7],run[0],run[1],run[8],run[9],start,todo,done,)

  # -------------------------------------------------------------------------------
  def printRuns(self, runs):
    l = len(run_hdr)
    s = '+'
    s = s + (l-2)*'-'
    s = s + '+'
    print >> self.output, run_line
    print >> self.output, run_hdr
    print >> self.output, run_line
    for run in runs:
      self.printRun(run)
    print >> self.output, run_line


# -------------------------------------------------------------------------------
utilities = Utils()
# -------------------------------------------------------------------------------
def runsByState(state):
  """
  Dump the runs with a given state from the database in a reasonable
  format useful for clients
  """
  print '*** Print runs in state: '+str(state)
  params = {}
  stmt = """SELECT 
                  runid, state, todo, done, checked, archived, 
                  partitionid, partitionname, activity, 
                  runtype, params, starttime
            FROM %s"""%(configuration.mondb_table,)
  if state:
    stmt = stmt + " WHERE state=:state"
    params = {'state': state }
  cursor = utilities.mondb().execute(stmt, params)
  runs = cursor.fetchall()
  cursor.close()
  utilities.printRuns(runs)

# -------------------------------------------------------------------------------
def runsByNumber(run_numbers):
  """
  Dump the runs with a given state from the database in a reasonable
  format useful for clients
  """
  if not len(run_numbers):
    print '*** Cannot print: no run nubers supplied.'
    return False
  
  print '*** Print runs in with run numbers: '+str(run_numbers)
  runs = ''
  for i in run_numbers:
    runs = runs + ' runid='+str(i)+' OR '
  runs = runs[:-4]
  stmt = """SELECT 
                  runid, state, todo, done, checked, archived, 
                  partitionid, partitionname, activity, 
                  runtype, params, starttime
            FROM %s
            WHERE %s"""%(configuration.mondb_table,runs,)
  cursor = utilities.mondb().execute(stmt, {})
  runs = cursor.fetchall()
  cursor.close()
  utilities.printRuns(runs)

# -------------------------------------------------------------------------------
def rescheduleRun(run):
  run_no = int(run)
  if utilities.mondb().hasRun(run_no):
    return utilities.mondb().setRunTodo(run_no)
  print '*** Cannot reschedule: Run %d is not known to the database'%(run_no,)
  return None

# -------------------------------------------------------------------------------
def archiveRun(run):
  run_no = int(run)
  if utilities.mondb().hasRun(run_no):
    return utilities.mondb().setRunTodo(run_no)
  print '*** Cannot archive: Run %d is not known to the database'%(run_no,)
  return None

# -------------------------------------------------------------------------------
def run(run_auto=False):
  runsByState(configuration.st_todo)
  runsByState(configuration.st_running)
  runsByState(configuration.st_done)
  rescheduleRun(123)
  archiveRun(123)

# -------------------------------------------------------------------------------
def processArgs():
  import optparse
  parser = optparse.OptionParser()
  parser.description = "DataQuality Utilities  [by M.Frank]"

  parser.add_option("-r", "--reschedule", dest="reschedule", default=False,
                    help="Define rescheduling action to be executed",
                    action="store_true", metavar="<boolean>")

  parser.add_option("-a","--archive",
                    dest="archive", default=False,
                    help="Define archiveing action to be scheduled",
                    metavar="<boolean>")

  parser.add_option("-d","--dump",
                    dest="dump", default=False,
                    help="Dump the database content",
                    action="store_true", metavar="<boolean>")

  parser.add_option("-s", "--state", dest="state", default=None,
                  help="Define state of runs to be dumped",
		  metavar="<string>")

  (opts, args) = parser.parse_args()
  print 'Options: ',opts, 'Arguments: ',args
  
  if not len(args) and not opts.dump:
    print '     ',len(args),opts.dump,opts.state,\
          not (opts.dump and opts.state),\
          not (opts.dump and (opts.state is not None))
    print '     ',parser.format_help()
  elif opts.reschedule:
    for run in args:  rescheduleRun(run)
    if opts.dump:     runsByNumber(args)
  elif opts.archive:
    for run in args:  rescheduleRun(run)
    if opts.dump:     runsByNumber(args)
  elif opts.dump and opts.state:
    runsByState(opts.state)
  elif opts.dump and len(args):
    runsByNumber(args)
  elif opts.dump:
    runsByState(None)
  else:
    print '      ',parser.format_help()
  sys.exit(0)
    
# -------------------------------------------------------------------------------
if __name__=="__main__":
  processArgs()
