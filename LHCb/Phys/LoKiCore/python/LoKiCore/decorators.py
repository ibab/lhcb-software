#!/usr/bin/env python
# =============================================================================
# $Id$ 
# =============================================================================
# $URL$ 
# =============================================================================
## @file decorators.py LoKiCore/decorators.py
#
#  The set of basic decorator for objects from LoKiCore library
#
#        This file is a part of LoKi project - 
#    "C++ ToolKit  for Smart and Friendly Physics Analysis"
#
#  The package has been designed with the kind help from
#  Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas, 
#  contributions and advices from G.Raven, J.van Tilburg, 
#  A.Golutvin, P.Koppenburg have been used in the design.
#
#  @author Vanya BELYAEV ibelyaev@physics.syr.edu
#
#  $Revision$
#  Last modification $Date$
#                 by $Author$
# =============================================================================
"""
The set of basic decorators for objects from LoKiCore library

    This file is a part of LoKi project - 
``C++ ToolKit  for Smart and Friendly Physics Analysis''

The package has been designed with the kind help from
Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas, 
contributions and advices from G.Raven, J.van Tilburg, 
A.Golutvin, P.Koppenburg have been used in the design.

"""
# =============================================================================
__author__  = "Vanya BELYAEV ibelyaev@physics.syr.edu" 
__date__    = "????-??-??"
__version__ = "SVN $Revision$ "
# =============================================================================

from LoKiCore.basic import cpp, std, LoKi, LHCb, Gaudi  

# (auto) load the objects from LoKiCoreDict dictionary 
LoKi.RangeBase_ = cpp.Gaudi.RangeBase_ 
LoKi.KeeperBase

from LoKiCore.functions import vct_from_list

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
    """
    Get from the module all types inheriting from the given base
    """
    if not issubclass ( base.__class__ , type ) : base = base.__class__
    ##
    result = set()
    # get the whole content of the module:
    _mod = {}
    import sys
    if sys.modules.has_key ( name ) : _mod = sys.modules[name].__dict__
    # loop over the module members:
    for key in _mod :
        item = _mod[key]
        if not hasattr    ( item , '__class__'      ) : continue 
        if not issubclass ( item.__class__  ,  type ) : item = item.__class__ 
        if     issubclass ( item            ,  base ) : result.add ( item )
    return result                                           ## RETURN

# =============================================================================
## get from the module all types inheriting from the given base
def getInherited2 ( name , base ) :
    """
    Get from the module all types inheriting from the given base
    """
    if not issubclass ( base.__class__ , type ) : base = base.__class__
    ##
    import sys
    # get the whole content of the module:
    _mod = {}
    
    if not sys.modules.has_key ( name ) :
        try :
            _module = __import__ ( name )
        except ImportError :
            pass
        
    if sys.modules.has_key ( name ) :
        _mod = sys.modules[name].__dict__
        
    # loop over the module members:
    instances = {}
    types     = {}
    for key in _mod :
        item = _mod[key]
        if not hasattr    ( item , '__class__'      ) : continue
        ok = False 
        if not issubclass ( item.__class__  ,  type ) :
            ok   = True 
            item = item.__class__ 
        if     issubclass ( item            ,  base ) :
            if ok : instances[key] = item
            else  : types    [key] = item 
            
    return (instances,types)                                         ## RETURN

# =============================================================================
## get the signature of the functor 
def getSignature ( functor ) :
    """
    Get the signature of the functor
    """
    bases = list ( functor.__bases__ )
    based = [ functor ] + bases
    for base in bases :
        name = base.__name__
        pattern = 'LoKi::Functor<'
        pos = name.find( pattern ) 
        if 0 <= pos :
            args = name[pos+len(pattern):-1]
            pat2 = 'LoKi::Holder<'
            pos2 = args.find ( pat2 )
            if 0 > pos2 : return tuple( args.split ( ',' ) )
            args    = args[pos2+len(pat2):]
            args    = args.split(',')
            args[1] = args[1][:-1]
            return tuple( args )
        
    return None

# =============================================================================
def buildDoc ( instances      ,
               ftypes         ,
               verbose = True ) :
    
    from LoKiCore.doxygenurl import getURL, searchURL , lbglimpse 

    docs = {}
    
    for key in instances :

        _type = instances[key]

        if vebose : print ' symbol/instance : KEY/TYPE' , key, _type 
        line  = { 'Symbol'     :  key   ,
                  'Instance'   :  True  ,
                  'Type'       :  _type ,
                  'Signature'  : getSignature ( _type ) ,
                  'URL1'       :  searchURL ( 'LoKi::Cuts::' + key ) , 
                  'URL2'       :  getURL    ( _type ) ,
                  'Lbglimpse1' :  lbglimpse ( 'LoKi::Cuts::' + key ) ,
                  'Lbglimpse2' :  lbglimpse ( _type                ) }
        
        docs[key] = line
        
    for key in ftypes :
        
        _type = ftypes [key]
        
        if vebose : print ' symbol/type     : KEY/TYPE' , key, _type 
        line  = { 'Symbol'     :  key   ,
                  'Instance'   :  False ,
                  'Type'       :  _type ,
                  'Signature'  : getSignature ( _type ) ,
                  'URL1'       :  searchURL ( 'LoKi::Cuts::' + key ) , 
                  'URL2'       :  getURL    ( _type ) ,
                  'Lbglimpse1' :  lbglimpse ( 'LoKi::Cuts::' + key ) ,
                  'Lbglimpse2' :  lbglimpse ( _type                ) }
        
        docs[key] = line
        
    return docs 

# =============================================================================
def getSymbols ( modules = [] ) :
    """
    Get all symbols form the list of modules 
    """
    if not modules :
        modules = ( 'LoKiCore.decorators'          ,
                    'LoKiNumbers.decorators'       ,
                    'LoKiGen.decorators'           ,
                    'LoKiMC.decorators'            ,
                    'LoKiGenMC.decorators'         ,
                    'LoKiPhys.decorators'          ,
                    'LoKiPhysMC.decorators'        ,
                    'LoKiAlgo.decorators'          ,
                    'LoKiAlgoMC.decorators'        ,
                    'LoKiArrayFunctors.decorators' ,
                    'LoKiTrigger.decorators'       ,
                    'LoKiHlt.decorators'           ,
                    'LoKiHlt.algorithms'           ) 
                    
    if  not issubclass ( type(modules) , ( list , tuple )  ) :
        return getSymbols ( [ modules ] )
    
    instances = {}
    types     = {}
    for module in modules :
        i,t = getInherited2 ( module , LoKi.AuxFunBase )
        instances.update ( i )
        types.update     ( t )
        
    return ( instances , types )
    
