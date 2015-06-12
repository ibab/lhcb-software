"""
        Package module

        \author  M.Frank
        \version 1.0

"""
import os
VERBOSE         = 1
DEBUG           = 2
INFO            = 3
WARNING         = 4
ERROR           = 5
ALWAYS          = 6

minPrintLevel = INFO
utgid = ''
if os.environ.has_key('UTGID'): 
  utgid = os.environ['UTGID']+': '

# ------------------------------------------------------------------------------
def log(level,msg):
  """
  
  \author   M.Frank
  \version  1.0
  \date     30/06/2002
  """
  import sys
  f = sys.stdout
  if level < minPrintLevel:
    return
  elif level == DEBUG:
    print >>f, '%s[DEBUG]   %s'%(utgid,msg,)
  elif level == INFO:
    print >>f, '%s[INFO ]   %s'%(utgid,msg,)
  elif level == WARNING:
    print >>f, '%s[WARNING] %s'%(utgid,msg,)
  elif level == ERROR:
    print >>f, '%s[ERROR]   %s'%(utgid,msg,)
  elif level == ALWAYS:
    print >>f, '%s[SUCCESS] %s'%(utgid,msg,)
  elif level >  ALWAYS:
    print >>f, '%s[INFO]    %s'%(utgid,msg,)
  f.flush()

# ------------------------------------------------------------------------------
def print_item(msg, r):
  if type(r) is type({}):
    for i in r.keys():
      print_item(msg+'.'+str(i), r[i])
  elif type(r) is type([]):
    for i in xrange(len(r)):
      print_item('  '+msg+' ['+str(i)+']',r[i])
  else:
    print '  '+msg+' : '+str(r)

# ------------------------------------------------------------------------------
def make_directory(nam):
  """
  
  \author   M.Frank
  \version  1.0
  \date     30/06/2002
  """
  import os
  ##print 'make dir',nam
  try:
    os.mkdir(nam)
  except Exception, X:
    log(ERROR,'Failed to create directory:%s [%s]'%(nam,str(X),))
    pass

# ------------------------------------------------------------------------------
def move_run_dir(runid, from_dir, to_dir):
  """
  
  \author   M.Frank
  \version  1.0
  \date     30/06/2002
  """
  import os
  from_dir_entry = from_dir + os.sep + str(runid)
  to_dir_entry   = to_dir   + os.sep + str(runid)

  # Check if source directory exists:
  try:
    os.stat(from_dir_entry)
  except Exception,X:
    raise(X)
  # Check if target directory already exists:
  try:
    os.stat(to_dir_entry)
    raise(Exception('Todo entry '+to_dir_entry+' already exists!'))
  except Exception,X:
    pass

  # Now move it:
  try:
    os.rename(from_dir_entry,to_dir_entry)
  except OSError, e:
    if e.errno==39: # OSError: [Errno 39] Directory not empty
      os.system('rm -rf '+to_dir_entry)
      os.rename(from_dir_entry,to_dir_entry)
