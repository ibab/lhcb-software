import os

try:
  os.mkdir('/tmp/overflow')
except:
  pass

for i in xrange(200):
  cmd = 'ln -s /home/frankm/data/Run_0134444_20121216-150116.hltf0122.MEP /tmp/overflow/Run_0134444_20121216-15%04d.hltf0122.MEP'%(i,)
  os.system(cmd)
  
