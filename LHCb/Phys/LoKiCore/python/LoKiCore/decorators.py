#!/usr/bin/env python
# =============================================================================
# CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.2 $
# =============================================================================
# $Log: not supported by cvs2svn $
# Revision 1.1  2007/06/01 11:35:27  ibelyaev
#  prepare for v3r0
#
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
            Construct the absoluet value function: abs(fun)
            
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
        # finally redefine the fcuntions:
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
# the special case of decoration of ID/ANSID functions:
def decoratePID ( fun , opers ) :
    """ the special case of decoration of ID/ANSID functions """
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
