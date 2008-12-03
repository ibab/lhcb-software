#!/usr/bin/env python
# =============================================================================
# $Id: decorators.py,v 1.3 2008-12-03 17:35:54 ibelyaev Exp $ 
# =============================================================================
## @file PartProp/decorators.py
#  The set of basic decorator for objects from Kernel/PartProp package 
#  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
#  @date   2008-12-01
# =============================================================================
"""
The set of basic decorators for objects from Kernel/PartProp package 
"""
# =============================================================================
__author__  = "Vanya BELYAEV Ivan.Belyaev@nikhef.nl" 
__version__ = "CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.3 $" 
# =============================================================================

import PyCintex

# construct the global namespace 
_global   = PyCintex.makeNamespace('')
# namespaces
cpp    = _global 
std    = _global.std
LHCb   = _global.LHCb
Decays = _global.Decays


## get all particl eproeprties form the service 
def _get_all_ ( self , asList = False ) :
    """
    Get all particle properties form the service

    >>> svc = ... # get the service
    >>> all = svc.all ()  # get all properties
    
    """
    _all = LHCb.ParticleProperties.allProperties ( self )
    if not asList  : return _all
    return _all.toList () 

## decorate service
LHCb.IParticlePropertySvc.all = _get_all_


## simple "get" method for the service 
def _get_pp_ ( self , cut , asList = False ) :
    """
    Simple 'get' method for LHCb::IParticlePropertySvc
    service to extract the properties which satisfy some criteria
    
    >>> svc = ...  # get service (LHCb::IParticlePropertySvc) or vector 
    >>> leptons   = svc.get ( lambda s : s.pid().isLepton() )  # get all leptons 
    >>> longlived = svc.get ( lambda s : s.ctau() > 0.001   )  # get longlived

    """
    _all = self.all()
    return _all.get ( cut , asList )


## simple "get" method for the service 
def _get_ppv_ ( self , cut , asList = False ) :
    """
    Simple 'get' method for LHCb::IParticlePropertySvc
    service to extract the properties which satisfy some criteria
    
    >>> svc = ...  # get service (LHCb::IParticlePropertySvc) or vector 
    >>> leptons   = svc.get ( lambda s : s.pid().isLepton() )  # get all leptons 
    >>> longlived = svc.get ( lambda s : s.ctau() > 0.001   )  # get longlived

    """
    result = []
    for pp in self :
        if cut ( pp ) : result.append( pp )
    if asList : return result
    vct = LHCb.IParticlePropertySvc.ParticleProperties()
    vct.fromList ( result )
    return vct 


## decorate service
LHCb.IParticlePropertySvc.get = _get_pp_
## decorate the vector 
LHCb.IParticlePropertySvc.ParticleProperties.get = _get_ppv_

# =============================================================================
## Convert LHCb::IParticlePropertySvc::ParticleProperties into python list
def _ppv_2_list_ ( self ) :
    """
    Convert LHCb::IParticlePropertySvc::ParticleProperties into python list

    >>> ppv = ...   # get the vector
    >>> lst = ppv.toList () # convert it to the list 

    """
    result = []
    size   = self.size()
    index  = 0
    while index < size :
        pp = self.at(index)
        result.append ( pp )
        index += 1 
    return result

# ==============================================================================
## convert python list into LHCb.IParticlePropertisvc.ParticleProperties 
def _ppv_from_lst_ ( self , lst ) :
    """
    Append the iterable sequence 'lst' to the vector of
    particle properties:

    >>> Vct = std.vector('const LHCb::ParticleProperty*') 
    >>> lst = [ pp1 , pp2 , pp3 ]
    >>> vct = Vct()
    >>> vct.fromList ( lst )
    
    """
    for item in lst :
        self.push_back ( item )

# =============================================================================
## Print vector of particle properties in a form of table
def _prnt_as_table_ ( self , *args ) :
    """
    Print vector of particle properties in a form of table

    >>> print vct.asTable()

    """
    return LHCb.ParticleProperties.printAsTable ( self , *args )



