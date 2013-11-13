#!/usr/bin/env python
# =============================================================================
# $Id$
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
   
## prepare some input vector with some data 
inp = 5 >> FONE >> ( 10 * XRAND_(1) )  
print 'inp                                    :%s' % inp
print 'inp >>   sin(X)                        :%s' % ( inp >>    sin(X)        )
print 'inp >> ( sin(X) < 0 )                  :%s' % ( inp >>  ( sin(X)  < 0 ) )
print 'inp >>   sin(X) >> abs(X)              :%s' % ( inp >>    sin(X) >> abs(X) )
print 'inp >>   sin(X) >> abs(X) >> min_value(X*X)   >> pow(X,20) :%s' % ( inp >>    sin(X) >> abs(X) >> min_value(X*X)   >> pow(X,2) )
print 'inp >>   sin(X) >> abs(X) >> min_element(X*X) >> pow(X,20) :%s' % ( inp >>    sin(X) >> abs(X) >> min_element(X*X) >> pow(X,2) )
print 'inp >> process( sin(X) ) >> pow2(X)    :%s' % ( inp >> process( sin(X) ) >> pow2(X)  )
print 'inp >> process( sin(X) ) >> pow(X,3)   :%s' % ( inp >> process( sin(X) ) >> pow(X,3) )
print 'inp >> count ( X < 5 )                 :%s' % ( inp >> count ( X < 5 ) )
print 'inp >> has   ( X > 5 )                 :%s' % ( inp >> has   ( X > 5 ) )
print 'inp >> XEMPTY                          :%s' % ( inp >> XEMPTY  )
print 'inp >> XSIZE                           :%s' % ( inp >> XSIZE   )


print 'inp >>       in_range ( 1,X,5 )  :%s' % ( inp >>       in_range(1,X,5)  )
print 'inp >> count(in_range ( 1,X,5 )) :%s' % ( inp >> count(in_range(1,X,5)) )

## print 'inp >> XSCALE ( 0.5 )   :%s' % ( inp >> select ( XSCALE ( 0.5 ) ) )
print 'inp >> XSKIP  ( 3 )   :%s' % ( inp >> XSKIP ( 3 ) ) 
print 'inp >> XSKIP  ( 2 )   :%s' % ( inp >> XSKIP ( 2 ) ) 
print 'inp >> XSKIP  ( 1 )   :%s' % ( inp >> XSKIP ( 1 ) ) 

print 'inp >> scale ( XALL , SKIP  ( 2 ) ) :%s' % ( inp >> scale ( XALL , SKIP ( 2 ) ) )


print 'inp >>  equal_to  ( X , 1 )  :%s' % ( inp >>  equal_to ( X , 1 ) )
print 'inp >> ~equal_to  ( X , 1 )  :%s' % ( inp >> ~equal_to ( X , 1 ) )
print 'inp >>  equal_to  ( X , [ 1 , 2, 3 ] )  :%s' % ( inp >>  equal_to ( X , [ 1 , 2, 3] ) )
print 'inp >> ~equal_to  ( X , [ 1 , 2, 3 ] )  :%s' % ( inp >> ~equal_to ( X , [ 1 , 2, 3] ) )

print 'inp >>  ( X == ( 1. , 2  , 3 ) ) :%s' % ( inp >> ( X ==  ( 1. , 2  , 3 ) ) ) 
print 'inp >>  ( X != [ 1  , 2. , 3 ] ) :%s' % ( inp >> ( X !=  [ 1  , 2. , 3 ] ) )  

print 'inp >>  ( X == ( 1 , 2, 3 ) ) :%s' % ( inp >> ( X == ( 1 , 2, 3 ) ) )
print 'inp >>  ( X != [ 1 , 2, 3 ] ) :%s' % ( inp >> ( X != [ 1 , 2, 3 ] ) )  

print 'inp >>  in_list ( X ,   1 , 2, 3   ) ) :%s' % ( inp >>  in_list ( X ,   1 , 2, 3 ) )
print 'inp >> ~in_list ( X , [ 1 , 2, 3 ] ) ) :%s' % ( inp >> ~in_list ( X , [ 1 , 2, 3 ] ) )


inp2  = doubles ( -10 , 1 , -9 , 10 , 0 , 1 , 1 , 1 ,  -1  )
print 'inp2 >> ( X > -5 )                         :%s ' % ( inp2 >> ( X > -5 ) )
print 'inp2 >> ( X <  5 )                         :%s ' % ( inp2 >> ( X <  5 ) )
print 'inp2 >> union          ( X > -5 , X < 5 )  :%s ' % ( inp2 >> union           ( X > -5 , X < 5 ) )
print 'inp2 >> intersection   ( X > -5 , X < 5 )  :%s ' % ( inp2 >> intersection    ( X > -5 , X < 5 ) )
print 'inp2 >> difference     ( X > -5 , X < 5 )  :%s ' % ( inp2 >> difference      ( X > -5 , X < 5 ) )
print 'inp2 >> sym_difference ( X > -5 , X < 5 )  :%s ' % ( inp2 >> sym_difference  ( X > -5 , X < 5 ) )
print 'inp2 >> includes       ( X > -5 , X > 0 )  :%s ' % ( inp2 >> includes        ( X > -5 , X > 0 ) )

