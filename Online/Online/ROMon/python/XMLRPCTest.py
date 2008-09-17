#!/bin/bash
export UTGID
exec -a ${UTGID} /usr/bin/python <</EOF
import os, sys, socket, xmlrpclib,SimpleXMLRPCServer
class XMLRPCTestSrv:
  def ping(self,who,count,size):
    ti = xmlrpclib.DateTime();
    return (1,2,3.,4.,'hello_0',ti,True,(11,22,33.,44.,'hello_1',False,),[111,222,333.,444.,'hello_2',ti],
            {'d1':1111,'d2':2222,'d3':3333.,'d4':4444.,'ds':'hello_3'})

def log_req(self, code='-', size='-'): pass

if __name__ == "__main__":
  port=8088
  if len('${1}')>0:
    port=int('${1}')
  print 'Port:',port
  server = SimpleXMLRPCServer.SimpleXMLRPCServer((socket.gethostname(),port),logRequests=False)
  server.log_request = log_req
  server.register_instance(XMLRPCTestSrv())
  print '..... XMLRPCTest server instance is now running.....'
  server.serve_forever()
/EOF
