#!/usr/bin/env python
# -*- coding: utf-8 -*-
# =============================================================================
# $Id$
# =============================================================================
## @file  Ostap/PidCalib.py
#  Oversimplified script to run PIDCalib machinery from Urania project with Ostap
#
#  One needs to specify a function with the interface required nby PIDCalib (see ex_fun).
#
#  And user scrips looks as:
#
#  @code
#  def PION ( particle        ,   ## INPUT 
#             daatset         ,   ## INPUT 
#             plots   =  None ,   ## UPDATE 
#             verbose = False ) :
#      ...
#      return plots
#
#  from Ostap.PidCalib import run_pid_calib 
#  run_pid_calib ( PION , 'PIDCALIB.db')
#
#  @endcode
# 
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2014-05-10
# 
#  @attention Run-time dependency on Urania is required! 
#  
#                    $Revision$
#  Last modification $Date$
#  by                $Author$
# =============================================================================
""" Oversimplified module to run PIDCalib machinery form Urania project

One needs to specify a function with interface required by PIDCalib (see ex_func).

And user scrips looks as:

#  def PION ( particle        , ## INPUT 
#             daatset         , ## INPUT 
#             plots   =  None , ## UPDATE 
#             verbose = False ) :
#      ...
#      return plots
#
#  from Ostap.PidCalib import run_pid_calib 
#  run_pid_calib ( PION , 'PIDCALIB.db')  

"""
# =============================================================================
__version__ = "$Revision$"
__author__  = "Vanya BELYAEV Ivan.Belyaev@itep.ru"
__date__    = "2011-06-07"
__all__     = (
    'makeParser'  , ## make  a parser 
    'runPidCalib' , ## run pid-calib machinery 
    'saveHistos'  , ## save the histogram historgams
    'ex_func'     , ## an example of end-user function 
    'ex_func2'    , ## another example of end-user function 
    )
# =============================================================================
import ROOT,cppyy, os
import Ostap.PyRoUts 
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
        a) stripping version <stripVersion> (e.g. '20' )
        b) magnet polarity   <magPol>       ( 'MagUp' or 'MagDown' or 'Both' )
        c) particle type <partName>         ( 'K', 'P' , 'Pi' , 'e' , 'Mu'   )  
        """ , 
        epilog =
        """To use the 'MuonUnBiased' hadron samples for muon misID studies, one of the
        following tracks types should be used instead: \"K_MuonUnBiased\", \"Pi_MuonUnBiased\"
        or \"P_MuonUnBiased\"."""
        )
    
    ## add the positional arguments
    parser.add_argument ( 'particle'    ,
                          metavar = '<PARTICLE>'    , type=str    ,
                          choices = ('K', 'Pi', 'P' , 'e', 'Mu' ,  'P_LcfB' , 'P_TotLc' , 'P_IncLc' ) , 
                          help    = "Sets the particle type"     )
    
    parser.add_argument ( '-s' , '--stripping'       , nargs = '+' , 
                          metavar = '<STRIPPING>'    , ## type=str ,                           
                          help    = "Sets the stripping version"  )
    
    ## add the optional arguments
    parser.add_argument ( '-p' , '--polarity'            , default = 'Both' , 
                          metavar = '<MAGNET>'           , type=str ,
                          choices = ( 'MagUp', 'MagDown' , 'Both' ) , 
                          help    = "Sets the magnet polarity"    )
    
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
                          help="""List of cuts to apply to the calibration sample
                          prior to determine the PID efficiencies, 
                          e.g. fiducuial volume,  HASRICH, etc... 
                          """)
    parser.add_argument ( "-o", "--outputDir", dest="outputDir", metavar="DIR",
                          type=str , default = '.' , 
                          help="Save the performance histograms to directory DIR "
                          "(default: current directory)")
    
    addGroup = parser.add_argument_group("further options")
    addGroup.add_argument ("-q", "--quiet", dest="verbose", action="store_false",
                           default=True,
                           help="Suppresses the printing of verbose information")
    
    return parser 

