# =============================================================================
# $Id: benderfuncs.py,v 1.2 2006-11-09 14:10:37 ibelyaev Exp $ 
# =============================================================================
# CVS tag $Name: not supported by cvs2svn $ , version $Revision: 1.2 $
# =============================================================================
# $Log: not supported by cvs2svn $
# Revision 1.1  2006/10/11 14:45:07  ibelyaev
#  few steps towards v6r0
#
# =============================================================================
""" This is helper module for decoration of LoKi::Function/Predicate """
# =============================================================================
__author__ = 'Vanya BELYAEV belyaev@lapp.in2p3.fr'
# =============================================================================
## @file
#
# This is helper module for decoration of LoKi::Function/Predicate """
#
#  @date   2004-07-11
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
# =============================================================================

import gaudimodule

_LoKi   = gaudimodule.gbl.LoKi
_Bender = gaudimodule.gbl.Bender
_LHCb   = gaudimodule.gbl.LHCb

_Base   = _LoKi.AuxFunBase
_Func   = _LoKi.Function
_Cuts   = _LoKi.Predicate

_Fun    = _LoKi.FunctionFromFunction
_Cut    = _LoKi.PredicateFromPredicate


## the standard LoKi-printout of the functions 
_Base.__repr__ = _Base.printOut 

## the basic (interface) function for particles @see LoKi::Types::Func 
Func    = _Func ( 'const LHCb::Particle*'     )
## the basic (interface) function for vertices  @see LoKi::Types::VFunc 
VFunc   = _Func ( 'const LHCb::VertexBase*'   )
## the basic (interface) function for MC particles @see LoKi::Types::MCFunc 
MCFunc  = _Func ( 'const LHCb::MCParticle*'   )
## the basic (interface) function for MC vertices  @see LoKi::Types::MCVFunc 
MCVFunc = _Func ( 'const LHCb::MCVertex*'     )
## the basic (interface) function for Gen particles @see LoKi::Types::GFunc 
GFunc   = _Func ( 'const HepMC::GenParticle*' )
## the basic (interface) function for Gen vertices  @see LoKi::Types::GVFunc 
GVFunc  = _Func ( 'const HepMC::GenVertex*'   )

## the basic (interface) predicate for particles @see LoKi::Types::Cuts  
Cuts    = _Cuts ( 'const LHCb::Particle*'     )
## the basic (interface) predicate for vertices  @see LoKi::Types::VCuts  
VCuts   = _Cuts ( 'const LHCb::VertexBase*'   )
## the basic (interface) predicate for MC particles @see LoKi::Types::MCCuts  
MCCuts  = _Cuts ( 'const LHCb::MCParticle*'   )
## the basic (interface) predicate for MC vertices  @see LoKi::Types::MCVCuts  
MCVCuts = _Cuts ( 'const LHCb::MCVertex*'     )
## the basic (interface) predicate for Gen particles @see LoKi::Types::GCuts  
GCuts   = _Cuts ( 'const HepMC::GenParticle*' )
## the basic (interface) predicate for Gen vertices  @see LoKi::Types::GVCuts  
GVCuts  = _Cuts ( 'const HepMC::GenVertex*'   )

## the basic function for particles @see LoKi::Types::Fun
Fun     = _Fun  ( 'const LHCb::Particle*'     )
## the basic function for vertices  @see LoKi::Types::VFun
VFun    = _Fun  ( 'const LHCb::VertexBase*'   )
## the basic function for MC particles @see LoKi::Types::MCFun
MCFun   = _Fun  ( 'const LHCb::MCParticle*'   )
## the basic function for MC vertices  @see LoKi::Types::MCVFun
MCVFun  = _Fun  ( 'const LHCb::MCVertex*'     )
## the basic function for Gen particles @see LoKi::Types::GFun
GFun    = _Fun  ( 'const HepMC::GenParticle*' )
## the basic function for Gen vertices  @see LoKi::Types::GVFun
GVFun   = _Fun  ( 'const HepMC::GenVertex*'   )

