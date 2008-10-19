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
cpp  = _global 
std  = _global.std
LoKi = _global.LoKi
LHCb = _global.LHCb


# (auto) load the obejcts from LoKiCoreDict dictionary 
LoKi.RangeBase_
LoKi.KeeperBase

# =============================================================================
## simple function to provide the iteration over LoKi's range objects
def _iter_1_ ( self ) :
    """
    Iteration over the range:

    >>> r = ...
    >>> for o in r :
    ...   print o
    
    """
    _range = self
    _size  = _range.size()
    _curr  = 0 
    while _curr < _size :
        yield _range.at(_curr)                                  ## YIELD 
        _curr += 1
# =============================================================================
## simple function to provide the iteration over LoKi's keepers objects   
def _iter_2_ ( self ) :
    """
    Iteration over Keeper/UniqueKeeper

    >>> k = ...
    >>> for o in k :
    ...   print o

    
    """
    _range = self.toVector()
    _size  = _range.size()
    _curr  = 0 
    while _curr < _size :
        yield _range.at(_curr)                                   ## YIELD 
        _curr += 1
# =============================================================================
## get the possibility to build the slices 
def _slice_ ( self , i , j ) :
    """
    Slicing abilities for range and keepers:
    
    >>> o =
    >>> n = o[13:28]
    
    """    
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
    """
    Decorate the functions using the proper adapters
    """

    _call_      = None
    _lt_        = None 
    _le_        = None 
    _gt_        = None 
    _ge_        = None 
    _eq_        = None 
    _ne_        = None 
    _add_       = None 
    _sub_       = None 
    _mul_       = None 
    _div_       = None 
    _radd_      = None 
    _rsub_      = None 
    _rmul_      = None 
    _rdiv_      = None 
    _rshift_    = None 
    _rrshift_   = None 
    _neg_       = None 
    _abs_       = None 
    _pow_       = None 
    _rpow_      = None 

    _sin_       = None 
    _cos_       = None 
    _tah_       = None 
    _sinh_      = None 
    _cosh_      = None 
    _tanh_      = None 
    _asin_      = None 
    _acos_      = None 
    _atah_      = None 
    _atah2_     = None 
    _exp_       = None 
    _log_       = None 
    _log10_     = None 
    _sqrt_      = None 
    _cbrt_      = None 
    _pow2_      = None 
    _pow3_      = None 
    _pow4_      = None 
    _min_       = None 
    _max_       = None 
    _monitor_   = None 
    _equal_to_  = None 

    _yields_          = None
    _process_         = None
    _min_value_       = None
    _max_value_       = None
    _min_abs_value_   = None
    _max_abs_value_   = None
    _min_element_     = None
    _max_element_     = None
    _min_abs_element_ = None
    _max_abs_element_ = None
    
    # operator():
    if hasattr ( calls , '__call__' ) :
        def _call_ (s,*a) :
            """
            Invoke the functional operator: fun(arg)
            
            >>> arg = ...
            >>> result = fun(arg)
            >>> result = fun(arg1,arg2) # for 2-argument functions 
            
            Uses:\n
            """ 
            return calls.__call__ (s,*a)
        _call_ . __doc__  += calls.__call__ . __doc__


    # streamers: right shift 
    if hasattr ( calls , '__rshift__' ) :                    
        def _rshift_ ( s , a ) :
            """
            Streamer (map here)
            
            >>> a =
            >>> function =
            >>> result = a >> function
            
            """
            return calls.__rshift__ ( s , a )
        _rshift_  . __doc__  += calls.__rshift__  . __doc__
        

    # streamers: right right shift 
    if hasattr ( calls , '__rrshift__' ) :                    
        def _rrshift_ ( s , a ) :
            """
            Streamer (map here)
            
            >>> a =
            >>> function =
            >>> result = a >> function
            
            """
            return calls.__rrshift__ ( s , a )
        _rrshift_ . __doc__  += calls.__rrshift__ . __doc__
            
        
    # comparisons: operator <
    if hasattr ( opers , '__lt__' ) :
        def _lt_   (s,a) :
            """
            Construct the comparison predicate: fun<other
            
            >>> fun<2
            >>> fun<fun2
            
            Uses:\n
            """
            return opers.__lt__   (s,a)
        _lt_   . __doc__  += opers.__lt__   . __doc__
            

    # operator <= 
    if hasattr ( opers , '__le__' ) :
        def _le_   (s,a) :
            """
            Construct the comparison predicate: fun<=other
            
            >>> fun<=2
            >>> fun<=fun2
            
            Uses:\n
            """
            return opers.__le__   (s,a)
        _le_   . __doc__  += opers.__le__   . __doc__

            
    # operator >
    if hasattr ( opers , '__gt__' ) :
        def _gt_   (s,a) :
            """
            Construct the comparison predicate: fun>other
            
            >>> fun>2
            >>> fun>fun2
            
            Uses:\n
            """
            return opers.__gt__   (s,a)
        _gt_   . __doc__  += opers.__gt__   . __doc__
        
        
    # operator >= 
    if hasattr ( opers , '__ge__' ) :
        def _ge_   (s,a) :
            """
            Construct the comparison predicate: fun>=other
            
            >>> fun>=2
            >>> fun>=fun2
            
            Uses:\n
            """
            return opers.__ge__   (s,a)
        _ge_   . __doc__  += opers.__ge__   . __doc__

        
    # equality ==
    if hasattr ( opers , '__eq__' ) :        
        def _eq_   (s,a) :
            """
            Construct the comparison predicate: fun==other
            
            >>> fun==2
            >>> 4==fun
            >>> fun==fun2
            
            Uses:\n
            """
            return opers.__eq__   (s,a)
        _eq_   . __doc__  += opers.__eq__   . __doc__
        
        
    # operator != 
    if hasattr ( opers , '__ne__' ) :        
        def _ne_   (s,a) :
            """
            Construct the comparison predicate: fun!=other
            
            >>> fun!=2
            >>> 4!=fun
            >>> fun!=fun2
            
            Uses:\n
            """
            return opers.__ne__   (s,a)
        _ne_   . __doc__  += opers.__ne__   . __doc__
            
    # addition + 
    if hasattr ( opers , '__add__' ) :        
        def _add_  (s,a) :
            """
            Construct the addition function: fun+other
            
            >>> fun+2
            >>> fun+fun2
            
            Uses:\n
            """
            return opers.__add__  (s,a)
        _add_  . __doc__  += opers.__add__  . __doc__
        

    # subtraction - 
    if hasattr ( opers , '__sub__' ) :        
        def _sub_  (s,a) :
            """
            Construct the subtraction function: fun-other
            
            >>> fun-2
            >>> fun-fun2
            
            Uses:\n
            """
            return opers.__sub__  (s,a)
        _sub_  . __doc__  += opers.__sub__  . __doc__
        
    # multiplication 
    if hasattr ( opers , '__mul__' ) :        
        def _mul_  (s,a) :
            """
            Construct the multiplication function: fun*other
            
            >>> fun*2
            >>> fun*fun2
            
            Uses:\n
            """
            return opers.__mul__  (s,a)
        _mul_  . __doc__  += opers.__mul__  . __doc__
        
    # division
    if hasattr ( opers , '__div__' ) :        
        def _div_  (s,a) :
            """
            Construct the division function: fun/other
            
            >>> fun/2
            >>> fun/fun2
            
            Uses:\n
            """
            return opers.__div__  (s,a)
        _div_  . __doc__  += opers.__div__  . __doc__

    # "right" addition:
    if hasattr ( opers , '__radd__' ) :        
        def _radd_ (s,a) :
            """
            Construct the right addition function: other+fun
            
            >>> 2+fun
            >>> fun2+fun
            
            Uses:\n
            """
            return opers.__radd__ (s,a)
        _radd_ . __doc__  += opers.__radd__ . __doc__

    # "right" subtraction:
    if hasattr ( opers , '__rsub__' ) :                    
        def _rsub_ (s,a) :
            """
            Construct the right subtraction function: other-fun
            
            >>> 2-fun
            >>> fun2-fun
            
            Uses:\n
            """
            return opers.__rsub__ (s,a)
        _rsub_ . __doc__  += opers.__rsub__ . __doc__
        
    # "right" multiplication
    if hasattr ( opers , '__rmul__' ) :                    
        def _rmul_ (s,a) :
            """
            Construct the right multiplication function: other*fun
            
            >>> 2*fun
            >>> fun2*fun
            
            Uses:\n
            """
            return opers.__rmul__ (s,a)
        _rmul_ . __doc__  += opers.__rmul__ . __doc__
            
    # "right" division
    if hasattr ( opers , '__rdiv__' ) :                    
        def _rdiv_ (s,a) :
            """
            Construct the right division function: other/fun
            
            >>> 2/fun
            >>> fun2/fun
            
            Uses:\n
            """
            return opers.__rdiv__ (s,a)
        _rdiv_ . __doc__  += opers.__rdiv__ . __doc__
            
    # other: negation 
    if hasattr ( opers , '__neg__' ) :                    
        def _neg_  (s)   :
            """
            Construct the unary negate function: -fun
            
            >>> -fun
            
            Uses:\n
            """
            return opers.__neg__  (s)
        _neg_  . __doc__  += opers.__neg__  . __doc__
            
    # math: abs 
    if hasattr ( opers , '__abs__' ) :                    
        def _abs_  (s)   :
            """
            Construct the absolute value function: abs(fun)
            
            >>> abs(fun)
            
            Uses:\n
            """
            return opers.__abs__  (s)
        _abs_  . __doc__  += opers.__abs__  . __doc__
            
    # math: pow
    if hasattr ( opers , '__pow__' ) :                    
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
        _pow_  . __doc__  += opers.__pow__  . __doc__
        
    # math: 'right' pow
    if hasattr ( opers , '__rpow__' ) :                    
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
        _rpow_ . __doc__  += opers.__rpow__ . __doc__
            
    # home-made math (just to be coherent)
        
    # math: 'sin'
    if hasattr ( opers , '__sin__' ) :                    
        def _sin_(s) :
            """
            Construct the sin-function: sin(fun)
            
            >>> sin(fun)
            
            Uses:\n
            """
            return opers.__sin__ (s)
        _sin_  . __doc__  += opers.__sin__  . __doc__
            
    # math: 'cos'
    if hasattr ( opers , '__cos__' ) :                    
        def _cos_(s) :
            """
            Construct the cos-function: cos(fun)
            
            >>> cos(fun)
            
            Uses:\n
            """
            return opers.__cos__ (s)
        _cos_  . __doc__  += opers.__cos__  . __doc__
            
    # math: 'tan'
    if hasattr ( opers , '__tan__' ) :                    
        def _tan_(s) :
            """
            Construct the tan-function: tan(fun)
            
            >>> tan(fun)
            
            Uses:\n
            """
            return opers.__tan__ (s)
        _tan_  . __doc__  += opers.__tan__  . __doc__

    # math: 'sinh'
    if hasattr ( opers , '__sinh__' ) :                                
        def _sinh_(s) :
            """
            Construct the sinh-function: sinh(fun)
            
            >>> sinh(fun)
            
            Uses:\n
            """
            return opers.__sinh__ (s)
        _sinh_ . __doc__  += opers.__sinh__ . __doc__
            
    # math: 'cosh'
    if hasattr ( opers , '__cosh__' ) :                                
        def _cosh_(s) :
            """
            Construct the cosh-function: cosh(fun)
            
            >>> cosh(fun)
            
            Uses:\n
            """
            return opers.__cosh__ (s)
        _cosh_ . __doc__  += opers.__cosh__ . __doc__
        
    # math: 'tanh'
    if hasattr ( opers , '__tahh__' ) :
        def _tanh_(s) :
            """
            Construct the tanh-function: tanh(fun)
            
            >>> tanh(fun)
            
            Uses:\n
            """
            return opers.__tanh__ (s)
        _tanh_ . __doc__  += opers.__tanh__ . __doc__
        
    # math: 'asin'
    if hasattr ( opers , '__asin__' ) :
        def _asin_(s) :
            """
            Construct the asin-function: asin(fun)
            
            >>> asin(fun)
            
            Uses:\n
            """
            return opers.__asin__ (s)
    _asin_ . __doc__  += opers.__asin__ . __doc__
    
    # math: 'acos'
    if hasattr ( opers , '__acos__' ) :
        def _acos_(s) :
            """
            Construct the acos-function: acos(fun)
            
            >>> acos(fun)
            
            Uses:\n
            """
            return opers.__acos__ (s)
        _acos_ . __doc__  += opers.__acos__ . __doc__
        
    # math: 'atan'
    if hasattr ( opers , '__atan__' ) :
        def _atan_(s,*a) :
            """
            Construct the atan-function: atan(fun)
            
            >>> atan(fun)
            >>> atan(fun1,fun2)
            >>> atan(fun1,number)
            
            Uses:\n
            """
            return opers.__atan__ (s,*a)
        _atan_ . __doc__  += opers.__atan__ . __doc__
        
        
    # math: 'atan2'
    if hasattr ( opers , '__atan2__' ) :
        def _atan2_(s,a) :
            """
            Construct the atan2-function: atan(fun,fun2)
            
            >>> atan2(fun1,fun2)
            >>> atan2(fun1,number)
            
            Uses:\n
            """
            return opers.__atan2__ (s,a)
        _atan2_. __doc__  += opers.__atan2__. __doc__
            
    # math: 'exp'
    if hasattr ( opers , '__exp__' ) :
        def _exp_(s) :
            """
            Construct the exp-function: exp(fun)
            
            >>> exp(fun)
            
            Uses:\n
            """
            return opers.__exp__ (s)
        _exp_  . __doc__  += opers.__exp__  . __doc__
            
    # math: 'log'
    if hasattr ( opers , '__log__' ) :
        def _log_(s) :
            """
            Construct the log-function: log(fun)
            
            >>> log(fun)
            
            Uses:\n
            """
            return opers.__log__ (s)
        _log_  . __doc__  += opers.__log__  . __doc__
        
    # math: 'log10'
    if hasattr ( opers , '__log10__' ) :
        def _log10_(s) :
            """
            Construct the log10-function: log10(fun)
            
            >>> log10(fun)
            
            Uses:\n
            """
            return opers.__log10__ (s)
        _log10_. __doc__  += opers.__log10__. __doc__
        
    # math: 'sqrt'
    if hasattr ( opers , '__sqrt__' ) :
        def _sqrt_(s) :
            """
            Construct the sqrt-function: sqrt(fun)
            
            >>> sqrt(fun)
            
            Uses:\n
            """
            return opers.__sqrt__ (s)
        _sqrt_ . __doc__  += opers.__sqrt__ . __doc__    
            
    # math: 'cbrt'
    if hasattr ( opers , '__cbrt__' ) :
        def _cbrt_(s) :
            """
            Construct the cbrt-function: cbrt(fun)
            
            >>> cbrt(fun)
            
            Uses:\n
            """
            return opers.__cbrt__ (s)
        _cbrt_ . __doc__  += opers.__cbrt__ . __doc__
        
    # math: 'pow2'
    if hasattr ( opers , '__pow2__' ) :
        def _pow2_(s) :
            """
            Construct the pow2-function: pow2(fun)
            
            >>> pow2(fun)
            
            Uses:\n
            """
            return opers.__pow2__ (s)
        _pow2_ . __doc__  += opers.__pow2__ . __doc__
            
    # math: 'pow3'
    if hasattr ( opers , '__pow3__' ) :
        def _pow3_(s) :
            """
            Construct the pow3-function: pow3(fun)
            
            >>> pow3(fun)
            
            Uses:\n
            """
            return opers.__pow3__ (s)
        _pow3_ . __doc__  += opers.__pow3__ . __doc__
            
    # math: 'pow4'
    if hasattr ( opers , '__pow4__' ) :
        def _pow4_(s) :
            """
            Construct the pow4-function: pow4(fun)
            
            >>> pow4(fun)
            
            Uses:\n
            """
            return opers.__pow4__ (s)
        _pow4_ . __doc__  += opers.__pow4__ . __doc__
            
    # math: 'min'
    if hasattr ( opers , '__min__' ) :
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
        _min_  . __doc__  += opers.__min__  . __doc__
        
    # math: 'max'
    if hasattr ( opers , '__max__' ) :
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
        _max_  . __doc__  += opers.__max__  . __doc__
        
    # 'monitor'
    if hasattr ( opers , '__monitor__' ) :
        def _monitor_ (s,*m) :
            """
            Contruct the monitored function:
            
            Make monitored function ( see LoKi::Monitoring::Stat )
            >>>  counter = ...
            >>>  fun = monitor ( fun , counter )
            
            Make monitored function ( see LoKi::Monitoring::Printer )
            >>>  fun = monitor ( fun , '\n' )
            
            Make monitored function ( see LoKi::Monitoring::Plot )
            >>>  histogram = ...
            >>>  fun = monitor ( fun , histogram )
            
            Uses:\n
            """
            return opers.__monitor__(s,*m)
        _monitor_  . __doc__  += opers.__monitor__   . __doc__
        
    # 'equal_to'
    if hasattr ( opers , '__equal_to__' ) :
        def _equal_to_ (f,v) :
            """
            Create the predicate which efficiently checks the equality of the
            function to some predefined value. Logically it is just 'operator==',
            but it should be more efficient 
            
            >>> cut = equal_to ( TrTYPE , LHCb.Track.Long )
            
            Uses:\n
            """
            return opers.__equal_to__(f,v)
        _equal_to_ . __doc__  += opers.__equal_to__  . __doc__
            
    # 'yields'
    if hasattr ( opers , '__yields__' ) :
        def _yields_ ( s ) :
            """
            Create 'yields/map' vector-functor from the scalar functor
            
            >>> fun = ...
            >>> vfun = yields ( fun )
            
            The concept belongs to Gerhard 'The Great' Raven
            
            Uses:\n
            """
            return opers.__yields__ ( s )
        _yields_          . __doc__  += opers.__yields__          . __doc__ 
        
    # 'process'
    if hasattr ( opers , '__process__' ) :
        def _process_ ( s ) :
            """
            Create 'process' vector-functor from the scalar functor
            
            >>> fun = ...
            >>> vfun = process ( fun )
            
            The concept belongs to Gerhard 'The Great' Raven
            
            Uses:\n
            """
            return opers.__process__ ( s )
        _process_         . __doc__  += opers.__process__         . __doc__ 
        
    # 'min-value'
    if hasattr ( opers , '__min_value__' ) :
        def _min_value_ ( s ) :
            """
            Create 'min_value' vector-functor from the scalar functor
            
            >>> fun = ...
            >>> vfun = min_value ( fun )
            
            The concept belongs to Gerhard 'The Great' Raven
            
            Uses:\n
            """
            return opers.__min_value__ ( s )
        _min_value_       . __doc__  += opers.__min_value__       . __doc__ 
            
    # 'max-value'
    if hasattr ( opers , '__max_value__' ) :
        def _max_value_ ( s ) :
            """
            Create 'max_value' vector-functor from the scalar functor
            
            >>> fun = ...
            >>> vfun = max_value ( fun )
            
            The concept belongs to Gerhard 'The Great' Raven
            
            Uses:\n
            """
            return opers.__max_value__ ( s )
        _max_value_       . __doc__  += opers.__max_value__       . __doc__
        
    # 'min-abs-value'
    if hasattr ( opers , '__min_abs_value__' ) :
        def _min_abs_value_ ( s ) :
            """
            Create 'min_abs_value' vector-functor from the scalar functor
            
            >>> fun = ...
            >>> vfun = min_abs_value ( fun )
            
            The concept belongs to Gerhard 'The Great' Raven
            
            Uses:\n
            """
            return opers.__min_abs_value__ ( s )
        _min_abs_value_   . __doc__  += opers.__min_abs_value__   . __doc__ 
            
    # 'max-abs-value'
    if hasattr ( opers , '__max_abs_value__' ) :
        def _max_abs_value_ ( s ) :
            """
            Create 'max_abs_value' vector-functor from the scalar functor
            
            >>> fun = ...
            >>> vfun = max_abs_value ( fun )
            
            The concept belongs to Gerhard 'The Great' Raven
            
            Uses:\n
            """
            return opers.__max_abs_value__ ( s )
        _max_abs_value_   . __doc__  += opers.__max_abs_value__   . __doc__
        
    # 'min-element'
    if hasattr ( opers , '__min_element__' ) :
        def _min_element_ ( s ) :
            """
            Create 'min_element' vector-functor from the scalar functor
            
            >>> fun = ...
            >>> vfun = min_element ( fun )
            
            The concept belongs to Gerhard 'The Great' Raven
            
            Uses:\n
            """
            return opers.__min_element__ ( s )
        _min_element_     . __doc__  += opers.__min_element__     . __doc__ 
        
    # 'min-element'
    if hasattr ( opers , '__max_element__' ) :
        def _max_element_ ( s ) :
            """
            Create 'max_element' vector-functor from the scalar functor
            
            >>> fun = ...
            >>> vfun = max_element ( fun )
            
            The concept belongs to Gerhard 'The Great' Raven
            
            Uses:\n
            """
            return opers.__max_element__ ( s )
        _max_element_     . __doc__  += opers.__max_element__     . __doc__
            
    # 'min-abs-element'
    if hasattr ( opers , '__min_abs_element__' ) :
        def _min_abs_element_ ( s ) :
            """
            Create 'min_abs_element' vector-functor from the scalar functor
            
            >>> fun = ...
            >>> vfun = min_abs_element ( fun )
            
            The concept belongs to Gerhard 'The Great' Raven
            
            Uses:\n
            """
            return opers.__min_abs_element__ ( s )
        _min_abs_element_ . __doc__  += opers.__min_abs_element__ . __doc__ 
             
    # 'max-abs-element'
    if hasattr ( opers , '__max_abs_element__' ) :
        def _max_abs_element_ ( s ) :
            """
            Create 'max_abs_element' vector-functor from the scalar functor
            
            >>> fun = ...
            >>> vfun = max_abs_element ( fun )
            
            The concept belongs to Gerhard 'The Great' Raven
            
            Uses:\n
            """
            return opers.__max_abs_element__ ( s )
        _max_abs_element_ . __doc__  += opers.__max_abs_element__ . __doc__

    # decorate the functions 
    for fun in funcs :
        
        # finally redefine the methods:
        if _call_            : fun . __call__    = _call_      # operator() 
        if _lt_              : fun . __lt__      = _lt_        # operator<
        if _le_              : fun . __le__      = _le_        # operator<=
        if _gt_              : fun . __gt__      = _gt_        # operator>
        if _ge_              : fun . __ge__      = _ge_        # operator>=
        if _eq_              : fun . __eq__      = _eq_        # operator==
        if _ne_              : fun . __ne__      = _ne_        # operator!=
        if _add_             : fun . __add__     = _add_       # operator+
        if _sub_             : fun . __sub__     = _sub_       # operator-
        if _mul_             : fun . __mul__     = _mul_       # operator*
        if _div_             : fun . __div__     = _div_       # operator/
        if _radd_            : fun . __radd__    = _radd_      # right-operator+
        if _rsub_            : fun . __rsub__    = _rsub_      # right-operator-
        if _rmul_            : fun . __rmul__    = _rmul_      # right-operator*
        if _rdiv_            : fun . __rdiv__    = _rdiv_      # right-operator/
        if _rrshift_         : fun . __rrshift__ = _rrshift_   # (right) operator>> 
        if _rshift_          : fun . __rshift__  = _rshift_    #         operator>> 
        if _neg_             : fun . __neg__     = _neg_       #  
        if _abs_             : fun . __abs__     = _abs_       # 
        if _pow_             : fun . __pow__     = _pow_       # 
        if _rpow_            : fun . __rpow__            = _rpow_      #
        # some other math:
        if _sin_             : fun . __sin__             = _sin_       #
        if _cos_             : fun . __cos__             = _cos_       #
        if _tan_             : fun . __tan__             = _tan_       #
        if _sinh_            : fun . __sinh__            = _sinh_      #
        if _cosh_            : fun . __cosh__            = _cosh_      #
        if _tanh_            : fun . __tanh__            = _tanh_      #
        if _asin_            : fun . __asin__            = _asin_      #
        if _acos_            : fun . __acos__            = _acos_      #
        if _atan_            : fun . __atan__            = _atan_      #
        if _atan2_           : fun . __atan2__           = _atan2_     #
        if _exp_             : fun . __exp__             = _exp_       #
        if _log_             : fun . __log__             = _log_       #
        if _log10_           : fun . __log10__           = _log10_     #
        if _sqrt_            : fun . __sqrt__            = _sqrt_      #
        if _cbrt_            : fun . __cbrt__            = _cbrt_      #
        if _pow2_            : fun . __pow2__            = _pow2_      #
        if _pow3_            : fun . __pow3__            = _pow3_      #
        if _pow4_            : fun . __pow4__            = _pow4_      #
        if _min_             : fun . __min__             = _min_       #
        if _max_             : fun . __max__             = _max_       #
        if _monitor_         : fun . __monitor__         = _monitor_   #
        if _equal_to_        : fun . __equal_to__        = _equal_to_  #
        # functional part:
        if _yields_          : fun . __yields__          = _yields_           #
        if _process_         : fun . __process__         = _process_          #
        if _min_value_       : fun . __min_value__       = _min_value_        #
        if _max_value_       : fun . __max_value__       = _max_value_        #
        if _min_abs_value_   : fun . __min_abs_value__   = _min_abs_value_    #
        if _max_abs_value_   : fun . __max_abs_value__   = _max_abs_value_    #
        if _min_element_     : fun . __min_element__     = _min_element_      #
        if _max_element_     : fun . __max_element__     = _max_element_      #
        if _min_abs_element_ : fun . __min_abs_element__ = _min_abs_element_  #
        if _max_abs_element_ : fun . __max_abs_element__ = _max_abs_element_  #
        
        for attr in ( '__or__' , '__and__' ) :
            if hasattr ( fun , attr ) : setattr ( fun , attr , None )
            
    return funcs                                          ## RETURN 
