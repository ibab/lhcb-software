#!/usr/bin/env bender 
# -*- coding: utf-8 -*-
# =============================================================================
# $Id: PyRoUts.py 200161 2016-01-24 14:41:23Z ibelyaev $
# =============================================================================
## @file
#  very simple "bender" scripts 
#
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2016-01-27
#  
#                    $Revision: 200161 $
#  Last modification $Date: 2016-01-24 15:41:23 +0100 (Sun, 24 Jan 2016) $
#  by                $Author: ibelyaev $
# =============================================================================
__version__ = "$Revision: 200161 $"
__author__  = "Vanya BELYAEV Ivan.Belyaev@itep.ru"
__date__    = "2011-06-07"
# =============================================================================

## book n-tuple
from GaudiPython.TupleUtils import nTuple


t = nTuple('MyTuple','Tuple')

## event loop 
for i in range(20) :
    
    run(1)
    
    if  0 == i%10 : print ls('/Event')
    
    t.column_int   ( 'i'  , i       )
    t.column_float ( 'fi' , float(i)) 
    t.write() 


    
