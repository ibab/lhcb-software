#!/usr/bin/python

import socket
import struct

HOST1 = ''                 # Symbolic name meaning all available interfaces
PORT1 = 45247              # Arbitrary non-privileged port
HOST2 = ''                 # Symbolic name meaning all available interfaces
PORT2 = 45248              # Arbitrary non-privileged port
s1 = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s1.bind((HOST, PORT))
s1.listen(1)
conn1, addr1 = s1.accept()
s2 = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s2.bind((HOST, PORT))
s2.listen(1)
conn2, addr2 = s2.accept()
while 1:
    data = conn1.recv(1024)
    if not data: break
    cmdNo, runNo, fileName = struct.unpack("!II256s",data[:264])
    cmd=data[264:]
    seqNum=struct.unpack("!I", cmd[:4])

    ack=struct.pack("!III", runNo, seqNum, seqNum) 

    conn.send(ack)
conn.close()

