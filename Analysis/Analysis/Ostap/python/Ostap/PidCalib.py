#!/usr/bin/env python
# -*- coding: utf-8 -*-
# =============================================================================
# $Id:$
# =============================================================================
## @file  Ostap/PidCalib.py
#  Oversimplified script to run PIDCalib machinery from Urania project with Ostap
#
#  One needs to specify a function with the interface required nby PIDCalib (see ex_fun).
#
#  And user scrips looks as:
#  @code
#
#  def my_func ( .... ) : .....
#
#  from Ostap.PidCalib import * 
#  parser = makeParser ( )
#  config = parser.parse_args()
#  histos = runPidCalib ( my_func , config)
#  saveHistos ( histos , '' , config ) ## optionally
#
#  @endcode
# 
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2014-05-10
# 
#  @attention Run-time dependency on Urania is required! 
#  
#                    $Revision: 174877 $
#  Last modification $Date: 2014-07-13 14:11:41 +0200 (Sun, 13 Jul 2014) $
#  by                $Author: ibelyaev $
# =============================================================================
""" Oversimplified module to run PIDCalib machinery form Urania project

One needs to specify a function with interface rewuired by PIDClba (see ex_func).

And user scrips looks as:

from Ostap.PidCalib import * 
parser = makeParser ( )
config = parser.parse_args()
histos = runPidCalib ( my_func , config  )
saveHistos ( histos , '' , config ) ## optionally

"""
# =============================================================================
__version__ = "$Revision: 175050 $"
__author__  = "Vanya BELYAEV Ivan.Belyaev@itep.ru"
__date__    = "2011-06-07"
__all__     = (
    'makeParser'  , ## make  a parser 
    'runPidCalib' , ## run pid-calib machinery 
    'saveHistos'  , ## save the histogram historgams
    'ex_func'     , ## an example of end-user function 
    )
# =============================================================================
from   AnalysisPython.Logger import getLogger
if '__main__' == __name__ : logger = getLogger ( 'Ostap.PidCalib' )
else                      : logger = getLogger ( __name__         )
# =============================================================================
## prepare the parser
#  oversimplified version of parser from MakePerfHistsRunRange.py script 
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2014-07-19
def makeParser ( ) :
    """
    Prepare the parser
    - oversimplified version of parser from MakePerfHistsRunRange.py script 
    """
    import argparse, os, sys
    for v in ( 'CALIBDATASCRIPTSROOT' ,
               'PIDPERFTOOLSROOT'     ,
               'PIDPERFSCRIPTSROOT'   ) :
        if not os.environ.has_key ( v ) :
            logger.warning ('No variable %s is defined. Check Urania environment' % v )
            
    parser = argparse.ArgumentParser (
        formatter_class = argparse.RawDescriptionHelpFormatter,
        prog            = os.path.basename(sys.argv[0]),
        description     = """Make performance histograms for a given:
        a) stripping version <stripVersion> (e.g. \"20\")
        b) magnet polarity   <magPol>       (\"MagUp\" or \"MagDown\")
        c) particle type <partName>         (\"K\", \"P\", \"Pi\", \"e\" or \"Mu\")
        """ , 
        epilog =
        """To use the 'MuonUnBiased' hadron samples for muon misID studies, one of the
        following tracks types should be used instead: \"K_MuonUnBiased\", \"Pi_MuonUnBiased\"
        or \"P_MuonUnBiased\"."""
        )
    
    ## add the positional arguments
    parser.add_argument ( 'StripVersion'          ,
                          metavar = '<stripVersion>', type=str   , 
                          help    = "Sets the stripping version" )
    parser.add_argument ( 'MagPol'                ,
                          metavar = '<magPol>'      , type=str   ,
                          choices = ( 'MagUp', 'MagDown' )       , 
                          help    = "Sets the magnet polarity"   )
    
    parser.add_argument ( 'PartName'    ,
                          metavar = '<partName>', type=str      ,
                          choices = ("K", "Pi", "P", "e", "Mu") , 
                          help    = "Sets the particle type"    )
    
    ## add the optional arguments
    parser.add_argument ( '-x'   , '--minRun', default = 0 , 
                          dest = "RunMin" , metavar="NUM", type=int, 
                          help = "Sets the minimum run number to process (if applicable)")
    parser.add_argument ( '-y', '--maxRun', 
                          dest="RunMax"   , metavar="NUM", type=int, default = -1 ,
                          help="Sets the maximum run number to process (if applicable)")
    parser.add_argument ( '-f', '--maxFiles',
                          dest="MaxFiles" , metavar="NUM", type=int, default=-1 , 
                          help="Sets the maximum number of calibration files to run over")
    parser.add_argument ( '-c', '--cuts', dest='cuts', metavar='CUTS', default='',
                          help=("Sets the list of cuts to apply to the calibration "
                                "sample(s) prior to determine the PID efficiencies "
                                "(default: (default)s). "
                                "NB. It is up to the user to ensure that their reference "
                                "sample has the same cuts applied."
                                ))
    
    parser.add_argument ( "-o", "--outputDir", dest="outputDir", metavar="DIR",
                          type=str , default = '.' , 
                          help="Save the performance histograms to directory DIR "
                          "(default: current directory)")
    
    addGroup = parser.add_argument_group("further options")
    addGroup.add_argument ("-q", "--quiet", dest="verbose", action="store_false",
                           default=True,
                           help="Suppresses the printing of verbose information")
    
    addGroup.add_argument ("-M", "--allow-missing", dest="allowMissing",
                           action="store_true", default=False,
                           help="Allow missing calibration subsamples. "
                           "N.B. You should only use this option if requested to "
                           "do so by the PIDCalib authors")
    
    
    return parser 

