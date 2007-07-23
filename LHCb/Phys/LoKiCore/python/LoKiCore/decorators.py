#!/usr/bin/env python
# =============================================================================
## @file decorators.py LoKiCore/decorators.py
#  The set of basic decorator for objects from LoKiCore library
#  The file is a part of LoKi and Bender projects
#  @author Vanya BELYAEV ibelyaev@physics.syr.edu
# =============================================================================
""" The set of basic decorators for obejcts from LoKiCore library """
_author_ = "Vanya BELYAEV ibelyaev@physics.syr.edu" 
# =============================================================================

import PyCintex

# construct the global namespace 
_global   = PyCintex.makeNamespace('')
# namespace LoKi
std  = _global.std
LoKi = _global.LoKi
LHCb = _global.LHCb


# (auto) load the obejct form LoKiCoreDict dictionary 
LoKi.RangeBase_
LoKi.KeeperBase

# =============================================================================
## simple function to provide the iteration over LoKi's range objects
def _iter_1_ ( self ) :
    """ Iteration over the range """
    _range = self
    _size  = _range.size()
    _curr  = 0 
    while _curr < _size :
        yield _range.at(_curr)                                  ## YIELD 
        _curr += 1
# =============================================================================
## simple function to provide the iteration over LoKi's keepers objects   
def _iter_2_ ( self ) :
    """ Iteration over Keeper/UniqueKeeper """
    _range = self.toVector()
    _size  = _range.size()
    _curr  = 0 
    while _curr < _size :
        yield _range.at(_curr)                                   ## YIELD 
        _curr += 1
# =============================================================================
## get the possibility to build the slices 
def _slice_ ( self , i , j ) :
    """ slicing abilities for range and keepers """    
    return self.slice( i , j )                                   ## RETURN 
# =============================================================================


# =============================================================================
## decorate the ranges
# =============================================================================
LoKi.RangeBase_  .__iter__     = _iter_1_ 
LoKi.RangeBase_  .__getslice__ = _slice_ 
# =============================================================================
## decorate the functions 
# =============================================================================
LoKi.AuxFunBase  .__repr__    = LoKi.AuxFunBase.printOut
# =============================================================================
## decorate the keepers: 
# =============================================================================
LoKi.KeeperBase  .__iter__     = _iter_2_ 
LoKi.KeeperBase  .__getslice__ = _slice_ 
# =============================================================================


# =============================================================================
## get from the module all types inheriting from the given base
def getInherited ( name , base ) :
    " Get from the module all types inheriting from the given base "
    if not issubclass ( base.__class__ , type ) : base = base.__class__ 
    import sys,types,sets
    result = sets.Set()
    # get the whole content of the module:
    _mod = sys.modules[name].__dict__
    # loop over the module members:
    for key in _mod :
        item = _mod[key]
        if not hasattr    ( item , '__class__'      ) : continue 
        if not issubclass ( item.__class__  ,  type ) : item = item.__class__ 
        if     issubclass ( item            ,  base ) : result.add ( item )
    return result                                           ## RETURN