## the basic predicate for particles @see LoKi::Types::Cut
Cut     = _Cut  ( 'const LHCb::Particle*'     )
## the basic predicate for vertices  @see LoKi::Types::VCut
VCut    = _Cut  ( 'const LHCb::VertexBase*'   )
## the basic predicate for MC particles @see LoKi::Types::MCCut  
MCCut   = _Cut  ( 'const LHCb::MCParticle*'   )
## the basic predicate for MC vertices  @see LoKi::Types::MCVCut
MCVCut  = _Cut  ( 'const LHCb::MCVertex*'     )
## the basic predicate for Gen particles @see LoKi::Types::GCut  
GCut    = _Cut  ( 'const HepMC::GenParticle*' )
## the basic predicate for Gen vertices  @see LoKi::Types::GVCut
GVCut   = _Cut  ( 'const HepMC::GenVertex*'   )

_BF1 = _Bender.Function ( 'const LHCb::Particle*'     )
_BF2 = _Bender.Function ( 'const LHCb::VertexBase*'   )
_BF3 = _Bender.Function ( 'const LHCb::MCParticle*'   )
_BF4 = _Bender.Function ( 'const LHCb::MCVertex*'     )
_BF5 = _Bender.Function ( 'const HepMC::GenParticle*' )
_BF6 = _Bender.Function ( 'const HepMC::GenVertex*'   )

Func.    eval = lambda s,*a: _BF1._fcall_(s,*a)
Cuts.    eval = lambda s,*a: _BF1._ccall_(s,*a)
VFunc.   eval = lambda s,*a: _BF2._fcall_(s,*a)
VCuts.   eval = lambda s,*a: _BF2._ccall_(s,*a)
MCFunc.  eval = lambda s,*a: _BF3._fcall_(s,*a)
MCCuts.  eval = lambda s,*a: _BF3._ccall_(s,*a)
MCVFunc. eval = lambda s,*a: _BF4._fcall_(s,*a)
MCVCuts. eval = lambda s,*a: _BF4._ccall_(s,*a)
GFunc.   eval = lambda s,*a: _BF5._fcall_(s,*a)
GCuts.   eval = lambda s,*a: _BF5._ccall_(s,*a)
GVFunc.  eval = lambda s,*a: _BF6._fcall_(s,*a)
GVCuts.  eval = lambda s,*a: _BF6._ccall_(s,*a)

## Func.    eval.__doc__ = _BF1._fcall_.__doc__ 
## Cuts.    eval.__doc__ = _BF1._ccall_.__doc__ 
## VFunc.   eval.__doc__ = _BF2._fcall_.__doc__ 
## VCuts.   eval.__doc__ = _BF2._ccall_.__doc__ 
## MCFunc.  eval.__doc__ = _BF3._fcall_.__doc__ 
## MCCuts.  eval.__doc__ = _BF3._ccall_.__doc__ 
## MCVFunc. eval.__doc__ = _BF4._fcall_.__doc__ 
## MCVCuts. eval.__doc__ = _BF4._ccall_.__doc__ 
## GFunc.   eval.__doc__ = _BF5._fcall_.__doc__ 
## GCuts.   eval.__doc__ = _BF5._ccall_.__doc__ 
## GVFunc.  eval.__doc__ = _BF6._fcall_.__doc__ 
## GVCuts.  eval.__doc__ = _BF6._ccall_.__doc__ 

_BO1 = _Bender.Operators ( 'const LHCb::Particle*'     )
_BO2 = _Bender.Operators ( 'const LHCb::VertexBase*'   )
_BO3 = _Bender.Operators ( 'const LHCb::MCParticle*'   )
_BO4 = _Bender.Operators ( 'const LHCb::MCVertex*'     )
_BO5 = _Bender.Operators ( 'const HepMC::GenParticle*' )
_BO6 = _Bender.Operators ( 'const HepMC::GenVertex*'   )

