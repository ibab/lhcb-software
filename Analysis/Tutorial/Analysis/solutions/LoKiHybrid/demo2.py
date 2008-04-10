#
# Stolen from I. Belyaev at http://indico.cern.ch/getFile.py/access?contribId=1&sessionId=7&resId=2&materialId=0&confId=25000
#
from Gaudi.Configuration import *

importOptions('demo2.opts')

import GaudiKernel.SystemOfUnits as Units 

from GaudiPython import AppMgr

gaudi=AppMgr()

from LoKiPhys.decorators import *
from LoKiCore.functions  import monitor

evtSvc = gaudi.evtSvc()

minPT = MINTREE( 'mu+' ==ABSID , PT ) / Units.GeV 
# minPT = monitor(MINTREE( 'mu+' ==ABSID , PT ) / Units.GeV )

for i in range(1,100) :    
    gaudi.run(1)
    psis = evtSvc['Phys/Psi2MuMu/Particles']
    
    for psi in psis :
        print ' Mass=%s, ptMin=%s' % ( M ( psi ) , minPT ( psi ) )   