# =============================================================================
## Decorate the functions using the proper adapters 
def decorateFunctions ( funcs , calls , opers ) :
    """ Decorate the functions using the proper adapters """
    for fun in funcs :
        # operator(): 
        def _call_ (s,*a) :
            """
            Invoke the functional operator: fun(arg)
            
            >>> arg = ...
            >>> result = fun(arg)
            >>> result = fun(arg1,arg2) # for 2-argument functions 
            
            Uses:\n
            """ 
            return calls.__call__ (s,*a)
        # comparisons:
        def _lt_   (s,a) :
            """
            Construct the comparison predicate: fun<other

            >>> fun<2
            >>> fun<fun2

            Uses:\n
            """
            return opers.__lt__   (s,a)
        def _le_   (s,a) :
            """
            Construct the comparison predicate: fun<=other
            
            >>> fun<=2
            >>> fun<=fun2
            
            Uses:\n
            """
            return opers.__le__   (s,a)
        def _gt_   (s,a) :
            """
            Construct the comparison predicate: fun>other

            >>> fun>2
            >>> fun>fun2

            Uses:\n
            """
            return opers.__gt__   (s,a)
        def _ge_   (s,a) :
            """
            Construct the comparison predicate: fun>=other
            
            >>> fun>=2
            >>> fun>=fun2
            
            Uses:\n
            """
            return opers.__ge__   (s,a)
        # equality
        def _eq_   (s,a) :
            """
            Construct the comparison predicate: fun==other

            >>> fun==2
            >>> 4==fun
            >>> fun==fun2

            Uses:\n
            """
            return opers.__eq__   (s,a)
        def _ne_   (s,a) :
            """
            Construct the comparison predicate: fun!=other
            
            >>> fun!=2
            >>> 4!=fun
            >>> fun!=fun2
            
            Uses:\n
            """
            return opers.__ne__   (s,a)
        # algebra: 
        def _add_  (s,a) :
            """
            Construct the addition function: fun+other
            
            >>> fun+2
            >>> fun+fun2
            
            Uses:\n
            """
            return opers.__add__  (s,a)
        def _sub_  (s,a) :
            """
            Construct the subtraction function: fun-other

            >>> fun-2
            >>> fun-fun2

            Uses:\n
            """
            return opers.__sub__  (s,a)
        def _mul_  (s,a) :
            """
            Construct the multiplication function: fun*other

            >>> fun*2
            >>> fun*fun2
            
            Uses:\n
            """
            return opers.__mul__  (s,a)
        def _div_  (s,a) :
            """
            Construct the division function: fun/other
            
            >>> fun/2
            >>> fun/fun2
            
            Uses:\n
            """
            return opers.__div__  (s,a)
        # "rigth" algebra:
        def _radd_ (s,a) :
            """
            Construct the right addition function: other+fun
            
            >>> 2+fun
            >>> fun2+fun
            
            Uses:\n
            """
            return opers.__radd__ (s,a)
        def _rsub_ (s,a) :
            """
            Construct the right subtraction function: other-fun

            >>> 2-fun
            >>> fun2-fun

            Uses:\n
            """
            return opers.__rsub__ (s,a)
        def _rmul_ (s,a) :
            """
            Construct the right multiplication function: other*fun
            
            >>> 2*fun
            >>> fun2*fun
            
            Uses:\n
            """
            return opers.__rmul__ (s,a)
        def _rdiv_ (s,a) :
            """
            Construct the right division function: other/fun
            
            >>> 2/fun
            >>> fun2/fun
            
            Uses:\n
            """
            return opers.__rdiv__ (s,a)
        # other:
        def _neg_  (s)   :
            """
            Construct the unary negate function: -fun
            
            >>> -fun
            
            Uses:\n
            """
            return opers.__neg__  (s)
        def _abs_  (s)   :
            """
            Construct the absolute value function: abs(fun)
            
            >>> abs(fun)
            
            Uses:\n
            """
            return opers.__abs__  (s)
        def _pow_  (s,a) :
            """
            Construct the pow-function: fun**other
            
            >>> pow(fun,3)
            >>> pow(fun,fun2)
            >>> fun**3
            >>> fun**fun2
            
            Uses:\n
            """
            return opers.__pow__  (s,a)
        def _rpow_ (s,a) :
            """
            Construct the right pow-function: other**fun
            
            >>> pow(3,fun)
            >>> pow(fun2,fun)
            >>> 3**fun
            >>> fun2**fun
            
            Uses:\n
            """
            return opers.__rpow__ (s,a)
        # home-made math (just to be coherent)
        def _sin_(s) :
            """
            Construct the sin-function: sin(fun)
            
            >>> sin(fun)
            
            Uses:\n
            """
            return opers.__sin__ (s)
        def _cos_(s) :
            """
            Construct the cos-function: cos(fun)
            
            >>> cos(fun)
            
            Uses:\n
            """
            return opers.__cos__ (s)
        def _tan_(s) :
            """
            Construct the tan-function: tan(fun)
            
            >>> tan(fun)
            
            Uses:\n
            """
            return opers.__tan__ (s)
        def _sinh_(s) :
            """
            Construct the sinh-function: sinh(fun)
            
            >>> sinh(fun)
            
            Uses:\n
            """
            return opers.__sinh__ (s)
        def _cosh_(s) :
            """
            Construct the cosh-function: cosh(fun)
            
            >>> cosh(fun)
            
            Uses:\n
            """
            return opers.__cosh__ (s)
        def _tanh_(s) :
            """
            Construct the tanh-function: tanh(fun)
            
            >>> tanh(fun)
            
            Uses:\n
            """
            return opers.__tanh__ (s)
        def _asin_(s) :
            """
            Construct the asin-function: asin(fun)
            
            >>> asin(fun)
            
            Uses:\n
            """
            return opers.__asin__ (s)
        def _acos_(s) :
            """
            Construct the acos-function: acos(fun)
            
            >>> acos(fun)
            
            Uses:\n
            """
            return opers.__acos__ (s)
        def _atan_(s,*a) :
            """
            Construct the atan-function: atan(fun)
            
            >>> atan(fun)
            >>> atan(fun1,fun2)
            >>> atan(fun1,number)
            
            Uses:\n
            """
            return opers.__atan__ (s,*a)
        def _atan2_(s,a) :
            """
            Construct the atan2-function: atan(fun,fun2)
            
            >>> atan2(fun1,fun2)
            >>> atan2(fun1,number)
            
            Uses:\n
            """
            return opers.__atan2__ (s,a)
        def _exp_(s) :
            """
            Construct the exp-function: exp(fun)
            
            >>> exp(fun)
            
            Uses:\n
            """
            return opers.__exp__ (s)
        def _log_(s) :
            """
            Construct the log-function: log(fun)
            
            >>> log(fun)
            
            Uses:\n
            """
            return opers.__log__ (s)
        def _log10_(s) :
            """
            Construct the log10-function: log10(fun)
            
            >>> log10(fun)
            
            Uses:\n
            """
            return opers.__log10__ (s)
        def _sqrt_(s) :
            """
            Construct the sqrt-function: sqrt(fun)
            
            >>> sqrt(fun)
            
            Uses:\n
            """
            return opers.__sqrt__ (s)
        def _pow2_(s) :
            """
            Construct the pow2-function: pow2(fun)
            
            >>> pow2(fun)
            
            Uses:\n
            """
            return opers.__pow2__ (s)
        def _pow3_(s) :
            """
            Construct the pow3-function: pow3(fun)
            
            >>> pow3(fun)
            
            Uses:\n
            """
            return opers.__pow3__ (s)
        def _pow4_(s) :
            """
            Construct the pow4-function: pow4(fun)
            
            >>> pow4(fun)
            
            Uses:\n
            """
            return opers.__pow4__ (s)
        def _min_(s,a,*b) :
            """
            Construct the min-function: min(fun1,fun2,...)
            
            >>> min(fun1,10)
            >>> min(fun1,fun2)
            >>> min(fun1,fun2,fun3)
            >>> min(fun1,fun2,fun3,fun4)
            
            
            Uses:\n
            """
            return opers.__min__ (s,a,*b)
        def _max_(s,a,*b) :
            """
            Construct the max-function: max(fun1,fun2,...)
            
            >>> max(fun1,10)
            >>> max(fun1,fun2)
            >>> max(fun1,fun2,fun3)
            >>> max(fun1,fun2,fun3,fun4)
            
            
            Uses:\n
            """
            return opers.__max__ (s,a,*b)
        # documentation:
        _call_ . __doc__  += calls.__call__ . __doc__
        _lt_   . __doc__  += opers.__lt__   . __doc__
        _le_   . __doc__  += opers.__le__   . __doc__
        _gt_   . __doc__  += opers.__gt__   . __doc__
        _ge_   . __doc__  += opers.__ge__   . __doc__
        _eq_   . __doc__  += opers.__eq__   . __doc__
        _ne_   . __doc__  += opers.__ne__   . __doc__
        _add_  . __doc__  += opers.__add__  . __doc__
        _sub_  . __doc__  += opers.__mul__  . __doc__
        _mul_  . __doc__  += opers.__sub__  . __doc__
        _div_  . __doc__  += opers.__div__  . __doc__
        _radd_ . __doc__  += opers.__radd__ . __doc__
        _rsub_ . __doc__  += opers.__rmul__ . __doc__
        _rmul_ . __doc__  += opers.__rsub__ . __doc__
        _rdiv_ . __doc__  += opers.__rdiv__ . __doc__
        _neg_  . __doc__  += opers.__neg__  . __doc__
        _abs_  . __doc__  += opers.__abs__  . __doc__
        _pow_  . __doc__  += opers.__pow__  . __doc__
        _rpow_ . __doc__  += opers.__rpow__ . __doc__
        # others:
        _sin_  . __doc__  += opers.__sin__  . __doc__
        _cos_  . __doc__  += opers.__cos__  . __doc__
        _tan_  . __doc__  += opers.__tan__  . __doc__
        _sinh_ . __doc__  += opers.__sinh__ . __doc__
        _cosh_ . __doc__  += opers.__cosh__ . __doc__
        _tanh_ . __doc__  += opers.__tanh__ . __doc__
        _asin_ . __doc__  += opers.__asin__ . __doc__
        _acos_ . __doc__  += opers.__acos__ . __doc__
        _atan_ . __doc__  += opers.__atan__ . __doc__
        _exp_  . __doc__  += opers.__exp__  . __doc__
        _log_  . __doc__  += opers.__log__  . __doc__
        _log10_. __doc__  += opers.__log10__. __doc__
        _sqrt_ . __doc__  += opers.__sqrt__ . __doc__
        _pow2_ . __doc__  += opers.__pow2__ . __doc__
        _pow3_ . __doc__  += opers.__pow3__ . __doc__
        _pow4_ . __doc__  += opers.__pow4__ . __doc__
        _atan2_. __doc__  += opers.__atan2__. __doc__
        _min_  . __doc__  += opers.__min__  . __doc__
        _max_  . __doc__  += opers.__max__  . __doc__
        # use also other docs: 
        _call_ . __doc__  += '\n\t' + fun.__call__   . __doc__ 
        _call_ . __doc__  += '\n\t' + fun.eval       . __doc__ 
        _call_ . __doc__  += '\n\t' + fun.evaluate   . __doc__ 
        # finally redefine the methods:
        fun . __call__   = _call_  # operator() 
        fun . __lt__     = _lt_    # operator<
        fun . __le__     = _le_    # operator<=
        fun . __gt__     = _gt_    # operator>
        fun . __ge__     = _ge_    # operator>=
        fun . __eq__     = _eq_    # operator==
        fun . __ne__     = _ne_    # operator!=
        fun . __add__    = _add_   # operator+
        fun . __sub__    = _sub_   # operator-
        fun . __mul__    = _mul_   # operator*
        fun . __div__    = _div_   # operator/
        fun . __radd__   = _radd_  # operator+
        fun . __rsub__   = _rsub_  # operator-
        fun . __rmul__   = _rmul_  # operator*
        fun . __rdiv__   = _rdiv_  # operator/
        fun . __neg__    = _neg_   # 
        fun . __abs__    = _abs_   # 
        fun . __pow__    = _pow_   # 
        fun . __rpow__   = _rpow_  #
        # some other math:
        fun . __sin__    = _sin_   #
        fun . __cos__    = _cos_   #
        fun . __tan__    = _tan_   #
        fun . __sinh__   = _sinh_  #
        fun . __cosh__   = _cosh_  #
        fun . __tanh__   = _tanh_  #
        fun . __asin__   = _asin_  #
        fun . __acos__   = _acos_  #
        fun . __atan__   = _atan_  #
        fun . __atan2__  = _atan2_ #
        fun . __exp__    = _exp_   #
        fun . __log__    = _log_   #
        fun . __log10__  = _log10_ #
        fun . __sqrt__   = _sqrt_  #
        fun . __pow2__   = _pow2_  #
        fun . __pow3__   = _pow3_  #
        fun . __pow4__   = _pow4_  #
        fun . __min__    = _min_   #
        fun . __max__    = _max_   #
    return funcs                                          ## RETURN 
