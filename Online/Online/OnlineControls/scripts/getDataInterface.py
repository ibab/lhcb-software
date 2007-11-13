#!/usr/bin/python
import sys,socket
s = ''
try:
    s=socket.gethostbyname(socket.gethostname().split('.')[0]+'-d1')
    #print s
except:
    #print socket.gethostbyname(socket.gethostname().split('.')[0])
    print socket.gethostname().split('.')[0]
    sys.exit(0)
print socket.gethostname().split('.')[0]+'-d1'