print 'inp2 >> sum ( X          )    :%s ' % ( inp2 >> sum ( X          ) )
print 'inp2 >> sqrt( sum ( X**2 ) )  :%s ' % ( inp2 >> sqrt ( sum ( X**2 ) ) )
print 'inp2 >> sum ( X , X <  0 )    :%s ' % ( inp2 >> sum ( X , X <  0 ) )
print 'inp2 >> sum ( X , X >= 0 )    :%s ' % ( inp2 >> sum ( X , X >= 0 ) )


print 'inp2 >>     ( X %% 3 )    :%s ' % ( inp2 >>     ( X % 3 ) )
print 'inp2 >> sum ( X %% 3 )    :%s ' % ( inp2 >> sum ( X % 3 ) )


print 'inp2 >>   round ( sin(X)*5 )          :%s ' % ( inp2 >>   round ( sin(X)*5 ) )
print 'inp2 >> ( jbit  ( sin(X)*5 , 1     )  :%s ' % ( inp2 >> ( jbit  ( sin(X)*5,1  ) ) )
print 'inp2 >> ( jbits ( sin(X)*5 , 0 , 1 )  :%s ' % ( inp2 >> ( jbits ( sin(X)*5,0,1) ) )

print 'inp2 >> fetch ( X , 0   , -100 ) :%s ' % ( inp2 >> fetch ( X , 0   , -100 ) )

print 'inp2 >> mean    ( X**3 , X >= 0 ) :%s ' % ( inp2 >> mean    ( X**3 , X >= 0 ) )
print 'inp2 >> meanErr ( X**3 , X >= 0 ) :%s ' % ( inp2 >> meanErr ( X**3 , X >= 0 ) )
print 'inp2 >> rms     ( X**3 , X >= 0 ) :%s ' % ( inp2 >> rms     ( X**3 , X >= 0 ) )
print 'inp2 >> eff     ( X>0 ) :%s ' % ( inp2 >> eff    ( X > 0 ) )
print 'inp2 >> effErr  ( X>0 ) :%s ' % ( inp2 >> effErr ( X > 0 ) )

inp3 = doubles ( 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24 )
print 'inp3 >> ( ( X %% 3 ) == 2 ) :%s ' % ( inp3 >> ( ( X % 3 ) == 2 ) )
print 'inp3 >> ( ( X %% 5 ) == 2 ) :%s ' % ( inp3 >> ( ( X % 5 ) == 2 ) )
print 'inp3 >> ( ( ( X %% 3 ) == 2 ) & ( ( X %% 5 ) == 2 ) ) :%s ' % ( inp3 >> ( ( ( X % 3 ) == 2 ) & ( ( X % 5 ) == 2 ) ) )
print 'inp3 >> ( ( ( X %% 3 ) == 2 ) | ( ( X %% 5 ) == 2 ) ) :%s ' % ( inp3 >> ( ( ( X % 3 ) == 2 ) | ( ( X % 5 ) == 2 ) ) )
print 'inp3 >> ( ( ( X %% 2 ) == 1 ) & ( ( X %% 4 ) < 2 ) & ( ( X %% 8 ) < 4 ) ) :%s ' % \
     ( inp3 >> ( ( ( X %  2 ) == 1 ) & ( ( X %  4 ) < 2 ) & ( ( X %  8 ) < 4 ) ) )
print 'inp3 >> ( ( ( X %% 2 ) == 1 ) | ( ( X %% 4 ) < 2 ) & ( ( X %% 8 ) < 4 ) ) :%s ' % \
     ( inp3 >> ( ( ( X %  2 ) == 1 ) | ( ( X %  4 ) < 2 ) & ( ( X %  8 ) < 4 ) ) )
print 'inp3 >> ( ( ( X %% 2 ) == 1 ) & ( ( X %% 4 ) < 2 ) | ( ( X %% 8 ) < 4 ) ) :%s ' % \
     ( inp3 >> ( ( ( X %  2 ) == 1 ) & ( ( X %  4 ) < 2 ) | ( ( X %  8 ) < 4 ) ) )
print 'inp3 >> ( ( ( X %% 2 ) == 1 ) | ( ( X %% 4 ) < 2 ) | ( ( X %% 8 ) < 4 ) ) :%s ' % \
     ( inp3 >> ( ( ( X %  2 ) == 1 ) | ( ( X %  4 ) < 2 ) | ( ( X %  8 ) < 4 ) ) )

# =============================================================================
# The END 
# =============================================================================
