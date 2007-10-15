pvss_system = 'RECOTEST'
Name = 'Reco'
numFarm = 3
numSubFarm = 3
_numNodePerSubFarm = 10
subFarms = {}

for i in xrange(numSubFarm):
  sf = '%s%03d'%('HLT',i)
  cpus  = [2 for j in xrange(_numNodePerSubFarm)]
  nodes = ['%s%02d'%(sf,j) for j in xrange(_numNodePerSubFarm)]
  subFarms[sf] = (nodes,cpus)

storageSenderNodes = ['STORESTRM01','STORESTRM02']
storageReceiverNodes = ['STORERECV01','STORERECV02']