# =============================================================================
## a bit modified version of DataFuncs.GetDataSet from Urania/PIDCalib/PIDPerfScripts
def getDataSet ( particle          ,
                 stripping         ,
                 polarity          ,
                 trackcuts         ,
                 index             ,
                 verbose    =False ,
                 minEntries =1000  ) :
    
    from PIDPerfScripts.DataFuncs import CheckStripVer, CheckMagPol, CheckPartType  
    CheckStripVer ( stripping )
    CheckMagPol   ( polarity  )
    CheckPartType ( particle  )

    #======================================================================
    # Create dictionary holding:
    # - Reconstruction version    ['RecoVer']
    # - np.array of:
    #        - MagUp run limits   ['UpRuns']
    #        - MagDown run limits ['DownRuns']
    #======================================================================
    from PIDPerfScripts.DataFuncs import GetRunDictionary
    DataDict = GetRunDictionary ( stripping , particle , verbose = verbose )

    #======================================================================
    # Determine Mother and Workspace names
    #======================================================================
    from PIDPerfScripts.DataFuncs import GetDataSetNameDictionary
    DataSetNameDict = GetDataSetNameDictionary( particle )

    from PIDPerfScripts.DataFuncs import GetRealPartType, GetRecoVer
    particle_type = GetRealPartType ( particle  )
    RecoVer       = GetRecoVer      ( stripping )
    #
    ## new stuff:
    #
    StripVer      = stripping
    MagPolarity   = polarity
    PartType      = particle_type 

    fname_protocol = ""
    fname_query    = ""
    fname_extra    = ""
    
    CalibDataProtocol=os.getenv("CALIBDATAURLPROTOCOL")
    CalibDataExtra   =os.getenv("CALIBDATAEXTRA")
    
    # set the URL protocol (if applicable)
    if CalibDataProtocol is not None and CalibDataProtocol!="":
        fname_protocol = "{0}".format(CalibDataProtocol)
        
    if CalibDataExtra is not None and CalibDataExtra!="":
        fname_extra = "{0}".format(CalibDataExtra)

    vname_head = "CALIBDATASTORE" 
    fname_head = os.getenv(vname_head)
    if fname_head is None:
        raise GetEnvError("Cannot retrieve dataset, environmental variable %s has not been set." %vname_head)

    fname = ("{prtcol}/{extra}/{topdir}/Reco{reco}_DATA/{pol}/"
             "{mother}_{part}_{pol}_Strip{strp}_{idx}.root").format(
        prtcol=fname_protocol, extra=fname_extra,topdir=fname_head, reco=RecoVer,
        pol=MagPolarity, mother=DataSetNameDict['MotherName'],
        part=PartType, strp=StripVer, idx=index)

    merged_fname = ("{prtcol}//{extra}//{topdir}/Reco{reco}_DATA/{pol}/"
             "{mother}_{part}_{pol}_Strip{strp}_{idx}.root").format(
        prtcol=fname_protocol, extra=fname_extra,topdir=fname_head, reco=RecoVer,
        pol=MagPolarity, mother=DataSetNameDict['MotherName'],
        part=PartType, strp=StripVer, idx=index)


