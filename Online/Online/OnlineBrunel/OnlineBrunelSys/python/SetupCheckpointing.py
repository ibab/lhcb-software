import os, sys
import OnlineEnvBase as Online

vsn = os.environ['ONLINEBRUNELSYSROOT']
vsn = vsn[vsn.find('/OnlineBrunel_')+len('/OnlineBrunel_'):]
vsn = vsn[:vsn.find('/')]
checkpoint_file = '/group/online/dataflow/cmtuser/checkpoints/OnlineBrunel/OnlineBrunel_'+vsn+os.path.sep +\
       Online.CondDBTag+os.path.sep+Online.DDDBTag+os.path.sep+'Checkpoint.data'

base = os.path.basename(checkpoint_file)
try:
  os.stat(checkpoint_file)
except Exception,X:
  print 'echo "Checkpoint file is not accessible:'+str(X)+'";'
  print 'CHECKPOINT_FILE='+checkpoint_file+';'
  ###print 'exit 1;'
  sys.exit(2)

print 'CHECKPOINT_FILE=/dev/shm/'+base+';'
print 'cd /dev/shm;'
print '/group/online/dataflow/scripts/BitTorrentGet '+checkpoint_file+'.torrent -timeout=30 -print=4 -retry=2;'
print 'cd -;'