# =============================================================================
## Decorate the functions using the proper adapters 
def decorateShifts ( funcs , calls ) :
    """
    Decorate the functions calls 
    """
    
    _rshift_    = None
    _rrshift_   = None
    
    _rmod_      = None
    _timer_     = None
    
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

    # "right mod" - the special meaning, if exists 
    if hasattr ( calls , '__rmod__' ) :                    
        def _rmod_ ( s , a ) :
            """
            ``Right-mod'' : other % fun 
            
            >>> fun = ...
            >>> fun = Timer('aaa') % fun 
            
            """
            return calls.__rmod__ ( s , a )
        _rmod_ . __doc__  += calls.__rmod__ . __doc__

    # "timer" - the special meaning, if exists 
    if hasattr ( calls , '__timer__' ) :                    
        def _timer_ ( s , *a ) :
            """
            ``Timer'' : create timer functor 
            
            >>> fun = ...
            >>> fun = timer ( fun , 'ququ' )
            
            """
            return calls.__timer__ ( s , *a )
        _timer_ . __doc__  += calls.__timer__ . __doc__


    # decorate the functions 
    for fun in funcs :
        
        # finally redefine the methods:
        if _rrshift_         : fun . __rrshift__ = _rrshift_   # (right) operator>> 
        if _rshift_          : fun . __rshift__  = _rshift_    #         operator>> 
        if _rmod_            : fun . __rmod__    = _rmod_      #         operator%
        if _timer_           : fun . __timer__   = _timer_     # timer 
        
    return funcs

# =============================================================================
## Decorate the functions using the proper adapters 
def decorateCallsFun ( funcs , calls ) :
    """
    Decorate the functions calls 
    """
    
    _call_      = None
    
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
        
    # decorate the functions 
    for fun in funcs :
        # finally redefine the methods:
        if _call_  : fun . __call__ = _call_  

    return decorateShifts ( funcs , calls ) 

# =============================================================================
## Decorate the functions using the proper adapters 
def decorateCallsMap ( funcs , calls ) :
    """
    Decorate the functions calls 
    """
    return decorateCallsFun ( funcs , calls ) 
        
# =============================================================================
## Decorate the predicate using the proper adapters 
def decorateCallsCut ( funcs , calls ) :
    """
    Decorate the predicate calls 
    """
    
    _call_      = None
    
    # operator():
    if hasattr ( calls , '__call__' ) :
        def _call_ (s,*a) :
            """
            Invoke the functional operator for predicate : cut(arg)

            >>> arg = ...
            >>> result = cut(arg)
            >>> result = cut(arg1,arg2) # for 2-argument predicate 
            
            Uses:\n
            """
            result = calls.__call__ ( s , *a )
            
            if isinstance ( result ,  ( int , long ) ) :
                return True if result else False
            
            return result 
            # if not calls.__call__ (s,*a) : return False 
            # return True  
        
        _call_ . __doc__  += calls.__call__ . __doc__

    # decorate the functions 
    for fun in funcs :
        # finally redefine the methods:
        if _call_  : fun . __call__ = _call_
        
    return decorateShifts ( funcs , calls ) 