#    fname = ("{prtcol}{topdir}/{pol}/{part}/"
#             "{mother}_{part}_{pol}_Strip{strp}_{idx}.root{qry}").format(
#         prtcol=fname_protocol, topdir=fname_head,
#         pol=MagPolarity, mother=DataSetNameDict['MotherName'],
#         part=PartType, strp=StripVer, idx=index, qry=fname_query)



    ## if verbose:
    ##   print "Attempting to open file {0} for reading".format(fname)

    ## import ROOT 
    ## f = ROOT.TFile.Open(fname)
    ## if not f:
    ##   f = ROOT.TFile.Open ( merged_fname )

    ## fname_protocol = ""
    ## fname_query    = ""

    ## CalibDataProtocol = os.getenv ( "CALIBDATAURLPROTOCOL" )
    ## CalibDataQuery    = os.getenv ( "CALIBDATAURLQUERY"    )

    ## # set the URL protocol (if applicable)
    ## if CalibDataProtocol : fname_protocol = "{0}://".format(CalibDataProtocol)
    
    ## # set the URL query (if applicable)
    ## if CalibDataQuery    : fname_query    = "?{0}".format(CalibDataQuery)
    
    ## from PIDPerfScripts.DataFuncs import IsMuonUnBiased 
    ## vname_head = "CALIBDATASTORE" if not IsMuonUnBiased( particle ) else "MUONCALIBDATASTORE"
    
    ## fname_head = os.getenv(vname_head)
    ## if not fname_head :
    ##     raise AttributeError ("Environmental variable %s has not been set!" %vname_head )
    
    ## fname = ("{prtcol}{topdir}/Reco{reco}_DATA/{pol}/"
    ##          "{mother}_{part}_{pol}_Strip{strp}_{idx}.root{qry}").format(
    ##     prtcol=fname_protocol, topdir=fname_head, reco=DataDict['RecoVer'],
    ##     pol  = polarity     , mother=DataSetNameDict['MotherName'],
    ##     part = particle_type, strp  =stripping, idx=index, qry=fname_query)

    
    if verbose:
        logger.info( "Attempting to open file {0} for reading".format(fname) )
        
    import ROOT
    import Ostap.PyRoUts
    ## fname = fname
    ## with  ROOT.TFile.Open(fname, 'READ' ) as f :
    ##     if not f : fname = merged_fname 
    fname = merged_fname     
    with  ROOT.TFile.Open(fname, 'READ' ) as f :
        if not f: raise IOError("Failed to open file %s for reading" %fname)
        
        wsname = DataSetNameDict['WorkspaceName']
        ws     = f.Get(wsname)
        if not ws:
            raise AttributeError ( "Uanble to retriever RooFit workspace %s" % wsname )
        
        data = ws.data('data')
        if not data:
            raise AttributeError ( "RooDataSet not found in workspace %s"    % wsname )
        
        dataset = ROOT.RooDataSet(
            'Calib_Data'
            , data.GetTitle() 
            , data
            , data.get()
            , trackcuts        
            , 'nsig_sw'
            )
        
        ws.Delete()
        if ws   : del ws
            
    if verbose:
        logger.info ( "DataSet: %s" % dataset ) 

    #======================================================================
    # Sanity test: do we have a dataset, and is it empty?
    #======================================================================
    if not dataset              : raise AttributeError ( "Failed to create datatset" )
    if not len(dataset)         : raise AttributeError ( "DataSet is empty(1)"       )
    if not dataset.sumEntries() : raise AttributeError ( "DataSet is empty(2)"       ) 
    
    #======================================================================
    # Veto ranges with insufficient statistics
    #======================================================================
    if dataset.sumEntries() < minEntries:
        logger.warning ( "Insufficinent number of entries" )
        dataset.reset()
        del dataset 
        return None
    
    return dataset 


