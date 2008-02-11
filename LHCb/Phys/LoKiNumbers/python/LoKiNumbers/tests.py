#!/usr/bin/env python
# =============================================================================
# $Id: tests.py,v 1.3 2008-02-11 10:03:58 ibelyaev Exp $
# =============================================================================
## @file
#  @author Vanya BELYAEV ibelyaev@physics.syr.edu
#  @date 2007-05-29
# =============================================================================
"""
The basic tests for LoKiNumbers package
"""
_author_ = "Vanya BELYAEV ibelyaev@physics.syr.edu" 
# =============================================================================

import GaudiPython.Pythonizations

from LoKiNumbers.decorators import *
from LoKiCore.functions     import *
from LoKiCore.math          import *


## prepare some input vector with data 
inp=XVector()
inp.push_back(1)
inp.push_back(4)
inp.push_back(3)

print 'inp                                    :%s' % inp
print 'inp >>   sin(X)                        :%s' % ( inp >>    sin(X)        )
print 'inp >> ( sin(X) < 0 )                  :%s' % ( inp >>  ( sin(X)  < 0 ) )
print 'inp >>   sin(X) >> abs(X)              :%s' % ( inp >>    sin(X) >> abs(X) )
print 'inp >>   sin(X) >> abs(X) >> min_value(X*X)   >> pow(X,20) :%s' % ( inp >>    sin(X) >> abs(X) >> min_value(X*X)   >> pow(X,2) )
print 'inp >>   sin(X) >> abs(X) >> min_element(X*X) >> pow(X,20) :%s' % ( inp >>    sin(X) >> abs(X) >> min_element(X*X) >> pow(X,2) )
print 'inp >> process( sin(X) ) >> pow2(X)    :%s' % ( inp >> process( sin(X) ) >> pow2(X)  )
print 'inp >> process( sin(X) ) >> pow(X,3)   :%s' % ( inp >> process( sin(X) ) >> pow(X,3) )

# =============================================================================
# The END 
# =============================================================================
