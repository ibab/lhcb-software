import socket
import Online.PVSS as PVSS
import Online.PVSSSystems as Systems

mgr = Systems.controlsMgr('ECS')
dp  = PVSS.DataPoint(mgr,PVSS.DataPoint.original('ECS:LHCb_RunInfo.SubDetectors.tell1List'))
rd  = mgr.devReader()
rd.add(dp)
rd.execute()
tell1_ips = {}

for i in dp.data:
  ip = socket.gethostbyname(i)
  print '%-13s %-13s'%(i,ip)
  if tell1_ips.has_key(ip):
    print 'Tell1 board ',i,ip,' is already known:',tell1_ips[ip]
  tell1_ips[ip] = i

print 'Checked IP addresses of ',dp.data.size(),' tell 1 boards'
