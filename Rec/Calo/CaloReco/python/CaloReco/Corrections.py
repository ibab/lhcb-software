
#!/usr/bin/env python
# =============================================================================
# $Id: Corrections.py,v 1.3 2010-03-08 01:19:39 odescham Exp $ 
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
__version__ = "CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.3 $"
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
    ecorr.CorrectionLevel = [
        True,    ## alpha_1 ( E3x3)
        True,    ## alpha_2 ( barycenter )
        True,    ## alpha_3 ( Module Frame )
        True,    ## Preshower leakage
        True,    ## Global scaling
        True,    ## Spd / No Spd scaling
        True     ## SplitPhoton scaling
        ]
    # ============================================================================
    #                                    Outer         Middle       Inner 
    # ============================================================================
    ## Energy dependency
    ecorr.Corr1_constant  =     [      1.0105      , 1.0208      ,  1.0325      ]
    ecorr.Corr1_slope     =     [      0.11561E-03 , 0.33802E-04 , -0.28272E-04 ]
    ## Barycenter position dependency
    ecorr.Corr2_constant  =     [      0.10159E+01 , 0.10210E+01 ,  0.10256E+01 ]
    ecorr.Corr2_slope     =     [     -0.67599E-01 ,-0.78525E-01 , -0.89801E-01 ]
    ## Module frame dependency 
    ecorr.Corr3_slopeX    =     [      0.00        , 1.71        , 3.15         ]
    ecorr.Corr3_slopeY    =     [      0.00        , 3.62        , 4.79         ]
    ##  Preshower leakage 
    ecorr.CorrPrs         =     [      8.3         , 8.8         , 9.5          ]
    ## Global rescaling            No Spd   Spd/No Spd    SplitPhotons  Electrons
    ecorr.GlobalFactor    =     [   1.000   , 0.977      , 1.000 ,      0.988   ]
    ##
    log.info ('Configure E-Corrections for Ecal hypotheses: %s' % ecorr.name () )
    
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
