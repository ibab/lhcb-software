#!/usr/bin/env bender 
# -*- coding: utf-8 -*-
# =============================================================================
# $Id$
# =============================================================================
## @file
#  very simple "bender" script for batch processing 
#
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2016-01-27
#  
#                    $Revision$
#  Last modification $Date$
#  by                $Author$
# =============================================================================
__version__ = "$Revision$"
__author__  = "Vanya BELYAEV Ivan.Belyaev@itep.ru"
__date__    = "2011-06-07"
# =============================================================================


t = nTuple('MyTuple','Tuple')
h = book  ('path/to/my/histo', ## path in Histogram Transient Store
           'this it title '  , ## histogram title
           100               , ## number of bins
           0                 , ## low edge
           100               ) ## high edge

print 'TUPLE TYPE:', type(t)
## event loop 
for i in range(20) :
    
    run(1)
    
    if  0 == i%10 : print ls('/Event')
    
    t.column_int   ( 'i'  , i       )
    t.column_float ( 'fi' , float(i)) 
    t.write()

    h.fill ( i , i ) 


print h.dump(100,40) 


    
