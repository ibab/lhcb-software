#!/usr/bin/env python 
# =============================================================================
# $Id: CheckTrg.py 138072 2012-04-04 14:41:10Z ibelyaev $ 
# =============================================================================
## @file BenderTools/CheckTrg.py
#
#  Trivial Bender-based script to check trigger decisions for certain lines 
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
#    CheckTrg [options] line file1 [ file2 [ file3 [ file4 ....'
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

Trivial Bender-based script to check trigger decisions for certain lines 

This file is a part of BENDER project:

  ``Python-based Interactive Environment for Smart and Friendly Physics Analysis''

The project has been designed with the kind help from Pere MATO and Andrey TSAREGORODTSEV. 

And it is based on the LoKi project:
 
   ``C++ ToolKit for Smart and Friendly Physics Analysis''

By usage of this code one clearly states the disagreement with the smear campaign of Dr.O.Callot et al.:

   ``No Vanya's lines are allowed in LHCb/Gaudi software''

Usage:

    CheckTrg [options] line file1 [ file2 [ file3 [ file4 ....'

"""
# =============================================================================
__author__  = 'Vanya BELYAEV Ivan.Belyaev@nikhef.nl'
__date__    = "2010-09-10"
__version__ = '$Revision: 138072 $'
__all__     = ()  ## nothing to import 
__usage__   = 'CheckTrg [options] line file1 [ file2 [ file3 [ file4 ....'
# =============================================================================
import ROOT 
# =============================================================================
## logging
# =============================================================================
from Bender.Logger import getLogger 
logger = getLogger( __name__ )
# =============================================================================
## postpone the massive import from Bender 
def chkTrg  ( ) :
    
    """
    create the algorithnm for trigger checks 
    """
    from   Bender.Main          import Algo, SUCCESS 
    import BenderTools.TisTos   ## add methods for TisTos 
    # =============================================================================
    ## @class CheckTrg
    #  simple class to perform quick check for TisTos 
    #  @date   2010-09-10
    #  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
    class CheckTrg(Algo) :
        """
        Simple class to perform quick check for TisTos
        """
        ## the standard initialization 
        def initialize ( self ) :
            """
            Initialization
            """
            sc = Algo.initialize        ( self )
            if sc.isFailure() : return sc
            
            sc = self.tisTos_initialize (      )
            if sc.isFailure() : return sc
            
            return SUCCESS
    
        ## the only one esential method: 
        def analyse  ( self ) :
            """
            The only one essential method
            """
            particles = self.select ( 'all' , PALL )
            
            for p in particles :
                
                pname = p.pname() 
                if not self.triggers.has_key ( pname ) : self.triggers[ pname ] = {}
                self.decisions ( p , self.triggers[ pname ] ) 

            self.setFilterPassed ( not particles.empty() )


            return SUCCESS 

        ## finalization 
        def finalize ( self ) :
            """
            Standard finalization 
            """
            #
            Algo.tisTos_finalize ( self )
            return Algo.finalize ( self )


    return CheckTrg

# =============================================================================
if '__main__' == __name__ :


    from BenderTools.Parser import makeParser
    
    parser = makeParser ( usage = __usage__                ,
                          vers  = ' %prog '  + __version__ )
    
    ##
    parser.add_option (
        '-n'                          ,
        '--nevents'                   ,
        type    = 'int'               , 
        dest    = 'Nevents'           ,
        help    = "Number of events to process[default : %default]" ,
        default = '1000'                  
        )
    ##
    (options,arguments) = parser.parse_args() 

    print 120*'*'
    if options.Quiet :
        print ' Trivial Bender-based script to ceck the triggers '
    else :
        print __doc__

    
    print ' Author  : ', __author__ 
    print ' Version : ', __version__ 
    print ' Date    : ', __date__ 
    print 120*'*'
    
    ## Files must be specfied are mandatory!
    if 2 > len(arguments) :
        parser.error ( 'No input files/line  are specified' ) 

    ## line to be checked
    Line  = arguments [0]
    Files = arguments [1:] 
    
    #
    ## get the file type for the file extension
    #
    from BenderTools.Parser import dataType
    dtype, simu, ext = dataType ( Files  ) 
    
    if dtype and dtype != options.DataType :
        logger.info ( 'Redefine DataType from  %s to %s ' % ( options.DataType, dtype ) )
        options.DataType  = dtype
        
    if simu and not options.Simulation : 
        logger.info ( 'Redefine Simulation from  %s to %s ' % ( options.Simulation, simu ) )
        options.Simulation  = simu 
        
    if options.Simulation and '2009' == options.DataType :
        options.DataType = 'MC09'
        logger.info ( 'set DataType to be MC09' )


    if options.RootInTES and '/' == options.RootInTES[-1] :
        options.RootInTES = options.RootInTES[:-1]
    if options.RootInTES and  0  != options.RootInTES.find ( '/Event/' ) :
        options.RootInTES = '/Event/' + options.RootInTES 

    if options.RootInTES :
        if   0  < Line.find ( 'Phys' ) :
            Line = Line [ Line.find('Phys'): ]
            Line = options.RootInTES + '/' + Line
        elif 0 == Line.find ( 'Phys' ) :
            Line = options.RootInTES + '/'      + Line
        else :
            Line = options.RootInTES + '/Phys/' + Line

    if     0 != Line.find ('/Event/') :
        parser.error ( 'Invalid line name %s' % Line  ) 
        
    if not 0 < Line.rfind ('/Particles') :
        Line = Line + '/Particles'
        
    if 0 >= options.Nevents and -1 != options.Nevents : options.Nevents = 1000
    #

    #
    ## start the actual action:
    #
    
    if options.Simulation : 
        from Bender.MainMC   import *
    else                  : 
        from Bender.Main     import *
    
    from Gaudi.Configuration import * 
    from Configurables       import DaVinci

    #
    ## add filters:
    #
    from PhysConf.Filters import LoKi_Filters
    fltrs = LoKi_Filters ( VOID_Code  = " 0.5 < CONTAINS ('%s') " % Line )

  
    daVinci = DaVinci (
        DataType        = options.DataType         ,
        Simulation      = options.Simulation       ,
        EventPreFilters = fltrs.filters('Filters') ,
        InputType       = InputType                ,
        Lumi            = options.Lumi  
        )
    #
    ## check input type
    #
    if options.MicroDST or 'MDST' == ext : 
        logger.info ( 'Define input type as micro-DST' )
        DaVinci.InputType = 'MDST'
    
    if not options.Simulation : 
        if options.DataType in ( '2010' , '2011' , '2012' ) :
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
                        if os.path.exists ( os.environ['LHCBGRIDCONFIGROOT'] ) :
                            #
                            ## Use Oracle if possible
                            CondDB ( UseOracle = True  )
                            logger.info('Oracle DB will be used')
                            

    if options.RootInTES: 
        # ------- decoding set-up start ----------
        #from MicroDSTConf.TriggerConfUtils import configureL0AndHltDecoding
        #from Bender.uDstTisTos import configureL0AndHltDecoding
        from BenderTools.MicroDST import uDstConf 
        uDstConf(options.RootInTES)
        logger.info('Reconfigure uDST')
        # ------- decoding set-up end  -----------
        
    
    ## get xml-catalogs (if specified) 
    catalogs = [ options.XmlCatalogue ] if options.XmlCatalogue else []

    ## import all:
    from Bender.Main import *
    
    ## set input data
    setData ( Files , catalogs , options.Castor  )

    ## instantiate the application manager
    gaudi=appMgr ()
    
    rootInTES = options.RootInTES 

    ## Set properties of the TisTosTools
    if rootInTES : 
        for t in ( gaudi.tool ( 'CheckTrg.L0TriggerTisTos' ) ,
                   gaudi.tool ( 'CheckTrg.TriggerTisTos'   ) ) : 
            t . UseParticle2LHCbIDsMap = 2
            t . PropertiesPrint        = False
            t . TriggerInputWarnings   = True
    ##
    inputLine =  Line
    Alg = chkTrg()
    if rootInTES :
        inputLine =  inputLine[ len(rootInTES) + 1 : ] 
        alg = Alg (
            'CheckTrg'                       ,
            Inputs           = [ inputLine ] ,
            PropertiesPrint  = False         , 
            RootInTES        = rootInTES
            ) 
    else :
        alg = Alg (
            'CheckTrg' ,
            PropertiesPrint  =   False , 
            Inputs           = [ inputLine ]
            )
        
    #
    mainSeq = gaudi.algorithm ('GaudiSequencer/DaVinciUserSequence', True )
    mainSeq.Members += [ alg.name() ]
        
    ## initialize and read the first 1000 event
    gaudi.run( options.Nevents )

    
    # dod = gaudi.service('DataOnDemandSvc' )
    # dod.Dump = True
    
    print 90*'*'
    logger.info ( '  Line     : %s' %  Line     ) 
    logger.info ( '  RootInTES: %s' % rootInTES ) 
    print 90*'*'
    
    alg.trgDecs()

# =============================================================================
# The END 
# =============================================================================

