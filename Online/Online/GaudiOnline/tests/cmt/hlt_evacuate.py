import os, sys, time
batch = 0

def nextTest(msg):
  print msg
  val = ''
  try:
    if ( batch ):
      return 1
    val=str(input('Continue [Y=CR/N]'))
  except NameError, X:
    val = 'N'
  except SyntaxError, X:
    return 1
  except Exception, X:
    print X.__class__, str(X)
    val = 'N'
  if len(val) > 0:
    sys.exit(0)
    return 0
  return 1

def printResult(r):
    print '\n\n Test  with ',r[0],' receivers fed by ',r[1],' clients'
    print '     Test took  ',r[2],' seconds '
    print '             =  ',(r[0]*r[1]*30000)/(r[2])/1000,' kHz'

def startCmd(utgid,command,async=1):
    os.environ['UTGID'] = utgid
    if async: os.system(command + " &")
    else:     os.system(command)

def startTerm(typ,utgid,cmd,async):
    command = typ+" ${UTGID}@${HOST} -e \"/bin/sh -c '"+cmd+"'\""
    startCmd(utgid,command,async)

def startMiniTerm(utgid,cmd,async=1):  startTerm("$MINITERM",utgid,cmd,async)
def startWideTerm(utgid,cmd,async=1):  startTerm("$WIDETERM",utgid,cmd,async)
def startBigTerm(utgid,cmd,async=1):   startTerm("$BIGTERM",utgid,cmd,async)

def preamble():
    config = '/'+os.environ['CMTCONFIG']+'/'
    os.environ['TAN_PORT']='YES'
    os.environ['TAN_NODE']=os.environ['HOSTNAME']
    os.environ['DIM_DNS_NODE']=os.environ['HOSTNAME']
    os.environ['OPTS']=os.environ['GAUDIONLINEROOT']+'/options'
    os.environ['MSG_SVC']='LHCb::DimMessageSvc'
    os.environ['MSG_SVC']='MessageSvc'
    os.environ['test_exe']=os.environ['ONLINEKERNELROOT']+config+'test.exe'
    os.environ['gaudi_run']=os.environ['GAUDIONLINEROOT']+config+'Gaudi.exe libGaudiOnline.so OnlineStart '
    os.environ['gaudi_exe']=os.environ['GAUDIONLINEROOT']+config+'Gaudi.exe libGaudiOnline.so OnlineTask -msgsvc='+os.environ['MSG_SVC']+' -auto '
    os.environ['gaudi_exe2']=os.environ['GAUDIONLINEROOT']+config+'Gaudi.exe libGaudiOnline.so OnlineTask -msgsvc=MessageSvc -auto '
    os.environ['MINITERM']='xterm  -ls -132 -geometry 132x12 -title '
    os.environ['BIGTERM']='xterm  -ls -132 -geometry 132x45 -title '
    os.environ['WIDETERM']='xterm  -ls -132 -geometry 160x50 -title '

def killTasks():
    os.system('killall Gaudi.exe')
    os.system('killall Gaudi.exe')
    os.system('killall DNS.exe')


def submit(batches, clients):
    preamble()
    killTasks()
    time.sleep(1)
    startMiniTerm('DimDNS','../../../DIM/$CMTCONFIG/DNS.exe')
    startMiniTerm('OutputBuff','$gaudi_exe -main=$OPTS/MBMinit.opts -opt=$OPTS/Daemon.opts')
    startMiniTerm('InputBuff', '$gaudi_exe -main=$OPTS/MBMfarm.opts -opt=$OPTS/Daemon.opts')
    nextTest('Ready to sytart monitors?')
    startBigTerm('MbmMon','$gaudi_run libOnlineKernel.so mbm_mon')
    startBigTerm('TanMon','$gaudi_run libOnlineKernel.so tanmon -c')

    for b in xrange(batches):
      n = str(b)
      startMiniTerm('RECV_'+n,'$gaudi_exe -opt=$OPTS/NetReceiver.opts')
    time.sleep(5)
    for b in xrange(batches):
      n = str(b)
      for i in xrange(clients):
        m = str(i)
        startMiniTerm('SEND'+n+'_'+m,'$gaudi_exe -opt=$OPTS/NetSend_'+n+'.opts')

    nextTest('Start test ?')
    start = time.time()
    startCmd('Prod_0','time $GAUDIKERNELROOT/$CMTCONFIG/Gaudi.exe $OPTS/MDF2MBM.opts',0)
    stop = time.time()-3  # typical startup time for task - to be subtracted....
    res = (batches,clients,stop-start)
    printResult(res)
    return res

if __name__=="__main__":
    results=[]
    nextTest('Test configuration 1x1:')
    results.append(submit(1,1))
    nextTest('Test configuration 2x2:')
    results.append(submit(2,2))
    nextTest('Test configuration 1x15:')
    results.append(submit(1,15))
    nextTest('Test configuration 2x15:')
    results.append(submit(2,15))
    for r in results:
      printResult(r)
        