# =============================================================================
## a bit modified verison of DataFuncs.GetPerfPlotList from Urania/PIDCalib/PIDPerfScripts
def makePlots ( the_func        ,
                particle        ,  
                stripping       ,
                polarity        ,
                trackcuts       ,
                runMin=0        ,
                runMax   =   -1 ,
                verbose  = True ,
                maxFiles =   -1 ):

    #**********************************************************************
    from PIDPerfScripts.DataFuncs import CheckStripVer, CheckMagPol, CheckPartType  
    CheckStripVer ( stripping )
    CheckMagPol   ( polarity  )
    CheckPartType ( particle  )
    

    #======================================================================
    # Create dictionary holding:
    # - Reconstruction version    ['RecoVer']
    # - np.array of:
    #        - MagUp run limits   ['UpRuns']
    #        - MagDown run limits ['DownRuns']
    #======================================================================
    from PIDPerfScripts.DataFuncs import GetRunDictionary
    DataDict = GetRunDictionary ( stripping , particle , verbose = verbose )

    if trackcuts and  0 < runMin : trackcuts +=' && runNumber>=%d ' % runMin
    if trackcuts and  0 < runMax : trackcuts +=' && runNumber<=%d ' % runMax
    
    #======================================================================
    # Determine min and max file indicies
    #======================================================================
    if runMax < runMin : runMax = None 
    from PIDPerfScripts.DataFuncs import GetMinMaxFileDictionary
    IndexDict = GetMinMaxFileDictionary( DataDict , polarity ,
                                         runMin   , runMax   ,
                                         maxFiles , verbose  )
    
    #======================================================================
    # Append runNumber limits to TrackCuts
    #======================================================================
    
    logger.debug ( 'Track Cuts: %s ' % trackcuts ) 

    #======================================================================
    # Declare default list of PID plots
    #======================================================================
    plots      =   []
    minEntries = 1000

    #======================================================================
    # Loop over all calibration subsamples
    #======================================================================

    mn = IndexDict['minIndex']
    mx = IndexDict['maxIndex']

    from Ostap.Utils        import memory,NoContext
    
    logger.info('Start the loop over %d datafiles %s %s %s ' % ( mx - mn + 1  ,
                                                                 particle     ,
                                                                 stripping    ,
                                                                 polarity   ) )
    from Ostap.progress_bar import ProgressBar
    with ProgressBar( mn , mx+1 , 100 , mode='fixed' )  as bar :
        
        for index in xrange ( mn , mx + 1 ) :
            
            bar += 1
            
            manager = memory() if verbose else NoContext()
            with manager :
                
                dataset = getDataSet ( particle  ,
                                       stripping ,
                                       polarity  ,
                                       trackcuts , 
                                       index     ,
                                       verbose = verbose )
                
                if not dataset : continue 
                
                plots = the_func ( particle ,
                                   dataset  ,
                                   plots    ,
                                   verbose  )
                
                dataset.reset  ()
                dataset.store  ().reset () 
                dataset.store  ().Reset () 
                dataset.Delete ()
                if dataset : del dataset
                
            
    return plots 

# ====================================================================
## the basic function:
#  oversimplified version of MakePerfHistsRunRange.py script from Urania/PIDCalib
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2014-07-19
def runPidCalib ( the_func    ,
                  particle    ,  
                  stripping   ,
                  polarity    ,
                  trackcuts   , 
                  **config    ) :
    """
    The basic function:
    - oversimplified version of MakePerfHistsRunRange.py script from Urania/PIDCalib 
    """
    #
    ## perform some arguments check
    #
        
    ## 1) check the stripping version
    from PIDPerfScripts.DataFuncs import CheckStripVer 
    CheckStripVer ( stripping )
    
    ## 2) set the magnet polarity  [not-needed, since embedded into parser]
    from PIDPerfScripts.DataFuncs import CheckMagPol
    CheckMagPol   ( polarity  )
    
    ## 3) set the particle name [not-needed, since embedded into parser]
    from PIDPerfScripts.DataFuncs import CheckPartType
    CheckPartType ( particle  )
    
    runMin       = config.get ( 'RunMin'       ,     0 )
    runMax       = config.get ( 'RunMax'       ,    -1 )
    verbose      = config.get ( 'Verbose'      ,  True )
    maxFiles     = config.get ( 'MaxFiles'     ,    -1 )

    ## a bit strange treatment of runMax in PIDCalib :-(
    
    #
    ## finally call the standard PIDCalib machinery with user-specified function
    #
    histos =  makePlots ( the_func    ,
                          particle    ,  
                          stripping   ,
                          polarity    ,
                          trackcuts   ,
                          runMin    = runMin    ,
                          runMax    = runMax    ,
                          verbose   = verbose   ,
                          maxFiles  = maxFiles  )
    
    if config.get('dbname',None) :

        try :
            import Ostap.ZipShelve as DBASE 
            with DBASE.open ( config['dbname'] ) as db :
                if verbose : logger.info('Save data into %s' % config['dbname'] )
                ##
                key = 'PIDCalib(%s)@Stripping%s/%s' % ( particle  ,
                                                        stripping ,
                                                        polarity  )
                db [ key         ] = histos
                db [ key + 'Cuts'] = trackcuts 
                if verbose : db.ls()
                
        except :
            logger.error('Unable to save data in DB')
            
    return histos

