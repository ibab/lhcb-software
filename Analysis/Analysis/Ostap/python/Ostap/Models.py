#!/usr/bin/env python
# -*- coding: utf-8 -*-
# =============================================================================
# $Id$
# =============================================================================
## @file Models.py
#
#  Module with some useful fit-models
#
#  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
#  @date   2011-12-01
#
#                    $Revision$
#  Last modification $Date$
#  by                $Author$
# =============================================================================
"""Module with some useful fit-models"""
# =============================================================================
__version__ = "$Revision$"
__author__  = "Vanya BELYAEV Ivan.Belyaev@cern.ch"
__date__    = "2011-12-01"
__all__     = ()
# =============================================================================
import  ROOT 
from    Ostap.PyRoUts import funID, cpp 
# =============================================================================
# logging 
# =============================================================================
from   AnalysisPython.Logger import getLogger 
if '__main__' ==  __name__ : logger = getLogger ( 'Ostap.Models' )
else                       : logger = getLogger ( __name__       )
# =============================================================================
_wrappers_ = {} 
# =============================================================================
class _WO_ (object)  :
    def __init__ ( self , o              ) :        self._o   =  o 
    def __call__ ( self , x , pars  = [] ) : return self._o ( x [0] )
    
# =============================================================================
## convert the model into TF1
def _tf1_ ( self , *args ) :
    """
    Convert the function to TF1
    
    >>> obj = ...
    
    >>> fun = obj.tf1 ( 3.0 , 3.2 )
    
    >>> fun.Draw() 
    """
    key = funID ()
    #
    wo  = _WO_ ( self )
    #
    fun = ROOT.TF1 ( key , wo , *args )
    fun.SetNpx ( 500 ) 
    #
    _wrappers_ [ key ] = wo,fun 
    #
    return fun 

# =============================================================================
## get the regular complex value for amplitude 
def _amp_ ( self , x ) :
    """
    Get the complex value for amplitude
    
    >>> fun
    >>> a = fun.amp ( x )
    
    """
    v = self.amplitude ( x )
    #
    return complex( v.real () , v.imag () ) 

Gaudi = cpp.Gaudi 
for model in ( Gaudi.Math.Chebyshev              ,
               Gaudi.Math.Legendre               ,
               Gaudi.Math.Hermite                ,
               Gaudi.Math.Bernstein              ,
               Gaudi.Math.Positive               ,
               Gaudi.Math.BifurcatedGauss        ,
               Gaudi.Math.Bukin                  ,
               Gaudi.Math.Novosibirsk            ,
               Gaudi.Math.CrystalBall            ,
               Gaudi.Math.Needham                ,
               Gaudi.Math.CrystalBallDoubleSided ,
               Gaudi.Math.GramCharlierA          ,
               Gaudi.Math.PhaseSpace2            ,
               Gaudi.Math.PhaseSpaceLeft         ,
               Gaudi.Math.PhaseSpaceRight        ,
               Gaudi.Math.PhaseSpaceNL           ,
               Gaudi.Math.PhaseSpace23L          ,
               Gaudi.Math.BreitWigner            ,
               Gaudi.Math.Rho0                   ,
               Gaudi.Math.Rho0FromEtaPrime       ,
               Gaudi.Math.Flatte                 ,
               Gaudi.Math.Flatte2                ,
               Gaudi.Math.LASS                   ,
               Gaudi.Math.LASS23L                ,
               Gaudi.Math.Bugg23L                ,
               Gaudi.Math.BW23L                  ,
               Gaudi.Math.Flatte23L              ,
               Gaudi.Math.Gounaris23L            ,
               Gaudi.Math.StudentT               ,
               Gaudi.Math.BifurcatedStudentT     ,
               Gaudi.Math.Voigt                  ,
               Gaudi.Math.Spline                 ,
               Gaudi.Math.Linear                 ,
               Gaudi.Math.Parabola               ,
               #
               Gaudi.Math.GenGaussV1             ,
               Gaudi.Math.GenGaussV2             ,
               Gaudi.Math.SkewGauss              ,
               Gaudi.Math.GammaDist              ,
               Gaudi.Math.GenGammaDist           ,
               Gaudi.Math.Amoroso                ,
               Gaudi.Math.LogGammaDist           ,
               Gaudi.Math.Log10GammaDist         ,
               Gaudi.Math.LogGamma               ,
               Gaudi.Math.BetaPrime              ,
               Gaudi.Math.Landau                 ,
               #
               Gaudi.Math.BSpline                , 
               Gaudi.Math.PositiveSpline         ,
               Gaudi.Math.IncreasingSpline       ,
               Gaudi.Math.DecreasingSpline       ,
               #
               ) :
    model . tf1 = _tf1_ 

Gaudi.Math.LASS        . amp = _amp_
Gaudi.Math.LASS23L     . amp = _amp_
Gaudi.Math.Bugg23L     . amp = _amp_
Gaudi.Math.Flatte      . amp = _amp_
Gaudi.Math.Flatte2     . amp = _amp_
Gaudi.Math.Flatte23L   . amp = _amp_
Gaudi.Math.BreitWigner . amp = _amp_

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
