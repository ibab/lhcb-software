import os, re, sys, time, socket

dataFlowDirScripts = '/group/online/dataflow/cmtuser/Gaudi_v19r2/Online/OnlineControls/scripts'
onlineScripts      = '/group/online/dataflow/scripts'
default_exports    = 'export PATH=/opt/FMC/sbin:/opt/FMC/bin:$PATH;export LD_LIBRARY_PATH=/opt/FMC/lib;'
top_logger_host    = 'ecs03'
top_farm_host      = 'hlt01'

use_tan = 1
use_log = 1
use_mbm = 1
use_pub = 1

#------------------------------------------------------------------------------
def setupEnv():
  print default_exports

#------------------------------------------------------------------------------
def stopProcessingNodeTasks(host):
  if use_tan is not None:
    print 'tmStop -m '+host+' TANServ_'+host+';'
  print 'tmStop -m '+host+' MBMMON_0;'
  print 'tmStop -m '+host+' MBMWeb_'+host+';'
  if use_pub is not None:
    print 'tmStop -m '+host+' ROCollect_'+host+';'
    print 'tmStop -m '+host+' ROPublish_'+host+';'

#------------------------------------------------------------------------------
def startProcessingNodeTasks(host):
  dir = onlineScripts+os.sep
  if use_tan is not None:
    print 'tmStart -m '+host+' -e -o -u TANServ_'+host+'   '+dir+'tan_server.sh;'
  if use_mbm is not None:
    print 'tmStart -m '+host+' -e -o -u MBMMON_0           '+dir+'MBMMON.upi.sh;'
    print 'tmStart -m '+host+' -e -o -u MBMWeb_'+host+'    '+dir+'MBMMON.web.sh;'
  if use_pub is not None:
    print 'tmStart -m '+host+' -e -o -u ROCollect_'+host+' '+dir+'ROMONCollect.sh;'
    print 'tmStart -m '+host+' -e -o -u ROPublish_'+host+' '+dir+'ROMONPublish.sh;'

#------------------------------------------------------------------------------
def stopControlsNodeTasks(host,next_level_server=None):
  if use_tan is not None:
    print 'tmStop -m '+host+' TANServ_'+host+';'
  if use_log is not None:
    print 'tmStop -m '+host+' LogCollect_'+host+';'
  if use_pub is not None:
    print 'tmStop -m '+host+' RONodeCollect_'+host+';'
  if next_level_server is not None:
    if use_pub is not None:
      print 'tmStop -m '+host+' RONodePublish_'+host+';'
    if use_log is not None:
      print 'tmStop -m '+host+' LogServer_'+host+';'

#------------------------------------------------------------------------------
def startControlsNodeTasks(host,next_level_server=None):
  dir = onlineScripts+os.sep
  if use_tan is not None:
    print   'tmStart -m '+host+' -e -o -u TANServ_'+host+'       '+dir+'tan_server.sh;'
  if next_level_server is not None:
    if use_log is not None:
      print 'tmStart -m '+host+' -e -o -u LogServer_'+host+'     '+dir+'LogCollectSrv.sh '+next_level_server+';'
      print 'sleep 1;'
      print 'tmStart -m '+host+' -e -o -u LogCollect_'+host+'    '+dir+'LogCollect.sh;'
  if use_pub is not None:
    print   'tmStart -m '+host+' -e -o -u RONodeCollect_'+host+' '+dir+'ROMONNodeCollect.sh;'
    if next_level_server is not None:
      print 'tmStart -m '+host+' -e -o -u RONodePublish_'+host+' '+dir+'ROMONNodePublish.sh '+next_level_server+';'

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
