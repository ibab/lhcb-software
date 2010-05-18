"""

python -c "import urllib;print urllib.urlopen('http://www.cern.ch/~frankm').read()"   


"""


import sys, time, platform
import PyCintex as Dictionary
lib_prefix = ''
if platform.system()=='Linux':
  lib_prefix = 'lib'

gbl  = Dictionary.makeNamespace('')
std  = gbl.std
Dictionary.loadDict(lib_prefix+'LHCbStatusDict')
LHCbStatus=gbl.LHCbStatus
srv=LHCbStatus.Server()
s = srv.info()
s.hlt.subfarm(0)='OT'
print s.hlt.subfarm(0)
