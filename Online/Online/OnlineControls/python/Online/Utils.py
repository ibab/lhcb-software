import sys, time, platform
import PyCintex as Dictionary
#Dictionary.gbl.Cintex.SetDebug(1)
lib_prefix = ''
if platform.system()=='Linux':
  lib_prefix = 'lib'

Dictionary.loadDict(lib_prefix+'STLRflx')

gbl  = Dictionary.makeNamespace('')
std  = gbl.std

def makePrint(msg):
  print msg
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
def timeStamp():
  "Return current time as string"
  return time.ctime(time.time())
# =============================================================================
def log(msg, with_header=None, with_trailer=None,timestamp=0):
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
  if msg.__class__ == str().__class__:
    makePrint(stamp + '|  ' + msg)
  else:
    for line in msg:
      makePrint(stamp + '|  ' + line)
  if ( with_trailer ):
    makePrint(stamp + logTrailer)
  sys.stdout.flush()
# =============================================================================
def error(msg, with_header=0, with_trailer=0, timestamp=0):
  """
  Log error message to output device.

  @param msg           Message content
  @param with_header   Print header
  @param with_trailer  Print trailer message
  @param timestamp     Add timestamp to log message
  """
  log('Error:   '+msg, with_header, with_trailer, timestamp)
# =============================================================================
def warning(msg, with_header=0, with_trailer=0, timestamp=0):
  """
  Log warning message to output device.

  @param msg           Message content
  @param with_header   Print header
  @param with_trailer  Print trailer message
  @param timestamp     Add timestamp to log message
  """
  log('Warning: '+msg, with_header, with_trailer, timestamp)

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