# =============================================================================
## save histograms into the output file
#  @code
# 
#  histos = runPidCalib ( .... )
#  saveHistos ( histos , '' , **config )
# 
#  @endcode 
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2014-07-19
def saveHistos  (  histos         ,
                   fname   = ''   ,
                   verbose = True , **config ) :
    """Save histograms into the output file """
    ##
    verbose   = config.get('verbose')
    if   not fname and not config : fname = "PIDHistos.root"
    elif not fname and     config :
        fname = "PIDHistos_{part}_Strip{strp}_{pol}.root".format(
            part = config [ 'particle'  ] ,
            strp = config [ 'stripping' ] ,
            pol  = config [ 'polarity'  ] )

    if verbose : logger.info ( "Saving performance histograms to %s" %fname )
    
    import ROOT 
    import Ostap.PyRoUts  ## to be replaces with Ostap.TFileDeco
    
    with ROOT.TFile.Open ( fname, "RECREATE") as outfile :

        ## save all histograms 
        for h in histos : h.Write() 
        if verbose : outfile.ls() 
        
    return fname 

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
def  ex_func ( particle          ,
               dataset           ,
               plots     = None  , 
               verbose   = False ) :
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
    hA = ROOT.TH2D ( hID () , 'Accepted(%s)'% accepted , 15 , 0 , 150000 , 10 , 2 , 5 ) ; h1.Sumw2()
    hR = ROOT.TH2D ( hID () , 'Rejected(%s)'% rejected , 15 , 0 , 150000 , 10 , 2 , 5 ) ; h2.Sumw2()
    
    # 3) fill the historgams with 'accepted' and 'rejected' events
    #    For dataset structure and variable names see:
    #    https://twiki.cern.ch/twiki/bin/view/LHCb/PIDCalibPackage
    
    vlst = ROOT.RooArgList ()
    vlst.add ( dataset.Pi_P   ) ## note variable names 
    vlst.add ( dataset.Pi_Eta ) ## note variable name 
    
    hA = DataSet.fillHistogram ( hA , vlst , accepted ) ## fill histo 
    hR = DataSet.fillHistogram ( hR , vlst , rejected ) ## fill histo
    
    #
    ## and now update the output 
    #
    
    if not plots :

        hA.SetName ( hA.GetTitle() )
        hR.SetName ( hR.GetTitle() )
        
        plots     = [ hA , hR ] ## "Accepted" & "Rejected" histograms 

    else         :
        
        plots[0] += hA          ## "Accepted" histogram 
        plots[1] += hR          ## "Rejected" histogram
        
        hA.Delete ()
        hR.Delete ()
        if hA : del hA
        if hR : del hR
        
    if verbose :
        
        logger.info ( 'Accepted histo: %s' % plots[0].stat() )
        logger.info ( 'Rejected histo: %s' % plots[1].stat() )
            
    return plots


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
def  ex_func2 ( particle         ,
                dataset          ,
                plots    = None  ,
                verbose  = False ) :

    ## we need ROOT and Ostap machinery!    
    import ROOT
    from Ostap.PyRoUts import hID, h3_axes  

    ## the main:
    accepted = 'K_ProbNNK>0.1'    ## ACCEPTED sample 
    rejected = 'K_ProbNNK<0.1'    ## REJECTED sample 

    ## variables for the historgrams 
    vlst = ROOT.RooArgList ()
    vlst.add ( dataset.K_P   )
    vlst.add ( dataset.K_Eta )
    vlst.add ( dataset.nTracks )

    ## binning    
    pbins    = [ 3.2  , 6  , 9  ,  15 , 20  , 30  , 40 , 50 , 60 , 80 , 100 , 120 , 150 ]
    pbins    = [ p*1000 for p in pbins ]
    
    hbins    = [ 2.0 , 2.5 , 3.0 , 3.5 , 4.0 , 4.5 , 4.9 ]
    tbins    = [0, 150 , 250 , 400 , 1000]

    ## book histogams
    
    ha       = h3_axes ( pbins , hbins , tbins , title = 'Accepted(%s)' % accepted ) 
    hr       = h3_axes ( pbins , hbins , tbins , title = 'Rejected(%s)' % rejected )

    ## fill them
    
    ha = dataset.fillHistogram ( ha , vlst , accepted )
    hr = dataset.fillHistogram ( hr , vlst , rejected )

    if not plots :
        
        ha.SetName ( ha.GetTitle() )
        hr.SetName ( hr.GetTitle() )
        
        plots = [ ha , hr ]           ## create plots 
        
    else         :

        plots [0] += ha               ## update plots 
        plots [1] += hr               ## update plots 
        
        ha.Delete ()
        hr.Delete ()
        if ha : del ha
        if hr : del hr
        
    return plots

