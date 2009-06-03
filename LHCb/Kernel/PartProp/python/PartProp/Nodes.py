#!/usr/bin/env python
# =============================================================================
# $Id: Nodes.py,v 1.8 2009-06-03 08:32:30 ibelyaev Exp $ 
# =============================================================================
## @file PartProp/Nodes.py
#  Simple "decorator for nodes"
#  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
#  @date 2008-12-03
# =============================================================================
"""
Simple 'decorator for nodes'
"""
# =============================================================================
__author__  = "Vanya BELYAEV Ivan.Belyaev@nikhef.nl" 
__version__ = "CVS Tag $Name: not supported by cvs2svn $, version $Revision: 1.8 $"
# =============================================================================

import PyCintex

# construct the global namespace 
_global   = PyCintex.makeNamespace('')
# namespaces
Decays = _global.Decays
LHCb   = _global.LHCb
std    = _global.std


# =============================================================================
## Decorate the nodes 
def _decorate ( nodes  , opers ) :
    """
    Decorate the functions
    """
    _call_    = None
    _or_      = None
    _ror_     = None
    _and_     = None
    _rand_    = None
    _rshift_  = None
    _rrshift_ = None
    _invert_  = None
    
    ## __call__ 
    if hasattr ( opers , '__call__' ) :
        ## the regular call 
        def _call_ ( self , arg ) :
            """
            Evaluate the functor

            >>> fun = ... # get the functor
            >>> arg = ... # get the argument
            >>> res = fun ( arg )
            """
            result = opers.__call__ ( self , arg )
            return True if result else False 
        _call_ . __doc__ = opers . __call__ . __doc__ 

    ## __or__ 
    if hasattr ( opers , '__or__' ) :
        ## the regular call 
        def _or_ ( self , arg ) :
            """
            LOGICAL or 

            >>> fun1 = ... # get the functor
            >>> fun2 = ... # get the functor
            >>> fun  = fun1 | fun2 
            """
            return opers.__or__ ( self , arg )
        _or_ . __doc__ = opers . __or__ . __doc__ 

    ## __ror__ 
    if hasattr ( opers , '__ror__' ) :
        ## the regular call 
        def _ror_ ( self , arg ) :
            """
            LOGICAL or 

            >>> fun1 = ... # get the functor
            >>> fun2 = ... # get the functor
            >>> fun  = fun1 | fun2 
            """
            return opers.__ror__ ( self , arg )
        _or_ . __doc__ = opers . __or__ . __doc__ 

    ## __and__ 
    if hasattr ( opers , '__and__' ) :
        ## the regular call 
        def _and_ ( self , arg ) :
            """
            LOGICAL and 

            >>> fun1 = ... # get the functor
            >>> fun2 = ... # get the functor
            >>> fun  = fun1 & fun2 
            """
            return opers.__and__ ( self , arg )
        _and_ . __doc__ = opers . __and__ . __doc__ 

    ## __rand__ 
    if hasattr ( opers , '__rand__' ) :
        ## the regular call 
        def _rand_ ( self , arg ) :
            """
            LOGICAL and 

            >>> fun1 = ... # get the functor
            >>> fun2 = ... # get the functor
            >>> fun  = fun1 & fun2 
            """
            return opers.__rand__ ( self , arg )
        _rand_ . __doc__ = opers . __rand__ . __doc__ 


    ## __invert__ 
    if hasattr ( opers , '__invert__' ) :
        ## the regular call 
        def _invert_ ( self , *arg ) :
            """
            LOGICAL negation 

            >>> fun1 = ... # get the functor
            >>> fun  = ~fun2 
            """
            return opers.__invert__ ( self , *arg )
        _invert_ . __doc__ = opers . __invert__ . __doc__ 


    ## __rshift__
    if hasattr ( opers , '__rshift__' ) :
        ## 'right'-shift
        def _rshift_ ( self , arg ) :
            """
            Streamers 

            >>> fun1 = ... # get the functor
            >>> fun1 = ... # get the functor
            >>> fun = fun1 >> fun2 
            """
            return opers.__rshift__ ( self , arg )
        _rshift_ . __doc__ = opers . __rshift__ . __doc__ 


    ## __rrshift__
    if hasattr ( opers , '__rrshift__' ) :
        ## 'right/right'-shift
        def _rrshift_ ( self , arg ) :
            """
            Evaluate the functor as streametr shift 

            >>> fun = ... # get the functor
            >>> arg = ... # get the argument
            >>> res = arg >> fun 
            """
            result = opers.__rrshift__ ( self , arg )
            return True if result else False 
        _rrshift_ . __doc__ = opers . __rrshift__ . __doc__ 

        for node in nodes :
            
            if _call_    : node . __call__    = _call_
            if _or_      : node . __or__      = _or_
            if _ror_     : node . __ror__     = _ror_
            if _and_     : node . __and__     = _and_
            if _rand_    : node . __rand__    = _rand_
            if _rshift_  : node . __rshift__  = _rshift_
            if _rrshift_ : node . __rrshift__ = _rrshift_
            if _invert_  : node . __invert__  = _invert_

            node . __repr__ =  lambda s : s.toString()
            node . __str__  =  lambda s : s.toString()
            
    return nodes 

