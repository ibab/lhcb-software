
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
    'lCorrection'    ## L-corretions 
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
    ecorr.Parameters[ "alphaG"] = [ 0      ,     1    ,               1.0          , 1.0         ,  1.0           ]
    ecorr.Parameters["alphaE"]  = [ 0      ,     2    ,               1.0105       , 1.0208      ,  1.0325 ,  0.11561E-06  , 0.33802E-07 , -0.28272E-07   ]
    ecorr.Parameters["alphaB"]  = [ 0      ,     2    ,               1.0159       , 1.0210      ,  1.0256 , -0.67500E-01  ,-0.78525E-01 , -0.8901E-01    ]
    ecorr.Parameters["alphaX"]  = [ 0      ,     2    ,        1,1,1, 0.     , 0.0171 ,  0.0315    ]
    ecorr.Parameters["alphaY"]  = [ 0      ,     2    ,        1,1,1, 0.     , 0.0362 ,  0.0479    ]
    ecorr.Parameters["beta"]    = [ 1      ,     1    ,               8.3    , 8.8    ,  9.5       ]
    ecorr.Parameters["globalC"] = [ 0      ,     1    ,               0.977  , 0.977  ,  0.977     ]
    
    ##
    log.info ('Configure E-Corrections for Ecal hypotheses: %s' % ecorr.name () )
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
    scorr.CorrectionLevel = [  
        True ,    ## Asinh Sshape 
        True ,    ## Residual   asymmetries
        True       ## L/R &  B/U asymmetries 
        ]
    # ============================================================================
    ## Asinh Sshaping
    ## Main Parameter                  Outer   Middle   Inner 
    scorr.Par_Asinh    =  [ 0.102 , 0.129 ,  0.144 ] 
    ## Residual Sshape
    scorr.Par_ResOut   =  [ -0.47496E-03,0.44911,-0.34187E-01,-9.0335, 0.46190,53.041,-1.2801     ,-97.865 ]
    scorr.Par_ResMid   =  [ -0.97712E-05,0.33898, 0.14566E-01,-5.9958,-0.13533,32.711, 0.33460E-01,-58.743 ]
    scorr.Par_ResInn   =  [ -0.14401E-03,0.29037, 0.28848E-01,-5.1697,-0.34664,26.885, 0.99928    ,-45.032 ]
    ## L/R & B/U Asymmetries
    scorr.Par_AsOut    =  [ 0.24742E-01,0.11487E-01,-0.32640,-0.29386E-01,0.11472E-01,0.35928 ] 
    scorr.Par_AsMid    =  [ 0.28587E-01,0.11557E-01,-0.31548,-0.28587E-01,0.11557E-01,0.31548 ]
    scorr.Par_AsInn    =  [ 0.19870E-01,0.38043E-02,-0.21436,-0.19870E-01,0.38043E-02,0.21436 ] 
    ##
    log.info ('Configure S-Corrections for Ecal hypotheses: %s' % scorr.name () ) 
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
    lcorr.Par_Al1 = [ 17.936 , 19.053 , 22.305 ]
    lcorr.Par_Al2 = [  1.15  ,  1.53  ,  2.04  ]
    lcorr.Par_Al3 = [ 0.0392 , 0.455  , 0.0281 ]
    lcorr.Par_Be1 = [ 54.54  , 60.87  , 58.04  ]
    lcorr.Par_Be2 = [  4.02  ,  4.15  ,  4.38  ]
    lcorr.Par_Be3 = [ 0.0308 , 0.0597 , 0.0247 ]
    lcorr.Par_z0  = [ 12566. ]
    ##
    log.info ('Configure L-Corrections for Ecal hypotheses: %s' % lcorr.name () ) 
    return lcorr

# =============================================================================
if '__main__' == __name__ :
    print __doc__ 
    print __version__ 

# =============================================================================
# The END 
# =============================================================================
