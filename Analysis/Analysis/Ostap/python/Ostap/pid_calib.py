#!/usr/bin/env python
# -*- coding: utf-8 -*-
# =============================================================================
# $Id:$
# =============================================================================
## @file
#  An example of simple script to run PIDCalib machinery
#  Use '-h' option to know more
#
#  @code
#  pid_calib.py Pi -s 20 -p MagUp -c 'Pi_hasRich==1' 
#  @endocode
#
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2014-05-10
#  
#                    $Revision: 174877 $
#  Last modification $Date: 2014-07-13 14:11:41 +0200 (Sun, 13 Jul 2014) $
#  by                $Author: ibelyaev $
# =============================================================================
""" An example of simple script to run PIDCalib machinery

    pid_calib.py Pi -s 20 -p MagUp -c 'Pi_hasRich==1'

    Use '-h' option to know more 
"""
# =============================================================================
__version__ = "$Revision: $"
__author__  = "Vanya BELYAEV Ivan.Belyaev@itep.ru"
__date__    = "2011-06-07"
__all__     = ()
# =============================================================================
import ROOT, cppyy
ROOT.PyConfig.IgnoreCommandLineOptions = True
# =============================================================================
from   AnalysisPython.Logger import getLogger
if '__main__' == __name__ : logger = getLogger ( 'pid_calib' )
else                      : logger = getLogger ( __name__    )
# =============================================================================


# =============================================================================
## the actual function to fill PIDcalib histograms 
#  - it books two histogram  (3D in this case)
#  - it fill them with 'accepted' and 'rejected' events (3D in this case)
#  - update input historgams
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2014-05-10
def  PION ( particle         , 
            dataset          ,
            plots    = None  ,
            verbose  = False ) :
    """
    The actual function to fill PIDCalib histograms
    - it books two histogram  (3D in this case)
    - it fill them with 'accepted' and 'rejected' events (3D in this case)
    - update input historgams
    """
    #
    ## we need here ROOT and Ostap machinery!
    #
    import ROOT
    from Ostap.PyRoUts import hID, h3_axes  
    
    #
    ## the heart of the whole game: 
    # 
    accepted = 'Pi_V2ProbNNpi>0.1'  ## ACCEPTED sample 
    rejected = 'Pi_V2ProbNNpi<0.1'  ## REJECTED sample 

    if verbose :        
        logger.info ( "ACCEPTED: %s" % accepted ) 
        logger.info ( "REJECTED: %s" % rejected ) 

    #
    ## book histograms:
    # 
    vlst = ROOT.RooArgList ()
    vlst.add ( dataset.Pi_P    ) ## VARIABLE 
    vlst.add ( dataset.Pi_Eta  ) ## VARIABLE 
    vlst.add ( dataset.nTracks ) ## VARIABLE 
    #
    ## binning
    #
    ## binning in P 
    pbins    = [ 3.2  , 6  , 9  ,  15 , 20  , 30  , 40 , 50 , 60 , 80 , 100 , 120 , 150 ]
    pbins    = [ p*1000 for p in pbins ]

    ## binning in ETA 
    hbins    = [ 2.0 , 2.5 , 3.0 , 3.5 , 4.0 , 4.5 , 4.9 ]
    
    ## binning in #tracks 
    tbins    = [0, 150 , 250 , 400 , 1000]

    #
    ## book histograms
    #
    ha       = h3_axes ( pbins , hbins , tbins , title = 'Accepted(%s)' % accepted ) 
    hr       = h3_axes ( pbins , hbins , tbins , title = 'Rejected(%s)' % rejected )

    #
    ## fill them:
    #
    ha = dataset.fillHistogram ( ha , vlst , accepted )
    hr = dataset.fillHistogram ( hr , vlst , rejected )

    #
    ## prepare the output
    #
    if not plots : ## create outptu plots
        
        ha.SetName ( ha.GetTitle() )
        hr.SetName ( hr.GetTitle() )        
        plots = [ ha , hr ]
        
    else         : ## update output plots
        
        plots [0] += ha
        plots [1] += hr 
        ha.Delete ()
        hr.Delete ()
        if ha : del ha
        if hr : del hr
        
    return plots   ## return plots 


# =============================================================================
## the actual function to fill PIDcalib histograms 
#  - it books two histogram  (3D in this case)
#  - it fill them with 'accepted' and 'rejected' events (3D in this case)
#  - update input historgams
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2014-05-10
def  PROTON ( particle         , 
              dataset          ,
              plots    = None  ,
              verbose  = False ) :
    """
    The actual function to fill PIDCalib histograms
    - it books two histogram  (3D in this case)
    - it fill them with 'accepted' and 'rejected' events (3D in this case)
    - update input historgams
    """
    #
    ## we need here ROOT and Ostap machinery!
    #
    import ROOT
    from Ostap.PyRoUts import hID, h3_axes  
    
    #
    ## the heart of the whole game: 
    # 
    accepted = 'P_V2ProbNNpi>0.1'  ## ACCEPTED sample 
    rejected = 'P_V2ProbNNpi<0.1'  ## REJECTED sample 

    if verbose :        
        logger.info ( "ACCEPTED: %s" % accepted ) 
        logger.info ( "REJECTED: %s" % rejected ) 

    #
    ## book histograms:
    # 
    vlst = ROOT.RooArgList ()
    vlst.add ( dataset.P_P     ) ## VARIABLE 
    vlst.add ( dataset.P_Eta   ) ## VARIABLE 
    vlst.add ( dataset.nTracks ) ## VARIABLE 
    #
    ## binning
    #
    ## binning in P 
    pbins    = [ 10 ,  15 , 20  , 30  , 40 , 50 , 60 , 80 , 100 , 120 , 150 ]
    pbins    = [ p*1000 for p in pbins ]

    ## binning in ETA 
    hbins    = [ 2.0 , 2.5 , 3.0 , 3.5 , 4.0 , 4.5 , 4.9 ]
    
    ## binning in #tracks 
    tbins    = [0, 150 , 250 , 400 , 1000]

    #
    ## book histograms
    #
    ha       = h3_axes ( pbins , hbins , tbins , title = 'Accepted(%s)' % accepted ) 
    hr       = h3_axes ( pbins , hbins , tbins , title = 'Rejected(%s)' % rejected )

    #
    ## fill them:
    #
    ha = dataset.fillHistogram ( ha , vlst , accepted )
    hr = dataset.fillHistogram ( hr , vlst , rejected )

    #
    ## prepare the output
    #
    if not plots : ## create outptu plots
        
        ha.SetName ( ha.GetTitle() )
        hr.SetName ( hr.GetTitle() )        
        plots = [ ha , hr ]
        
    else         : ## update output plots
        
        plots [0] += ha
        plots [1] += hr 
        ha.Delete ()
        hr.Delete ()
        if ha : del ha
        if hr : del hr
        
    return plots   ## return plots 


# =============================================================================
if '__main__' == __name__ :

    logger.info ( 80*'*'   )
    logger.info ( __doc__  )
    logger.info ( 80*'*'   )
    logger.info ( ' Author  : %s' %         __author__    ) 
    logger.info ( ' Version : %s' %         __version__   ) 
    logger.info ( ' Date    : %s' %         __date__      )
    logger.info ( ' Symbols : %s' %  list ( __all__     ) )
    logger.info ( 80*'*'   )

    #
    ## import function from Ostap
    #
    from   Ostap.PidCalib import run_pid_calib

    ## use it!
    run_pid_calib ( PROTON , 'PIDCALIB.db') 

    logger.info ( 80*'*' )

# =============================================================================
# The END 
# =============================================================================
