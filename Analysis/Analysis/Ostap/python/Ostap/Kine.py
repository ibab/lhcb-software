#!/usr/bin/env python
# -*- coding: utf-8 -*-
# =============================================================================
# $Id: RooFitDeco.py 175050 2014-07-15 12:19:15Z ibelyaev $
# =============================================================================
## @file
#  Few very simple math-functions related to kinematics  
#
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2011-06-07
#
#  
#                    $Revision: 175050 $
#  Last modification $Date: 2014-07-15 14:19:15 +0200 (Tue, 15 Jul 2014) $
#  by                $Author: ibelyaev $
# =============================================================================
"""Decoration of some RooFit objects for efficient use in python"""
# =============================================================================
__version__ = "$Revision: 175050 $"
__author__  = "Vanya BELYAEV Ivan.Belyaev@itep.ru"
__date__    = "2011-06-07"
__all__     = (
    'EtaVsP'  , ## eta=eta(p)  for the fixed PT
    'EtaVsPt' , ## eta=eta(pt) for the fixed P
    'YvsP'    , ## y=yy(p)     for the fixed PT and mass  
    'YvsPt'   , ## y=y(pt)     for the fixed P  and mass 
    )
# =============================================================================
import math
# =============================================================================
# logging 
# =============================================================================
from AnalysisPython.Logger import getLogger 
if '__main__' ==  __name__ : logger = getLogger( 'Ostap.Kine' )
else                       : logger = getLogger( __name__     )
# =============================================================================
logger.debug ( 'Few very simple math-functions related to kinematics') 
# =============================================================================
_acosh = math.acosh
_atanh = math.atanh
_sqrt  = math.sqrt
if not hasattr ( math , 'coth' ) :
    math.coth  = lambda x : 1.0/math.tanh (     x )
    math.coth. __doc__ = """coth(x)
    Return the   hyperbolic cotangent of x 
    """
    logger.debug ("Insert coth function into math")

if not hasattr ( math , 'acoth' ) :
    math.acoth = lambda x :     math.tanh ( 1.0/x )
    math.acoth. __doc__ = """acoth(x)
    Return the hyperbolic area cotangent of x (|x|>1) 
    """
    logger.debug ("Insert acoth function into math")
    
# =============================================================================
## @class EtaVsP
#  very simple function \f$ \eta = \eta(p) \$  for the fixed transverse momentum
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date 2014-07-19
class EtaVsP(object) :
    """
    Very simple function \f$ \eta = \eta(p) \$  for the fixed transverse momentum
    """
    def __init__ ( self , pt ) :
        assert isinstance ( pt , (int,long,float) ) and 0<=pt , "PT is invalid!"
        self.pt = float ( pt )
    def __call__ ( self , p )  :
        return _acosh ( max ( p , self.pt )  / self.pt )
# =============================================================================
## @class EtaVsPt
#  very simple function \f$ \eta = \eta(p_T) \$  for the fixed momentum
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date 2014-07-19
class EtaVsPt(object) :
    """
    Very simple function \f$ \eta = \eta(p_T) \$  for the fixed momentum
    """
    def __init__ ( self , p  ) :
        assert isinstance ( p , (int,long,float) ) and 0<=p , "P is invalid!"
        self.p = float ( p )
    def __call__ ( self , pt )  :
        return _acosh ( self.p / min ( pt , self.p ) ) 
    

## rapidity as function of P for fixed pt and mass 
class YvsP(object) :
    def __init__ ( self , pt , mass ) :
        assert isinstance ( pt   , (int,long,float) ) and 0<=pt   , "PT is invalid!"
        assert isinstance ( mass , (int,long,float) ) and 0<=mass , "M  is invalid!"
        self.pt2 = float ( pt   ) * float ( pt   ) 
        self.m2  = float ( mass ) * float ( mass )
        
    def __call__ ( self , p ) :
        
        p2  = p * p
        e2  = p2 + self.m2  
        pz2 = p2 - self.pt2 
        
        return _atanh ( _sqrt ( max ( pz2 , 0.0 ) / e2  ) )
        
## rapidity as function of Pt for fixed p and mass 
class YvsPt(object) :
    def __init__ ( self , p , mass ) :
        assert isinstance ( p    , (int,long,float) ) and 0<=p    , "P  is invalid!"
        assert isinstance ( mass , (int,long,float) ) and 0<=mass , "M  is invalid!"
        self.p2  = float ( p    ) * float ( p    ) 
        self.m2  = float ( mass ) * float ( mass )
        self.e2  = self.p2 + self.m2
    def __call__ ( self , pt ) :
        pt2 = pt * pt
        pz2 = self.p2 - pt2 
        return _atanh ( _sqrt ( max ( pz2 , 0.0 ) / self.e2  ) )

# =============================================================================
## convert the objects to the functions 
def _as_TF1_ ( self , xmin , xmax ) :

    if not hasattr ( self , '_func' ) : self._funcs = []

    import ROOT 
    from   Ostap.PyRoUts import funID

    fun      = ROOT.TF1( funID() , self , xmin , xmax ) 
    self._funcs.append ( fun )
    fun._obj = self
    
    return fun


EtaVsP  . asTF1 = _as_TF1_
EtaVsPt . asTF1 = _as_TF1_
YvsP    . asTF1 = _as_TF1_
YvsPt   . asTF1 = _as_TF1_
    
# =============================================================================
if '__main__' == __name__ :
    
    import Ostap.Line
    logger.info ( __file__  + '\n' + Ostap.Line.line  ) 
    logger.info ( 80*'*'   )
    logger.info ( __doc__  )
    logger.info ( 80*'*' )
    logger.info ( ' Author  : %s' %         __author__    ) 
    logger.info ( ' Version : %s' %         __version__   ) 
    logger.info ( ' Date    : %s' %         __date__      )
    logger.info ( ' Symbols : %s' %  list ( __all__     ) )
    logger.info ( 80*'*' ) 
    
# =============================================================================
# The END 
# =============================================================================
