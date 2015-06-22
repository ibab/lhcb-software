#!/usr/bin/env python 
# -*- coding: utf-8 -*-
# =============================================================================
# $Id$ 
# =============================================================================
## @file BenderTools/DstExplorer.py
#
#  Trivial Bender-based script to explore the content of (x,mu,s,r,...)DSTs
#
#  This file is a part of 
#  <a href="http://cern.ch/lhcb-comp/Analysis/Bender/index.html">Bender project</a>
#  <b>``Python-based Interactive Environment for Smart and Friendly Physics Analysis''</b>
#
#  The package has been designed with the kind help from
#  Pere MATO and Andrey TSAREGORODTSEV. 
#  And it is based on the 
#  <a href="http://cern.ch/lhcb-comp/Analysis/LoKi/index.html">LoKi project:</a>
#  <b>``C++ ToolKit for Smart and Friendly Physics Analysis''</b>
#
#  By usage of this code one clearly states the disagreement 
#  with the smear campaign of Dr.O.Callot et al.: 
#  ``No Vanya's lines are allowed in LHCb/Gaudi software''
#
# Usage:
#
# @code
#
#    dst_explorer [options] file1 [ file2 [ file3 [ file4 ....'
#
#    1) exploring (r,s,m,fm)DSTs:
#
#       >>> ls('/Event')
#       >>> ls('/Event/Charm')
#       >>> ls('/Event/Charm/Phys')
#       >>> ls('/Event/Charm/Phys/D2KKK')
#
#    2) getting particles from TES and loop over them :
# 
#       >>> Ds = get('/Event/Charm/Phys/D2KKK')
#       >>> for D in Ds : print D.decay()
#
#    3) go to the next event:
#
#       >>> run(1)
#       >>> ls('/Event')
#
#    4) loop over events in search for ``valid'' data
#
#       >>> Ds, evt = seekForData('/Event/Charm/Phys/DstarForPromptCharm/Particles' )
#
#    5) loop in search for event with certain Stripping Decision:
#
#       >>> ok, evt = seekStripDecision('.*Lam.*' )
# 
# 
#  @endcode 
#
#  @date   2010-09-10
#  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
#
#                    $Revision$
#  Last modification $Date$
#                 by $Author$
# =============================================================================
"""Trivial Bender-based script to explore the content of (x,mu,s,r,...)DSTs

oooooooooo.                              .o8                     
`888'   `Y8b                            \"888                     
 888     888  .ooooo.  ooo. .oo.    .oooo888   .ooooo.  oooo d8b 
 888oooo888' d88' `88b `888P\"Y88b  d88' `888  d88' `88b `888\"\"8P 
 888    `88b 888ooo888  888   888  888   888  888ooo888  888     
 888    .88P 888    .o  888   888  888   888  888    .o  888     
o888bood8P'  `Y8bod8P' o888o o888o `Y8bod88P\" `Y8bod8P' d888b    
                                                                 
This file is a part of BENDER project:

  ``Python-based Interactive Environment for Smart and Friendly Physics Analysis''

The project has been designed with the kind help from Pere MATO and Andrey TSAREGORODTSEV. 

And it is based on the LoKi project:
 
   ``C++ ToolKit for Smart and Friendly Physics Analysis''

By usage of this code one clearly states the disagreement with the smear campaign of Dr.O.Callot et al.:

   ``No Vanya's lines are allowed in LHCb/Gaudi software''

Usage:

    dst_explorer [options] file1 [ file2 [ file3 [ file4 ....'

    1) exploring (r,s,m,fm)DSTs:

       >>> ls('/Event')
       >>> ls('/Event/Charm')
       >>> ls('/Event/Charm/Phys')
       >>> ls('/Event/Charm/Phys/D2KKK')

    2) getting particles from TES and loop over them :
 
       >>> Ds = get('/Event/Charm/Phys/D2KKK')
       >>> for D in Ds : print D.decay()

    3) go to the next event:

       >>> run(1)
       >>> ls('/Event')

    4) loop over events in search for ``valid'' data

       >>> Ds, evt = seekForData('/Event/Charm/Phys/DstarForPromptCharm/Particles' )

    5) loop in search for event with certain Stripping Decision:

       >>> ok, evt = seekStripDecision('.*Lam.*' )
 

                  $Revision$
Last modification $Date$
               by $Author$

"""
# =============================================================================
__author__  = 'Vanya BELYAEV Ivan.Belyaev@itep.ru'
__date__    = "2010-09-10"
__version__ = '$Revision$'
__all__     = ( 'configure',  ) 
__usage__   = 'dst_explorer [options] file1 [ file2 [ file3 [ file4 ....'
# =============================================================================
# logging 
# =============================================================================
from AnalysisPython.Logger import getLogger 
if '__main__' == __name__ : logger = getLogger ( 'BenderTools.DstExplorer' )
else                      : logger = getLogger ( __name__ )
# =============================================================================
## configure the application from parser data  
def configure ( options , arguments ) :
    """
    Configure the application from parser data 
    """
    # 
    ## redefine output level for 'quiet'-mode
    if options.OutputLevel > 5 :
        options.OutputLevel = 5
        logger.info('set OutputLevel to be %s ' % options.OutputLevel )
        
    if options.OutputLevel < 0 :
        options.OutputLevel = 0
        logger.info('set OutputLevel to be %s ' % options.OutputLevel )
        
    if options.Quiet and 4 > options.OutputLevel :
        options.OutputLevel = 4
        logger.info('set OutputLevel to be %s ' % options.OutputLevel )

    #
    ## start the actual action:
    #
    from Configurables       import DaVinci

    #
    ## get the file type for the file extension
    #
    from BenderTools.Parser import dataType
    pyfiles = [ i for i in arguments if  len(i) == 3 + i.find('.py') ]
    files   = [ i for i in arguments if  i not in pyfiles ]
    from BenderTools.Parser import fileList 
    files  += fileList ( options.FileList ) 
    #
    if not files and not options.ImportOptions : 
        raise AttributeError('No data files are specified!')
    
    ## get some info from file names/extensision
    dtype, simu, ext = None,None,None  
    if  files :
        dtype, simu, ext = dataType ( files )
    
    if '2013' == dtype :
        logger.info ('Data type 2013 is redefined to be 2012')
        dtype = '2012'
        
        # 
    if ext in ( 'gen'  , 'xgen' ,
                'GEN'  , 'XGEN' ,
                'ldst' , 'LDST' ) and not simu : simu = True 
    
    if dtype and dtype != options.DataType :
        logger.info ( 'Redefine DataType from  %s to %s '   % ( options.DataType, dtype ) )
        options.DataType  = dtype
        
    if simu and not options.Simulation : 
        logger.info ( 'Redefine Simulation from  %s to %s ' % ( options.Simulation, simu ) )
        options.Simulation  = simu 
        
    if options.Simulation and '2009' == options.DataType :
        options.DataType = 'MC09'
        logger.info('set DataType to be MC09')    
        
    daVinci = DaVinci (
        DataType    = options.DataType    ,
        Simulation  = options.Simulation  ,
        Lumi        = options.Lumi               
        )
    
    if options.MicroDST or 'mdst' == ext or 'MDST' == ext or 'uDST' == ext :
        logger.info ( 'Define input type as micro-DST' )
        daVinci.InputType = 'MDST'
        
    #
    ## try to guess RootInTES
    #
    from BenderTools.Parser import hasInFile 
    if   hasInFile ( files , 'CHARM.MDST'    ) and not options.RootInTES :
        options.RootInTES = '/Event/Charm'
        logger.info ('RootInTES is set according to CHARM.MDST'    )
        daVinci.InputType = 'MDST'        
    elif hasInFile ( files , 'LEPTONIC.MDST' ) and not options.RootInTES :
        options.RootInTES = '/Event/Leptonic'
        logger.info ('RootInTES is set according to LEPTONIC.MDST' )
        daVinci.InputType = 'MDST'
    elif hasInFile ( files , 'BHADRON.MDST'  ) and not options.RootInTES :
        options.RootInTES = '/Event/Bhadron'
        logger.info ('RootInTES is set according to BHADRON.MDST'  )
        daVinci.InputType = 'MDST'
    elif hasInFile ( files , 'PID.MDST'      ) and not options.RootInTES :
        options.RootInTES = '/Event/PID'
        logger.info ('RootInTES is set according to PID.MDST'      )
        daVinci.InputType = 'MDST'
    elif hasInFile ( files , 'PSIX.MDST'     ) and not options.RootInTES :
        options.RootInTES = '/Event/PSIX'
        logger.info ('RootInTES is set according to PSIX.MDST'     )
        daVinci.InputType = 'MDST'
    elif hasInFile ( files , 'PSIX0.MDST'    ) and not options.RootInTES :
        options.RootInTES = '/Event/PSIX0'
        logger.info ('RootInTES is set according to PSIX0.MDST'    )
        daVinci.InputType = 'MDST'
    elif hasInFile ( files , 'BOTTOM.MDST'   ) and not options.RootInTES :
        options.RootInTES = '/Event/BOTTOM'
        logger.info ('RootInTES is set according to BOTTOM.MDST'   )
        daVinci.InputType = 'MDST'

        
    if options.RootInTES and  0  != options.RootInTES.find ( '/Event' ) :
        options.RootInTES = '/Event/' + options.RootInTES
    if options.RootInTES and '/' == options.RootInTES[-1] :
        options.RootInTES = options.RootInTES[:-1]
    if options.RootInTES and '/Event' != options.RootInTES  : 
        daVinci.RootInTES = options.RootInTES 
    #
    ## check for Grid-access
    #
    if options.Grid : 
        from Bender.DataUtils import hasGridProxy
        if not hasGridProxy () :
            logger.warning ( 'GRID proxy is not available, switch off GRID-lookup' )
            options.Grid = ''  ## SWITCH OFF Grid-lookup

    if not options.Simulation and options.DataType in ( '2010' ,
                                                        '2011' ,
                                                        '2012' ,
                                                        '2013' ) :
        #
        ## try to use the latest available tags:
        #
        from Configurables import CondDB    
        ## CondDB ( UseLatestTags = [ options.DataType ] )
        ## logger.info('Use latest tags for %s' % options.DataType )
        CondDB( LatestGlobalTagByDataType = options.DataType ) 
        logger.info('Use latest global tag for data type %s' % options.DataType )
        import os 
        if options.UseOracle and os.environ.has_key('LHCBGRIDSYSROOT') :
            if os.environ.has_key('LHCBGRIDCONFIGROOT') :
                if os.path.exists ( os.environ['LHCBGRIDSYSROOT'] )  :
                    if os.path.exists ( os.environ['LHCBGRIDCONFIGROOT'] )  :
                        #
                        ## Use Oracle if possible
                        CondDB ( UseOracle = True  )
                        logger.info('Oracle DB will be used')

    if options.Simulation :
        #
        ## try to get the tags from Rec/Header
        from BenderTools.GetDBtags import useDBTagsFromData
        tags = useDBTagsFromData (
            files [ 0 ]       ,
            options.Castor    ,
            options.Grid      ,
            daVinci           ) 

    ## specific action for (x)gen files 
    if ext in ( 'gen' , 'xgen' , 'GEN' , 'XGEN' ) :
        from BenderTools.GenFiles import genAction
        genAction ( ext )
        
    ## Reset all DaVinci sequences 
    def _action ( ) :
        """
        Reset all DaVinci sequences
        """
        from Gaudi.Configuration import allConfigurables 
        from Gaudi.Configuration import getConfigurable 
        for seq in ( 'DaVinciInitSeq'      ,
                     'DaVinciMainSequence' ,
                     'DaVinciSequence'     ,
                     'MonitoringSequence'  ,
                     'FilteredEventSeq'    ) :

            if not seq in allConfigurables : continue 
            cSeq = getConfigurable( seq )
            if cSeq and hasattr ( cSeq , 'Members' ) :
                logger.info ( 'Reset the sequence %s' % cSeq.name() )
                cSeq.Members = []

            ## reset the list of top-level algorithms 
            from Configurables import ApplicationMgr
            a = ApplicationMgr()
            a.TopAlg = []
            a.OutputLevel = options.OutputLevel
            
            from Configurables import MessageSvc
            m = MessageSvc ( OutputLevel = options.OutputLevel )
            
            from GaudiConf import IOHelper
            ioh = IOHelper () 
            ioh.setupServices()
            
    ## prepare to copy good/marked/tagged evenst
    if hasattr ( options, 'OutputFile' ) and options.OutputFile :
        from BenderTools.GoodEvents import copyGoodEvents
        if 0 <= options.OutputFile.find ( '.' ) : 
            copyGoodEvents (             options.OutputFile         ) 
        else :
            copyGoodEvents ( "%s.%s" % ( options.OutputFile , ext ) ) 

    ## comment it out... Hm...
    ## from Gaudi.Configuration import appendPostConfigAction
    ## appendPostConfigAction ( _action )
    
    ## get xml-catalogs (if specified) 
    catalogs = [ options.XmlCatalogue ] if options.XmlCatalogue else []

    ## import options (if specified) 
    if options.ImportOptions :
        from Gaudi.Configuration import importOptions
        importOptions ( options.ImportOptions )  
    
    ## set input data
    from Bender.Main import setData 
    setData ( arguments       ,
              catalogs        ,
              options.Castor  ,
              options.Grid    )

    if not options.Quiet : print daVinci

    return pyfiles

