#!/usr/bin/env python
# =============================================================================
# $Id: bendersmartrefs.py,v 1.7 2005-02-08 20:31:51 ibelyaev Exp $ 
# =============================================================================
# CVS tag $Name: not supported by cvs2svn $ version $Revison:$
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
__version__ = "CVS tag $Name: not supported by cvs2svn $ version: $Revision: 1.7 $ "
# =============================================================================

import gaudimodule

def vector2list( vct ) :
    """
    Helper method which converts 'SmartRefVector' into python list

    Usage:

    mcparticle = ... 
    evs = mcparticle.endVertices()   # of type SmartRefVector
    evs = vector2list( evs )         # convert to list

    for ev in evs : print ev         # explicit loop is possible 

    This routine is used internally by other,
    but also could be used explicitely

    """
    if vct.empty()         : return  []
    if list == type( vct ) : return vct
    lst = []
    for i in range( 0 , vct.size() - 1 ) :
        lst.append( getattr( vct , '[]' )( i ) ) 
        return lst
    
def vector2tuple( vct ) :
    """
    Helper method which converts 'SmartRefVector' into python tuple

    Usage:

    mcparticle = ... 
    evs = mcparticle.endVertices()   # of type SmartRefVector
    evs = vector2tuple( evs )         # convert to tuple

    for ev in evs : print ev         # explicit loop is possible 

    This routine is used internally by other,
    but also could be used explicitely
    
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

    Usage:

    subSmartRefVector( 'MCParticle'  , 'endVertices' )

    The method redefines the method 'endVertices' for class 'MCParticle'
    to return python list ( toList = True) or Python tuple ( toList = False)
    instead of SmartRefVector
    
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

    
_dicts_ = [ 'Event'     ,
            'PhysEvent' ,
            'CaloEvent' ,
            'TrEvent'   ]

_list_ = [ #( 'MCParticle'    , 'endVertices' ) ,
           #( 'MCVertex'      , 'products'    ) ,
           # Event/PhysEvent
           #( 'Vertex'        , 'products'    ) ,
           ( 'PrimVertex'    , 'tracks'      ) ,
           #( 'ProtoParticle' , 'calo'        ) ,
           # Event/CaloEvent
           ( 'CaloParticle'  , 'hypos'       ) ,
           ( 'CaloHypo'      , 'hypos'       ) ,
           #( 'CaloHypo'      , 'clusters'    ) ,
           ( 'MCCaloDigit'   , 'hits'        ) ]

for klass,method in _list_ :
    subSmartRefVector( gaudimodule.getClass( klass , _dicts_ ) , method )


def indexSmartRefVector( klass ) :
    """
    Helper method which allows iteration (and indexed access!) for
    SmartRefVector<XXXX>

    Usage:

    indexSmartRefVector( 'MCParticle' )

    The method instruments the class  SmartRefVecttor<MCParticle> with
    proper __getitem__ method to allow the explicit looping
    (and indexed access) to elements
    
    """
    cklass = gaudimodule.getClass( "SmartRefVector<%s>" % klass )
    prefix  = '_yyy_'
    _n = prefix + '_SRV_' + klass + prefix 
    command  = "def %s( self, i ) :                                              \n"
    command += "\tif i < self.size() : return getattr( self , '[]' ) ( i ).ptr() \n"
    command += "\traise IndexError                                               \n"
    command += "\nsetattr( cklass , '__getitem__' , %s )                         \n"
    command  = command % (_n,_n)
    exec(command)

_list_ = [ 'MCVertex'      ,
           'MCParticle'    ,
           'Vertex'        ,
           'Particle'      ,
           'CaloHypo'      ,
           'CaloCluster'   ,
           'VeloCluster'   ,
           'TrStoredTrack' ,
           'MCCaloHit'     ]

for klass in _list_ : indexSmartRefVector( klass )

class KeyedContainerIterator(object) :
    """ Helper class: iterator over KeyedContainer<> class """
    def __init__ ( self , container ) :
        self._vector_ = container.containedObjects()
        self._index_  = 0
        self._size_   = self._vector_.size()
        print ' I am iterator: size=' , self._size_
    def next     ( self ) :
        #print ' I am iterator: index/size=' , self._index_ , self._size_         
        index = self._index_ 
        if index < self._size_ :
            obj  = self._vector_[ index ]
            self._index_ += 1
            return obj
        raise StopIteration

def _create_iterator_( self ) :
    return KeyedContainerIterator( self )

def keyedContainerIteration( klass ) :
    """
    Helper method which allow the explicit iteration over KeyedContainer

    Usage:

    name   = 'MCParticle'
    cnt    = 'KeyedContainer<%s,Containers::KeyedObjectManager<Containers::hashmap> > >' % name
    klass  = gaudimodule.getClass( cnt )
    keyedContainerIteration( klass )

    The methos intruments the KeyedContainer class with proper __iter__ method
    wich allows sequential iteration over the container 
    (underlying conversion to vector of Container Objects is used )
    
    """
    setattr( klass , '__iter__' , _create_iterator_ )

_list_  = [ 'MCVertex'      ,
            'MCParticle'    ,
            'Particle'      ,
            'Vertex'        ,
            'CaloCluster'   ,
            'ProtoParticle' ,
            'VeloCluster'   ,
            'MCCaloHit'     ,
            'TrStoredTrack' ]

for name in _list_ :
    cnt = 'KeyedContainer<%s,Containers::KeyedObjectManager<Containers::hashmap> > >' % name
    klass  = gaudimodule.getClass( cnt )
    keyedContainerIteration( klass )
    
# =============================================================================
# The END 
# =============================================================================