## helper function for decoration of "function-like" objects 
#  it decorates operators for cl1 from operator for cl2
#  @param cl1 class to be modified 
#  @param cl2 the source of modification 
def _opsf_ ( cl1 , cl2 ) :
    """
    Helper function for function-like objects 
    it decorates operators for cl1 from operator for cl2
    - cl1 class to be modified 
    - cl2 the source of modification 
    """
    #
    cl1.__lt__    = lambda s,*a: cl2.__lt__(s,*a)
    cl1.__le__    = lambda s,*a: cl2.__le__(s,*a)
    #
    cl1.__gt__    = lambda s,*a: cl2.__gt__(s,*a)
    cl1.__ge__    = lambda s,*a: cl2.__ge__(s,*a)
    #
    cl1.__eq__    = lambda s,*a: cl2.__eq__(s,*a)
    cl1.__ne__    = lambda s,*a: cl2.__ne__(s,*a)
    #
    cl1.__add__   = lambda s,*a: cl2.__add__(s,*a)
    cl1.__mul__   = lambda s,*a: cl2.__mul__(s,*a)
    cl1.__sub__   = lambda s,*a: cl2.__sub__(s,*a)
    cl1.__div__   = lambda s,*a: cl2.__div__(s,*a)
    #
    cl1.__radd__  = lambda s,*a: cl2.__radd__(s,*a)
    cl1.__rmul__  = lambda s,*a: cl2.__rmul__(s,*a)
    cl1.__rsub__  = lambda s,*a: cl2.__rsub__(s,*a)
    cl1.__rdiv__  = lambda s,*a: cl2.__rdiv__(s,*a)
    #
    cl1.__neg__   = lambda s,*a: cl2.__neg__(s,*a) 
    cl1.__abs__   = lambda s,*a: cl2.__abs__(s,*a)
    cl1.__pow__   = lambda s,*a: cl2.__pow__(s,*a)
    cl1.__rpow__  = lambda s,*a: cl2.__rpow__(s,*a)
    #
##     ## documentation strings:
##     cl1.__lt__   .__doc__ = cl2.__lt__   .__doc__ 
##     cl1.__le__   .__doc__ = cl2.__le__   .__doc__ 
##     cl1.__gt__   .__doc__ = cl2.__gt__   .__doc__ 
##     cl1.__ge__   .__doc__ = cl2.__ge__   .__doc__ 
##     cl1.__eq__   .__doc__ = cl2.__eq__   .__doc__ 
##     cl1.__ne__   .__doc__ = cl2.__ne__   .__doc__ 
##     cl1.__add__  .__doc__ = cl2.__add__  .__doc__ 
##     cl1.__mul__  .__doc__ = cl2.__mul__  .__doc__ 
##     cl1.__sub__  .__doc__ = cl2.__sub__  .__doc__ 
##     cl1.__div__  .__doc__ = cl2.__div__  .__doc__ 
##     cl1.__radd__ .__doc__ = cl2.__radd__ .__doc__ 
##     cl1.__rmul__ .__doc__ = cl2.__rmul__ .__doc__ 
##     cl1.__rsub__ .__doc__ = cl2.__rsub__ .__doc__ 
##     cl1.__rdiv__ .__doc__ = cl2.__rdiv__ .__doc__ 
##     cl1.__neg__  .__doc__ = cl2.__neg__  .__doc__ 
##     cl1.__abs__  .__doc__ = cl2.__abs__  .__doc__ 
##     cl1.__pow__  .__doc__ = cl2.__pow__  .__doc__ 
##     cl1.__rpow__ .__doc__ = cl2.__rpow__ .__doc__ 
    #
    
    
## helper function for decoration of "predicate-like" objects 
#  it decorates operators for cl1 from operator for cl2
#  @param cl1 class to be modified 
#  @param cl2 the source of modification 
def _opsc_ ( cl1 , cl2 ) :
    """
    Helper function for predicate-like objects 
    it decorates operators for cl1 from operator for cl2
    - cl1 class to be modified 
    - cl2 the source of modification 
    """
    #
    cl1.__or__     = lambda s,*a: cl2.__or__(s,*a)
    cl1.__and__    = lambda s,*a: cl2.__and__(s,*a)
    cl1.__ror__    = lambda s,*a: cl2.__ror__(s,*a)
    cl1.__rand__   = lambda s,*a: cl2.__rand__(s,*a)
    cl1.__invert__ = lambda s,*a: cl2.__invert__(s,*a)
##     ## documentation strings
##     cl1.__or__     .__doc__ = cl2.__or__     .__doc__ 
##     cl1.__and__    .__doc__ = cl2.__and__    .__doc__ 
##     cl1.__ror__    .__doc__ = cl2.__ror__    .__doc__ 
##     cl1.__rand__   .__doc__ = cl2.__rand__   .__doc__ 
##     cl1.__invert__ .__doc__ = cl2.__invert__ .__doc__ 