# ================================================================================
## Decorate the function operations  using the proper adapters 
def decorateFunctionOps ( funcs , opers ) :
    """
    Decorate the functions using the proper adapters
    """

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
    _neg_       = None 
    _abs_       = None 
    _pow_       = None 
    _rpow_      = None 

    _rmod_      = None 
    _mod_       = None
    
    _round_     = None
    _jbit_      = None
    _jbits_     = None

    _sin_       = None 
    _cos_       = None 
    _tan_       = None 
    _sinh_      = None 
    _cosh_      = None 
    _tanh_      = None 
    _asin_      = None 
    _acos_      = None 
    _atan_      = None 
    _atan2_     = None 
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
    _in_range_  = None 
    _in_list_   = None
    _dump1_     = None

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

    _fetch_           = None
    _sort_            = None 
    
    _sum_             = None
    _product_         = None

    _mean_            = None
    _meanErr_         = None
    _rms_             = None

    _timer_ = None 

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
    if hasattr ( opers , '__cpp_eq__' ) :        
        def _eq_   (s,a) :
            """
            Construct the comparison predicate: fun==other
            
            >>> fun==2
            >>> 4==fun
            >>> fun==fun2
            
            Uses:\n
            """
            if issubclass ( type(a) , ( list, tuple ) ) :
                a = vct_from_list ( a ) 
            return opers.__cpp_eq__   (s,a)
        _eq_   . __doc__  += opers.__cpp_eq__   . __doc__
    elif hasattr ( opers , '__eq__' ) :        
        def _eq_   (s,a) :
            """
            Construct the comparison predicate: fun==other
            
            >>> fun==2
            >>> 4==fun
            >>> fun==fun2
            
            Uses:\n
            """
            if issubclass ( type(a) , ( list, tuple ) ) :
                a = vct_from_list ( a ) 
            return opers.__eq__   (s,a)
        _eq_   . __doc__  += opers.__eq__   . __doc__
        
        
    # operator != 
    if hasattr ( opers , '__cpp_ne__' ) :        
        def _ne_   (s,a) :
            """
            Construct the comparison predicate: fun!=other
            
            >>> fun!=2
            >>> 4!=fun
            >>> fun!=fun2
            
            Uses:\n
            """
            if issubclass ( type(a) , ( list, tuple ) ) :
                a = vct_from_list ( a )
            return opers.__cpp_ne__   (s,a)
        _ne_   . __doc__  += opers.__cpp_ne__   . __doc__
    elif hasattr ( opers , '__ne__' ) :        
        def _ne_   (s,a) :
            """
            Construct the comparison predicate: fun!=other
            
            >>> fun!=2
            >>> 4!=fun
            >>> fun!=fun2
            
            Uses:\n
            """
            if issubclass ( type(a) , ( list, tuple ) ) :
                a = vct_from_list ( a )
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

            
    # math: modulo % 
    if hasattr ( opers , '__mod__' ) :                    
        def _mod_ (s,a) :
            """
            Construct the ``modulo''-function: 
            
            >>> fun = ...
            >>> fun = fun % 2 
            
            Uses:\n
            """
            return opers.__mod__ (s,a)
        _mod_ . __doc__  += opers.__mod__ . __doc__

    # pseudo-math:  rigth % 
    if hasattr ( opers , '__rmod__' ) :                    
        def _rmod_ (s,a) :
            """
            Construct the ``right mod''-function:  other % fun
            
            >>> fun = ...
            >>> fun = Timer ('timer') % fun 
            
            Uses:\n
            """
            return opers.__rmod__ (s,a)
        _rmod_ . __doc__  += opers.__rmod__ . __doc__


    # math: round 
    if hasattr ( opers , '__round__' ) :                    
        def _round_ ( s ) :
            """
            Construct the ``round''-function: 
            
            >>> fun = ...
            >>> fun = round ( fun ) 
            
            Uses:\n
            """
            return opers.__round__ (s)
        _round_ . __doc__  += opers.__round__ . __doc__


    # math: get jth bit
    if hasattr ( opers , '__jbit__' ) :                    
        def _jbit_ ( s , j ) :
            """
            Get j-th bit of the result
            
            >>> fun   = ...
            >>> fun_1 = jbit ( fun , 1 ) 
            
            Uses:\n
            """
            return opers.__jbit__ ( s , j )
        _jbit_ . __doc__  += opers.__jbit__ . __doc__

    # math: get j1-j2 bit range 
    if hasattr ( opers , '__jbits__' ) :                    
        def _jbits_ ( s , j1 , j2 ) :
            """
            Get j1-j2 bits range of the result
            
            >>> fun   = ...
            >>> fun_1 = jbit2 ( fun , 0 , 5  ) 
            
            Uses:\n
            """
            return opers.__jbits__ ( s , j1 , j2 )
        _jbits_ . __doc__  += opers.__jbits__ . __doc__


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


    # "in_range"
    if hasattr ( opers , '__in_range__' ) :
        def _in_range_ ( fun , low , high ) :
            """
            Create the predicate which checks if  the function value 'in range'

            >>> fun  = ... 
            >>> cut  = in_range ( -1 , fun , 1000 )

            """
            return opers.__in_range__ ( fun , low , high ) 
        _in_range_ . __doc__ += opers.__in_range__ .  __doc__ 


    # "in_list"
    if hasattr ( opers , '__in_list__' ) :
        def _in_list_ ( fun , lst , *args ) :
            """
            Create the predicate which checks if  the function value 'in-list'

            >>> fun  = ... 
            >>> cut  = in_list ( fun , -1 , 1000 , 34253 )
            >>> cut  = in_list ( fun , [ -1 , 1000 , 3412] )

            """
            if issubclass ( type(lst) , ( tuple , list ) ) or args :
                vct = vct_from_list ( lst , *args ) 
                return opers.__in_list__ ( fun , vct )
            #
            return opers.__in_list__ ( fun , lst )    
        _in_list_ . __doc__ += opers.__in_list__ .  __doc__ 

    # "timer"
    if hasattr ( opers , '__timer__' ) :
        def _timer_ ( fun , *args ) :
            """
            Create the ``timer'' for fiven functor 

            >>> fun  = ... 
            >>> fun  = timer ( fun , 'ququ' ) 

            """
            #
            return opers.__timer__ ( fun , *args )    
        _timer_ . __doc__ += opers.__timer__ .  __doc__ 

   # dump argument 
    if hasattr ( opers , '__dump1__' ) :
        def _dump1_ ( s , *args ) :
            """
            Simple ``Dump''-metafunctor 
            
            >>> cut = ...
            >>> cut = dump1 ( cut , True )
            
            Uses:\n
            """
            return opers.__dump1__  ( s , *args )
        _dump1_  . __doc__  += opers.__dump1__   . __doc__

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
        def _min_value_ ( s , *a ) :
            """
            Create 'min_value' vector-functor from the scalar functor
            
            >>> fun = ...
            >>> vfun = min_value ( fun )
            
            The concept belongs to Gerhard 'The Great' Raven
            
            Uses:\n
            """
            return opers.__min_value__ ( s , *a )
        _min_value_       . __doc__  += opers.__min_value__       . __doc__ 
            
    # 'max-value'
    if hasattr ( opers , '__max_value__' ) :
        def _max_value_ ( s , *a ) :
            """
            Create 'max_value' vector-functor from the scalar functor
            
            >>> fun = ...
            >>> vfun = max_value ( fun )
            
            The concept belongs to Gerhard 'The Great' Raven
            
            Uses:\n
            """
            return opers.__max_value__ ( s , *a )
        _max_value_       . __doc__  += opers.__max_value__       . __doc__
        
    # 'min-abs-value'
    if hasattr ( opers , '__min_abs_value__' ) :
        def _min_abs_value_ ( s , *a ) :
            """
            Create 'min_abs_value' vector-functor from the scalar functor
            
            >>> fun = ...
            >>> vfun = min_abs_value ( fun )
            
            The concept belongs to Gerhard 'The Great' Raven
            
            Uses:\n
            """
            return opers.__min_abs_value__ ( s , *a )
        _min_abs_value_   . __doc__  += opers.__min_abs_value__   . __doc__ 
        
    # 'max-abs-value'
    if hasattr ( opers , '__max_abs_value__' ) :
        def _max_abs_value_ ( s , *a ) :
            """
            Create 'max_abs_value' vector-functor from the scalar functor
            
            >>> fun = ...
            >>> vfun = max_abs_value ( fun )
            
            The concept belongs to Gerhard 'The Great' Raven
            
            Uses:\n
            """
            return opers.__max_abs_value__ ( s , *a )
        _max_abs_value_   . __doc__  += opers.__max_abs_value__   . __doc__

    # 'sum'
    if hasattr ( opers , '__sum__' ) :
        def _sum_ ( s , *a ) :
            """
            Create 'sum-over-stream' vector-functor from the scalar functor
            
            >>> fun = ...
            >>> vfun = sum( fun )
            
            Uses:\n
            """
            return opers.__sum__ ( s , *a )
        _sum_   . __doc__  += opers.__sum__   . __doc__ 

    # 'product'
    if hasattr ( opers , '__product__' ) :
        def _product_ ( s , *a ) :
            """
            Create 'product-over-stream' vector-functor from the scalar functor
            
            >>> fun = ...
            >>> vfun = product( fun )
            
            Uses:\n
            """
            return opers.__product__ ( s , *a )
        _product_ . __doc__  += opers.__product__   . __doc__ 

    # 'mean'
    if hasattr ( opers , '__mean__' ) :
        def _mean_ ( s , *a ) :
            """
            Create 'mean-over-stream' functor:
            
            >>> fun = ...
            >>> vfun = mean ( fun )
            
            Uses:\n
            """
            return opers.__mean__ ( s , *a )
        _mean_ . __doc__  += opers.__mean__   . __doc__ 

    # 'meanErr'
    if hasattr ( opers , '__meanErr__' ) :
        def _meanErr_ ( s , *a ) :
            """
            Create 'meanErr-over-stream' functor:
            
            >>> fun = ...
            >>> vfun = meanErr ( fun )
            
            Uses:\n
            """
            return opers.__meanErr__ ( s , *a )
        _meanErr_ . __doc__  += opers.__meanErr__   . __doc__ 

    # 'rms'
    if hasattr ( opers , '__rms__' ) :
        def _rms_ ( s , *a ) :
            """
            Create 'rms-over-stream' functor:
            
            >>> fun = ...
            >>> vfun = rms ( fun )
            
            Uses:\n
            """
            return opers.__rms__ ( s , *a )
        _rms_ . __doc__  += opers.__rms__   . __doc__ 
        
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

    # 'fetch/reduce' 
    if hasattr ( opers , '__fetch__' ) :
        def _fetch_ ( s , *a ) :
            """
            Fetch/reduce the vector stream : pick up the certain element from
            the stream and evaluate the scalar function: 

            >>> fun = fetch ( PT , 1 , -1 )
            
            
            Uses:\n
            """
            return opers.__fetch__ ( s , *a )
        _fetch_ . __doc__  += opers.__fetch__ . __doc__
        
    # 'sort' 
    if hasattr ( opers , '__sort__' ) :
        def _sort_ ( s , *a ) :
            """
            Sort the stream, and (optionally) select the first N-elements

            >>> fun = sort ( PT )

            >>> fun = sort ( PT , 5 ) ## select not more that 5 elements  
            
            Uses:\n
            """
            return opers.__sort__ ( s , *a )
        _sort_ . __doc__  += opers.__sort__ . __doc__

    # decorate the functions 
    for fun in funcs :
        
        # finally redefine the methods:
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
        if _neg_             : fun . __neg__     = _neg_       #  
        if _abs_             : fun . __abs__     = _abs_       # 
        if _pow_             : fun . __pow__     = _pow_       # 
        if _rpow_            : fun . __rpow__    = _rpow_      #

        if _round_           : fun . __round__   = _round_     #
        if _jbit_            : fun . __jbit__    = _jbit_      #
        if _jbits_           : fun . __jbits__   = _jbits_     #

        if _mod_             : fun . __mod__     = _mod_       #
        # pseudo-math
        if _rmod_            : fun . __rmod__            = _rmod_      #
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
        if _in_range_        : fun . __in_range__        = _in_range_  #
        if _in_list_         : fun . __in_list__         = _in_list_   #

        if _timer_           : fun . __timer__           = _timer_     #
        if _dump1_           : fun . __dump1__           = _dump1_     #

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
        
        if _sum_             : fun . __sum__             = _sum_       #
        if _product_         : fun . __product__         = _product_   #

        if _fetch_           : fun . __fetch__           = _fetch_     #
        if _sort_            : fun . __sort__            = _sort_      #

        if _mean_            : fun . __mean__            = _mean_       #
        if _meanErr_         : fun . __meanErr__         = _meanErr_    #
        if _rms_             : fun . __rms__             = _rms_        #
        
        for attr in ( '__or__'     ,
                      '__and__'    ,
                      '__invert__' ) :
            if hasattr ( fun , attr ) : setattr ( fun , attr , None )
            
    return funcs                                          ## RETURN 