## decorate the nodes
_decorated = _decorate ( ( Decays.iNode               ,
                           Decays.Node                ,
                           #
                           Decays.Nodes.Any           , 
                           Decays.Nodes.Pid           , 
                           Decays.Nodes.CC            , 
                           #
                           Decays.Nodes.Lepton        ,
                           Decays.Nodes.Nu            ,
                           Decays.Nodes.Ell           ,
                           Decays.Nodes.EllPlus       ,
                           Decays.Nodes.EllMinus      ,
                           Decays.Nodes.Hadron        ,
                           Decays.Nodes.Meson         ,
                           Decays.Nodes.Baryon        ,
                           Decays.Nodes.Charged       ,
                           Decays.Nodes.Positive      ,
                           Decays.Nodes.Negative      ,
                           Decays.Nodes.Neutral       , 
                           Decays.Nodes.HasQuark      , 
                           Decays.Nodes.JSpin         , 
                           Decays.Nodes.SSpin         , 
                           Decays.Nodes.LSpin         , 
                           Decays.Nodes.Nucleus       , 
                           Decays.Nodes.CTau          ,
                           Decays.Nodes.ShortLived_   ,
                           Decays.Nodes.LongLived_    ,
                           Decays.Nodes.Stable        ,
                           Decays.Nodes.StableCharged ,
                           Decays.Nodes.CTau          ,
                           Decays.Nodes.Mass          ,
                           Decays.Nodes.Light         ,
                           Decays.Nodes.Heavy         ,
                           Decays.Nodes.Symbol        ,
                           Decays.Nodes.Invalid       ,
                           Decays.Nodes._Node         ,
                           #
                           Decays.Nodes.Or            ,
                           Decays.Nodes.And           ,
                           Decays.Nodes.Not           ) , ## nodes 
                         # operations 
                         opers = Decays.Dict.NodeOps ) 

## full list of known nodes 
    
iNode     = Decays.iNode
Node      = Decays.Node
#
Any       = Decays.Nodes.Any      ()  # instance  
Pid       = Decays.Nodes.Pid         # type 
CC        = Decays.Nodes.CC           # type 
Lepton    = Decays.Nodes.Lepton   ()  # instance  
Nu        = Decays.Nodes.Nu       ()  # instance  
Ell       = Decays.Nodes.Ell      ()  # instance  
EllPlus   = Decays.Nodes.EllPlus  ()  # instance  
EllMinus  = Decays.Nodes.EllMinus ()  # instance  
Hadron    = Decays.Nodes.Hadron   ()  # instance  
Meson     = Decays.Nodes.Meson    ()  # instance  
Baryon    = Decays.Nodes.Baryon   ()  # instance  
Charged   = Decays.Nodes.Charged  ()  # instance  
Positive  = Decays.Nodes.Positive ()  # instance  
Negative  = Decays.Nodes.Negative ()  # instance  
Neutral   = Decays.Nodes.Neutral  ()  # instance  
HasQuark  = Decays.Nodes.HasQuark     # type   
JSpin     = Decays.Nodes.JSpin        # type   
SSpin     = Decays.Nodes.SSpin        # type   
LSpin     = Decays.Nodes.LSpin        # type   
Nucleus   = Decays.Nodes.Nucleus  ()  # instance  
Or        = Decays.Nodes.Or           # type   
And       = Decays.Nodes.And          # type   
Not       = Decays.Nodes.Not          # type
Invalid   = Decays.Nodes.Invalid ()   # instance
_Node     = Decays.Nodes._Node        # type 

Up        = HasQuark ( LHCb.ParticleID.up      )
Down      = HasQuark ( LHCb.ParticleID.down    )
Strange   = HasQuark ( LHCb.ParticleID.strange )
Charm     = HasQuark ( LHCb.ParticleID.charm   )
Beauty    = HasQuark ( LHCb.ParticleID.bottom  )
Bottom    = HasQuark ( LHCb.ParticleID.bottom  )
Top       = HasQuark ( LHCb.ParticleID.top     )

Xu        = HasQuark ( LHCb.ParticleID.up      )
Xd        = HasQuark ( LHCb.ParticleID.down    )
Xs        = HasQuark ( LHCb.ParticleID.strange )
Xc        = HasQuark ( LHCb.ParticleID.charm   )
Xb        = HasQuark ( LHCb.ParticleID.bottom  )
Xb        = HasQuark ( LHCb.ParticleID.bottom  )
Xt        = HasQuark ( LHCb.ParticleID.top     )

Scalar    = JSpin    ( 1 )
Spinor    = JSpin    ( 2 )
Vector    = JSpin    ( 3 )
Tensor    = JSpin    ( 5 )

OneHalf   = JSpin    ( 2 ) 
ThreeHalf = JSpin    ( 4 ) 
FiveHalf  = JSpin    ( 6 ) 


CTau          = Decays.Nodes.CTau              # type 
LongLived_    = Decays.Nodes.LongLived_        # type 
LongLived     = Decays.Nodes.LongLived_    ()  # instance
ShortLived_   = Decays.Nodes.ShortLived_       # type 
ShortLived    = Decays.Nodes.ShortLived_   ()  # instance
Stable        = Decays.Nodes.Stable        ()  # instance
StableCharged = Decays.Nodes.StableCharged ()  # instance
Mass          = Decays.Nodes.Mass              # type 
Light         = Decays.Nodes.Light             # type 
Heavy         = Decays.Nodes.Heavy             # type 
Symbol        = Decays.Nodes.Symbol            # type 

NodeList      = Decays.Nodes.NodeList


if '__main__' == __name__ :
    print ' decorated objects: %s ' % str(_decorated)
    print _decorated 
    print dir() 
    
# =============================================================================
# The END 
# =============================================================================
