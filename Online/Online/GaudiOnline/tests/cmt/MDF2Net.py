#!/usr/bin/python
import os, sys, time
def startTask(i,j):
  os.environ['UTGID'] = 'PROD_%02d'%j
  cmd = os.environ['gaudi_exe']+" -opt=MDF2Net%d.opts -msgsvc=$MSGSVC -main=$GAUDIONLINEROOT/options/Main.opts"%i
  args = cmd.split(' ')
  print args
  exe = args[0]
  args[0] = "Gaudi.exe"
  print exe,args
  return os.spawnvp(os.P_NOWAIT,exe,args)

host = os.environ['HOSTNAME']
if host == 'storerecv01':
  p1 = startTask(1,1)
  p2 = startTask(2,2)
  p3 = startTask(1,3)
  p4 = startTask(2,4)
if host == 'storerecv02':
  p1 = startTask(3,5)
  p2 = startTask(3,6)

while(1):
  try:
    print time.time()
    time.sleep(10)
  except:
    print 'Done....'
    sys.exit(0)
