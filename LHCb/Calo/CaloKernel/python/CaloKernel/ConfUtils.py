#!/usr/bin/env python
# =============================================================================
# $Id: ConfUtils.py,v 1.2 2009-08-10 13:39:59 ibelyaev Exp $
# =============================================================================
## Useful utulities for building Calo-configurables 
#  @author Vanya BELYAEV Ivan.Belyaev@nikhe.nl
#  @date 2008-08-04
# =============================================================================
"""
Trivial module with few helper utilities to build  Calo-configurables
"""
# =============================================================================
__author__  = "Vanya BELYAEV Ivan.Belyaev@nikhef.nl"
__version__ = "CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.2 $"
# =============================================================================
__all__ = (
    'hltContext'     ,              ##    trivial function to check HLT context
    'setTheProperty' ,              ##           recursively set the properties 
    'addAlgs'        ,              ##         add algorithm(s) to the sequence
    'prntCmp'        ,              ##  print the structure of the configurable 
    'onDemand'       ,              ##                 declare on-demand action 
    'caloOnDemand'   ,              ##       list of declared on-demand actions
    'printOnDemand'  ,              ##  print current 'on-demand' configuration
    'getAlgo'                       ##        create the algorithm/configurable 
    )
# =============================================================================
from Gaudi.Configuration import *
from Configurables       import GaudiSequencer 
from Configurables       import DataOnDemandSvc 

# =============================================================================
## check the existence of certain property:
def _hasProp ( obj , prop ) :
    """
    Check the existence of certain property
    
    """
    if hasattr ( obj , prop ) : return True
    if hasattr ( obj , 'getDefaultProperties' ) :
        _def = obj.getDefaultProperties()
        return _def.has_key( prop ) 
    return False
    
# =============================================================================
## check HLT context 
def hltContext ( context , hlt = 'HLT') :
    """
    Check Hlt context:

    >>> context 
    >>> print hltContext ( context )

    """
    return hlt.upper() == context.upper()

    
# =============================================================================
## Helper utility to print (recursively) the structure of sequences
def prntCmp ( cmp , level = 0 ) :
    """
    Helper utility to print (recursively) the structure of sequences

    >>> obj = ..                   ## get the object/configurable 
    >>> print prntCmp ( obj ) 
    
    """
    
    if list == type ( cmp ) :
        result = ''
        for l in cmp : result += prntCmp ( l , level + 1 )
        return result
    
    result = '\n%-2d '%level  + '    ' * level

    if   GaudiSequencer == type ( cmp )  : result += cmp.name    ()
    elif hasattr ( cmp , 'getFullName' ) : result += cmp.getFullName()
    elif hasattr ( cmp , 'name'        ) : result += cmp.name    ()
    elif hasattr ( cmp , 'Name'        ) : result += cmp.Name    ()
    elif hasattr ( cmp , 'getType'     ) : result += cmp.getType ()
    else :                                 result += str ( type ( cmp ) )
    
    if hasattr ( cmp , 'Members' ) :
        for m in cmp.Members : result += prntCmp ( m , level + 1 )
        
    return result


# =============================================================================
## Set the (recursively) the property across the dependence tree
def setTheProperty ( obj , name , value ) :
    """
    Set the (recursively) the property across the dependence tree


    >>> obj = ..           ## get the object
    >>> setTheProperty ( obj , 'Context' , 'HLT' )

    
    """
    ## plain list? propagate to all members:
    if  list == type(obj) :
        for o in obj : setTheProperty ( o , name , value )
        return
    
    if    hasattr ( obj , name ) : setattr ( obj , name , value )
    elif _hasProp ( obj , name ) : setattr ( obj , name , value )
    
    ## propagate to the members
    if  hasattr ( obj , 'Members'  ) :
        for member in obj.Members    : setTheProperty ( member , name , value )
        
    ## propagate to the tools
    if  hasattr ( obj , 'getTools' ) :
        for tool   in obj.getTools() : setTheProperty ( tool   , name , value )


# =============================================================================    
## add the algorithm (or list) into the sequence
def addAlgs  ( seq , algs ) :
    """
    add the algorithm (or list) into the sequence

    >>> seq = ... # get the sequence

    >>> alg = ...  # get the algorithm
    >>> addAlg ( seq, alg )

    >>> addAlg ( seq , [ alg1 , alg2 , alg3 ] )
    
    """
    if list == type(algs) :
        for alg in algs : addAlgs ( seq , alg )
        return
    if list == type(seq)   :
        seq.append ( algs )
        return
    seq.Members.append ( algs )

# =============================================================================
## local variable to store on-demand actions 
__caloOnDemand = {}

# =============================================================================
##  Configure Data-On-Demand service
def onDemand ( location , alg ) :
    """
    Configure Data-On-Demand service

    >>> alg = ...       # get the algorithm
    >>> tes = ...       # get TES-location

    >>>  onDemand ( tes , alg )
    
    """
    if not location : return
    dod = DataOnDemandSvc() 
    dod.AlgMap     .update ( { location : alg.getFullName () } ) 
    __caloOnDemand .update ( { location : alg.getFullName () } ) 

# =============================================================================
def caloOnDemand () :
    """
    Get known configured on-demand actions:

    >>> print caloOnDemand()
    
    """
    return __caloOnDemand

    
# =============================================================================
## Get the algorithm by type & name
def getAlgo ( typ , name , context , location = None , enable = True ) :
    """
    Get the algorithm by type & name, & configure Data-On-Demand 
    
    """
    if hltContext ( context ) :
        
        if -1 == name.find ('Hlt' ) and -1 == name.find('HLT') :
            name = "Hlt" + name
            
        if location and -1 == location.find('Hlt/') : 
            location = location.replace ('Rec/','Hlt/')
                
    alg = typ ( name )
    if _hasProp ( alg , 'Context' )  : setattr ( alg , 'Context' , context ) 
    if location and enable : onDemand ( location , alg ) 
    return alg 


# =============================================================================
## Print current Calo-On-Demand configuration
def printOnDemand () :
    """
    
    Print current Calo-On-Demand configuration
    
    """
    recs   = caloOnDemand()
    result = 'No "Calo-On-Demand" Configuration available'
    if recs :
        line   = '+' + 102*'-' + '+'
        result  = '\n' + line 
        result += '\n' + 35*' ' + ' Calo-On-Demand configuration '
        result += '\n' + line 
        for l in recs :
            result += '\n| %-45.45s --> %-50.50s |' % ( l , recs[l] )    
        result += '\n' + line

    return result



# =============================================================================
if '__main__' == __name__ :
    print __doc__
    
# =============================================================================
# The END 
# =============================================================================
