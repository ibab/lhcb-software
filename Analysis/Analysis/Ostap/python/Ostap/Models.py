#!/usr/bin/env python
# -*- coding: utf-8 -*-
# =============================================================================
# $Id$
# =============================================================================
## @file Models.py
#
#  Module with some useful utilities for simple functions and fit models.
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
# helper adapter for 1D-functions 
class _WO1_ (object)  :
    "Helper adapter for 1D-functions"
    def __init__ ( self , o              ) :        self._o   =  o 
    def __call__ ( self , x , pars  = [] ) : return self._o ( x [0]        )
# helper adapter for 1D-functions 
class _WO2_ (object)  :
    "Helper adapter for 2D-functions"
    def __init__ ( self , o              ) :        self._o   =  o 
    def __call__ ( self , x , pars  = [] ) : return self._o ( x [0] , x[1] )

# =============================================================================
## convert the model into TF1
def _tf1_ ( self , *args ) :
    """
    Convert the function to TF1
    
    >>> obj = ...
    
    >>> fun = obj.tf1 ( 3.0 , 3.2 )
    
    >>> fun.Draw() 
    """
    #
    if not hasattr ( self , '_wo1' ) : self._wo1 = _WO1_ ( self )
    if not self._wo1                 : self._wo1 = _WO1_ ( self )
    ## 
    _wo = self._wo1 
    fun = ROOT.TF1 ( funID()  , _wo , *args )
    fun.SetNpx ( 500 ) 
    #
    return fun 

# =============================================================================
## convert the model into TF2
def _tf2_ ( self , *args ) :
    """
    Convert the function to TF2
    
    >>> obj = ...
    
    >>> fun = obj.tf2 ( 3.0 , 3.2 , 3.0 , 3.2 )
    
    >>> fun.Draw() 
    """
    ##
    if not hasattr ( self , '_wo2' ) : self._wo2 = _WO2_ ( self )
    if not self._wo2                 : self._wo2 = _WO2_ ( self )
    ## 
    _wo = self._wo2
    fun = ROOT.TF2 ( funID ()  , _wo , *args )
    fun.SetNpx ( 100 ) 
    fun.SetNpy ( 100 ) 
    #
    return fun 

# =============================================================================
## draw the function 
def _f1_draw_ ( self , *opts ) :
    """
    Drawing the function object through conversion to ROOT.TF1
    
    >>> fun = ...
    >>> fun.draw()
    
    """
    if not hasattr ( self , '_tf1'  ) :
        
        if hasattr ( self , 'xmin' ) and hasattr ( self , 'xmax' ) :
            
            if hasattr ( self , 'npars' ) :
                
                self._tf1  = _tf1_ ( self          ,
                                     self.xmin  () ,
                                     self.xmax  () ,
                                     self.npars () )
                for i in range ( 0, self.npars() ) :
                    self._tf1.SetParameter ( i , self.par ( i ) )
                    
            else : self._tf1  = _tf1_ ( self , self.xmin() , self.xmax() )
            
            if type(self) in ( Gaudi.Math.Positive          ,
                               Gaudi.Math.Monothonic        , 
                               Gaudi.Math.Convex            , 
                               Gaudi.Math.ConvexOnly        , 
                               Gaudi.Math.PositiveSpline    , 
                               Gaudi.Math.MonothonicSpline  , 
                               Gaudi.Math.ConvexSpline      ,
                               Gaudi.Math.ConvexOnlySpline  ,
                               Gaudi.Math.ExpoPositive      ,
                               Gaudi.Math.TwoExpoPositive   ) :                                
                self._tf1.SetMinimum(0)
            
    return self._tf1.Draw ( *opts )
    
    
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

Gaudi.Math.LASS        . amp = _amp_
Gaudi.Math.LASS23L     . amp = _amp_
Gaudi.Math.Bugg23L     . amp = _amp_
Gaudi.Math.Flatte      . amp = _amp_
Gaudi.Math.Flatte2     . amp = _amp_
Gaudi.Math.Flatte23L   . amp = _amp_
Gaudi.Math.BreitWigner . amp = _amp_

# =============================================================================
## make 1D-integration using SciPy
#  @see http://www.scipy.org/
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date 2014-12-01
def sp_integrate_1D ( func , xmin , xmax , *args , **kwargs ) :
    """
    Make 1D-integration using SciPy
    
    >>> func = ...
    >>> print func.sp_integrate ( -10 , 10 )
    
    """    
    from scipy import integrate
    ##
    result = integrate.quad ( func , xmin , xmax , *args , **kwargs )
    return result[0]

