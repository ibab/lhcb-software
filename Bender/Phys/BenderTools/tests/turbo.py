#!/usr/bin/env bender
# -*- coding: utf-8 -*-
# =============================================================================
# $Id$
# =============================================================================
## @file
#  very simple "bender" script: read TURBO stream  
#
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2016-02-09
#  
#                    $Revision$
#  Last modification $Date$
#  by                $Author$
# =============================================================================
__version__ = "$Revision$"
__author__  = "Vanya BELYAEV Ivan.Belyaev@itep.ru"
__date__    = "2011-06-07"
# =============================================================================

for i in  range(10) :
    
    data, nevt = seekForData('/Event/Turbo/Hlt2CharmHadDpToKpPimPipTurbo/Particles') 
    if data :
        print data
    run(1)



    
