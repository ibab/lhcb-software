import os, socket
if os.environ.has_key('TEST_CHECKPOINT'):
  print 'TEST_CHECKPOINT='+os.environ['TEST_CHECKPOINT']
  print 'PARTITIONOPTS='+os.environ['CHECKPOINT_DIR']+'/OnlineEnv.opts'
elif os.environ.has_key('PARTITIONOPTS'):
  print 'PARTITIONOPTS='+os.environ['PARTITIONOPTS']    
print "RESTARTOPTS="+os.environ['MOOREROOT']+"/options/RestartFromCheckpoint.opts"
print "DIM_DNS_NODE="+os.environ['DIM_DNS_NODE']

numChildren = os.sysconf('SC_NPROCESSORS_ONLN')
host = socket.gethostname().split('.')[0].upper()
if host[:3]=='HLT':
  if len(host)==8:
    id = int(host[6:])
    if id < 5: numChildren=7
    elif id < 12: numChildren=17
    elif id < 23: numChildren=19
print "NUMBER_OF_CHILDREN="+str(numChildren)