# =============================================================================
## make 2D-integration using SciPy
#  @see http://www.scipy.org/
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date 2014-12-01
def sp_integrate_2D ( func  ,
                      xmin  , xmax ,
                      ymin  , ymax , *args , **kwargs ) :
    """
    Make 2D-integration using SciPy

    >>> func = ...  ## func ( x , y )
    ##                            xmin , xmax , ymin , ymax 
    >>> print func.sp_integrate ( -10  , 10   , -20  , 20   ) 
    
    Note different naming with respect to SciPy:
    - SciPy first integrates over 2nd variable
    """
    from scipy import integrate
    ##
    result = integrate.dblquad ( func ,
                                 ymin ,
                                 ymax ,
                                 lambda x : xmin ,
                                 lambda x : xmax , 
                                 *args , **kwargs )
    return result[0]

# =============================================================================
## make 1D-integration using SciPy
#  @see http://www.scipy.org/
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date 2014-12-01
def sp_integrate_1D_ ( pdf , xmin , xmax , *args , **kwargs ) :
    """
    Make 1D integration over the PDF using SciPy
    """
    if hasattr ( pdf , 'setPars' ) : pdf.setPars() 
    func = pdf.function()
    return func.sp_integrate_1D ( xmin , xmax , *args , **kwargs ) 

# =============================================================================
## make 2D-integration using SciPy
#  @see http://www.scipy.org/
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date 2014-12-01
def sp_integrate_2D_ ( pdf   ,
                      xmin  , xmax ,
                      ymin  , ymax , *args , **kwargs ) :
    """    
    Make 3D integration over the PDF using SciPy
    """
    if hasattr ( pdf , 'setPars' ) : pdf.setPars() 
    func = pdf.function()
    return func.sp_integrate_2D ( xmin , xmax , ymin , ymax , *args , **kwargs ) 


from LHCbMath.deriv import moment   as sp_moment
from LHCbMath.deriv import mean     as sp_mean
from LHCbMath.deriv import variance as sp_variance
from LHCbMath.deriv import rms      as sp_rms 
from LHCbMath.deriv import mediane  as sp_mediane
from LHCbMath.deriv import quantile as sp_quantile
from LHCbMath.deriv import mode     as sp_mode 

# =============================================================================
## decorate 1D-models/functions 
# =============================================================================
Gaudi = cpp.Gaudi 
for model in ( Gaudi.Math.Chebyshev              ,
               Gaudi.Math.ChebyshevU             ,
               Gaudi.Math.Legendre               ,
               Gaudi.Math.Hermite                ,
               Gaudi.Math.Bernstein              ,
               Gaudi.Math.ChebyshevSum           ,
               Gaudi.Math.LegendreSum            ,
               Gaudi.Math.HermiteSum             ,
               Gaudi.Math.FourierSum             ,
               Gaudi.Math.CosineSum              ,
               Gaudi.Math.Polynomial             ,               
               Gaudi.Math.Positive               ,
               Gaudi.Math.Monothonic             ,
               Gaudi.Math.Convex                 ,
               Gaudi.Math.ConvexOnly             ,
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
               Gaudi.Math.JohnsonSU              ,
               Gaudi.Math.Atlas                  ,
               Gaudi.Math.Argus                  ,
               Gaudi.Math.Tsallis                ,
               Gaudi.Math.QGSM                   ,
               Gaudi.Math.TwoExpos               ,
               Gaudi.Math.Sigmoid                ,
               #
               Gaudi.Math.BSpline                , 
               Gaudi.Math.PositiveSpline         ,
               Gaudi.Math.MonothonicSpline       ,
               Gaudi.Math.ConvexSpline           ,
               Gaudi.Math.ConvexOnlySpline       ,
               #
               ) :
    model . tf1 = _tf1_ 
    model.sp_integrate = sp_integrate_1D
    
    if not hasattr ( model , 'mean'     ) : model.mean     = sp_mean 
    if not hasattr ( model , 'variance' ) : model.variance = sp_variance 
    if not hasattr ( model , 'rms'      ) : model.rms      = sp_rms  
    if not hasattr ( model , 'mediane'  ) : model.mediane  = sp_mediane
    if not hasattr ( model , 'mode'     ) : model.mode     = sp_mode 
    if not hasattr ( model , 'moment'   ) : model.moment   = sp_moment
    if not hasattr ( model , 'quantile' ) : model.quantile = sp_quantile 
        
