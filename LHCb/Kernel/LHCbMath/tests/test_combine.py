#!/usr/bin/env python
# -*- coding: utf-8 -*-
# =============================================================================
# $Id:$
# =============================================================================
## @file  test_combine.py
#  Test utilities for combining correlated measurements
#  @see P.Avery "Combining measurements with correlated errors", CBX 95 55
#  @see http://www.phys.ufl.edu/~avery/fitting/error_correl.ps.gz
#  @see http://www.researchgate.net.publication/2345482_Combining_Measurements_with_Correlated_Errors
#
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2015-09-28
#  
#                    $Revision: 193473 $
#  Last modification $Date: 2015-08-09 16:20:28 +0300 (Sun, 09 Aug 2015) $
#  by                $Author: ibelyaev $
# =============================================================================
""" Test utilities for combining correlated measurements
- see P.Avery ``Combining measurements with correlated errors'', CBX 95 55
- see http://www.phys.ufl.edu/~avery/fitting/error_correl.ps.gz
- see http://www.researchgate.net.publication/2345482_Combining_Measurements_with_Correlated_Errors
"""
__author__  = "Vanya BELYAEV Ivan.Belyaev@itep.ru"
__date__    = "2015-09-29"
__version__ = "$Revision: 195053 $"
__all__     = ()
# =============================================================================
import ROOT
from   LHCbMath.combine import Combine

## make printout of the own documentations
print '*'*120
print                      __doc__
print ' Author  : %s ' %   __author__
print ' Version : %s ' %   __version__
print ' Date    : %s ' %   __date__
print '*'*120


from LHCbMath.Types   import Gaudi
VE = Gaudi.Math.ValueWithError


print 'Example from Avery '

x = VE ( 0.95 , 0.08**2 )
y = VE ( 1.08 , 0.08**2 )

syst1 = Gaudi.Math.SymMatrix2x2()
syst1[0,0] = 0.08**2
syst1[1,1] = 0.08**2

syst2 = Gaudi.Math.SymMatrix2x2()
syst2[0,0] = 0.08**2
syst2[0,1] = 0.08**2
syst2[1,1] = 0.08**2

c1 = Combine( [x,y] , syst1 )
c2 = Combine( [x,y] , syst2 )

print 'CORRELATED  :' , c2.combiner.result(), c2.errComponents() 
print 'UNCORRELATED:' , c1.combiner.result(), c1.errComponents()

print '*'*120

print 'Lb mass average '

x = VE(5619.44 , 0.70**2 )
y = VE(5619.44 , 0.13**2 )

syst1 = Gaudi.Math.SymMatrix2x2()
syst1[0,0] = 0.30**2
syst1[1,1] = 0.45**2


syst2 = Gaudi.Math.SymMatrix2x2()
syst2[0,0] = 0.30**2
syst2[1,1] = 0.45**2
syst2[0,1] = 0.30*0.45

c1 = Combine( [x,y] , syst1 )
c2 = Combine( [x,y] , syst2 )

print 'CORRELATED  : ' , c2.combiner.result(), c2.errComponents() 
print 'UNCORRELATED: ' , c1.combiner.result(), c1.errComponents()

print '*'*120

print 'sigma_eff(Y+D)'

s_YD0_7    = VE ( 19.4 , 2.6**2 )
s_YDp_7    = VE ( 15.2 , 3.6**2 )

## c = 0.76 
## c = 0.60 ## correlation 

## syst7      = Gaudi.Math.SymMatrix2x2()
## syst7[0,0] =   1.3**2
## syst7[1,1] =   1.5**2
## syst7[0,1] = c*1.3*1.5

## s_YD0_8    = VE ( 17.2 , 1.9**2 )
## s_YDp_8    = VE ( 22.3 , 4.4**2 )

## syst8      = Gaudi.Math.SymMatrix2x2()
## syst8[0,0] =   1.2**2
## syst8[1,1] =   2.2**2
## syst8[0,1] = c*1.2*2.2

## systA      =  Gaudi.Math.SymMatrix4x4()
## systA[0,0] = 1.2**2
## systA[1,1] = 2.2**2
## systA[2,2] = 1.3**2
## systA[3,3] = 1.5**2

## systA[0,1] = c   *1.3*1.5
## systA[0,2] =      1.3*1.2
## systA[2,3] = c   *1.2*2.2
## systA[1,3] =      1.5*2.2


## c7 = Combine( [ s_YD0_7 , s_YDp_7 ] , syst7 )
## c8 = Combine( [ s_YD0_8 , s_YDp_8 ] , syst8 )
## cA = Combine( [ s_YD0_7 , s_YDp_7 ] + [ s_YD0_8 , s_YDp_8 ] , systA )

## print ' -   7TeV: ' , c7.result(), c7.errComponents() 
## print ' -   8TeV: ' , c8.result(), c8.errComponents() 
## print ' - 7&8TeV: ' , cA.result(), cA.errComponents() 

# =============================================================================
# The END 
# =============================================================================
