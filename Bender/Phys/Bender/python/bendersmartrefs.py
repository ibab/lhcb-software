#!/usr/bin/env python
# =============================================================================
# $Id: bendersmartrefs.py,v 1.1 2005-01-21 09:33:06 ibelyaev Exp $ 
# =============================================================================
# CVS tag $Name: not supported by cvs2svn $ 
# =============================================================================
# @file 
# @author Vanya BeLYAEV Ivan.BElyaev@itep.ru
# @date 2005-01-20
# =============================================================================
"""
Helper module  to (re)define few sipmple methods for few useful event classes
"""
# =============================================================================
__author__  = "Vanya BELYAEV Ivan.Belyaev@itep.ru"
__version__ = "CVS: $Revision: 1.1 $; tag=$Name: not supported by cvs2svn $ "
# =============================================================================

import gaudimodule

def vector2list( vct ) :
    """
    Simple routine which converts 'SmartRefVector' into python list
    """
    if vct.empty()         : return  []
    if list == type( vct ) : return vct
    lst = []
    for i in range( 0 , vct.size() - 1 ) :
        lst.append( getattr( vct , '[]' )( ia ) ) 
        return lst
    
def vector2tuple( vct ) :
    """
    Simple routine which converts 'SmartRefVector' into python tuple
    """
    if vct.empty()          : return  () 
    if tuple == type( vct ) : return vct
    return tuple( vector2list( vct ) )

def subSmartRefVector( klass , method , toList = True ) :
    """
    redefinition of Klass:method
    for C++ method with semantics:
    
    const SmartRefVector<XXXX>&  Klass::method() const ;
    or
    SmartRefVector<XXXX>&        Klass::method() ;
    or
    const SmartRefVector<XXXX>   Klass::method() const ;
    or
    SmartRefVector<XXXX>         Klass::method() ;

    the returned value is converted either to Python list (toList = True)
    or to Python tuple( toList=False)
    
    """
    # 'save' original method
    prefix = '_xxx_'
    _o = prefix + method
    if not hasattr( klass , _o ) : setattr( klass , _o , getattr( klass , method )  )
    _n = prefix + klass.__name__  + prefix + method 
    command  = "def %s( self ) :                           \n"
    command += "\t_res = self.%s()                         \n"
    if toList : command += "\treturn vector2list  ( _res ) \n"
    else      : command += "\treturn vector2tuple ( _res ) \n" 
    command += "\nsetattr( klass , method , %s )           \n" 
    command  = command % ( _n ,_o , _n )
    exec(command)
    
_dicts_ = ( 'Event'     ,
            'PhysEvent' ,
            'CaloEvent' ,
            'TrEvent'   )

for dct in _dicts_ : gaudimodule.loaddict( dct + 'Dict' )

_list_ = [ ( gaudimodule.gbl.MCParticle    , 'endVertices' ) ,
           ( gaudimodule.gbl.MCVertex      , 'products'    ) ,
           # Event/PhysEvent
           ( gaudimodule.gbl.Vertex        , 'products'    ) ,
           ( gaudimodule.gbl.PrimVertex    , 'tracks'      ) ,
           ( gaudimodule.gbl.ProtoParticle , 'calo'        ) ,
           # Event/CaloEvent
           ( gaudimodule.gbl.CaloParticle  , 'hypos'       ) ,
           ( gaudimodule.gbl.CaloHypo      , 'hypos'       ) ,
           ( gaudimodule.gbl.CaloHypo      , 'clusters'    ) ,
           ( gaudimodule.gbl.MCCaloDigit   , 'hits'        ) ]

for klass,method in _list_ :  subSmartRefVector( klass , method )


def indexSmartRefVector( klass ) :
    """
    """
    cklass = gaudimodule.gbl.SmartRefVector(klass)
    prefix  = '_yyy_'
    _n = prefix + '_SRV_' + klass + prefix 
    command  = "def %s( self, i ) :                                              \n"
    command += "\tif i < self.size() : return getattr( self , '[]' ) ( i ).ptr() \n"
    command += "\traise StopIteration                                            \n"
    command += "\nsetattr( cklass , '__getitem__' , %s )                         \n"
    command  = command % (_n,_n)
    print command 
    exec(command)

_list_ = [ 'MCVertex'      ,
           'MCParticle'    ,
           'Vertex'        ,
           'Particle'      ,
           'CaloHypo'      ,
           'TrStoredTrack' ,
           'MCCaloHit'     ]

for klass in _list_ : indexSmartRefVector( klass )

# =============================================================================
# The END 
# =============================================================================
