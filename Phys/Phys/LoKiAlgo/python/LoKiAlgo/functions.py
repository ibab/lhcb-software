#!/usr/bin/env python
# =============================================================================
# CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.4 $
# =============================================================================
# $Log: not supported by cvs2svn $
# Revision 1.2  2007/06/10 20:13:35  ibelyaev
#  prepare for v4r0
#
# Revision 1.1  2007/06/01 12:21:33  ibelyaev
#  prepare for v4r0
#
# =============================================================================
## @file decorators.py LoKiAlgo/decorators.py
#  The set of basic decorator for objects from LoKiAlgo library
#  The file is a part of LoKi and Bender projects
#  @author Vanya BELYAEV ibelyaev@physics.syr.edu
# =============================================================================
""" The set of basic decorators for obejcts from LoKiAlgo library """
_author_ = "Vanya BELYAEV ibelyaev@physics.syr.edu" 
# =============================================================================

import LoKiCore.decorators as _LoKiCore 
import LoKiPhys.decorators as _LoKiPhys 

LoKi  = _LoKiPhys.LoKi
LHCb  = _LoKiPhys.LHCb


# =============================================================================
if '__main__' == __name__ :
    for o in dir() : print o        
        
# =============================================================================
# The END 
# =============================================================================