## add some drawing method for some shapes 
for model in ( Gaudi.Math.Bernstein         ,
               Gaudi.Math.Positive          ,
               Gaudi.Math.Monothonic        ,
               Gaudi.Math.Convex            ,
               Gaudi.Math.ConvexOnly        ,
               Gaudi.Math.ChebyshevSum      ,               
               Gaudi.Math.LegendreSum       ,               
               Gaudi.Math.HermiteSum        ,               
               Gaudi.Math.FourierSum        ,               
               Gaudi.Math.CosineSum         ,               
               Gaudi.Math.Polynomial        ,               
               Gaudi.Math.ExpoPositive      , 
               Gaudi.Math.TwoExpoPositive   , 
               #
               Gaudi.Math.BSpline           ,
               Gaudi.Math.MonothonicSpline  ,
               Gaudi.Math.PositiveSpline    , 
               Gaudi.Math.ConvexSpline      , 
               Gaudi.Math.ConvexOnlySpline  ) : 
    
    model.draw = _f1_draw_


# =============================================================================
def _f_print_ ( self , typ = '' ) :
    if not typ : typ = str(type(self))
    return '%s(%s,%s,%s)' % ( typ ,  self.pars() , self.xmin() , self.xmax() )

Gaudi.Math.LegendreSum .__str__  = lambda s : _f_print_ ( s , 'LegendreSum'  )
Gaudi.Math.ChebyshevSum.__str__  = lambda s : _f_print_ ( s , 'ChebyshevSum' )
Gaudi.Math.Polynomial  .__str__  = lambda s : _f_print_ ( s , 'Polynomial'   )
Gaudi.Math.Bernstein   .__str__  = lambda s : _f_print_ ( s , 'Bernstein'    )
Gaudi.Math.Positive    .__str__  = lambda s : _f_print_ ( s , 'Positive'     )
Gaudi.Math.FourierSum  .__str__  = lambda s : _f_print_ ( s , 'FourierSum'   )
Gaudi.Math.CosineSum   .__str__  = lambda s : _f_print_ ( s , 'CosineSum'    )

Gaudi.Math.LegendreSum .__repr__ = lambda s : _f_print_ ( s , 'LegendreSum'  )
Gaudi.Math.ChebyshevSum.__repr__ = lambda s : _f_print_ ( s , 'ChebyshevSum' )
Gaudi.Math.HermiteSum  .__repr__ = lambda s : _f_print_ ( s , 'HermiteSum'   )
Gaudi.Math.Polynomial  .__repr__ = lambda s : _f_print_ ( s , 'Polynomial'   )
Gaudi.Math.Bernstein   .__repr__ = lambda s : _f_print_ ( s , 'Bernstein'    )
Gaudi.Math.Positive    .__repr__ = lambda s : _f_print_ ( s , 'Positive'     )
Gaudi.Math.Convex      .__repr__ = lambda s : _f_print_ ( s , 'Convex'       )
Gaudi.Math.ConvexOnly  .__repr__ = lambda s : _f_print_ ( s , 'ConvexOnly'   )
Gaudi.Math.Monothonic  .__repr__ = lambda s : _f_print_ ( s , 'Monothonic'   )
Gaudi.Math.FourierSum  .__repr__ = lambda s : _f_print_ ( s , 'FourierSum'   )
Gaudi.Math.CosineSum   .__repr__ = lambda s : _f_print_ ( s , 'CosineSum'    )


# =============================================================================
## decorate 2D-models/functions 
# =============================================================================
Gaudi = cpp.Gaudi 
for model in ( Gaudi.Math.Spline2D       ,
               Gaudi.Math.Spline2DSym    , 
               Gaudi.Math.Bernstein2D    ,
               Gaudi.Math.Positive2D     ,
               Gaudi.Math.Bernstein2DSym ,
               Gaudi.Math.Positive2DSym  ,
               Gaudi.Math.PS2DPol        ,
               Gaudi.Math.PS2DPolSym     ,
               Gaudi.Math.ExpoPS2DPol    ,
               Gaudi.Math.Expo2DPol      ,
               Gaudi.Math.Expo2DPolSym   ) :
    
    model . tf2 = _tf2_ 
    model.sp_integrate = sp_integrate_2D

