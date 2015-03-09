"""
        Package module

        \author  M.Frank
        \version 1.0

"""

DEBUG           = 1
INFO            = 2
WARNING         = 3
ERROR           = 4

# ------------------------------------------------------------------------------
def log(level,msg):
  """
  
  @author   M.Frank
  @version  1.0
  @date     30/06/2002
  """
  if level == DEBUG:
    print 'DEBUG  ',msg
  elif level == INFO:
    print 'INFO   ',msg
  elif level == WARNING:
    print 'WARNING',msg
  elif level == ERROR:
    print 'ERROR',msg

# ------------------------------------------------------------------------------
def make_directory(nam):
  """
  
  @author   M.Frank
  @version  1.0
  @date     30/06/2002
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
  
  @author   M.Frank
  @version  1.0
  @date     30/06/2002
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
