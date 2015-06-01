#!/usr/bin/env python
# =============================================================================
# $Id$
# =============================================================================
## @file   LoKiMC/tests.py
#   
#        This file is a part of LoKi project - 
#    "C++ ToolKit  for Smart and Friendly Physics Analysis"
#
#  The package has been designed with the kind help from
#  Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas, 
#  contributions and advices from G.Raven, J.van Tilburg, 
#  A.Golutvin, P.Koppenburg have been used in the design.
#
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date 2007-05-29
# =============================================================================
"""
The basic tests for LoKiMC package

      This file is a part of LoKi project - 
'C++ ToolKit  for Smart and Friendly Physics Analysis'

The package has been designed with the kind help from
Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas, 
contributions and advices from G.Raven, J.van Tilburg, 
A.Golutvin, P.Koppenburg have been used in the design.
"""
# =============================================================================
__author__  = "Vanya BELYAEV Ivan/Belyaev@itep.ru" 
__date__    = "2007-05-29"
__version__ = "$Revision$ "
# =============================================================================

## import GaudiPython.Pythonizations

import LoKiMC.MC
import LoKiNumbers.decorators 
from   LoKiMC.decorators      import *
from   LoKiCore.functions     import *
from   LoKiCore.math          import *


## The most trivial test function
def test0() :
    """
    The most trivial test function
    """
    from LoKiMC.decorators import _decorated
    print 'LoKiMCTest: decorated objects %s'%len(_decorated)


## the test function for various "functional streamers"
def test1() :
    """
    The test function for various 'functional streamers'
    """
    p = LHCb.MCParticle()
    p.setParticleID( LHCb.ParticleID(11) )
    
    print 'particle: %s/%s '%(p.name(),pname(p))
    print 'particle: (MCPX,MCPY,MCPZ)=(%s,%s,%s)'%(MCPX(p),MCPY(p),MCPZ(p))
    print 'particle: ( p>>MCID , p>>cos(MCE) )=(%s,%s)'%(p>>MCID,p>>cos(MCE))
    
    v = std.vector('const LHCb::MCParticle*')()
    
    v.push_back(p)
    p1 = LHCb.MCParticle()
    p1.setParticleID(LHCb.ParticleID(13))
    v.push_back(p1)
    p2 = LHCb.MCParticle()
    p2.setParticleID(LHCb.ParticleID(22))
    v.push_back( p2 )
    
    print ' v>>MCID                           : %s'%(v>>MCID)
    print ' v>>yields(MCID)                   : %s'%(v>>yields(MCID))
    print ' v>>process(MCID)>>min_value(MCP)  : %s'%(v>>process(MCID)>>min_value(MCP))
    print ' v>>(process(MCID)>>min_value(MCP)): %s'%(v>>(process(MCID)>>min_value(MCP)))
    print '(v>>min_element(MCID))[0].name()   : %s'%(v>>min_element(MCID))[0].name()
    print ' v>>min_value(MCID)                : %s'%(v>>min_value(MCID))
    print 'v>>("e+"!=MCABSID)>>MCSIZE         : %s'%(v>>('e+'!=MCABSID)>>MCSIZE)
    print 'v>>("e+"!=MCABSID)>>MCSIZE         : %s'%(v>>('e+'!=MCABSID)>>MCSIZE)
    print 'v>>select("mu+"!=MCABSID)>>MCEMPTY : %s'%(v>>select('mu+'!=MCABSID)>>~MCEMPTY)
    
    f1 = in_list ( MCID    , [ "e+" , "e-" , 'mu+' , 'mu-'] ) 
    f2 = in_list ( MCABSID , [ "e+" , "e-" , 'mu+' , 'mu-'] ) 
    print ' in_list ( MCID    , [ "e+" , "e-" , "mu+" , "mu-"] ) : %s %s ' % ( f1 , f1(p1) ) 
    print ' in_list ( MCABSID , [ "e+" , "e-" , "mu+" , "mu-"] ) : %s %s ' % ( f2 , f2(p1) ) 
    
# =============================================================================
## Perform all known tests
def testAll() :
    """
    Perform all known tests
    """
    test0()
    test1()
    
    
# =============================================================================
if '__main__' == __name__ :
    
    print 80*'*'
    print __doc__
    print ' Author  : ' , __author__
    print ' Version : ' , __version__
    print ' Date    : ' , __date__    
    print 80*'*'
    for i in dir() : print i 

    testAll ()
# =============================================================================

# =============================================================================
# The END 
# =============================================================================
