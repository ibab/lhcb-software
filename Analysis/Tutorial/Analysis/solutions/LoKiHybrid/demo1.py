#
# Stolen from I. Belyaev at http://indico.cern.ch/getFile.py/access?contribId=1&sessionId=7&resId=0&materialId=0&confId=25000
#
from math import sqrt 

from LoKiPhys.decorators import *
from LoKiCore.functions        import monitor

p = LHCb.Particle()
p.setParticleID( LHCb.ParticleID(11) )

m = p.momentum()
m.SetPx (   1000 )
m.SetPy (  -1000 )
m.SetPz (  10000 )
m.SetE  ( sqrt( m.P2() + 5000*5000 ) )

p.setMomentum ( m )


fun = PX+PY
print PX(p), PY(p) , fun(p)

fun2 = PX>750
print fun2(p)

fun3 = monitor(fun2)
print fun3(p)