_BM = _Bender.Math
_BM1 = _BM ( 'const LHCb::Particle*'     )
_BM2 = _BM ( 'const LHCb::VertexBase*'   )
_BM3 = _BM ( 'const LHCb::MCParticle*'   )
_BM4 = _BM ( 'const LHCb::MCVertex*'     )
_BM5 = _BM ( 'const HepMC::GenParticle*' )
_BM6 = _BM ( 'const HepMC::GenVertex*'   )


## helper function to switch inbetween different
#  provider of mathematical functon, depending on the
#  argument.
#  @param arg type of the argumen
#  @return known math-provider (None for unknown types)
def _matho_ ( arg ) :
    """
    Helper function to switch inbetween different
    provider of mathematical functon, depending on the argument.
    - arg type of the argumen
    return known math-provider (None for unknown types)
    """
    if   issubclass ( arg ,    Func ) : return _BM1 
    elif issubclass ( arg ,   VFunc ) : return _BM2 
    elif issubclass ( arg ,  MCFunc ) : return _BM3 
    elif issubclass ( arg , MCVFunc ) : return _BM4 
    elif issubclass ( arg ,   GFunc ) : return _BM5 
    elif issubclass ( arg ,  GVFunc ) : return _BM6
    return None 


## helper function to switch inbetween different
#  provider of mathematical functon, depending on the
#  argument.
#  @param arg type of the argumen
#  @return known math-provider (math-module for unknown types)
def _math_ ( arg ) :
    """
    Helper function to switch inbetween different
    provider of mathematical functon, depending on the argument.
    - arg type of the argumen
    return known math-provider (math-module for unknown types)
    """    
    res = _matho_ ( arg )
    if not res : return math
    return res

## standard function @see LoKi::exp
def  exp  ( arg ) : return _math_ ( type(arg) ).  exp  ( arg )
#
## standard function @see LoKi::sin
def  sin  ( arg ) : return _math_ ( type ( arg ) ) .  sin  ( arg )
## standard function @see LoKi::sinh
def  sinh ( arg ) : return _math_ ( type ( arg ) ) .  sinh ( arg )
## standard function @see LoKi::asin
def asin  ( arg ) : return _math_ ( type ( arg ) ) . asin  ( arg )
## standard function @see LoKi::cos
def  cos  ( arg ) : return _math_ ( type ( arg ) ) .  cos  ( arg )
## standard function @see LoKi::cosh
def  cosh ( arg ) : return _math_ ( type ( arg ) ) .  cosh ( arg )
## standard function @see LoKi::acos
def acos  ( arg ) : return _math_ ( type ( arg ) ) . acos  ( arg )
## standard function @see LoKi::tan
def  tan  ( arg ) : return _math_ ( type ( arg ) ) .  tan  ( arg )
## standard function @see LoKi::tanh
def  tanh ( arg ) : return _math_ ( type ( arg ) ) .  tanh ( arg )
## standard function @see LoKi::atan
def atan  ( arg ) : return _math_ ( type ( arg ) ) . atan  ( arg )
#
def sqrt  ( arg ) : return _math_ ( type ( arg ) ) . sqrt  ( arg )
def  log  ( arg ) : return _math_ ( type ( arg ) ) .  log  ( arg )
def log10 ( arg ) : return _math_ ( type ( arg ) ) . log10 ( arg )
def fabs  ( arg ) : return _math_ ( type ( arg ) ) . fabs  ( arg )

def pow2  ( arg ) :
    res = _matho_ ( type ( arg ) )
    if not res : return math.pow ( arg , 2 ) 
    return res.pow2( arg )

def pow3  ( arg ) :
    res = _matho_ ( type ( arg ) )
    if not res : return math.pow ( arg , 3 ) 
    return res.pow3( arg )

def pow4  ( arg ) :
    res = _matho_ ( type ( arg ) )
    if not res : return math.pow ( arg , 4 ) 
    return res.pow4( arg )

def atan2 ( a1 , a2 ) :
    res = _matho_( type(a1) )    
    if not res : res = _matho_ ( type(a2) )
    if not res : return math.atan2( a1 , a2 )
    return res.atan2 ( a1 , a2 )

_BPIDs = _Bender.PIDOperators