# ====================================================================
## the basic function:
#  oversimplified version of MakePerfHistsRunRange.py script from Urania/PIDCalib
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2014-07-19
def runPidCalib ( the_func , config ) :
    """
    The basic function:
    - oversimplified version of MakePerfHistsRunRange.py script from Urania/PIDCalib 
    """
    #
    ## perform some arguments check
    #
    
    ## 1) check the stripping version
    from PIDPerfScripts.DataFuncs import CheckStripVer 
    CheckStripVer ( config.StripVersion)
    
    ## 2) set the magnet polarity  [not-needed, since embedded into parser]
    from PIDPerfScripts.DataFuncs import CheckMagPol
    CheckMagPol   ( config.MagPol )
    
    ## 3) set the particle name [not-needed, since embedded into parser]
    from PIDPerfScripts.DataFuncs import CheckPartType
    CheckPartType ( config.PartName )
    
    ## 4) check range names 
    if   config.RunMin and 0 < config.RunMin and config.RunMax and config.RunMin < config.RunMax    : pass
    elif config.RunMin and 0 < config.RunMin and  ( -1 == config.MaxFiles  or 0 < config.MaxFiles ) : pass
    else : logger.warning('Suspicios setting of Run-Ranges ')

    #
    ## finally call the standard PIDCalib machinery with user-specified function
    #
    
    runMax = config.RunMax if  0 < config.RunMax else None 
    
    from PIDPerfScripts.DataFuncs import GetPerfPlotList 
    histopair = GetPerfPlotList ( the_func            , ## Use the function! 
                                  config.StripVersion ,
                                  config.MagPol       ,
                                  config.PartName     ,
                                  None                , ## PID-cuts...
                                  config.cuts         , ## Track-cuts 
                                  None                , ## Binninng schemes   
                                  str(config.RunMin)  ,
                                  runMax              ,
                                  config.verbose      ,
                                  config.allowMissing ,
                                  config.MaxFiles     )
    
    return histopair 

