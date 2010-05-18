import os
lines=os.popen('ps -ef | grep -i RECHLT | grep PROJ').readlines()
pids='kill -9 '
cnt = 0
for l in lines:
    if l.find('grep')>=0: continue
    #online   26997 26865  0 Jan08 ?        00:00:15 /opt/pvss/pv
    items=l.split()
    pid  = items[1]
    pids = pids+' '+pid
    cnt  = cnt + 1
    print pid, l[:-1]

if cnt>0:
  print pids
  os.system('sudo '+pids)
