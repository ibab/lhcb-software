#!/usr/bin/env python 
# =============================================================================
# $Id: tests.py,v 1.9 2010-05-30 17:09:55 ibelyaev Exp $
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
__author__  = "Vanya BELYAEV ibelyaev@physics.syr.edu" 
__date__    = "2007-05-29"
__version__ = "CVS Tag: $Name: not supported by cvs2svn $, version $Revision: 1.9 $ "
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

    gpv = std.vector('const HepMC::GenParticle*')
    v = gpv()
    
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
    print ' v>>("e+"!=GABSID)>>GSIZE         : %s'%(v>>('e+'!=GABSID)>>GSIZE)
    print ' v>>select("mu+"!=GABSID)>>GEMPTY : %s'%(v>>select('mu+'!=GABSID)>>GEMPTY)
    
    f1 = in_list ( GID    , [ "e+" , "e-" , 'mu+' , 'mu-'] ) 
    f2 = in_list ( GABSID , [ "e+" , "e-" , 'mu+' , 'mu-'] ) 
    print ' in_list ( GID    , [ "e+" , "e-" , "mu+" , "mu-"] ) : %s %s ' % ( f1 , f1(p1) ) 
    print ' in_list ( GABSID , [ "e+" , "e-" , "mu+" , "mu-"] ) : %s %s ' % ( f2 , f2(p1) ) 


# =============================================================================
def testAll() :
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

     testAll() 
        

# =============================================================================
# The END 
# =============================================================================