# =============================================================================
if '__main__' == __name__ :
    
    logger.info ( 100*'*')
    logger.info ( ' Author  : %s ' % __author__   ) 
    logger.info ( ' Version : %s ' % __version__  ) 
    logger.info ( ' Date    : %s ' % __date__     )
    
    from BenderTools.Parser import makeParser
    parser = makeParser  ( usage = __usage__   ,
                           vers  = __version__ )
    
    parser.add_option (
        '-o'                     ,
        '--output'               ,
        type    = 'str'          ,
        dest    = 'OutputFile'   ,
        help    = 'The name of output file with selected events' ,
        default = ''            
        )
    ##
    import sys
    options , arguments = parser.parse_args()

    print 120*'*'
    if options.Quiet :
        logger.info ( ' Trivial Bender-based script to explore the content of (x,mu,s,r,...)DSTs ')
    else :
        logger.info ( __doc__ ) 

    ## Files must be specfied are mandatory!
    if not arguments and not options.FileList and not options.ImportOptions :
        parser.error ( 'No input files/data are specified' ) 
    
    pyfiles = configure ( options , arguments ) 
    
    if options.Simulation : from Bender.MainMC   import *
    else                  : from Bender.Main     import *

    from BenderTools.Seek       import * 
    from BenderTools.GoodEvents import * 
        
    ## instantiate the application manager 
    gaudi  = appMgr ()
    
    evtSel = gaudi.evtSel()
    
    evtSvc = gaudi.evtSvc()
    
    detSvc = gaudi.detSvc()

    evt    = evtSvc
    det    = detSvc
    
    ## initialize and read the first event
    run ( 1 )

    ## execute the files, defined as arguments
    if pyfiles :
        
        from   copy import deepcopy
        _myself = sys.modules[ __name__ ] 
        _mykeys = deepcopy ( dir( _myself ) ) 
        _copied = set() 
        _skept  = set() 
        
        for a in pyfiles : 
            
            p     = a.find( '.py' )
            if p <= 0          : continue 
            if len(a) != p + 3 : continue  
            if not os.path.exists ( a ) :
                logger.warning   ('No file is found  "%s"' % a )
                continue 
            logger.info  ('Try    to execute "%s"' % a )
            try :
                myvars =  {} 
                execfile ( a , myvars )
                for k,val in myvars.iteritems() : 
                ## do not redefine already defined symbols!  
                    if k in _mykeys : 
                        # do not redefine local symbols [ needed? ] 
                        _skept.add ( k )
                        continue
                    ## copy symbol into the local namespace  
                    setattr ( _myself , k , val ) 
                    _copied.add ( k ) 
            except :
                logger.error ('Unable to execute "%s"'     % a    )
                logger.error ('Exception: %s' % sys.exc_info()[0] ) 
                import traceback 
                print  traceback.format_exc() 
                
        if _copied : logger.info ( 'Copied %d symbols' % len(_copied) )  
        if _skept  : logger.info ( 'Skept  %d symbols' % len(_skept ) )  
        
        ## do not pollute the namespace 
        del _copied
        del _skept
        del _mykeys 
        del _myself 
        

# =============================================================================
# The END 
# =============================================================================

