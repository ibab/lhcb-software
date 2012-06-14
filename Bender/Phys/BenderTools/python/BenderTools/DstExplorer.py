#!/usr/bin/env python 
# =============================================================================
# $Id: DstExplorer.py 138072 2012-04-04 14:41:10Z ibelyaev $ 
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
#                    $Revision: 138072 $
#  Last modification $Date: 2012-04-04 16:41:10 +0200 (Wed, 04 Apr 2012) $
#                 by $Author: ibelyaev $
# =============================================================================
"""
Trivial Bender-based script to explore the content of (x,mu,s,r,...)DSTs

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
 

                  $Revision: 138072 $
Last modification $Date: 2012-04-04 16:41:10 +0200 (Wed, 04 Apr 2012) $
               by $Author: ibelyaev $

"""
# =============================================================================
__author__  = 'Vanya BELYAEV Ivan.Belyaev@nikhef.nl'
__date__    = "2010-09-10"
__version__ = '$Revision: 138072 $'
__all__     = ()  ## nothing to import 
__usage__   = 'dst_explorer [options] file1 [ file2 [ file3 [ file4 ....'
# =============================================================================
## logging
# =============================================================================
from Bender.Logger import getLogger 
logger = getLogger( __name__ )
# =============================================================================
from BenderTools.Seek       import * 
from BenderTools.GoodEvents import * 
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
    dtype, simu, ext = dataType ( arguments ) 

    if dtype and dtype != options.DataType :
        logger.info ( 'Redefine DataType from  %s to %s ' % ( options.DataType, dtype ) )
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
        
    if options.RootInTES and '/' == options.RootInTES[-1] :
        options.RootInTES = options.RootInTES[:-1]
    if options.RootInTES and  0  != options.RootInTES.find ( '/Event/' ) :
        options.RootInTES = '/Event/' + options.RootInTES
        
    if options.RootInTES :
        from BenderTools.MicroDST import uDstConf 
        uDstConf(options.RootInTES)
   
    if not options.Simulation and options.DataType in ( '2010' ,
                                                        '2011' ,
                                                        '2012' ) :
        #
        ## try to use the latest available tags:
        #
        from Configurables import CondDB    
        CondDB ( UseLatestTags = [ options.DataType ] )
        logger.info('Use latest tags for %s' % options.DataType )
        import os 
        if options.UseOracle and os.environ.has_key('LHCBGRIDSYSROOT') :
            if os.environ.has_key('LHCBGRIDCONFIGROOT') :
                if os.path.exists ( os.environ['LHCBGRIDSYSROOT'] )  :
                    if os.path.exists ( os.environ['LHCBGRIDCONFIGROOT'] )  :
                        #
                        ## Use Oracle if possible
                        CondDB ( UseOracle = True  )
                        logger.info('Oracle DB will be used')
                        
                    
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

    from Gaudi.Configuration import appendPostConfigAction
    appendPostConfigAction ( _action )
    
    ## get xml-catalogs (if specified) 
    catalogs = [ options.XmlCatalogue ] if options.XmlCatalogue else []
    
    ## set input data
    from Bender.Main import setData 
    setData ( arguments , catalogs , options.Castor  )

    if not options.Quiet : print daVinci
        
# =============================================================================
if '__main__' == __name__ :
    
    print 120*'*'
    
    print ' Author  : ', __author__ 
    print ' Version : ', __version__ 
    print ' Date    : ', __date__ 
    
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
    options , arguments = parser.parse_args()
    
    print 120*'*'
    if options.Quiet :
        print ' Trivial Bender-based script to explore the content of (x,mu,s,r,...)DSTs '
    else :
        print __doc__

    
    ## Files must be specfied are mandatory!
    if not arguments : parser.error ( 'No input files are specified' ) 
    
    configure ( options , arguments ) 
    
    if options.Simulation : from Bender.MainMC   import *
    else                  : from Bender.Main     import *
    
    ## instantiate the application manager 
    gaudi  = appMgr ()
    
    evtSel = gaudi.evtSel() 

    ## initialize and read the first event
    run ( 1 )
    
# =============================================================================
# The END 
# =============================================================================

