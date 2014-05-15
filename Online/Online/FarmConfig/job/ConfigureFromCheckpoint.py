import os, socket
print 'UTGID='+os.environ['UTGID']+';'
if os.environ.has_key('TEST_CHECKPOINT'):
  print 'TEST_CHECKPOINT='+os.environ['TEST_CHECKPOINT']+';'
  print 'PARTITIONOPTS='+os.environ['CHECKPOINT_DIR']+'/OnlineEnv.opts;'
elif os.environ.has_key('PARTITIONOPTS'):
  print 'PARTITIONOPTS='+os.environ['PARTITIONOPTS']+';'
if os.environ.has_key('CHECKPOINT_RESTART_OPTS'):
  print "RESTARTOPTS="+os.environ['CHECKPOINT_RESTART_OPTS']+';'
print "DIM_DNS_NODE="+os.environ['DIM_DNS_NODE']+';'
print "NUMBER_OF_CHILDREN="+os.environ['NBOFSLAVES']+';'
