#!/usr/bin/env python
# =============================================================================
# $Id: tests.py,v 1.5 2010-01-01 15:27:10 ibelyaev Exp $
# =============================================================================
## @file   LoKiTrigger/tests.py
#  The simple test script for Phys/LoKiTrigger package
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
"""
The basic tests for LoKiTrigger package
"""
# =============================================================================
__author__   = "Vanya BELYAEV  Ivan.Belyaev@nikhef.nl"
__version__  = "CVS tag $Name: not supported by cvs2svn $, verison $Revision: 1.5 $ "
# =============================================================================

## needed since there is no autoloading of Tracking dictionaries:
import sys,PyCintex

if sys.platform == 'win32' : PyCintex.loadDict (    "TrackEventDict" )  
else                       : PyCintex.loadDict ( "libTrackEventDict" )  

import GaudiPython.Pythonizations

import LoKiNumbers.decorators 
from LoKiTrigger.decorators import *
from LoKiCore.functions     import *
from LoKiCore.math          import *


if not hasattr ( std , 'stringstream' ) :
    LHCb.Track.__repr__ = lambda s : LoKi.Print.toString ( s ) 
    LHCb.State.__repr__ = lambda s : LoKi.Print.toString ( s ) 
    
    
# =============================================================================
## The most trivial test function
def test0() :
    """
    The most trivial test function
    """
    from LoKiTrigger.decorators import _decorated
    print 'LoKiTriggerTest: decorated objects %s'%len(_decorated)
    pass


## the test function for various "functional streamers"
def test1() :
    """
    The test function for various 'functional streamers'
    """

    s = LHCb.State()
    s.setState ( 0.0 , 0.0 , 0.0 , 0.01 , 0.01 , 0.2 )  
    t = LHCb.Track()
    t.addToStates ( s ) 
    
    print 'track: (TrP,TrPT)=(%s,%s)'%(TrP(t),TrPT(t))
    print 'track: ( t>>TrP , p>>cos(TrPT) )=(%s,%s)'%(t>>TrP,t>>cos(TrPT))
    
    v = std.vector('LHCb::Track*')()
    
    v.push_back(t)
    
    t1 = LHCb.Track()
    s1 = LHCb.State()
    s1.setState ( 0.0 , 0.0 , 0.0 , -0.01 , -0.01 , 0.1 )  
    t1.addToStates ( s1 ) 
    v.push_back ( t1 )
    
    t2 = LHCb.Track()
    s2 = LHCb.State()
    s2.setState ( 0.0 , 0.0 , 0.0 , +0.01 , -0.01 , 0.05 )  
    t2.addToStates ( s2 ) 
    v.push_back ( t2 )

    t3 = LHCb.Track()
    s3 = LHCb.State()
    s3.setState ( 0.0 , 0.0 , 0.0 , -0.01 , +0.01 , 0.01 )  
    t3.addToStates ( s3 ) 
    v.push_back ( t3 )

    t4 = LHCb.Track()
    s4 = LHCb.State()
    s4.setState ( 0.0 , 0.0 , 0.0 , 0.01 , 0.01 , 0.005 )  
    t4.addToStates ( s4 ) 
    v.push_back ( t4 )

    t5 = LHCb.Track()
    s5 = LHCb.State()
    s5.setState ( 0.0 , 0.0 , 0.0 , 0.01 , 0.01 , 0.001 )  
    t5.addToStates ( s5 ) 
    v.push_back ( t5 )

    print ' v>>TrP                            : %s'%(v>>TrP)
    print ' v>>yields(TrP)                    : %s'%(v>>yields(TrP))
    print ' v>>process(TrP)>>min_value(TrP)   : %s'%(v>>process(TrP)>>min_value(TrP))
    print ' v>>(process(TrP)>>min_value(TrP)) : %s'%(v>>(process(TrP)>>min_value(TrP)))
    print ' v>>min_value(TrPT)                : %s'%(v>>min_value(TrPT))
    print ' v>>(select(TrP>10)>>TrSIZE)       : %s'%(v>>(select(TrP>10)>>TrSIZE))  
    print ' v>>select(TrP>10)>>TrSIZE         : %s'%(v>>select(TrP>10)>>TrSIZE)  
    print ' v>>(select(TrP>10)>>TrEMPTY)      : %s'%(v>>(select(TrP>10)>>TrEMPTY))  
    print ' v>>select(TrP>10)>>TrEPMTY        : %s'%(v>>select(TrP>10)>>TrEMPTY)  

    print ' v>>min_element(TrP)               : %s'%(v>>min_element(TrP))
    print ' v>>max_element(TrP)               : %s'%(v>>max_element(TrP))

def test2() :
    """
    Test #3 
    """

    track = LHCb.Track()

    f1  = TrIDC     (   'isVelo'   )
    
    f2  = TrOTIDC   (   'module',1 )
    f3  = TrOTIDC   ( 1,'module',5 )
    f4  = TrOTIDC   ( 'module', uints( 1, 2, 3, 4, 5 ) )
    
    f5  = TrSTIDC   (   'sector',1 )
    f6  = TrSTIDC   ( 1,'sector',5 )
    f7  = TrSTIDC   ( 'sector', uints( 1, 2, 3, 4, 5 ) )

    f8  = TrVELOIDC (   'sensor',1 )
    f9  = TrVELOIDC ( 1,'sensor',5 )
    f10 = TrVELOIDC ( 'sensor', uints( 1, 2, 3, 4, 5 ) )
    f11 = TrVELOIDC ( 'pileUp' ) 

    for f in ( f1 , f2, f3, f4, f5, f6, f7, f8, f9, f10, f11 ) :
        print f , f(track)
    
    
# =============================================================================
## Perform all known tests
def testAll() :
    """
    Perform all known tests
    """
    test0()
    test1()
    test2()


# =============================================================================
if '__main__' == __name__ : testAll ()
# =============================================================================
    
# =============================================================================
# The END 
# =============================================================================