# =============================================================================        
## Decorate the predicates using the proper adapters 
def decoratePredicates ( cuts , calls , opers ) :
    """ Decorate the predicates using the proper adapters """

    _call_    = None
    _or_      = None
    _and_     = None
    _invert_  = None
    _rshift_  = None
    _rrshift_ = None
    _monitor_ = None
    _switch_  = None
    _select_  = None
    _process_ = None
    _count_   = None
    _has_     = None
    
    # operator()
    if hasattr ( calls , '__call__' ) :
        def _call_ (s,a) :
            """
            Invoke the functional operator: cut(arg)
            
            >>> arg = ...
            >>> result = cut(arg)
            
            Uses:\n
            """
            result = calls.__call__ (s,a)
            if result : return True
            return False 
        _call_    . __doc__  += calls.__call__     . __doc__

    # streamers: right right shift 
    if hasattr ( calls , '__rrshift__' ) :
        def _rrshift_ ( s , a ) :
            """
            Streamer (filter here)
            
            >>> a = ...
            >>> predicate  = ...
            >>> result = a >> predicate
            
            """
            return calls.__rrshift__ ( s , a )
        _rrshift_ . __doc__  += calls.__rrshift__  . __doc__ 

    # streamers: right shift 
    if hasattr ( calls , '__rshift__' ) :
        def _rshift_ (s, *a) :
            """
            
            NO DOCS YET
            
            Uses:\n
            """
            return calls.__rshift__(s,*a)
        _rshift_  . __doc__  += calls.__rshift__   . __doc__
        

    # boolean operations: OR 
    if hasattr ( opers , '__or__' ) :
        def _or_  (s,a) :
            """
            Construct the 'logical-OR' predicate: cut|other
            
            >>> (cut|True)
            >>> (cut|cut2)
            
            Uses:\n
            """
            return opers.__or__  (s,a)
        _or_      . __doc__  += opers.__or__       . __doc__
    
    # boolean operations: AND 
    if hasattr ( opers , '__and__' ) :
        def _and_  (s,a) :
            """
            Construct the 'logical-AND' predicate:  cut&other
            
            >>> (cut&True)
            >>> (cut&cut2)
            
            Uses:\n
            """
            return opers.__and__  (s,a)
        _and_     . __doc__  += opers.__and__      . __doc__
        
    # invert
    if hasattr ( opers , '__invert__' ) :
        def _invert_ ( s ) :
            """
            Construct the logical negation predicate: !cut
            
            >>> !cut
            
            Uses:\n
            """
            return opers.__invert__  (s)
        _invert_  . __doc__  += opers.__invert__   . __doc__

    # monitor
    if hasattr ( opers , '__monitor__' ) :
        def _monitor_ (s,*m) :
            """
            Contruct the monitored predicate:
            
            Make monitored predicate ( see LoKi::Monitoring::Counter)
            >>>  counter = ...
            >>>  cut = monitor ( cut , counter )
            
            Uses:\n
            """
            return opers.__monitor__(s,*m)
        _monitor_ . __doc__  += opers.__monitor__  . __doc__

    # switch
    if hasattr ( opers , '__switch__' ) :        
        def _switch_ (s,v1,v2) :
            """
            Contruct the function which acts according to the rule  ' s ? v1 : v2 '  
            
            >>>  fun = switch ( Q > 0 , P , PT ) 
            
            Uses:\n
            """
            return opers.__switch__(s,v1,v2)
        _switch_  . __doc__  += opers.__switch__   . __doc__
        
    # select
    if hasattr ( opers , '__select__' ) :        
        def _select_ (s) :
            """
            Construct 'select/filter' vector function fron the scalar predicate:
            
            >>> cut = ...
            >>> selector = select( cut )
            
            The concept belongs to Gerhard 'The Great' Raven
            
            Uses:\n
            """
            return opers.__select__(s)
        _select_  . __doc__  += opers.__select__   . __doc__

    
    # process
    if hasattr ( opers , '__process__' ) :        
        def _process_ (s) :
            """
            Construct 'process' vector function from the scalar predicate:
            
            >>> cut = ...
            >>> processor = process( cut )
            
            The concept belongs to Gerhard 'The Great' Raven
            
            Uses:\n
            """
            return opers.__process__(s)
        _process_ . __doc__  += opers.__process__  . __doc__

    # count
    if hasattr ( opers , '__count__' ) :        
        def _count_ (s) :
            """
            Construct 'count' vector function from the scalar predicate:
            
            >>> cut = ...
            >>> counter = count( cut )
            
            The concept belongs to Gerhard 'The Great' Raven
            
            Uses:\n
            """
            return opers.__count__(s)
        _count_ . __doc__  += opers.__count__  . __doc__


    # has
    if hasattr ( opers , '__has__' ) :        
        def _has_ (s) :
            """
            Construct 'has' vector function from the scalar predicate:
            
            >>> cut = ...
            >>> _has = has( cut )
            
            The concept belongs to Gerhard 'The Great' Raven
            
            Uses:\n
            """
            return opers.__has__(s)
        _has_ . __doc__  += opers.__has__  . __doc__


    # perform the actual decoration 
    for cut in cuts : 

        if _call_    : cut .__call__     = _call_      # operator() 
        if _or_      : cut .__or__       = _or_        # operator||
        if _and_     : cut .__and__      = _and_       # operator&&
        if _invert_  : cut .__invert__   = _invert_    # operator!
        if _rshift_  : cut . __rshift__  = _rshift_    #         operator>>
        if _rrshift_ : cut .__rrshift__  = _rrshift_   # (right) operator>>
        if _monitor_ : cut . __monitor__ = _monitor_   # monitoring 
        if _switch_  : cut . __switch__  = _switch_    # switch 
        if _select_  : cut . __select__  = _select_    # select
        if _process_ : cut . __process__ = _process_   # process 
        if _count_   : cut . __count__   = _count_     # process 
        if _has_     : cut . __has__     = _has_       # process 
        
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
## Decorate the maps/yields using the proper adapters
def decorateMaps ( funcs , opers ) :
    """
    Decorate all mapping functions
    """

    _rshift_  = None
    _rrshift_ = None
    _tee_     = None
    
    ## Use the vector function as streamer
    if hasattr ( opers , '__rrshift__' ) : 
        def _rrshift_ ( s ,*a ) :
            """
            Use the vector function as streamer
            
            >>> object =
            >>> result = object >> functor
            
            Uses:\n
            """
            return opers.__rrshift__ ( s , *a )
        # documentation
        _rrshift_.__doc__     += opers . __rrshift__ . __doc__ 
        
    ## Use the vector function as streamer
    if hasattr ( opers , '__rshift__' ) : 
        def _rshift_ ( s ,*a ) :
            """
            Use the Streamers 
            
            >>> fun1 = ...
            >>> fun2 = ...
            >>> func = fun1 >> fun2 
            
            Uses:\n
            """
            return opers.__rshift__ ( s , *a )
        # documentation
        _rshift_ .__doc__   += opers . __rshift__  . __doc__ 
            
    if hasattr ( opers , '__tee__' ) : 
        def _tee_  ( s ) :
            """
            'Tee' the action :
            
            >>> functor1 = ...
            >>> functor2 = tee ( functor )
            
            The concept belongs to Gerhard 'The Great' Raven
            
            Uses:\n
            """
            return opers.__tee__ ( s )
        # documentation        
        _tee_ .__doc__  += opers . __tee__     . __doc__
                

    # finally redefine the functions:
    for fun in funcs :
        if _rrshift_ : fun . __rrshift__ =  _rrshift_ 
        if _rshift_  : fun . __rshift__  =  _rshift_ 
        if _tee_     : fun . __tee__     =  _tee_ 

        
            
    return funcs                                 ## RETURN

