#!/usr/bin/env python
# =============================================================================
## @file LoKiCore/math.py
#  helper file for redefine the standard mathematical functions
#  using pseudo-operators
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
#  @date 2007-07-17
# =============================================================================
"""
The set of basic math for various ROOT objects

      This file is a part of LoKi project - 
``C++ ToolKit  for Smart and Friendly Physics Analysis''

The package has been designed with the kind help from
Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas, 
contributions and advices from G.Raven, J.van Tilburg, 
A.Golutvin, P.Koppenburg have been used in the design.

"""
# =============================================================================
__author__  = "Vanya BELYAEV ibelyaev@nikhef.nl"
__date__    = "2007-07-17"
__version__ = "CVS tag $Name: not supported by cvs2svn $, version $Revision$" 
# =============================================================================
__all__     = ()
# =============================================================================
import LHCbMath.Types 

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
# The end 
# =============================================================================
