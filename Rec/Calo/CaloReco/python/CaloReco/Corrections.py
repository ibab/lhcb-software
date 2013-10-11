
#!/usr/bin/env python
# =============================================================================
# $Id: Corrections.py,v 1.4 2010-05-20 09:47:06 odescham Exp $ 
# =============================================================================
## @file CaloReco/Corrections.py
#  Set of E/S/L-correction parameters
#  @see CaloECorrection
#  @see CaloSCorrection
#  @see CaloLCorrection
#  @todo Move the correction to Conditions Data Base 
# =============================================================================
"""
Set of Calo E/S/L-correction parameters
"""
# =============================================================================
__version__ = "CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.4 $"
# =============================================================================
__all__  = (
    'eCorrection'  , ## E-corretions 
    'sCorrection'  , ## S-corretions 
    'lCorrection'  , ## L-corretions 
    'showerProfile'    ## Transversal shower profile
    )
# =============================================================================
from Gaudi.Configuration import log 
# =============================================================================
## function which sets the E-correction parameters
#  @see CaloECorrection
def eCorrection ( ecorr , version = None ) : 
    """
    Function which sets the E-correction parameters:

    >>> tool = ...
    >>> eCorrection ( tool )
    """    
    # ============================================================================
    #                      function   #params      Outer         Middle       Inner 
    # ============================================================================
    ecorr.Parameters["alphaG"] = [ 0      ,     1    ,               1.0          , 1.0         ,  1.0           ]
    ecorr.Parameters["alphaE"]  = [ 0      ,     2    ,               1.0105       , 1.0208      ,  1.0325 ,  0.11561E-06  , 0.33802E-07 , -0.28272E-07   ]
    ecorr.Parameters["alphaB"]  = [ 0      ,     2    ,               1.0159       , 1.0210      ,  1.0256 , -0.67500E-01  ,-0.78525E-01 , -0.8901E-01    ]
    ecorr.Parameters["alphaX"]  = [ 0      ,     2    ,        1,1,1, 0.     , 0.0171 ,  0.0315    ]
    ecorr.Parameters["alphaY"]  = [ 0      ,     2    ,        1,1,1, 0.     , 0.0362 ,  0.0479    ]
    ecorr.Parameters["beta"]    = [ 1      ,     1    ,               8.3    , 8.8    ,  9.5       ]
    ecorr.Parameters["globalC"] = [ 0      ,     1    ,               0.977  , 0.977  ,  0.977     ]
    
    ##
    log.debug ('Configure E-Corrections for Ecal hypotheses: %s' % ecorr.name () )
    #    ecorr.OutputLevel = 2
    return ecorr

# =============================================================================
## function which sets the S-correction parameters
#  @see CaloSCorrection
def sCorrection ( scorr , version = None ) :
    """
    Function which sets the S-correction parameters:
    
    >>> tool = ...
    >>> sCorrection ( tool )
    """    
    scorr.Parameters["shapeX"] = [ 5        ,   1     ,              0.102      , 0.129       , 0.144] 
    scorr.Parameters["shapeY"] = [ 5        ,   1     ,              0.102      , 0.129       , 0.144] 
    scorr.Parameters["residual"] = [ 1        ,   8     ,
                                     -0.47496E-03, -0.97712E-05, -0.14401E-03,
                                     0.44911    ,  0.33898    ,  0.29037,
                                     -0.34187E-01,  0.14566E-01,  0.28848E-01,
                                     -9.0335     , -5.9958     , -5.1697,
                                     0.46190    , -0.13533    , -0.34664,
                                     53.041      , 32.711      , 26.885,
                                     -1.2801     ,  0.33460E-01,  0.99928,  
                                     -97.865      ,-58.743      ,-45.032]
    scorr.Parameters["asymP"] = [ 1       ,    3     ,
                                  0.24742E-01, 0.28587E-01, 0.19870E-01,
                                  0.11487E-01, 0.11557E-01, 0.38043E-02,
                                  -0.32640    ,-0.31548    ,-0.21436]
    scorr.Parameters["asymM"] = [ 1       ,    3     ,
                                  -0.29386E-01,-0.28587E-01,-0.19870E-01,
                                  0.11472E-01, 0.11557E-01, 0.38043E-02,
                                  0.35928    , 0.31548    , 0.21436]    
    ##
    log.debug ('Configure S-Corrections for Ecal hypotheses: %s' % scorr.name () ) 
    return scorr


# =============================================================================
## function which sets the L-correction parameters
#  @see CaloLCorrection
def lCorrection ( lcorr , version = None ) :
    """
    Function which sets the L-correction parameters:
    
    >>> tool = ...
    >>> lCorrection ( tool )
    """    
    lcorr.Parameters["gamma0"] = [ 1      ,     1    ,     22.305 ,    19.053  ,  17.936 ]
    lcorr.Parameters["gammaP"] = [ 2      ,     2    ,      2.04  ,     1.53   ,   1.15  , -0.0281, -0.455, -0.0392]
    lcorr.Parameters["delta0"] = [ 1      ,     1    ,     49.04  ,     51.87  ,  45.54  ]
    lcorr.Parameters["deltaP"] = [ 2      ,     2    ,      4.38  ,      4.15  ,   4.02  , -0.0247,    -0.0597 ,  -0.0308 ]
    
    ##
    log.debug ('Configure L-Corrections for Ecal hypotheses: %s' % lcorr.name () ) 
    return lcorr



# =============================================================================
## function which provide the Transversal Shower profile (for MergedPi0 reconstruction)
#  @see CaloMergedPi0ALg
def showerProfile ( shape , version = None ) : 
    """
    Function which sets the E-correction parameters:
    """    

    # ============================================================================
    #                      function   #params      Outer         Middle       Inner 
    # ============================================================================
    shape.Parameters["profile"] = [6 , 10 ,  
                                   -0.0060,  0.0464  ,0.0981,
                                   2.4956,  2.0384,  2.2529,
                                   115.0827, 36.5885, 33.8837,
                                   9.8842,  8.0260,  8.0532,
                                   0.0320,  0.0460,  0.0654,
                                   2.0982,  2.3936,  2.2046,
                                   1.0302,  1.0703,  1.1092,
                                   0.0409,  0.1611,  0.1645,
                                   0.0030,  0.0238,  0.0248,
                                   -9.6135, -5.8899, -5.7248]
    shape.Parameters["profileC"] = [6 , 10 ,     
                                   0.0486,  0.0882,  0.1111,
                                   2.7847,  1.7910,  1.7909,
                                   68.4815, 24.4324, 18.0852,
                                   9.0870,  7.4802,  7.1122,
                                   0.0116,  0.0258,  0.0261,
                                   1.2591,  2.7719,  1.3889,
                                   1.0464,  1.1294,  1.1846,
                                   -0.0900, -0.0802, -0.0934,
                                   0.0005,  0.0024,  0.0029,
                                   -12.9098, -9.7273, -8.9966]
    
    
    ##
    log.debug ('Configure showerShape for Ecal hypotheses: %s' % shape.name () )
    #    shape.OutputLevel = 2
    return shape


# =============================================================================
if '__main__' == __name__ :
    print __doc__ 
    print __version__ 

