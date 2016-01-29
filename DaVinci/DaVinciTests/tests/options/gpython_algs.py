#!/usr/bin/env python
# -*- coding: utf-8 -*- 
# =============================================================================
## @file gpython_algs
#  simmple test for two problems
#  - contrintuitive IAlgorithm::isInitialize  [But ok...]
#  - ALG-fuctors, created from python prompt/loop
#    [it is ok when they are created  via Hybrid framework]
#  @author Vanya BELYAEV
#  @date  2016-01-29
# =============================================================================

##
## Configurables: 
## 
from Configurables import DaVinci, MessageSvc 
dv = DaVinci(
    DataType  = '2015' ,
    InputType = 'DST'  ,
    Lumi      = True   
    )

##
## put some algorithms into "user" list
##
from StandardParticles      import StdLooseKaons 
from PhysSelPython.Wrappers import SelectionSequence
seq = SelectionSequence( 'KAONS' , StdLooseKaons )
dv.UserAlgorithms += [  seq.sequence() ] 

## produde a lot of prints 
## msg = MessageSvc ( OutputLevel = 2 )


## get some input data 
from Gaudi.Configuration import importOptions
importOptions('$STRIPPINGSELECTIONSROOT/tests/data/Reco15a_Run164668.py')


##
## GaudiPython 
##

from GaudiPython.Bindings import AppMgr

gaudi = AppMgr()


alg   = gaudi.algorithm('KAONS')

mainseq = 'DaVinciEventSeq'

from LoKiHlt.algorithms import ALG_EXECUTED, ALG_PASSED, ALG_ENABLED 
## start event loop 
for i in range(10) :

    gaudi.run(1)
    
    alg   = gaudi.algorithm('KAONS')
    print ' [%d] KAONS      initialized? %s [counterintuitive]' % ( i , alg._ialg.isInitialized () )
    print ' [%d] KAONS         executed? %s '                   % ( i , alg._ialg.isExecuted    () )


    print ' without fix in $LOKICORE/src/AlgFunctors.cpp following  lines produce segmentation violation'
    fun1 = ALG_EXECUTED ( mainseq )
    fun2 = ALG_PASSED   ( mainseq )
    fun3 = ALG_ENABLED  ( mainseq )
    
    print ' [%d] main sequence executed? %s ' % ( i , fun1() )
    print ' [%d] main sequence   passed? %s ' % ( i , fun2() )
    print ' [%d] main sequence  enabled? %s ' % ( i , fun3() )
    

exit()

    
# =============================================================================
# The END 
# =============================================================================
