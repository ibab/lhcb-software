#!/usr/bin/env python
# =============================================================================
## @file PartProp/Service.py
#  Useful decorator for LHcb::(I)ParticlePropertySvc
#  @see LHCb::IParticlePropertySvc
#  @see LHCb::ParticlePropertySvc
#  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
#  @date 2008-12-01
# =============================================================================
"""
Useful decorator for LHCb::(I)ParticlePropertySvc
"""
__author__  = "Vanya BELYAEV Ivan.Belyaev@nikhef.nl"
__version__ = "CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.1.1.1 $"
# =============================================================================
from GaudiPython.Bindings import iService, InterfaceCast 
from GaudiPython.Bindings import gbl as cpp 



class iParticlePropertySvc(iService) :
    """
    Class iParticlePropertySvc: 'python'-twin for C++ class
    
    LHCb::IParticlePropertySvc
    
    """

    def __init__( self , name , isvc ) :
        """
        Constructor from the name and the service 
        """
        iService.__init__( self , name, isvc )
        self.__dict__['_ipps'] = InterfaceCast(cpp.LHCb.IParticlePropertySvc)(isvc)
        
        
# =============================================================================
# The END 
# =============================================================================