# =============================================================================        
## Decorate the function operations  using the proper adapters 
def decorateFunctions ( funcs , calls , opers ) :
    """
    decorate the functions  (everything whiich evaluated to double)
    """
    funcs = decorateCallsFun   ( funcs , calls )
    return decorateFunctionOps ( funcs , opers ) 

# =============================================================================
## Decorate the predicates using the proper adapters 
def decoratePredicateOps ( cuts , opers ) :
    """ Decorate the predicates using the proper adapters """

    _or_      = None
    _and_     = None
    
    _invert_  = None
    _monitor_ = None
    _switch_  = None
    _select_  = None
    _process_ = None
    _count_   = None
    _has_     = None
    _scale_   = None

    _union_           = None
    _intersection_    = None
    _difference_      = None
    _sym_difference_  = None
    _includes_        = None
    _fetch_           = None
    
    _eff_     = None
    _effErr_  = None
    _dump1_   = None

    _timer_  = None

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

    # efficiency
    if hasattr ( opers , '__eff__' ) :
        def _eff_ ( s , *args ) :
            """
            Construct ``efficiency'' functor: 
            
            >>> cut = ...
            >>> cut = eff ( cut )
            
            Uses:\n
            """
            return opers.__eff__  ( s , *args )
        _eff_  . __doc__  += opers.__eff__   . __doc__

    # efficiency error
    if hasattr ( opers , '__effErr__' ) :
        def _effErr_ ( s , *args ) :
            """
            Construct ``efficiency-error'' functor: 
            
            >>> cut = ...
            >>> cut = effErr ( cut )
            
            Uses:\n
            """
            return opers.__effErr__  ( s , *args )
        _effErr_  . __doc__  += opers.__effErr__   . __doc__

   # dump argument 
    if hasattr ( opers , '__dump1__' ) :
        def _dump1_ ( s , *args ) :
            """
            Simple ``Dump''-metafunctor 
            
            >>> cut = ...
            >>> cut = dump1 ( cut , True )
            
            Uses:\n
            """
            return opers.__dump1__  ( s , *args )
        _dump1_  . __doc__  += opers.__dump1__   . __doc__


    # timer 
    if hasattr ( opers , '__timer__' ) :
        def _timer_ ( s , *args ) :
            """
            Construct ``timing'' functor: 
            
            >>> cut = ...
            >>> cut = timer ( cut , 'ququ' )
            
            Uses:\n
            """
            return opers.__timer__  ( s , *args )
        _timer_  . __doc__  += opers.__timer__   . __doc__


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

    # scale
    if hasattr ( opers , '__scale__' ) :        
        def _scale_ ( s , sf ) :
            """
            Contruct the 'scaled' predicate function which acts according to the rule :
            
            result =  s ( value ) ? sf() : False 

            It is equivalent to logical AND  between predicate and 'void' predicate

            Keep the maximal rate os access less than 50 Hz:
            >>>  cut1 = scale ( L0_DECISION , FRATE  ( 50 * Gaudi.Units.Hz ) ) 
            
            ## postscale at 10% o fpositive decisions (random) 
            >>>  cut2 = scale ( L0_DECISION , FSCALE ( 0.1 ) )
            
            ## accept only every 3rd positive decion :
            >>>  cut3 = scale ( L0_DECISION , FSKIP  ( 3   ) )
            
            Uses:\n
            """
            return opers.__scale__(s,sf)
        _scale_  . __doc__  += opers.__scale__   . __doc__
    
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


    if hasattr ( opers , '__union__' ) :
        def _union_ ( s , s2 ) :
            """
            Union of two streamers

            >>> fun1 = ...   # the first  streamer 
            >>> fun2 = ...   # the second streamer 
            >>> u = union ( fun1 , fun2 ) 
            
            """
            return opers.__union__ ( s , s2 )
        _union_ .__doc__ += opers .__union__ . __doc__ 

    if hasattr ( opers , '__intersection__' ) :
        def _intersection_ ( s , s2 ) :
            """
            Intersection of two streamers

            >>> fun1 = ...   # the first  streamer 
            >>> fun2 = ...   # the second streamer 
            >>> u = intersection ( fun1 , fun2 ) 
            
            """
            return opers.__intersection__ ( s , s2 )
        _intersection_ .__doc__ += opers .__intersection__ . __doc__ 

    if hasattr ( opers , '__difference__' ) :
        def _difference_ ( s , s2 ) :
            """
            Difference of two streamers

            >>> fun1 = ...   # the first  streamer 
            >>> fun2 = ...   # the second streamer 
            >>> u = difference ( fun1 , fun2 ) 
            
            """
            return opers.__difference__ ( s , s2 )
        _difference_ .__doc__ += opers .__difference__ . __doc__ 

    if hasattr ( opers , '__sym_difference__' ) :
        def _sym_difference_ ( s , s2 ) :
            """
            Symmetric difference of two streamers

            >>> fun1 = ...   # the first  streamer 
            >>> fun2 = ...   # the second streamer 
            >>> u = sym_difference ( fun1 , fun2 ) 
            
            """
            return opers.__sym_difference__ ( s , s2 )
        _sym_difference_ .__doc__ += opers .__sym_difference__ . __doc__ 

    if hasattr ( opers , '__includes__' ) :
        def _includes_ ( s , s2 ) :
            """
            ``Includes'' of two streamers

            >>> fun1 = ...   # the first  streamer 
            >>> fun2 = ...   # the second streamer 
            >>> u = includes ( fun1 , fun2 ) 
            
            """
            return opers.__includes__ ( s , s2 )
        _includes_ .__doc__ += opers .__includes__ . __doc__ 

    # 'fetch/reduce' 
    if hasattr ( opers , '__fetch__' ) :
        def _fetch_ ( s , *a ) :
            """
            Fetch/reduce the vector stream : pick up the certain element from
            the stream and evaluate the scalar predicate 

            >>> fun = fetch ( 1 * GeV < PT , 1 , -1 )
            
            
            Uses:\n
            """
            return opers.__fetch__ ( s , *a )
        _fetch_ . __doc__  += opers.__fetch__ . __doc__


    # perform the actual decoration 
    for cut in cuts : 

        if _or_      :
            cut .__or__       = _or_        # operator||
            cut .__ror__      = _or_        # operator||

        if _and_     :
            cut .__and__      = _and_       # operator&&
            cut .__rand__     = _and_       # operator&&
            
        if _invert_  : cut .__invert__   = _invert_    # operator!
        if _monitor_ : cut . __monitor__ = _monitor_   # monitoring 
        if _switch_  : cut . __switch__  = _switch_    # switch 
        if _scale_   : cut . __scale__   = _scale_     # scale 
        if _select_  : cut . __select__  = _select_    # select
        if _process_ : cut . __process__ = _process_   # process 
        if _count_   : cut . __count__   = _count_     # process 
        if _has_     : cut . __has__     = _has_       # process 
        if _timer_   : cut . __timer__   = _timer_     # timer
        if _dump1_   : cut . __dump1__   = _dump1_     # dump-1

        if _union_         :
            cut . __union__         =  _union_
            if not hasattr ( cut , '__add__'  ) : cut .__add__  = _union_
            
        if _intersection_  :
            cut . __intersection__  =  _intersection_
            
        if _difference_  :
            cut . __difference__  =  _difference_
            cut . __sub__         = _difference_

        if _sym_difference_    :
            cut . __sym_difference__  =  _sym_difference_
        
        if _includes_          : cut . __includes__  =  _includes_
        if _fetch_             : cut . __fetch__     =  _fetch_

        if _eff_       : cut .__eff__     =  _eff_
        if _effErr_    : cut .__effErr__  =  _effErr_
        
        for attr in ( '__eq__' , '__ne__' ,
                      '__lt__' , '__lt__' ,
                      '__gt__' , '__ge__' ) :
            if hasattr ( cut , attr ) : setattr ( cut , attr , None )
           
    return cuts                                          ## RETURN 