def decorateFunctors ( name = '__main__' , verbose = True ) :

    from benderpcuts   import   ID ,   ABSID
    from bendermccuts  import MCID , MCABSID
    from bendergcuts   import  GID ,  GABSID
    
    import sys, types ,sets 
    dct = sys.modules[ name ].__dict__
    _call  = sets.Set()
    _funcs = sets.Set() 
    for key in dct :
        item = dct[key]
        if not hasattr    ( item , '__class__'      ) : continue 
        if not issubclass ( item.__class__  ,  type ) : item = item.__class__ 
        if not issubclass ( item            , _Base ) : continue
        _funcs.add ( item )
    for item in _funcs :
        if hasattr(item,'eval') and hasattr(item,'__call__') :
            item.__call__ = item.eval
            _call.add(item)
        if   item is    Func : _opsf_ ( item , _BO1 )
        elif item is   VFunc : _opsf_ ( item , _BO2 )
        elif item is  MCFunc : _opsf_ ( item , _BO3 )
        elif item is MCVFunc : _opsf_ ( item , _BO4 )
        elif item is   GFunc : _opsf_ ( item , _BO5 )
        elif item is  GVFunc : _opsf_ ( item , _BO6 )
        #
        elif item is    Cuts : _opsc_ ( item , _BO1 )
        elif item is   VCuts : _opsc_ ( item , _BO2 )
        elif item is  MCCuts : _opsc_ ( item , _BO3 )
        elif item is MCVCuts : _opsc_ ( item , _BO4 )
        elif item is   GCuts : _opsc_ ( item , _BO5 )
        elif item is  GVCuts : _opsc_ ( item , _BO6 )

        if   issubclass ( item , type (      ID ) ) :
            item.__eq__ = lambda s,*a: _BPIDs.__eq1__  (s,*a)
            item.__ne__ = lambda s,*a: _BPIDs.__ne1__  (s,*a)
##             item.__eq__.__doc__ = _BPIDs.__eq1__.__doc__
##             item.__ne__.__doc__ = _BPIDs.__ne1__.__doc__
        elif issubclass ( item , type (   ABSID ) ) :
            item.__eq__ = lambda s,*a: _BPIDs.__eq11__ (s,*a)
            item.__ne__ = lambda s,*a: _BPIDs.__ne11__ (s,*a)
##             item.__eq__.__doc__ = _BPIDs.__eq11__.__doc__
##             item.__ne__.__doc__ = _BPIDs.__ne11__.__doc__
        elif issubclass ( item , type (    MCID ) ) :
            item.__eq__ = lambda s,*a: _BPIDs.__eq2__  (s,*a)
            item.__ne__ = lambda s,*a: _BPIDs.__ne2__  (s,*a)
##             item.__eq__.__doc__ = _BPIDs.__eq2__.__doc__
##             item.__ne__.__doc__ = _BPIDs.__ne2__.__doc__
        elif issubclass ( item , type ( MCABSID ) ) :
            item.__eq__ = lambda s,*a: _BPIDs.__eq21__ (s,*a)
            item.__ne__ = lambda s,*a: _BPIDs.__ne21__ (s,*a)
##             item.__eq__.__doc__ = _BPIDs.__eq21__.__doc__
##             item.__ne__.__doc__ = _BPIDs.__ne21__.__doc__
        elif issubclass ( item , type (     GID ) ) :
            item.__eq__ = lambda s,*a: _BPIDs.__eq3__  (s,*a)
            item.__ne__ = lambda s,*a: _BPIDs.__ne3__  (s,*a)
##             item.__eq__.__doc__ = _BPIDs.__eq3__.__doc__
##             item.__ne__.__doc__ = _BPIDs.__ne3__.__doc__
        elif issubclass ( item , type (  GABSID ) ) :
            item.__eq__ = lambda s,*a: _BPIDs.__eq31__ (s,*a)
            item.__ne__ = lambda s,*a: _BPIDs.__ne31__ (s,*a)
##             item.__eq__.__doc__ = _BPIDs.__eq31__.__doc__
##             item.__ne__.__doc__ = _BPIDs.__ne31__.__doc__
            
    #if verbose :
    #    print ' __call__ is modified for %s ' % _call 




if __name__ == '__main__' :
    print __doc__
    print "dir(%s) : %s" % ( __name__ , dir() ) 
