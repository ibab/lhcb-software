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
def configure ( config ) :
    """
    Configure the application from parser data 
    """
    #
    if config.OutputLevel <= 3 and not config.Quiet :
        clogger = getLogger('Bender.Configuration') 
        _vars   = vars ( config )
        _keys   = _vars.keys()
        _keys .sort()        
        clogger.info ( 100 * '*')
        clogger.info ( 'Configuration:')
        for _k in _keys : clogger.info ( '  %15s : %-s ' % ( _k , _vars[_k] ) )
        clogger.info ( 100 * '*')
    
    ## redefine output level for 'quiet'-mode
    if config.OutputLevel > 5 :
        config.OutputLevel = 5
        logger.info('set OutputLevel to be %s ' % config.OutputLevel )
        
    if config.OutputLevel < 0 :
        config.OutputLevel = 0
        logger.info('set OutputLevel to be %s ' % config.OutputLevel )

    if config.Quiet and 4 > config.OutputLevel :
        config.OutputLevel = 4
        logger.info('set OutputLevel to be %s ' % config.OutputLevel )
        from BenderTools.Utils import silence
        silence ()

        
    import logging
    if   config.OutputLevel <= 1 : logging.disable ( logging.NOTSET  - 1 ) 
    elif config.OutputLevel <= 2 : logging.disable ( logging.DEBUG   - 1 ) 
    elif config.OutputLevel <= 3 : logging.disable ( logging.INFO    - 1 ) 
    elif config.OutputLevel <= 4 : logging.disable ( logging.WARNING - 1 ) 
    elif config.OutputLevel <= 5 : logging.disable ( logging.ERROR   - 1 )
    #
    ## start the actual action:
    #
    from Configurables       import DaVinci

    #
    ## get the file type for the file extension
    #
    from BenderTools.Parser import dataType
    pyfiles = [ i for i in config.files if  len(i) == 3 + i.find('.py') ]
    files   = [ i for i in config.files if  i not in pyfiles ]
    
    from BenderTools.Parser import fileList
    for f in config.FileList : files  += fileList ( f )
    
    if not files and not config.ImportOptions : 
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
    
    if dtype and dtype != config.DataType :
        logger.info ( 'Redefine DataType from  %s to %s '   % ( config.DataType, dtype ) )
        config.DataType  = dtype
        
    if simu and not config.Simulation : 
        logger.info ( 'Redefine Simulation from  %s to %s ' % ( config.Simulation, simu ) )
        config.Simulation  = simu 
        
    if config.Simulation and config.Lumi :
        logger.info('suppress Lumi for Simulated data')    
        config.Lumi = False
        
    ## summary information (when available) 
    from Configurables import LHCbApp
    LHCbApp().XMLSummary = 'summary.xml'

    daVinci = DaVinci (
        DataType    = config.DataType    ,
        Simulation  = config.Simulation  ,
        Lumi        = config.Lumi               
        )
    
    if config.MicroDST or 'mdst' == ext or 'MDST' == ext or 'uDST' == ext :
        logger.info ( 'Define input type as micro-DST' )
        daVinci.InputType = 'MDST'
        
    #
    ## try to guess RootInTES
    #
    from BenderTools.Parser import hasInFile 
    if   hasInFile ( files , 'CHARM.MDST'    ) and not config.RootInTES :
        config.RootInTES = '/Event/Charm'
        logger.info ('RootInTES is set according to CHARM.MDST'    )
        daVinci.InputType = 'MDST'        
    elif hasInFile ( files , 'LEPTONIC.MDST' ) and not config.RootInTES :
        config.RootInTES = '/Event/Leptonic'
        logger.info ('RootInTES is set according to LEPTONIC.MDST' )
        daVinci.InputType = 'MDST'
    elif hasInFile ( files , 'BHADRON.MDST'  ) and not config.RootInTES :
        config.RootInTES = '/Event/Bhadron'
        logger.info ('RootInTES is set according to BHADRON.MDST'  )
        daVinci.InputType = 'MDST'
    elif hasInFile ( files , 'PID.MDST'      ) and not config.RootInTES :
        config.RootInTES = '/Event/PID'
        logger.info ('RootInTES is set according to PID.MDST'      )
        daVinci.InputType = 'MDST'
    elif hasInFile ( files , 'PSIX.MDST'     ) and not config.RootInTES :
        config.RootInTES = '/Event/PSIX'
        logger.info ('RootInTES is set according to PSIX.MDST'     )
        daVinci.InputType = 'MDST'
    elif hasInFile ( files , 'PSIX0.MDST'    ) and not config.RootInTES :
        config.RootInTES = '/Event/PSIX0'
        logger.info ('RootInTES is set according to PSIX0.MDST'    )
        daVinci.InputType = 'MDST'
    elif hasInFile ( files , 'BOTTOM.MDST'   ) and not config.RootInTES :
        config.RootInTES = '/Event/BOTTOM'
        logger.info ('RootInTES is set according to BOTTOM.MDST'   )
        daVinci.InputType = 'MDST'
    
    if config.RootInTES and  0  != config.RootInTES.find ( '/Event' ) :
        config.RootInTES  = '/Event/' + config.RootInTES        
    if config.RootInTES and '/' == config.RootInTES[-1] :
        config.RootInTES  = config.RootInTES[:-1]        
    if config.RootInTES and '/Event' != config.RootInTES  : 
        daVinci.RootInTES = config.RootInTES 
    #
    ## check for Grid-access
    #
    if config.Grid : 
        from Bender.DataUtils import hasGridProxy
        if not hasGridProxy () :
            logger.warning ( 'GRID proxy is not available, switch off GRID-lookup' )
            config.Grid = ''  ## SWITCH OFF Grid-lookup

    if not config.Simulation and config.DataType in ( '2010' ,
                                                      '2011' ,
                                                      '2012' ,
                                                      '2013' , 
                                                      '2015' ) :
        #
        ## try to use the latest available tags:
        #
        from Configurables import CondDB    
        ## CondDB ( UseLatestTags = [ options.DataType ] )
        ## logger.info('Use latest tags for %s' % options.DataType )
        CondDB( LatestGlobalTagByDataType = config.DataType ) 
        logger.info('Use latest global tag for data type %s' % config.DataType )

    if config.Simulation :
        #
        ## try to get the tags from Rec/Header
        from BenderTools.GetDBtags import useDBTagsFromData
        tags = useDBTagsFromData (
            files [ 0 ]       ,
            True              , ## castor 
            config.Grid       ,
            daVinci           ) 

    if config.IgnoreDQFlags :
        logger.info('DataQuality flags will be ignored')
        daVinci.IgnoreDQFlags = config.IgnoreDQFlags
        
    ## specific action for (x)gen files 
    if ext in ( 'gen' , 'xgen' , 'GEN' , 'XGEN' ) :
        from BenderTools.GenFiles import genAction
        genAction ( ext )
        
    ## prepare to copy good/marked/tagged evenst
    if hasattr ( config , 'OutputFile' ) and config.OutputFile :
        from BenderTools.GoodEvents import copyGoodEvents
        if 0 <= config.OutputFile.find ( '.' ) : 
            copyGoodEvents (             config.OutputFile         ) 
        else :
            copyGoodEvents ( "%s.%s" % ( config.OutputFile , ext ) ) 

    ## import options (if specified) 
    for i in config.ImportOptions :
        logger.info ("Import options from file %s'" % i )
        from Gaudi.Configuration import importOptions
        importOptions ( i )  

    ## set input data
    from Bender.Utils import setData 
    setData ( files                ,
              config.XmlCatalogs   ,  ## XML-catalogues 
              config.Castor        ,  ## use Castor/EOS lookup 
              config.Grid          )  ## Use GRID to locate files 


    return pyfiles



# =============================================================================
## Reset all DaVinci sequences (if needed)
def resetDaVinci() :
    """
    Reset all DaVinci sequences
    """
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
            
    ## comment it out... Hm...
    from Gaudi.Configuration import appendPostConfigAction
    appendPostConfigAction ( _action )

# =============================================================================
if '__main__' == __name__ :
    
    logger.info ( 100*'*')
    logger.info ( ' Author  : %s ' % __author__   ) 
    logger.info ( ' Version : %s ' % __version__  ) 
    logger.info ( ' Date    : %s ' % __date__     )
    
# =============================================================================
# The END 
# =============================================================================

