import os, socket
pvss_system = 'RECOTEST'
storage_system = 'RECSTORAGE'
reco_control = 'RECFARM'

gaudiScript = '/group/online/dataflow/scripts/runReco.sh'

Name = 'Farm'

numFarm = 6
numSubFarm = 6
_numNodePerSubFarm = 10
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
nodes = [sf+'01',sf+'02']
nodes= []
lines=os.popen("tmLs -u zzzzzzzzz").readlines()
for i in lines:
  l=i.replace(' ','').split('"')
  if l[0]=='Node:' and l[1].upper()!=dns: nodes.append(l[1].upper())

cpus  = [8 for j in xrange(len(nodes))]
subFarms = {sf: (nodes,cpus)}
numFarm = 16
numSubFarm = len(subFarms.keys())

print 'Farm setup "%s" of system: %s  / %s'%(Name,pvss_system,dns)
for i in xrange(len(nodes)):
  print 'Node: %-32s [%2d Cores] '%("'"+nodes[i]+"'",cpus[i])