# =============================================================================        
## Decorate the predicates using the proper adapters 
def decoratePredicates ( cuts , calls , opers ) :
    """ Decorate the predicates using the proper adapters """
    for cut in cuts :
        # operator()
        def _call_ (s,a) :
            """
            Invoke the functional operator: cut(arg)
            
            >>> arg = ...
            >>> result = cut(arg)
            
            Uses:\n
            """ 
            return calls.__call__   (s,a)
        # boolean operations:
        def _or_  (s,a) :
            """
            Construct the 'logical-OR' predicate: cut|other
            
            >>> (cut|True)
            >>> (cut|cut2)
            
            Uses:\n
            """
            return opers.__or__  (s,a)
        def _and_  (s,a) :
            """
            Construct the 'logical-AND' predicate:  cut&other
            
            >>> (cut&True)
            >>> (cut&cut2)
            
            Uses:\n
            """
            return opers.__and__  (s,a) 
        # "right" boolean operations:
        def _ror_  (s,a) :
            """
            Construct the right 'logical-OR' predicate: other|cut
            
            >>> (True|cut)
            >>> (cut2|cut)
            
            Uses:\n
            """
            return opers.__ror__  (s,a)
        def _rand_  (s,a) :
            """
            Construct the right 'logical-AND' predicate: other&cut
            
            >>> (True&cut)
            >>> (cut2&cut)
            
            Uses:\n
            """
            return opers.__rand__  (s,a)
        # other:
        def _invert_ ( s ) :
            """
            Construct the logical negation predicate: !cut
            
            >>> !cut
            
            Uses:\n
            """
            return opers.__invert__  (s)
            
        # documentation: 
        _call_   . __doc__ += calls.__call__   . __doc__
        _or_     . __doc__ += opers.__or__     . __doc__
        _and_    . __doc__ += opers.__and__    . __doc__
        _ror_    . __doc__ += opers.__ror__    . __doc__
        _rand_   . __doc__ += opers.__rand__   . __doc__
        _invert_ . __doc__ += opers.__invert__ . __doc__
        # use also other docs: 
        _call_ . __doc__  += '\n\t' + cut.__call__   . __doc__ 
        _call_ . __doc__  += '\n\t' + cut.evaluate   . __doc__ 
        _call_ . __doc__  += '\n\t' + cut.evaluate   . __doc__ 
        # finally redefine the functions:
        cut .__call__   = _call_   # operator() 
        cut .__or__     = _or_     # operator||
        cut .__and__    = _and_    # operator&&
        cut .__ror__    = _ror_    #
        cut .__rand__   = _rand_   #
        cut .__invert__ = _invert_ # operator!
        for attr in ( '__eq__' , '__ne__' ,
                      '__lt__' , '__lt__' ,
                      '__gt__' , '__ge__' ) :
            if hasattr ( cut , attr ) : setattr ( cut , attr , None ) 
    return cuts                                          ## RETURN 
