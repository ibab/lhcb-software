import time, PVSS
std = PVSS.gbl.std
DataPoint = PVSS.DataPoint

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
  return time.ctime(time.time())
# =============================================================================
def log(msg, with_header=0, with_trailer=0,timestamp=0):
  stamp = ''
  if timestamp: stamp = ' ' + timeStamp()
  if ( with_header != 0 ):
    print logPrefix + ' ' + stamp + ' ' + logHeader
  if msg.__class__ == str().__class__:
    print logPrefix + stamp + ' |  ' + msg
  else:
    for line in msg:
      print logPrefix + stamp + ' |  ' + line
  if ( with_trailer != 0 ):
    print logPrefix + stamp + ' ' + logTrailer
# =============================================================================
def error(msg, with_header=0, with_trailer=0, timestamp=0):
  log('Error:   '+msg, with_header, with_trailer, timestamp)
# =============================================================================
def warning(msg, with_header=0, with_trailer=0, timestamp=0):
  log('Warning: '+msg, with_header, with_trailer, timestamp)

# =============================================================================
def printSlots(slots,prefix,mx,width=14):
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
  count = 0
  for i in cont:
    count = count + i
  return count

# =============================================================================
def vector2String(c):
  s = ''
  for i in c:
    s = s + ' ' + str(i)
  return s