# =============================================================================        
## Decorate the function operations  using the proper adapters 
def decoratePredicates ( funcs , calls , opers ) :
    """
    decorate the predicates  (everything which evaluated to bool)
    """
    funcs = decorateCallsCut    ( funcs , calls )
    return decoratePredicateOps ( funcs , opers ) 

# =============================================================================
## get all functors and decorate them 
def getAndDecorateFunctions  ( name , base , calls , opers ) :
    """
    get all functors and decorate them
    """
    funcs = getInherited ( name , base )
    return decorateFunctions ( funcs , calls , opers )   ## RETURN 
# =============================================================================
## get all functors and decorate them 
def getAndDecoratePredicates ( name , base , calls , opers ) :
    """
    get all functors and decorate them
    """
    funcs = getInherited ( name , base )
    return decoratePredicates ( funcs , calls , opers )  ## RETURN 
# =============================================================================
## get all primitive void functors and decorate them
#  void -> double
#  void -> bool
def getAndDecoratePrimitiveVoids ( name ) :
    """
    Get all primitive void functors and decorate them
    
    void -> double
    void -> bool
    
    """
    ## void-stuff
    _d = 'double'
    _b = 'bool'
    _v = 'void'
    # void -> double 
    _decorated   = getAndDecorateFunctions  (
        name                                , ## module name 
        LoKi.Functor        ( _v , _d )     , ## the base 
        LoKi.Dicts.FunCalls ( _v      )     , ## call-traits 
        LoKi.Dicts.FuncOps  ( _v , _v )       ## operators
        )
    
    # void -> bool 
    _decorated  |= getAndDecoratePredicates (
        name                                , ## module name 
        LoKi.Functor        ( _v , _b )     , ## the base 
        LoKi.Dicts.CutCalls ( _v      )     , ## call-traits 
        LoKi.Dicts.CutsOps  ( _v , _v )       ## operators
        )
    
    return _decorated