# =============================================================================
## save histograms into the output file
#  @code
# 
#  histos = runPidCalib ( .... )
#  saveHistos ( histos , '' , config  )
# 
#  @endcode 
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2014-07-19
def saveHistos  (  histos , fname = '' , config = None ) :
    """Save histograms into the output file """
    ##
    if   not fname and not config : fname = "PIDHistos.root"
    elif not fname and     config :
        fname = "PIDHistos_{part}_Strip{strp}_{pol}.root".format(
            part = config.PartName     ,
            strp = config.StripVersion ,
            pol  = config.MagPol       )
        
    if config and config.outputDir :
        fname = "%s/%s" % ( config.outputDir , fname )
        
    logger.info ( "Saving performance histograms to %s" %fname )

    import ROOT 
    import Ostap.PyRoUts
    
    with ROOT.TFile.Open ( fname, "RECREATE") as outfile :

        ## save all histograms 
        for h in histos : h.Write() 
        outfile.ls() 
        
    return histos

# =============================================================================
## the example of the actual function that builds the histos
#
#  In this example it builds two histograms:
#  - accepted events
#  - rejected events
#  the efficiency historgam can be later build in Ostap as :
#  @code
#
#  eff = 1/(1 + h_rej/h_acc)
# 
#  @endcode
#  For dataset structure and variable names see
#  @see https://twiki.cern.ch/twiki/bin/view/LHCb/PIDCalibPackage
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2014-07-19
def  ex_func ( PartName             ,
               DataSet              ,
               DLLCutList    = None , ## not used 
               BinningScheme = None , ## not needed 
               Plots         = None , 
               verbose       = True ) :
    """The example of the actual function that build histos
    
    In this example it builds two histograms:
    - accepted events
    - rejected events
    
    For dataset structure and variable names see:
    https://twiki.cern.ch/twiki/bin/view/LHCb/PIDCalibPackage
    
    The efficiency historgam can be later build in Ostap as :
    
    >>> h_acc = ...
    >>> h_rej = ...
    >>> eff = 1/(1 + h_rej/h_acc)
    """
    
    ## we need ROOT and Ostap machinery!
    
    import ROOT
    from Ostap.PyRoUts import hID 
    
    # 1) define PID-cut and its negation  
    #    For dataset structure and variable names see:
    #    https://twiki.cern.ch/twiki/bin/view/LHCb/PIDCalibPackage
    accepted = 'Pi_ProbNNpi>0.5' ## note variable names 
    rejected = 'Pi_ProbNNpi<0.5' ## note variable names 
    
    # 2) prepare the histogtrams 
    h1 = ROOT.TH2D ( hID () , 'Accepted(%s)'% accepted , 15 , 0 , 150000 , 10 , 2 , 5 ) ; h1.Sumw2()
    h2 = ROOT.TH2D ( hID () , 'Rejected(%s)'% rejected , 15 , 0 , 150000 , 10 , 2 , 5 ) ; h2.Sumw2()

    # 3) fill the historgams with 'accepted' and 'rejected' events
    #    For dataset structure and variable names see:
    #    https://twiki.cern.ch/twiki/bin/view/LHCb/PIDCalibPackage
    
    vlst = ROOT.RooArgList ()
    vlst.add ( DataSet.Pi_P   ) ## note variable names 
    vlst.add ( DataSet.Pi_Eta ) ## note variable name 
    
    h1 = DataSet.fillHistogram ( h1 , vlst , accepted ) ## fill histo 
    h2 = DataSet.fillHistogram ( h2 , vlst , rejected ) ## fill histo

    #
    ## and now update the output 
    #
    
    if not Plots :
        Plots     = [ h1 , h2 ] ## "Accepted" & "Rejected" historgams 
    else         :
        Plots[0] += h1          ## "Accepted" histogram 
        Plots[1] += h2          ## "Rejected" histogram 
        
    if verbose :
        logger.info ( 'Accepted histo: %s' % Plots[0].stat() )
        logger.info ( 'Rejected histo: %s' % Plots[1].stat() )
            
    return Plots


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
## The END 
# =============================================================================
    

