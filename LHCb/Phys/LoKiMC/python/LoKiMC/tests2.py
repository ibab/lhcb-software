#!/usr/bin/env python
# =============================================================================
# $Id: tests.py 124207 2011-06-02 16:05:29Z ibelyaev $
# =============================================================================
## @file   LoKiMC/tests2.py
#   
#        This file is a part of LoKi project - 
#    "C++ ToolKit  for Smart and Friendly Physics Analysis"
#
#  The package has been designed with the kind help from
#  Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas, 
#  contributions and advices from G.Raven, J.van Tilburg, 
#  A.Golutvin, P.Koppenburg have been used in the design.
#
#  @author Pieter David pieter.david@nikhef.nl
#  @date 2015-06-01
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
__author__  = "Pieter David pieter.david@nikhef.nl"
__date__    = "2007-05-29"
__version__ = "$Revision: 124207 $ "
# =============================================================================

from LoKiCore.basic import LoKi, std,cpp 
FALL  = LoKi.Constant ( "void" , bool ) ( True  ) 
_c1   = std.vector( 'const LHCb::MCParticle*' )  
MCRange = cpp.Gaudi.NamedRange_ ( _c1 )

# =============================================================================
if '__main__' == __name__ :
    
    print 80*'*'
    print __doc__
    print ' Author  : ' , __author__
    print ' Version : ' , __version__
    print ' Date    : ' , __date__    
    print 80*'*'
    for i in dir() : print i 

# =============================================================================

# =============================================================================
# The END 
# =============================================================================