# =============================================================================
## get all mapping functors and decorate them 
def getAndDecorateMaps   ( name , base , opers ) :
    """
    get all maps  and decorate them
    """
    funcs = getInherited ( name , base )
    return decorateMaps  ( funcs , opers )  ## RETURN 
# =============================================================================

## Decorate the functions 'extra-info' using the proper adapters
def decorateFunInfo ( funcs , infos  ) :
    """
    Decorate the functions 'extra-info' using the proper adapters
    """

    _info_ = None 
    if hasattr ( infos , '__info__' ) :
        def _info_ (s,*a) :
            """
            Construct ''Smart-extra-info'' functor,
            which returns the valeu of extraInfo(index), if the information
            present, otherwise it evaluates the functor and (optionally)
            updates the extraInfo field
            
            >>> functor = ...
            >>> smart = info ( 15 , functor )
            
            >>> particle = ...
            >>> value = smart ( particle )
            
            The concept belongs to Jose Angel Hernado Morata and Hugo Ruiz Peres
            
            """ 
            return infos . __info__ ( s , *a )
        # update the documentation 
        _info_ . __doc__ += infos . __info__ . __doc__

    # finally decorate the function 
    for fun in funcs :
        # operator():
        
        if _info_ : fun    . __info__  =         _info_ 

    return funcs

# =============================================================================
## get all 'info'-functors and decorate them 
def getAndDecorateInfos  ( name , base , opers ) :
    """
    Det all infos  and decorate them
    """
    funcs = getInherited ( name , base )
    return decorateFunInfo ( funcs , opers )  ## RETURN 

# =============================================================================
## get all pipeing functors and decorate them 
def getAndDecoratePipes  ( name , base , opers ) :
    """ get all maps  and decorate them """
    funcs = getInherited ( name , base )
    return decorateMaps  ( funcs , opers )  ## RETURN 

# =============================================================================
## import all dependent functions 
# =============================================================================
from LoKiCore.functions import * 

# =============================================================================
# The END
# =============================================================================