Analysis = cpp.Analysis
for pdf in ( Analysis.Models.BreitWigner          , 
             Analysis.Models.Rho0               ,
             Analysis.Models.Kstar              ,
             Analysis.Models.Phi                , 
             Analysis.Models.Flatte             ,
             Analysis.Models.Flatte2            ,  
             Analysis.Models.Bukin              ,
             Analysis.Models.PhaseSpace2        ,
             Analysis.Models.PhaseSpaceNL       ,
             Analysis.Models.PhaseSpace23L      ,
             Analysis.Models.PhaseSpaceLeft     ,
             Analysis.Models.PhaseSpaceRight    ,
             Analysis.Models.PhaseSpacePol      ,
             Analysis.Models.Needham            ,
             Analysis.Models.CrystalBall        ,
             Analysis.Models.CrystalBallRS      ,
             Analysis.Models.CrystalBallDS      , 
             Analysis.Models.Apolonios          ,
             Analysis.Models.Apolonios2         , 
             Analysis.Models.GramCharlierA      , 
             Analysis.Models.Voigt              ,
             Analysis.Models.LASS               ,
             Analysis.Models.Bugg               ,
             Analysis.Models.LASS23L            ,
             Analysis.Models.Bugg23L            , 
             Analysis.Models.BW23L              , 
             Analysis.Models.PolyPositive       ,
             Analysis.Models.ExpoPositive       ,
             Analysis.Models.TwoExpoPositive    ,
             Analysis.Models.PositiveSpline     ,
             Analysis.Models.MonothonicSpline   , 
             
             Analysis.Models.StudentT           ,
             Analysis.Models.BifurcatedStudentT , 
             Analysis.Models.GammaDist          , 
             Analysis.Models.GenGammaDist       , 
             Analysis.Models.Amoroso            ,
             Analysis.Models.LogGammaDist       ,
             Analysis.Models.Log10GammaDist     ,
             Analysis.Models.LogGamma           ,
             Analysis.Models.BetaPrime          ,
             Analysis.Models.Landau             ,
             Analysis.Models.SinhAsinh          , 
             Analysis.Models.JohnsonSU          ,
             Analysis.Models.Atlas              ,
             Analysis.Models.Argus              ,
             Analysis.Models.Tsallis            ,
             Analysis.Models.QGSM               ,
             Analysis.Models.BifurcatedGauss    ,
             Analysis.Models.GenGaussV1         , 
             Analysis.Models.GenGaussV2         , 
             Analysis.Models.SkewGauss          ) :

    pdf.sp_integrate = sp_integrate_1D_

for pdf in ( Analysis.Models.Poly2DPositive     ,
             Analysis.Models.Poly2DSymPositive  , 
             Analysis.Models.PS2DPol            ,
             Analysis.Models.PS2DPolSym         , 
             Analysis.Models.ExpoPS2DPol        , 
             Analysis.Models.Expo2DPol          ,
             Analysis.Models.Expo2DPolSym       , 
             Analysis.Models.Spline2D           ,
             Analysis.Models.Spline2DSym        ) :
    
    pdf.sp_integrate = sp_integrate_2D_

    
# =============================================================================
## set parameter for polynomial/spline functions
#  @code
#  fun = ...
#  fun[1] = 10.0
#  @endcode 
def _p_set_par_ ( o , index , value ) :
    """Set parameter for polynomial/spline function
    >>> fun = ...
    >>> fun[1] = 10.0
    """
    return o.setPar ( index , value )

# =============================================================================
## get parameter from polynomial/spline functions
#  @code
#  fun = ...
#  print fun[1]
#  @endcode 
def _p_get_par_ ( o , index ) :
    """Get parameter from polynomial/spline function
    >>> fun = ...
    >>> print fun[1]
    """
    return o.par ( index )

for f in ( Gaudi.Math.Positive       ,
           Gaudi.Math.Bernstein2D    ,
           Gaudi.Math.Positive2D     ,
           Gaudi.Math.Bernstein2DSym ,
           Gaudi.Math.Positive2DSym  ,
           ##
           Gaudi.Math.BSpline        ,
           Gaudi.Math.PositiveSpline ,
           Gaudi.Math.Spline2D       ,
           Gaudi.Math.Spline2DSym    ,
           ## 
           Gaudi.Math.PolySum        ) :

    f.__setitem__ = _p_set_par_
    f.__getitem__ = _p_get_par_

    
# =============================================================================
## add complex amplitudes 
# =============================================================================
Gaudi.Math.LASS        . amp = _amp_
Gaudi.Math.LASS23L     . amp = _amp_
Gaudi.Math.Bugg23L     . amp = _amp_
Gaudi.Math.Flatte      . amp = _amp_
Gaudi.Math.Flatte2     . amp = _amp_
Gaudi.Math.Flatte23L   . amp = _amp_
Gaudi.Math.BreitWigner . amp = _amp_
    
# =============================================================================


import LHCbMath.deriv as _M
for i in ( _M.Derivative , _M.Integral , _M.IntegralCache ) :
    if not hasattr ( i , 'tf1' ) : i.tf1 = _tf1_
                
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