# =============================================================================
# the special case of decoration of ID/ABSID functions:
def decoratePID ( fun , opers ) :
    """
    the special case of decoration of ID/ABSID functions
    """
    # equality

    _eq_      = None
    _ne_      = None
    _in_list_ = None
    
    if hasattr ( opers , '__cpp_eq__' ) : 
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
            return opers.__cpp_eq__  (s,a)
        _eq_   . __doc__  += opers.__cpp_eq__   . __doc__    
    elif hasattr ( opers , '__eq__' ) :
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
        _eq_   . __doc__  += opers.__eq__   . __doc__    
    
    # non-equality
    if hasattr ( opers , '__cpp_ne__' ) : 
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
            return opers.__cpp_ne__  (s,a)
        _ne_   . __doc__  += opers.__cpp_ne__   . __doc__    
    elif hasattr ( opers , '__ne__' ) : 
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
        _ne_   . __doc__  += opers.__ne__   . __doc__    
        
    # "in_list"
    if hasattr ( opers , '__in_list__' ) :
        def _in_list_ ( fun , lst , *args ) :
            """
            Create the predicate which checks if  the function value 'in-list'

            >>> fun  = ... 
            >>> cut  = in_list ( fun , 'K-' ,'pi-' ) 
            >>> cut  = in_list ( fun , [ 'D0' , 'D+' ] )

            """
            if issubclass ( type(lst) , ( tuple , list ) ) or args :
                vct = vct_from_list ( lst , *args ) 
                return opers.__in_list__ ( fun , vct )
            #
            return opers.__in_list__ ( fun , lst )    
        _in_list_ . __doc__ += opers.__in_list__ .  __doc__ 

    
    # decorate the function:
    if not not _eq_      : fun . __eq__      = _eq_
    if not not _ne_      : fun . __ne__      = _ne_
    if not not _in_list_ : fun . __in_list__ = _in_list_
    
    fun . _pid_opers_ = opers
    
    return fun                                    ## RETURN