# =============================================================================
## run pid-calib procedure
def run_pid_calib ( FUNC , db_name = 'PID_eff.db' ) :
    """
    Run PID-calib procedure 
    """

    import Ostap.Line 
    logger.info ( __file__  + '\n' + Ostap.Line.line  ) 
    logger.info ( 80*'*'   )
    logger.info ( __doc__  )
    logger.info ( 80*'*'   )
    
    ## from Ostap.PidCalib import makeParser, runPidCalib, saveHistos

    ## needed ? probably not, to be removed... 
    import ROOT 
    RAD = ROOT.RooAbsData
    if RAD.Tree != RAD.getDefaultStorageType() :
        logger.info ( 'DEFINE default storage type to be TTree! ') 
        RAD.setDefaultStorageType ( RAD.Tree )
        
    parser  = makeParser        ()
    config  = parser.parse_args ()

    polarity  =  config.polarity
    if 'Both' == polarity  : polarity  = [ 'MagUp' , 'MagDown' ]
    else                   : polarity  = [ polarity ]

    
    stripping =  config.stripping
    ##if not stripping : stripping = [ '21' , '21r1' ]
    if not stripping : stripping = [ '20' , '20r1' ]
    ## 
    particle  = config.particle
    
    logger.info ( 'Stripping versions:  %s' % stripping )  
    logger.info ( 'Magnet polarities :  %s' % polarity  )  

    particles = [ particle ]
    if   'P' == particle :
        if '20' in stripping or '20r1' in stripping :
            particles = [ 'P_IncLc' ] + [ particle ]            
            logger.info ( 'Use reduced set of protons species %s ' % particles )
        else :
            particles = [ 'P_LcfB' , 'P_TotLc' , 'P_IncLc' ]
            logger.info ( 'Use all species of protons %s ' % particles )
            
    hfiles = []

    ## loop over the magnet polarities 
    for m in polarity :
        
        ## loop over stripping versions 
        for s in stripping :
            
            ## loop over calibration techniques/particle species 
            for p in particles :
                
                histos = runPidCalib ( FUNC        ,
                                       p           ,
                                       s           ,
                                       m           ,
                                       config.cuts , 
                                       RunMin   = config.RunMin      ,
                                       RunMax   = config.RunMax      ,
                                       MaxFiles = config.MaxFiles    ,
                                       Verbose  = config.verbose     ,
                                       dbname   = db_name            )
                
                hfile = saveHistos  ( histos              ,
                                      particle  = p       ,
                                      stripping = s       ,
                                      polarity  = m       ,
                                      verbose   = config.verbose ) 
                
                hfiles.append ( hfile )

    logger.info('Produced files: ')
    for i in hfiles : logger.info ( i )  

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
    