# =============================================================================
## get all functors and decorate them 
def getAndDecorateFunctions  ( name , base , calls , opers ) :
    """ get all functors and decorate them """
    funcs = getInherited ( name , base )
    return decorateFunctions ( funcs , calls , opers )   ## RETURN 
# =============================================================================
## get all functors and decorate them 
def getAndDecoratePredicates ( name , base , calls , opers ) :
    """ get all functors and decorate them """
    funcs = getInherited ( name , base )
    return decoratePredicates ( funcs , calls , opers )  ## RETURN 
# =============================================================================
# the special case of decoration of ID/ABSID functions:
def decoratePID ( fun , opers ) :
    """ the special case of decoration of ID/ABSID functions """
    # equality 
    def _eq_ (s,a) :
        """
        Construct the predicate: id==other 
        
        >>> id == 11
        >>> id == LHCbParticleID(11)
        >>> id == 'e+'
        >>> 11 == id
        >>> LHCbParticleID(11) == id
        >>> 'e+' == id
        
        Uses:\n
        """ 
        return opers.__eq__  (s,a)
    # non-equality 
    def _ne_ (s,a) :
        """
        Construct the predicate: id!=other 
        
        >>> id != 11
        >>> id != LHCbParticleID(11)
        >>> id != 'e+'
        >>> 11 != id
        >>> LHCbParticleID(11) != id
        >>> 'e+' != id
        
        Uses:\n
        """ 
        return opers.__ne__  (s,a)
    # documentation
    _eq_   . __doc__  += opers.__eq__   . __doc__
    _ne_   . __doc__  += opers.__ne__   . __doc__
    # decorate the function:
    fun . __eq__ = _eq_
    fun . __ne__ = _ne_
    return fun                                    ## RETURN


# =============================================================================
## import all dependent functions 
# =============================================================================
from LoKiCore.functions import * 

# =============================================================================
# The END
# =============================================================================
