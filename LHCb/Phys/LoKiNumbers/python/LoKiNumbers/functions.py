#!/usr/bin/env python
# =============================================================================
# $Id: functions.py,v 1.8 2010-02-13 16:30:32 ibelyaev Exp $ 
# =============================================================================
## @file LoKiNumbers/functions.py
#  The full set of useful objects from LoKiNumbers library 
#  The file is a part of LoKi and Bender projects
#  @author Vanya BELYAEV ibelyaev@physics.syr.edu
# =============================================================================
"""
Few useful objects from LoKiNumbers library
"""
__author__  = "Vanya BELYAEV ibelyaev@physics.syr.edu"
__version__ = "CVS Tag $Name: not supported by cvs2svn $, version $Revision: 1.8 $ " 
# =============================================================================

import LoKiCore.decorators as _LoKiCore 

# Namespaces:
cpp      = _LoKiCore.cpp
std      = _LoKiCore.std
LoKi     = _LoKiCore.LoKi


_d = 'double'
## _v = std.vector( _d ) 
_v = 'std::vector<double>' 
XFunc     = LoKi.Functor            ( _d , _d   )
XFun      = LoKi.FunctorFromFunctor ( _d , _d   )
XCuts     = LoKi.Functor            ( _d , bool )
XCut      = LoKi.FunctorFromFunctor ( _d , bool )

XMaps     = LoKi.Functor            ( _v , _v   )
XMap      = LoKi.FunctorFromFunctor ( _v , _v   )
XPipes    = XMaps 
XPipe     = XMap

XFunVals  = LoKi.Functor            ( _v , _d   )
XFunVal   = LoKi.FunctorFromFunctor ( _v , _d   )
XElements = XFunVals 
XElement  = XFunVal 

XVCuts    = LoKi.Functor            ( _v , bool )
XVCut     = LoKi.FunctorFromFunctor ( _v , bool )
XCutVals  = XVCuts
XCutVal   = XVCut

XTRUE     = LoKi.Constant ( _d , bool ) (True)
XALL      = LoKi.Constant ( _d , bool ) (True)
XFALSE    = LoKi.Constant ( _d , bool ) (False)
XNONE     = LoKi.Constant ( _d , bool ) (False)

XONE      = LoKi.Constant ( _d , _d   ) (1.0)
XZERO     = LoKi.Constant ( _d , _d   ) (0.0)
XNULL     = LoKi.Constant ( _d , _d   ) (0.0)

XVONE     = LoKi.Constant ( _v , _d   ) (1.0)
XVZERO    = LoKi.Constant ( _v , _d   ) (0.0)

XVTRUE    = LoKi.Constant ( _v , bool ) (True)
XVALL     = LoKi.Constant ( _v , bool ) (True)
XVFALSE   = LoKi.Constant ( _v , bool ) (False)
XVNONE    = LoKi.Constant ( _v , bool ) (False)

X         = LoKi.Identity ( _d , _d   ) ()
XID       = X
XV        = LoKi.Identity ( _v , _v   ) ()
XVID      = XV


XVector   = std.vector(_d)

# TES checkers

## @see LoKi::Cuts::EXISTS
EXISTS    = LoKi.TES.Exists
## @see LoKi::Cuts::CONTAINS 
CONTAINS  = LoKi.TES.Contains

# random numbers:
## @see LoKi::Cuts::XRANDOM 
XRANDOM   = LoKi.Random.Numbers
## @see LoKi::Cuts::XRND 
XRND      = LoKi.Random.Uniform
## @see LoKi::Cuts::XRNDM
XRNDM     = XRND  ( 0 , 1 )
## @see LoKi::Cuts::XGAUS
XGAUS     = LoKi.Random.Gaussian
## @see LoKi::Cuts::XGAUSS
XGAUSS    = XGAUS ( 0 , 1 ) 
## @see LoKi::Cuts::XRAND_
XRAND_    = LoKi.Random.Rand
## @see LoKi::Cuts::XRAND
XRAND     = LoKi.Random.Rand ( )

# scalers

## @see LoKi::Cuts::XRATE 
XRATE = LoKi.Scalers.RateLimit
## @see LoKi::Cuts::XSCALE
XSCALE = LoKi.Scalers.RandomScale 
## @see LoKi::Cuts::XSKIP
XSKIP  = LoKi.Scalers.Skipper 

## trivia

## @see LoKi::Cuts::XEMPTY 
XEMPTY    = LoKi.Functors.Empty ( 'double' ) ()
## @see LoKi::Cuts::XSIZE 
XSIZE     = LoKi.Functors.Size  ( 'double' ) ()



# =============================================================================
## The iterator function for the vector of something
def _vct_iter_ ( self ) :
    """
    The iterator function for the vector of something

    >>> vct = ...
    >>> for item in vct :
        ...
        
    """
    _range = self
    _size  = _range.size()
    _curr  = 0 
    while _curr < _size :
        yield _range.at(_curr)                                  ## YIELD 
        _curr += 1

def _vct_prnt_ ( self ) :
    """
    Print vector as list
    """
    lst = []
    for item in self : lst += [item]
    return str(lst)  

if not hasattr ( XVector , "__iter__" ) :
    XVector. __iter__ = _vct_iter_
    XVector. __str__  = _vct_prnt_
    XVector. __repr__ = _vct_prnt_
    

# =============================================================================
if '__main__' == __name__ :
    for o in dir() : print o

# =============================================================================
# The END 
# =============================================================================