# =============================================================================
## Decorate the maps/yields using the proper adapters
def decorateMaps ( funcs , opers ) :
    """
    Decorate all mapping functions
    """
    
    _tee_             = None

    _rmul_            = None
    _union_           = None
    _intersection_    = None
    _difference_      = None
    _sym_difference_  = None
    _includes_        = None
    _cause_           = None
    
    ## streamers: right multiplication 
    #  @thanks Roel AAIJ    
    if hasattr ( opers , '__rmul__' ) :                    
        def _rmul_ ( s , a ) :
            """
            Conditional source (cause) 
            
            >>> condition = 
            >>> source    = 
            >>> newsource = condition * source 

            Thanks to Roel AAIJ
            """
            return opers.__rmul__ ( s , a )
        _rmul_ . __doc__  += opers.__rmul__ . __doc__

    ## streamers: conditional source
    #  @thanks Roel AAIJ    
    if hasattr ( opers , '__cause__' ) :                    
        def _cause_ ( s , a ) :
            """
            Conditional source (cause) 
            
            >>> condition = 
            >>> source    = 
            >>> newsource = cause ( condition , source )
            
            Thanks to Roel AAIJ
            """
            return opers.__cause__ ( s , a )
        _cause_ . __doc__  += opers.__cause__ . __doc__

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
        
    if hasattr ( opers , '__union__' ) :
        def _union_ ( s , s2 ) :
            """
            Union of two streamers

            >>> fun1 = ...   # the first  streamer 
            >>> fun2 = ...   # the second streamer 
            >>> u = union ( fun1 , fun2 ) 
            
            """
            return opers.__union__ ( s , s2 )
        _union_ .__doc__ += opers .__union__ . __doc__ 

    if hasattr ( opers , '__intersection__' ) :
        def _intersection_ ( s , s2 ) :
            """
            Intersection of two streamers

            >>> fun1 = ...   # the first  streamer 
            >>> fun2 = ...   # the second streamer 
            >>> u = intersection ( fun1 , fun2 ) 
            
            """
            return opers.__intersection__ ( s , s2 )
        _intersection_ .__doc__ += opers .__intersection__ . __doc__ 

    if hasattr ( opers , '__difference__' ) :
        def _difference_ ( s , s2 ) :
            """
            Difference of two streamers

            >>> fun1 = ...   # the first  streamer 
            >>> fun2 = ...   # the second streamer 
            >>> u = difference ( fun1 , fun2 ) 
            
            """
            return opers.__difference__ ( s , s2 )
        _difference_ .__doc__ += opers .__difference__ . __doc__ 

    if hasattr ( opers , '__sym_difference__' ) :
        def _sym_difference_ ( s , s2 ) :
            """
            Symmetric difference of two streamers

            >>> fun1 = ...   # the first  streamer 
            >>> fun2 = ...   # the second streamer 
            >>> u = sym_difference ( fun1 , fun2 ) 
            
            """
            return opers.__sym_difference__ ( s , s2 )
        _sym_difference_ .__doc__ += opers .__sym_difference__ . __doc__ 

    if hasattr ( opers , '__includes__' ) :
        def _includes_ ( s , s2 ) :
            """
            ``Includes'' of two streamers

            >>> fun1 = ...   # the first  streamer 
            >>> fun2 = ...   # the second streamer 
            >>> u = includes ( fun1 , fun2 ) 
            
            """
            return opers.__includes__ ( s , s2 )
        _includes_ .__doc__ += opers .__includes__ . __doc__ 

    # finally redefine the functions:
    for fun in funcs :

        if _rmul_    : fun . __rmul__     =  _rmul_
        if _cause_   : fun . _cause_      =  _cause_        
        if _tee_     : fun . __tee__      =  _tee_

        if _union_         :
            fun . __union__         =  _union_
            fun . __add__           =  _union_
            fun . __radd__          =  _union_
            
        if _intersection_  :
            fun . __intersection__  =  _intersection_
            
        if _difference_  :
            fun . __difference__  =  _difference_
            fun .__sub__          = _difference_

        if _sym_difference_    :
            fun . __sym_difference__  =  _sym_difference_
            fun . __xor__             =  _sym_difference_
            fun . __rxor__            =  _sym_difference_
        
        if _includes_  :
            fun . __includes__  =  _includes_


    return decorateShifts ( funcs , opers )   ## RETURN 

# =============================================================================
## get all Mapping functors and decorate them 
def getAndDecorateMaps   ( name , base , opers ) :
    """
    get all ``maps''  and decorate them
    """
    funcs = getInherited     ( name  , base  )
    funcs = decorateCallsMap ( funcs , opers )
    return decorateMaps      ( funcs , opers )  ## RETURN

## get all Piping functors and decorate them 
def getAndDecoratePipes ( name , base , opers ) :
    """
    get all ``pipes'' and decorate them
    """
    funcs = getInherited     ( name  , base  )
    funcs = decorateCallsMap ( funcs , opers ) 
    return decorateMaps      ( funcs , opers )  ## RETURN

## get all Cun-val functors and decorate them 
def getAndDecorateFunVals ( name , base , opers ) :
    """
    get all ``fun-vals''  and decorate them
    """
    funcs = getInherited        ( name  , base  )
    funcs = decorateCallsFun    ( funcs , opers )
    funcs = decorateFunctionOps ( funcs , opers )
    return decorateMaps         ( funcs , opers )  ## RETURN

## get all Cut-val functors and decorate them 
def getAndDecorateCutVals ( name , base , opers ) :
    """
    get all ``cut-vals''  and decorate them
    """
    funcs = getInherited         ( name  , base  )
    funcs = decorateCallsCut     ( funcs , opers )
    funcs = decoratePredicateOps ( funcs , opers )
    return decorateMaps          ( funcs , opers )  ## RETURN

## get all Source functors and decorate them 
def getAndDecorateSources ( name , base , opers ) :
    """
    get all ``sources''  and decorate them
    """
    funcs = getInherited         ( name , base   )
    funcs = decorateCallsFun     ( funcs , opers )
    return decorateMaps          ( funcs , opers )  ## RETURN


# =============================================================================

