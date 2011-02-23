import os, socket, subprocess
pvss_system    = 'RECOTEST'
storage_system = 'RECSTORAGE'
reco_control   = 'RECFARM'

gaudiScript    = '/group/online/dataflow/scripts/runReco.sh'

Name = 'Farm'

numFarm = 6
numSubFarm = 6
_numNodePerSubFarm = 19
subFarms = {}

for i in xrange(numSubFarm):
  sf = '%s%03d'%('HLT',i)
  cpus  = [2 for j in xrange(_numNodePerSubFarm)]
  nodes = ['%s%02d'%(sf,j) for j in xrange(_numNodePerSubFarm)]
  subFarms[sf] = (nodes,cpus)

pvss_system = 'RECMON'
if os.environ.has_key('DIM_DNS_NODE'):
  dns = os.environ['DIM_DNS_NODE'].upper()
else:
  dns = socket.gethostname().upper()
  if dns.find('.')>0:
    dns = dns[:dns.find('.')]

print 'DIM DNS Node:',dns
pvss_system = 'REC'+dns.upper()
sf = dns.upper() # socket.gethostname().upper()

#nodes= []
#lines=os.popen("tmLs -u zzzzzzzzz").readlines()
#for i in lines:
#  l=i.replace(' ','').split('"')
#  if l[0]=='Node:' and l[1].upper()!=dns: nodes.append(l[1].upper())
#
#nodes = sorted(nodes)
#cpus  = [8 for j in xrange(len(nodes))]

def getFarmInfo(sf):
  p1=subprocess.Popen(["memViewer"], stdout=subprocess.PIPE)
  p2=subprocess.Popen(["grep", "-e MemTotal","-e NODE" ], stdin=p1.stdout, stdout=subprocess.PIPE)
  output=p2.communicate()[0].split('\n')
  nodes=[]
  cpus=[]
  if len(nodes)==0:
    n={}
    for i in range(0,len(output),2):
      node=output[i].replace('NODE','').replace(':','').replace(' ','')
      if not node: break
      if node.upper() == sf.upper(): continue
      mem=output[i+1].replace('KiB','').replace('MemTotal','').replace(' ','')
      n[node]=int(round(float(mem)/2./1024./1024.))
    for k in sorted(n):
      nodes.append(k.upper())
      cpus.append(n[k])
  return nodes,cpus

nodes,cpus=getFarmInfo(sf)

subFarms = {sf: (nodes,cpus)}
numFarm = 16
numSubFarm = len(subFarms.keys())

print 'Farm setup "%s" of system: %s  / %s'%(Name,pvss_system,dns)
for i in xrange(len(nodes)):
  print 'Node: %-32s [%2d Cores] '%("'"+nodes[i]+"'",cpus[i])
