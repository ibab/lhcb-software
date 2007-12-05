import os, re, sys, time, socket

dataFlowDirScripts = '/group/online/dataflow/cmtuser/Gaudi_v19r2/Online/OnlineControls/scripts'
default_exports    = 'export PATH=/opt/FMC/sbin:/opt/FMC/bin:$PATH;export LD_LIBRARY_PATH=/opt/FMC/lib;'
top_logger_host    = 'ecs03'
top_farm_host      = 'hlt01'

#------------------------------------------------------------------------------
def setupEnv():
  print default_exports

#------------------------------------------------------------------------------
def stopProcessingNodeTasks(host):
  print 'tmStop -m '+host+' TANServ_0;'
  print 'tmStop -m '+host+' MBMMON_0;'
  print 'tmStop -m '+host+' MBMWeb_0;'

#------------------------------------------------------------------------------
def startProcessingNodeTasks(host):
  print 'tmStart -m '+host+' -e -o -u TANServ_0 '+dataFlowDirScripts+os.sep+'tan_server.sh;'
  print 'tmStart -m '+host+' -e -o -u MBMMON_0  '+dataFlowDirScripts+os.sep+'MBMMON.upi.sh;'
  print 'tmStart -m '+host+' -e -o -u MBMWeb_0  '+dataFlowDirScripts+os.sep+'MBMMON.web.sh;'

#------------------------------------------------------------------------------
def stopControlsNodeTasks(host,next_level_server=None):
  print 'tmStop -m '+host+' TANServ_0;'
  #print 'tmStop -m '+host+' FSMMON_0;'
  print 'tmStop -m '+host+' LogCollect_0;'
  if next_level_server is not None:
    print 'tmStop -m '+host+' LogServer_0;'

#------------------------------------------------------------------------------
def startControlsNodeTasks(host,next_level_server=None):
  print 'tmStart -m '+host+' -e -o -u TANServ_0 '+dataFlowDirScripts+os.sep+'tan_server.sh;'
  #print 'tmStart -m '+host+' -e -o -u FSMMON_0  '+dataFlowDirScripts+os.sep+'FSMMON.upi.sh;'
  if next_level_server is not None:
    print 'tmStart -m '+host+' -e -o -u LogServer_0 '+dataFlowDirScripts+os.sep+'LogCollectSrv.sh '+next_level_server+';'
    print 'sleep 1;'
  print 'tmStart -m '+host+' -e -o -u LogCollect_0 '+dataFlowDirScripts+os.sep+'LogCollect.sh;'

#------------------------------------------------------------------------------
def check(match,test):
  match = re.match(match.upper(),test.upper())
  if match:
    if match.pos==0 and match.endpos==len(test):
      return 1
  return None

#------------------------------------------------------------------------------
def startTasks(host):
  HOST = host.upper()
  if check('hlt[a-f][0-9][0-9][0-9][0-9]',HOST):
    print 'echo "This is a farm node:'+host+'";'
    startProcessingNodeTasks(host)
  elif check('hlt[a-f][0-9][0-9]',HOST):
    print 'echo "This is a farm controls node:'+host+'";'
    startControlsNodeTasks(host,top_logger_host)
    #startControlsNodeTasks(host,top_farm_host)
  elif check('mon[a-f][0-9][0-9][0-9][0-9]',HOST):
    print 'echo "This is a monitoring node:'+host+'";'
    startProcessingNodeTasks(host)
  elif check('mon[a-f][0-9][0-9]',HOST):
    print 'echo "This is a monitoring controls node:'+host+'";'
    startControlsNodeTasks(host,top_logger_host)
  elif check('store[rs][et][cr][mv]0[12]',HOST):
    print 'echo "This is a storage node:'+host+'";'
    startProcessingNodeTasks(host)
  elif check('storectl01',HOST):
    print 'echo "This is a storage controls node:'+host+'";'
    startControlsNodeTasks(host,top_logger_host)
  elif check(top_farm_host,HOST):
    print 'echo "This is the TOP farm controls node:'+host+'";'
    startControlsNodeTasks(host,top_logger_host)
  else:
    print 'echo "Got node of unknown type:',HOST,'";'

def stopTasks(host):
  HOST = host.upper()
  if check('hlt[a-f][0-9][0-9][0-9][0-9]',HOST):
    print 'echo "This is a farm node:'+host+'";'
    stopProcessingNodeTasks(host)
  elif check('hlt[a-f][0-9][0-9]',HOST):
    print 'echo "This is a farm controls node:'+host+'";'
    #stopControlsNodeTasks(host,top_farm_host)
    stopControlsNodeTasks(host,top_logger_host)
  elif check('mon[a-f][0-9][0-9][0-9][0-9]',HOST):
    print 'echo "This is a monitoring node:'+host+'";'
    stopProcessingNodeTasks(host)
  elif check('mon[a-f][0-9][0-9]',HOST):
    print 'echo "This is a monitoring controls node:'+host+'";'
    stopControlsNodeTasks(host,top_logger_host)
  elif check('store[rs][et][cr][mv]0[12]',HOST):
    print 'echo "This is a storage node:'+host+'";'
    stopProcessingNodeTasks(host)
  elif check('storectl01',HOST):
    print 'echo "This is a storage controls node:'+host+'";'
    stopControlsNodeTasks(host,top_logger_host)
  elif check(top_farm_host,HOST):
    print 'echo "This is the TOP farm controls node:'+host+'";'
    stopControlsNodeTasks(host,top_logger_host)
  else:
    print 'echo "Got node of unknown type:',HOST,'";'

def hostName():
  host = socket.gethostname()
  if host.find('.')>0:
    host = host[:host.find('.')]
  return host

def start():
  host = hostName()
  setupEnv()
  startTasks(host)

def stop():
  host = hostName()
  setupEnv()
  stopTasks(host)
  
def restart():
  host = hostName()
  setupEnv()
  stopTasks(host)
  print 'sleep 1;'
  startTasks(host)
  
#host_names = ['storectl01','storerecv01','storerecv02','storestrm01','storestrm02',
#              'mona08','mona0801','mona0802','mona0803','mona0804',
#              'hlt01','hlte08','hlte0801','hlte0802','hlte0803','hlte0804'
#              ]
#for i in host_names:
#  startTasks(i)

if __name__ == "__main__":
  start()
