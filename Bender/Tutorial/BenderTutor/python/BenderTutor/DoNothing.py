#!/usr/bin/env python
# -*- coding: utf-8 -*-
# =============================================================================
# $Id$
# =============================================================================
## @file
#  The most simple "do-nothing" Bender module 
#
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2014-05-16
#  
#                    $Revision$
#  Last modification $Date$
#  by                $Author$
# =============================================================================
"""
The most simple ``do-nothing'' Bender module 
"""
# =============================================================================
__version__ = "$Revision$"
__author__  = "Vanya BELYAEV Ivan.Belyaev@itep.ru"
__date__    = "2011-06-07"
__all__     = ()  ## nothing to be exported 
# =============================================================================


## 1) some user code :
def run ( nEvents ) :
    
    for i in range( 0 , min( nEvents , 10 ) ) : 
        
        print ' I run event %i ' % i
        
    return 0

## 2) configuration step 
def configure (  datafiles ,  catalogs = [] , castor = False , params = {} ) :   
    
    print 'I am configuration step!'

    return 0

## 3) steer the job
if '__main__' == __name__ : 
    
    print 'This runs only if modeule is used as the script! '
    
    run ( 10 ) 

# =============================================================================
# The END 
# =============================================================================
