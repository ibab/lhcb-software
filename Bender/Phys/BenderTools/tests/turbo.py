#!/usr/bin/env bender 
# -*- coding: utf-8 -*-
# =============================================================================
# $Id:$
# =============================================================================
## @file
#  very simple "bender" scripts, read TURBO stream  
#
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2016-02-09
#  
#                    $Revision: 200161 $
#  Last modification $Date: 2016-01-24 15:41:23 +0100 (Sun, 24 Jan 2016) $
#  by                $Author: ibelyaev $
# =============================================================================
__version__ = "$Revision: 200161 $"
__author__  = "Vanya BELYAEV Ivan.Belyaev@itep.ru"
__date__    = "2011-06-07"
# =============================================================================


for i in  range(10) :
    
    data, nevt = seekForData('/Event/Turbo/Hlt2CharmHadDpToKpPimPipTurbo/Particles') 
    if data :
        print data
    run(1)



    
