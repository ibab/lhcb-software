import os, sys
if os.environ.has_key('PARTITION_NAME'):
  pname = os.environ['PARTITION_NAME']
else:
  pname = os.environ['PARTITION']
print 'UTGID='+os.environ['UTGID']+';'
print "DIM_DNS_NODE="+os.environ['DIM_DNS_NODE']+';'
print "NUMBER_OF_CHILDREN="+os.environ['NBOFSLAVES']+';'
if os.environ.has_key('TEST_CHECKPOINT'):
  print 'TEST_CHECKPOINT='+os.environ['TEST_CHECKPOINT']+';'
  print 'PARTITIONOPTS='+os.environ['CHECKPOINT_DIR']+'/OnlineEnv.opts;'
else:
  print 'PARTITIONOPTS=/group/online/dataflow/options/'+pname+'/'+pname+'_Info.opts;'
  if os.environ.has_key('CHECKPOINT_SETUP_OPTIONS'):
    print "CHECKPOINT_SETUP_OPTIONS="+os.environ['CHECKPOINT_SETUP_OPTIONS']+';'
  if os.environ.has_key('CHECKPOINT_RESTART_OPTIONS'):
    print "RESTARTOPTS="+os.environ['CHECKPOINT_RESTART_OPTIONS']+';'
