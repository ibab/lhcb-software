import sys, time, platform
import cppyy as Dictionary
#Dictionary.gbl.Cintex.SetDebug(1)
lib_prefix = ''
if platform.system()=='Linux':
  lib_prefix = 'lib'

Dictionary.loadDict(lib_prefix+'PVSS_STLDict')

gbl  = Dictionary.gbl
std  = gbl.std
fifo = None
partition = ''

def setPartition(name):
  global partition
  partition = name
  
def openFMC(fifo_name='/tmp/logPVSSConfig.fifo'):
  global fifo
  if fifo:
    fifo.close()
  fifo = open(fifo_name,'a')
  
def closeFMC():
  global fifo
  if fifo:
    fifo.close()
    fifo = None

def makePrint(msg):
  print msg

def printFMC(severity, msg):
  if fifo:
    global partition
    import socket
    mm = '[%s]'%(severity,)
    if len(mm)<7: mm = mm + ' '
    n = time.strftime('%b%d-%H%M%S'+mm) + socket.gethostname() + ': PVSS(' + partition + '): ' + msg + '\n'
    fifo.write(n)
    fifo.flush()

logPrefix     = ''
logHeader     = '+----------------------------------------------------------------------------------------------------'
logTrailer    = logHeader

#----enable tab completion---------------------------------------------------------------
try:
  import rlcompleter,readline    
  readline.parse_and_bind("tab: complete")
except:
  pass
# =============================================================================
def load(dict):
  res = Dictionary.loadDict(lib_prefix+dict)
  return res
# =============================================================================
def timeStamp():
  "Return current time as string"
  return time.ctime(time.time())
# =============================================================================
def log(msg, with_header=None, with_trailer=None,timestamp=0,tag='INFO'):
  """
  Log normal message to output device.

  @param msg           Message content
  @param with_header   Print header
  @param with_trailer  Print trailer message
  @param timestamp     Add timestamp to log message
  """
  stamp = ''
  if len(logPrefix)>0: stamp = logPrefix + ' '
  if timestamp:        stamp = timeStamp() + ' '
  if ( with_header ):
    makePrint(stamp + logHeader)
    printFMC(tag,logHeader)
  if msg.__class__ == str().__class__:
    makePrint(stamp + '|  ' + msg)
    printFMC(tag,'|  ' + msg)
  else:
    for line in msg:
      makePrint(stamp + '|  ' + line)
      printFMC(tag,'|  ' + line)
  if ( with_trailer ):
    makePrint(stamp + logTrailer)
    printFMC(tag,'|  ' + logTrailer)
  sys.stdout.flush()

# =============================================================================
def fatal(msg, with_header=0, with_trailer=0, timestamp=0):
  """
  Log fatal message to output device.

  @param msg           Message content
  @param with_header   Print header
  @param with_trailer  Print trailer message
  @param timestamp     Add timestamp to log message
  """
  log('FATAL Error:   '+msg, with_header, with_trailer, timestamp, tag='FATAL')

# =============================================================================
def error(msg, with_header=0, with_trailer=0, timestamp=0):
  """
  Log error message to output device.

  @param msg           Message content
  @param with_header   Print header
  @param with_trailer  Print trailer message
  @param timestamp     Add timestamp to log message
  """
  log('Error:   '+msg, with_header, with_trailer, timestamp, tag='ERROR')
# =============================================================================
def warning(msg, with_header=0, with_trailer=0, timestamp=0):
  """
  Log warning message to output device.

  @param msg           Message content
  @param with_header   Print header
  @param with_trailer  Print trailer message
  @param timestamp     Add timestamp to log message
  """
  log('Warning: '+msg, with_header, with_trailer, timestamp, tag='WARN')

# =============================================================================
def printSlots(slots,prefix,mx,width=14):
  "Helper to print containers."
  s = prefix
  cnt = 0
  fmt = '%%-%ds'%width
  for i in slots:
    cnt = cnt + 1
    s = s + ' ' + fmt%str(i)
    if (cnt%mx == 0):
      cnt = 0
      log(s)
      s =prefix
  if cnt > 0: log(s)      

# =============================================================================
def sum(cont):
  "Calculate the sum of all items in a container"
  count = 0
  for i in cont:
    count = count + i
  return count

# =============================================================================
def vector2String(c):
  "Convert a container into a string separated by blanks"
  s = ''
  for i in c:
    s = s + ' ' + str(i)
  return s
