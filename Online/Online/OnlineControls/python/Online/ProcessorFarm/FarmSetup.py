pvss_system = 'RECOTEST'
Name = 'Calibration'
Name = 'Farm'
Name = 'Trigger'
numFarm = 6
numSubFarm = 6
_numNodePerSubFarm = 10
subFarms = {}

for i in xrange(numSubFarm):
  sf = '%s%03d'%('HLT',i)
  cpus  = [2 for j in xrange(_numNodePerSubFarm)]
  nodes = ['%s%02d'%(sf,j) for j in xrange(_numNodePerSubFarm)]
  subFarms[sf] = (nodes,cpus)
