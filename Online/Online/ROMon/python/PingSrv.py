#!/bin/bash
export UTGID
exec -a ${UTGID} /usr/bin/python <</EOF
import os, socket, SimpleXMLRPCServer
class PingSrv:
  def ping(self,who,count,size):
    cmd = '/bin/ping '
    if size: cmd = cmd+' -s '+str(size)
    return os.popen(cmd+' -q -W 1 -c '+str(count)+' '+who).readlines()

def log_req(self, code='-', size='-'): pass

if __name__ == "__main__":
  server = SimpleXMLRPCServer.SimpleXMLRPCServer((socket.gethostname(),8088),logRequests=False)
  server.log_request = log_req
  server.register_instance(PingSrv())
  print '..... Ping server instance is now running.....'
  server.serve_forever()
/EOF