## Decorate the functions 'extra-info' using the proper adapters
def decorateFunInfo ( funcs , infos  ) :
    """
    Decorate the functions 'extra-info' using the proper adapters
    """

    _info_    = None 
    _logging_ = None

    if hasattr ( infos , '__info__' ) :
        def _info_ (s,*a) :
            """
            Construct ''Smart-extra-info/cache'' functor,
            which returns the value of extraInfo(index)/cache(key),
            if the information present, otherwise it evaluates the functor
            and (optionally) updates the extraInfo/cache field
            
            >>> functor  = ...
            >>> smart    = info ( functor , 15 )
            
            >>> particle = ...
            >>> value    = smart ( particle )
            
            The concept belongs to Jose Angel Hernado Morata and Hugo Ruiz Peres
            
            """ 
            return infos . __info__ ( s , *a )
        # update the documentation 
        _info_ . __doc__ += infos . __info__ . __doc__

    if hasattr ( infos , '__logging__' ) :
        def _logging_ (s,*a) :
            """
            Construct ``logging''-functor, that 
            
            >>> functor = ...
            >>> logged = logging ( functor )
                        
            The concept belongs to Gerhard ``The Great'' Raven 
            
            """ 
            return infos . __logging__ ( s , *a )
        # update the documentation 
        _logging_ . __doc__ += infos . __logging__ . __doc__

    # finally decorate the function 
    for fun in funcs :

        if _info_    : fun    . __info__     = _info_ 
        if _logging_ : fun    . __logging__  = _logging_ 


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
## Decorate the trees 
def decorateTrees ( trees , opers ) :
    """
    Decorate the trees using the proper adapters
    """
    _call_    = None
    _and_     = None
    _or_      = None
    _invert_  = None
    _rshift_  = None
    _rrshift_ = None
    _mark_    = None
    _str_     = None

    
    ## the regular call  
    if hasattr ( opers , '__call__' ) : 
        def _call_ ( s , *a ) :
            """
            Use the regular call:
            
            >>> object =
            >>> result = functor ( object ) 
            
            Uses:\n
            """
            return opers.__call__ ( s , *a )
        # documentation
        _call_.__doc__     += opers . __call__ . __doc__ 


    ## and ? 
    if hasattr ( opers , '__and__' ) : 
        def _and_ ( s , a ) :
            """
            Logical 'AND' for two trees:
            
            >>> tree1 = ...
            >>> tree2 = ...
            >>> tree  = tree1 & tree2
            
            Uses:\n
            """
            return opers.__and__ ( s , a )
        # documentation
        _and_.__doc__     += opers . __and__ . __doc__ 


    ## or ? 
    if hasattr ( opers , '__or__' ) : 
        def _or_ ( s , a ) :
            """
            Logical 'OR' for two trees:
            
            >>> tree1 = ...
            >>> tree2 = ...
            >>> tree  = tree1 | tree2
            
            Uses:\n
            """
            return opers.__or__ ( s , a )
        # documentation
        _or_.__doc__     += opers . __or__ . __doc__ 


    ## mark ? 
    if hasattr ( opers , '__mark__' ) : 
        def _mark_ ( s ) :
            """
            Mark the tree
            
            >>> tree1 = ...
            >>> tree  = mark ( tree1 ) 
            
            Uses:\n
            """
            return opers.__mark__ ( s )
        # documentation
        _mark_.__doc__     += opers . __mark__ . __doc__ 

    ## invert/negate?
    if hasattr ( opers , '__invert__' ) : 
        def _invert_ ( s ) :
            """
            Invert/negate the tree
            
            >>> tree1 = ...
            >>> tree  = ~tree1  
            
            Uses:\n
            """
            return opers.__invert__ ( s )
        # documentation
        _invert_.__doc__     += opers . __invert__ . __doc__ 

    ## right shift/streamer 
    if hasattr ( opers , '__rshift__' ) : 
        def _rshift_ ( s , a ) :
            """
            Right shoift/streamer
            
            >>> obj  = ...
            >>> tree = ...
            >>> res = tree >> obj 
            
            Uses:\n
            """
            return opers.__rshift__ ( s , a )
        # documentation
        _rshift_.__doc__     += opers . __rshift__ . __doc__
        
    ## Right-right shift/streamer 
    if hasattr ( opers , '__rrshift__' ) : 
        def _rrshift_ ( s , a ) :
            """
            Right-right shift/streamer
            
            >>> obj  = ...
            >>> tree = ...
            >>> res = obj >> tree 
            
            Uses:\n
            """
            return opers.__rrshift__ ( s , a )
        # documentation
        _rrshift_.__doc__     += opers . __rshift__ . __doc__ 

    ## string representation
    if hasattr ( opers , '__toString__' ) :
        def _str_ ( s ) :
            """
            String representation of the object
            
            >>> obj 
            
            Uses:\n
            """
            return opers.__toString__ ( s )
        # documentation
        _str_.__doc__     += opers . __str__ . __doc__
        
    
    # finally redefine the functions:
    for tree in trees : 
        if _call_     : tree . __call__    =  _call_ 
        if _and_      : tree . __and__     =  _and_ 
        if _or_       : tree . __or__      =  _or_ 
        if _invert_   : tree . __invert__  =  _invert_ 
        if _rrshift_  : tree . __rrshift__ =  _rrshift_ 
        if _rshift_   : tree . __rshift__  =  _rshift_ 
        if _mark_     : tree . __mark__    =  _mark_
        if _str_      : tree . __str__     =  _str_
        if _str_      : tree . __repr__    =  _str_
        if _str_      : tree . toString    =  _str_
        
    return trees 


# =============================================================================
## Decorate the decay finder 
def decorateFinder ( finder , opers ) :

    _hasDecay_ = None 
    if hasattr ( opers , '__hasDecay__' ) :
        def _hasDecay_ ( self , container ) :
            """
            Check the presence of certain decay in the particle container
            
            >>> finder    = ...  ## the decay finder 
            >>> container = ...  ## the container of particles
            >>> print finder.hasDecay ( container )
            
            """
            return opers.__hasDecay__ ( self , container )
        # documentation:
        _hasDecay_.__doc__ += opers.__hasDecay__.__doc__ 
        
        
    _findDecay_ = None 
    if hasattr ( opers , '__findDecay__' ) :
        def _findDecay_ ( self , input , output  ) :
            """
            Check the presence of certain decay in the particle container
            
            >>> finder = ...  ## the decay finder 
            >>> input  = ...  ## the input  container of particles
            >>> output = ...  ## the output container of 'good' decays
            >>> num = finder.findDecay ( input , output )
            >>> print ' # decays found: ', num
            >>> for p in output : print p.decay() 
            
            """
            return opers.__findDecay__ ( self , input , output )        
        # documentation:
        _findDecay_.__doc__ += opers.__findDecay__.__doc__ 

    
    if _hasDecay_ and not hasattr ( finder , '__hasDecay__' ) :
        finder.__hasDecay__   = _hasDecay_
        finder.  hasDecay     = _hasDecay_
    if _hasDecay_ and not hasattr ( finder , '__hasDecays__' ) :
        finder.__hasDecays__  = _hasDecay_
        finder.  hasDecays    = _hasDecay_
    if _findDecay_ and not hasattr ( finder , '__findDecay__' ) :
        finder.__findDecay__  = _findDecay_
        finder.  findDecay    = _findDecay_
    if _findDecay_ and not hasattr ( finder , '__findDecays__' ) :
        finder.__findDecays__ = _findDecay_
        finder.  findDecays   = _findDecay_

    return finder 

# =============================================================================
## import all dependent functions 
# =============================================================================
from LoKiCore.functions import * 



# =============================================================================
if '__main__' == __name__ :

    print 80*'*'
    print __doc__
    print ' Author  : ' , __author__
    print ' Version : ' , __version__
    print ' Date    : ' , __date__    
    print 80*'*'
    for i in dir() : print i 
        

# =============================================================================
# The END
# =============================================================================
