#!/usr/bin/env python
# =============================================================================
# $Id: Service.py,v 1.4 2008-12-03 17:35:54 ibelyaev Exp $ 
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
# =============================================================================
__author__  = "Vanya BELYAEV Ivan.Belyaev@nikhef.nl"
__version__ = "CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.4 $"
# =============================================================================
__all__ = ( 'iParticlePropertySvc', )
# =============================================================================
import GaudiPython.Bindings
import PartProp.decorators

cpp           = GaudiPython.Bindings.gbl
iService      = GaudiPython.Bindings.iService
Helper        = GaudiPython.Bindings.Helper
InterfaceCast = GaudiPython.Bindings.InterfaceCast
AppMgr        = GaudiPython.Bindings.AppMgr 
LHCb          = cpp.LHCb
# =============================================================================
## 
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
        self.__dict__['_ipps'] = InterfaceCast(LHCb.IParticlePropertySvc)(isvc)

    def retrieveInterface(self) :
       iService.retrieveInterface ( self )
       iParticlePropertySvc.__init__ ( self , self._name , self._isvc ) 

    ## Find the Particle Property by particle name or particle ID
    def find ( self , what ) :
        """
        Find the Particle Property by particle name or particle ID

        >>> svc = ...                                 # get the service
        >>> pp1 = svc.find ( 'pi0' )                  # find by name 
        >>> pp2 = svc.find ( LHCb.ParticleID( 511 ) ) # find by Particle ID
        
        """
        if not self._ipps : self.retrieveInterface() 
        return self._ipps.find ( what )  # return
    
    ## get the Particle Properties by particle name or particle ID
    def get ( self , cut , asList = False ) :
        """
        Simple 'get' method for LHCb::IParticlePropertySvc
        service to extract the properties which satisfy some criteria
        
        >>> svc = ...  # get service (LHCb::IParticlePropertySvc) or vector 
        >>> leptons   = svc.get ( lambda s : s.pid().isLepton() )  # get all leptons 
        >>> longlived = svc.get ( lambda s : s.ctau() > 0.001   )  # get longlived
        
        """
        if not self._ipps : self.retrieveInterface() 
        return self._ipps.get ( cut , asList )  # return

    ## begin-iterator 
    def begin ( self ) :
        """
        Get the begin-iterator for the sequence of particle proeprties

        >>> svc = ... # get the service
        >>> begin = svc.begin()
        
        """
        if not self._ipps : self.retrieveInterface() 
        return self._ipps.begin () # return

    
    ## end-iterator 
    def end   ( self ) :
        """
        Get the end-iterator for the sequence of particle proeprties

        >>> svc = ... # get the service
        >>> end = svc.end()
        
        """
        if not self._ipps : self.retrieveInterface() 
        return self._ipps.end   () # return

    ## get all 
    def all   ( self , asList = False ) :
        """
        Get all particle properties form the service
        
        >>> svc = ... # get the service
        >>> all = svc.all ()  # get all properties
        
        """
        if not self._ipps : self.retrieveInterface() 
        return self._ipps.all ( asList ) # return

    ## dump the particle property table
    def dump ( self ) :
        """
        Dump the particle property table 
        """
        self.Dump = True 

    ## CC-conjugation
    def cc   ( self , decay ) :
        """
        Get CC-conjugationfor decay -descriptor
                
        >>> svc = ...                       # get the service
        >>> cc = svc.cc ( 'B0 -> K- pi+' )  # get CC-conjugation
        
        """
        if not self._ipps : self.retrieveInterface() 
        return self._ipps.cc ( decay ) # return

    ## get the C++ service itself  
    def svc ( self ) :
        """
        Get C++ service

        >>> svc = ...  #
        >>> svccpp = svc.svc ()
        
        """
        if not self._ipps : self.retrieveInterface() 
        return self._ipps    # return
    
    ## get the C++ service itself  
    def service  ( self ) :
        """
        Get C++ service

        >>> svc = ...  #
        >>> svccpp = svc.service ()
        
        """
        return self.svc()

    ## validate the node/decay/tree
    def validate ( self , obj ) :
        """
        Validate the node/decay/tree

        >>> svc  = ...  # get the service
        >>> node = ...  # get the node
        >>> sc = svc.validate ( node )
        """
        return obj.validate ( self.svc () ) 
    
# useful types 
iParticlePropertySvc.ParticleIDs = LHCb.IParticlePropertySvc.ParticleIDs
iParticlePropertySvc.Decay       = LHCb.IParticlePropertySvc.Decay
iParticlePropertySvc.Decays      = LHCb.IParticlePropertySvc.Decays
iParticlePropertySvc.Item        = LHCb.IParticlePropertySvc.Item
iParticlePropertySvc.Items       = LHCb.IParticlePropertySvc.Items


# =============================================================================
## accessor to the service 
def _ppSvc_ ( self, name='LHCb::ParticlePropertySvc') :
    """
    Get particle property service form application manager
    
    >>> gaudi = ...
    >>> pps = gaudi.ppSvc()
    
    """
    svc = Helper.service( self._svcloc, name )
    return iParticlePropertySvc ( name , svc)


AppMgr.ppSvc  = _ppSvc_
AppMgr.ppsvc  = _ppSvc_



# =============================================================================
# The END 
# =============================================================================
