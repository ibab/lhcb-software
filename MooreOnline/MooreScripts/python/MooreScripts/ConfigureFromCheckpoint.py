import os, socket
#print 'export UTGID=Some_long_value_bigger_than_true_UTGID;'
print 'UTGID='+os.environ['UTGID']+';'
if os.environ.has_key('TEST_CHECKPOINT'):
  print 'TEST_CHECKPOINT='+os.environ['TEST_CHECKPOINT']+';'
  print 'PARTITIONOPTS='+os.environ['CHECKPOINT_DIR']+'/OnlineEnv.opts;'
elif os.environ.has_key('PARTITIONOPTS'):
  print 'PARTITIONOPTS='+os.environ['PARTITIONOPTS']+';'
print "RESTARTOPTS="+os.environ['MOOREROOT']+"/options/RestartFromCheckpoint.opts;"
print "DIM_DNS_NODE="+os.environ['DIM_DNS_NODE']+';'
"""
# Forget about all this. the number of children is now passed as an argument to the
# Moore startup script.
numChildren = os.sysconf('SC_NPROCESSORS_ONLN')
host = socket.gethostname().split('.')[0].upper()
if host[:3]=='HLT':
  if host[3]=='F': numChildren=19
  if host[3]!='F':
    if len(host)==8:
      id = int(host[6:])
      if id < 12: numChildren=7
      if id > 11: numChildren=19
"""
numChildren = os.environ['NBOFSLAVES']
print "NUMBER_OF_CHILDREN="+str(numChildren)+';'
