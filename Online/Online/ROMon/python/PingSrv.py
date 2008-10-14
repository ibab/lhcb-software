#!/bin/bash
export UTGID
runit()
{
#exec -a ${UTGID} /usr/bin/python <</EOF
/usr/bin/python <</EOF
import os, sys, socket, SimpleXMLRPCServer
class PingSrv:
  def ping(self,who,count,size):
    cmd = '/bin/ping '
    if size: cmd = cmd+' -s '+str(size)
    return os.popen(cmd+' -q -W 1 -c '+str(count)+' '+who).readlines()

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
  server.serve_forever()
/EOF

sleep 10;
}

while [ 1 ]; do  runit; done;
