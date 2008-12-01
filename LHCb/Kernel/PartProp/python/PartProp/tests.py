#!/usr/bin/env python
# =============================================================================
## @file PartProp/tests.py
#  The set of basic tests for objects from Kernel/PartProp package 
#  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
#  @date   2008-12-01
# =============================================================================
"""
The set of basic decorators for objects from Kernel/PartProp package 
"""
# =============================================================================
__author__  = "Vanya BELYAEV Ivan.Belyaev@nikhef.nl" 
__version__ = "CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.1.1.1 $" 
# =============================================================================

from PartProp.decorators import *

pp1 = LHCb.ParticleProperty ( 'NAME1' , LHCb.ParticleID(121) ,
                              1. , 501.0 , 0.1 , 0.001 )
pp2 = LHCb.ParticleProperty ( 'NAME2' , LHCb.ParticleID(122) ,
                              -1 , 502.0 , 0.1 , 0.001 )
pp3 = LHCb.ParticleProperty ( 'NAME3' , LHCb.ParticleID(123) ,
                              0  , 503.0 , 0.1 , 0.001 )

pp1.setAntiParticle ( pp2 )
pp2.setAntiParticle ( pp1 )

lst = [ pp3 , pp2 , pp1 ]

v1 = LHCb.IParticlePropertySvc.ParticleProperties()

v1.fromLst ( lst )

for pp in v1 :
    print pp

print v1

lst.sort() 
v1.clear() 
v1.fromLst ( lst )
print v1


pid1 = LHCb.ParticleID(121)
pid2 = LHCb.ParticleID(122)
pid3 = LHCb.ParticleID(123)

lst2 = [ pid3 , pid2 , pid1 ]

v2 = LHCb.ParticleIDs()
v2.fromLst ( lst2 )
print v2 

lst2.sort() 

v2 = LHCb.ParticleIDs()
v2.fromLst ( lst2 )
print v2 

for pid in lst2 :
    print pid, pid.pid() 

# =============================================================================
# The END 
# =============================================================================

