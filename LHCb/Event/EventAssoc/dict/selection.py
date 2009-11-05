#!/usr/bin/env python
# =============================================================================
## @file
#  Simple script to create proper selection files to buils relations
#  @author Vanya BELYAEV ibelyaev@physics.syr.edu
#  @date   2006-11-26
# =============================================================================
""" Simple script to create proper selection files to buils relations """
# =============================================================================
__author__ = 'Vanya BELYAEV ibelyaev@physics.syr.edu'

if "__main__" == __name__ :


    import Relations.Rels as Rels 

    # make XML and C++ selction files:
    import phys
    Rels.prepare ( phys.classes ,
                   phys.includes,
                   xmlname = "selPhys.xml",
                   cppname = "selPhys.h" ) 
    import mc
    Rels.prepare ( mc.classes ,
                   mc.includes,
                   xmlname = "selMC.xml",
                   cppname = "selMC.h" ) 
# =============================================================================
# The END 
# =============================================================================
