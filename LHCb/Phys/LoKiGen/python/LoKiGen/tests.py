#!/usr/bin/env python 
# =============================================================================
# $Id: tests.py,v 1.5 2008-02-11 10:05:34 ibelyaev Exp $
# =============================================================================
## @file   LoKiGen/tests.py
#
#  This file is a part of LoKi project - 
#   'C++ ToolKit  for Smart and Friendly Physics Analysis'
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
The basic tests for LoKiGen package

This file is a part of LoKi project - 
'C++ ToolKit  for Smart and Friendly Physics Analysis'

The package has been designed with the kind help from
Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas, 
contributions and advices from G.Raven, J.van Tilburg, 
A.Golutvin, P.Koppenburg have been used in the design.
"""
# =============================================================================
__author__ = "Vanya BELYAEV ibelyaev@physics.syr.edu" 
# =============================================================================

# temporary
import GaudiPython.Pythonizations

import LoKiNumbers.decorators
import LoKiGen.HepMC

from   LoKiGen.decorators     import *
from   LoKiCore.functions     import *
from   LoKiCore.math          import *


def test1() :
    
    p = HepMC.GenParticle()
    p.set_pdg_id(11)
    
    print 'particle: %s/%s '%(p.name(),pname(p))
    print 'particle: (GPX,GPY,GPZ,GM)=(%s,%s,%s,%s)'%(GPX(p),GPY(p),GPZ(p),GM(p))
    print 'particle: ( p>>GID , p>>cos(GE) )=(%s,%s)'    %(p>>GID,p>>cos(GE))
    
    v = std.vector('const HepMC::GenParticle*')()
    
    v.push_back(p)
    p1 = HepMC.GenParticle()
    p1.set_pdg_id(13)
    v.push_back(p1)
    p2 = HepMC.GenParticle()
    p2.set_pdg_id(22)
    v.push_back( p2 )
    
    print ' v>>GID                           : %s'%(v>>GID)
    print ' v>>yields(GID)                   : %s'%(v>>yields(GID))
    print ' v>>process(GID)>>min_value(GP)   : %s'%(v>>process(GID)>>min_value(GP))
    print '(v>>min_element(GID)).name()      : %s'%(v>>min_element(GID)).name()
    print ' v>>min_value(GID)                : %s'%(v>>min_value(GID))
    print '(v>>("e+"!=GABSID)).size()        : %s'%(v>>('e+'!=GABSID)).size()
    print '(v>>select("mu+"!=GABSID)).size() : %s'%(v>>select('mu+'!=GABSID)).size()

# =============================================================================
def testAll() :
    test1()
    
# =============================================================================
if '__main__' == __name__ :
    testAll() 
        

# =============================================================================
# The END 
# =============================================================================
