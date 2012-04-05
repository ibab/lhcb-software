import os
print "UTGID="         + os.environ['UTGID']
print "LOGFIFO="       + os.environ['LOGFIFO']
print "DIM_DNS_NODE="  + os.environ['DIM_DNS_NODE']
if os.environ.has_key('PARTITIONOPTS'):
  print "PARTITIONOPTS=" + os.environ['PARTITIONOPTS']
print "RESTARTOPTS="   + os.environ['ONLINEBRUNELSYSROOT']+"/options/BrunelOnlineRestart.opts"
if os.environ.has_key('TEST_CHECKPOINT'):
    print "TEST_CHECKPOINT=" + os.environ['TEST_CHECKPOINT']
