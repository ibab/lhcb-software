#!/usr/bin/env python
# =============================================================================
## @file   LoKiPhys/tests.py
#  The simple tets script for Phys/LoKiPhys package
#
#        This file is a part of LoKi project - 
#    "C++ ToolKit  for Smart and Friendly Physics Analysis"
#
#  The package has been designed with the kind help from
#  Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas, 
#  contributions and advices from G.Raven, J.van Tilburg, 
#  A.Golutvin, P.Koppenburg have been used in the design.
#
#  @author Vanya BELYAEV ibelyaev@physics.syr.edu
#  @date 2007-05-29
# =============================================================================
""" The basic tests for LoKiPhys package """
_author_ = "Vanya BELYAEV ibelyaev@physics.syr.edu" 
# =============================================================================

# temporary:
import GaudiPython.Pythonizations

import LoKiPhys.Phys
import LoKiNumbers.decorators 
from LoKiPhys.decorators import *
from LoKiCore.functions  import *
from LoKiCore.math       import *
# =============================================================================
## The most trivial test function
def test0() :
    """ The most trivial test function """
    from LoKiPhys.decorators import _decorated
    print 'LoKiPhysTest: decorated objects %s'%len(_decorated)
    pass

## the test function for various "functional streamers"
def test1() :
    """
    The test function for various 'functional streamers'
    """
    p = LHCb.Particle()
    p.setParticleID( LHCb.ParticleID(11) )
    
    print 'particle: %s/%s '%(p.name(),pname(p))
    print 'particle: (PX,PY,PZ)=(%s,%s,%s)'%(PX(p),PY(p),PZ(p))
    print 'particle: ( p>>ID , p>>cos(E) )=(%s,%s)'%(p>>ID,p>>cos(E))
    
    v = std.vector('const LHCb::Particle*')()
    
    v.push_back(p)
    p1 = LHCb.Particle()
    p1.setParticleID(LHCb.ParticleID(13))
    v.push_back(p1)
    p2 = LHCb.Particle()
    p2.setParticleID(LHCb.ParticleID(22))
    v.push_back( p2 )
    
    print ' v>>ID                           : %s'%(v>>ID)
    print ' v>>yields(ID)                   : %s'%(v>>yields(ID))
    print ' v>>process(ID)>>min_value(P)    : %s'%(v>>process(ID)>>min_value(P))
    print ' v>>(process(ID)>>min_value(P))  : %s'%(v>>(process(ID)>>min_value(P)))
    print '(v>>min_element(ID)).name()      : %s'%(v>>min_element(ID)).name()
    print ' v>>min_value(ID)                : %s'%(v>>min_value(ID))
    print ' v>>("e+"!=ABSID)>>SIZE          : %s'%(v>>('e+'!=ABSID)>>SIZE)
    print ' v>>select("mu+"!=ABSID)>>EPMTY  : %s'%(v>>select('mu+'!=ABSID)>>EMPTY)

# =============================================================================
## Perform all known tests
def testAll() :
    """
    Perform all known tests
    """
    test0()
    test1()


# =============================================================================
if '__main__' == __name__ : testAll ()
# =============================================================================
    
# =============================================================================
# The END 
# =============================================================================
