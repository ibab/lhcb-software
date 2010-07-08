#!/bin/bash
export UTGID
runit()
{
exec -a ${UTGID} /usr/bin/python <</EOF
import os, sys, time, socket, threading, SimpleXMLRPCServer

class Thread(threading.Thread):
  def __init__(self):
    threading.Thread.__init__(self)
    self.commands = {}
    self.results = {}
  def ping(self,command):
    ##print 'Exec',command
    sys.stdout.flush
    res = os.popen(command).readlines()
    result = (time.time(),res,)
    self.results[command] = result
    return result

  def run(self):
    while(1):
      time.sleep(1)
      for i in self.commands:
        try:
          t = self.results[0]
          if time.time()-t > 30:
            self.ping(command)
        except Exception, X:
          pass
    
class PingSrv:
  def __init__(self):
    self.thread = Thread()
    self.thread.start()

  def ping2(self,who,count,size):
    cmd = '/bin/ping '
    if size: cmd = cmd+' -s '+str(size)
    command = cmd+' -q -W 1 -c '+str(count)+' '+who
    return os.popen(command).readlines()
  
  def ping(self,who,count,size):
    cmd = '/bin/ping '
    if size: cmd = cmd+' -s '+str(size)
    command = cmd+' -q -W 1 -c '+str(count)+' '+who
    if self.thread.results.has_key(command):
      return self.thread.results[command][1]
    return self.thread.ping(command)[1]
  
def log_req(self, code='-', size='-'): pass

if __name__ == "__main__":
  port=8088
  if len('${1}')>0:
    port=int('${1}')
  print 'Port:',port
  server = SimpleXMLRPCServer.SimpleXMLRPCServer((socket.gethostname(),port),logRequests=False)
  server.log_request = log_req
  server.register_instance(PingSrv())
  print '..... Ping server instance is now running.....'
  ##print '..... Running test instance'
  sys.stdout.flush
  server.serve_forever()
/EOF

sleep 10;
}

while [ 1 ]; do  runit; done;