## decorate the vector of properties 
LHCb.IParticlePropertySvc.ParticleProperties.toList   = _ppv_2_list_   
LHCb.IParticlePropertySvc.ParticleProperties.toLst    = _ppv_2_list_   
## decorate the vector of properties 
LHCb.IParticlePropertySvc.ParticleProperties.fromList = _ppv_from_lst_   
LHCb.IParticlePropertySvc.ParticleProperties.fromLst  = _ppv_from_lst_   
## decorate the vector of properties 
LHCb.IParticlePropertySvc.ParticleProperties.__repr__ = lambda s : s.toList().__repr__()
LHCb.IParticlePropertySvc.ParticleProperties.__str__  = _prnt_as_table_


## decorate the printout for LHCb::ParticleProperty
LHCb.ParticleProperty .__str__  = LHCb.ParticleProperty.toString
LHCb.ParticleProperty .__repr__ = LHCb.ParticleProperty.toString


## decorate the printout for LHCb::ParticleID 
LHCb.ParticleID       .__str__  = LHCb.ParticleID.toString
LHCb.ParticleID       .__repr__ = LHCb.ParticleID.toString

## defibne the type for std::vector<LHCb::ParticleID>
LHCb.ParticleIDs = std.vector( LHCb.ParticleID )

## get particleID objects whcih satisfy some criteria 
def _get_pid_ ( self , cut ) :
    """
    Get particleID objects which satisfy some criteria

    >>> pids   = ...  # vector of LCHb::ParticleID objects
    >>> good   = pids.get( lambda s :   s.isLepton() )  # get leptons
    >>> scalar = pids.get( lambda s : 1 == s.jSpin() )  # get scalars 
    
    """
    size   = self.size() 
    index  = 0
    result = []
    while index < size :
        pid = self.at(index) 
        if cut ( pid ) : result.append( pid ) 
        index += 1
    return result 
        
## decorate the vector of properties 
LHCb.ParticleIDs.toList   = _ppv_2_list_   
LHCb.ParticleIDs.toLst    = _ppv_2_list_   
## decorate the vector of properties 
LHCb.ParticleIDs.fromList = _ppv_from_lst_   
LHCb.ParticleIDs.fromLst  = _ppv_from_lst_   
## decorate the vector of properties 
LHCb.ParticleIDs.__repr__ = lambda s : s.toList().__repr__()
LHCb.ParticleIDs.__str__  = lambda s : s.toList().__str__()
## decorate the vector of properties
LHCb.ParticleIDs.get      = _get_pid_


Decays.Decay.Item . __str__  = lambda s : s.name()
Decays.Decay.Item . __repr__ = lambda s : s.name()

Decays.Decay. __str__  = lambda s : s.toString ()
Decays.Decay. __repr__ = lambda s : s.toString()

_items  = std.vector( Decays.Decay.Item )
_items.toList    = lambda s : [ i for i in s ]
_items.toLst     = lambda s : [ i for i in s ]
_items.__str__   = lambda s : s.toList().__str__ ()
_items.__repr__  = lambda s : s.toList().__repr__()
_decays = std.vector( Decays.Decay )
_decays.toList   = lambda s : [ i for i in s ]
_decays.toLst    = lambda s : [ i for i in s ]
_decays.__str__  = lambda s : s.toList().__str__ () 
_decays.__repr__ = lambda s : s.toList().__repr__()


LHCb.IParticlePropertySvc.ParticleIDs = LHCb.ParticleIDs
LHCb.IParticlePropertySvc.Decays      = _decays
LHCb.IParticlePropertySvc.Items       = _items 
LHCb.IParticlePropertySvc.Decay       = Decays.Decay
LHCb.IParticlePropertySvc.Item        = Decays.Decay.Item 

def _validate_ ( self , svc ) :
    """
    Validate the vector of items/decays

    >>> vct = ...            # get the vector of items/decays
    >>> svc = ...            # get the service
    >>> vcs.vaildate ( svc ) # validate
    """
    for o in self :
        sc = o.validate ( svc )
        if sc.isFailure() : return sc
    return cpp.StatusCode( cpp.StatusCode.SUCCESS ) 

_decays.validate = _validate_
_items .validate = _validate_


from PartProp.Nodes import *

# =============================================================================
# The END 
# =============================================================================

