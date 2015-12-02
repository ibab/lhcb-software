import sys
import os
import re
import socket
CounterDebugSvcs = ["DskWriter"]
HistDebugSvcs =[]
vnode=False
def selectFiles(srcdir):
  sfiles = os.listdir(srcdir)
  return sfiles
if len(sys.argv) >= 3:
  destfarm = sys.argv[2]
  srcdir = sys.argv[1]
else:
  destfarm = sys.argv[1]
  srcdir = "/localdisk/hlt1"
sfiles = selectFiles(srcdir)
sfiles.sort()
nfiles = len(sfiles)
nnodes=28
fpernode = nfiles/nnodes
nrest = nfiles % nnodes
destdir="hlt_COPY"
nmap = dict()
nods = []
totf=0
for i in range(nnodes):
  k = i+1
  nod = "%s%02d"%(destfarm,k,)
  nods.append(nod)
  nmap[nod] = [sfiles[l] for l in range(i*fpernode,(i+1)*fpernode)]
#  print nod,len(nmap[nod]),nmap[nod]
fbase = nnodes*fpernode
for i in range(nrest):
  k = i+1
  nod = "%s%02d"%(destfarm,k,)
  nmap[nod].append(sfiles[fbase+i])
# print nod,len(nmap[nod]),nmap[nod]
for i in range(nnodes):
  k = i+1
  nod = "%s%02d"%(destfarm,k,)
  totf += len(nmap[nod])
print "Number of files in directory "+str(nfiles)+" number of files in map "+str(totf)
for i in range(nnodes):
  cfname = "/tmp/"+nods[i]+".sh"
  f = open(cfname,"w")
  f.write("#!/bin/bash\n")
  for j in range(len(nmap[nods[i]])):
    f.write("scp "+srcdir+"/"+nmap[nods[i]][j]+" online@"+nods[i]+
            ":/localdisk/"+destdir+"\n")
  f.close()
  cmd = "chmod 777 "+cfname
  os.system(cmd)
  cmd = cfname+" &"
  print "excuting "+cmd+"\n"
  os.system(cmd)
