#!/usr/bin/env python 
# =============================================================================
# $Id$ 
# =============================================================================
## @file Bender/CheckTrg.py
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
#    check-trg the_line line file1 [ file2 [ file3 [ file4 ....'
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

    check-trg [options] line file1 [ file2 [ file3 [ file4 ....'

"""
# =============================================================================
__author__  = 'Vanya BELYAEV Ivan.Belyaev@nikhef.nl'
__date__    = "2010-09-10"
__version__ = '$Revision$'
__all__     = ()  ## nothing to import 
__usage__   = 'dst_explorer [options] file1 [ file2 [ file3 [ file4 ....'
# =============================================================================
import ROOT 


## postpone the massive import from Bender 
def chkTrg  ( ) :
    
    """
    create the algorithnm for trigger checks 
    """
    from   Bender.Main    import Algo, SUCCESS 
    import Bender.TisTos          ## add methods for TisTos 
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

    from optparse import OptionParser as OptParser
    parser = OptParser( usage   = __usage__   ,
                        version = ' %prog '  + __version__ )
    
    ##
    parser.add_option (
        '-t'                  ,
        '--datatype'          ,
        dest    = 'DataType'  ,
        type    = 'str'       , 
        help    = "``DataType''    attribute to be specified for DaVinci [default : %default]" ,
        default = '2011'  
        )
    ## 
    parser.add_option (
        '-s'                          ,
        '--simulation'                ,
        action  = "store_true"        ,
        dest    = 'Simulation'        ,
        help    = "``Simulation''  flag to be propagated to DaVinci" ,
        default = False   
        )
    ## 
    parser.add_option (
        '-c'                          ,
        '--castor'                    ,
        action  = "store_true"        ,
        dest    = 'Castor'            ,
        help    = "Enable direct access to Castor Grid Storage to access LFN-files" ,
        default = True   
        )
    ## 
    parser.add_option (
        '-p'                          ,
        '--persistency'               ,
        type    = 'str'               , 
        dest    = 'Persistency'       ,
        help    = "``Persistency'' attribute for DaVinci [default : %default]" ,
        default = 'ROOT'                  
        )
    ## 
    parser.add_option (
        '-x'                          ,
        '--xml'                        ,
        dest    = 'XmlCatalogue'       ,
        help    = "``XmlCatalog'' to be transferred to setData-function [default : %default]" ,
        default = ''                  
        )
    ## 
    parser.add_option (
        '-q'                          ,
        '--quiet'                     ,
        action  = "store_true"        ,
        dest    = 'Quiet'             ,
        help    = "``Quiet'' processing"  ,
        default = False   
        )
    ## 
    parser.add_option (
        '-m'                          ,
        '--micro'                     ,
        action  = "store_true"        ,
        dest    = 'MicroDST'          ,
        help    = "MicroDST format?"  ,
        default = False   
        )
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
        print ' Trivial Bender-based script to explore the content of (x,mu,s,r,...)DSTs '
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
        
    if not 0 <= Line.find ('Phys') :
        parser.error ( 'Invalid line name %s' % options.Line  ) 

    if     0 != Line.find ('/Event/') :
        parser.error ( 'Invalid line name %s' % options.Line  ) 

    if not 0 <= Line.find ('/Particles') :
        Line = options.Line + '/Particles'
        
    if 0>= options.Nevents <= 0 and -1 != nEvents : 
        nEvents = 1000
    #
    ## data type for MC 
    #
    if options.Simulation and '2009' == options.DataType :
        options.DataType = 'MC09'
    if options.Simulation and '2010' == options.DataType :
        options.DataType = 'MC10'            

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
    ## check input type
    #
    micro = False
    for a in arguments :
        if 0 <= a.find ('.mdst') or \
           0 <= a.find ('.MDST') or \
           0 <= a.find ('.uDST') or \
           0 <= a.find ('.uDst') or \
           0 <= a.find ('.udst')  : micro = True     
    InputType = 'MDST' if micro or options.MicroDST else 'DST'

    #
    ## add filters:
    #
    from PhysConf.Filters import LoKi_Filters
    fltrs = LoKi_Filters ( VOID_Code  = " 0.5 < CONTAINS ('%s') " % Line )
    
    daVinci = DaVinci (
        DataType        = options.DataType         ,
        Simulation      = options.Simulation       ,
        Persistency     = options.Persistency      ,
        EventPreFilters = fltrs.filters('Filters') ,
        UseTrigRawEvent = True                     , 
        InputType       = InputType                ,
        Lumi            = False          
        )
    
    if not options.Simulation and options.DataType in ( '2010' , '2011' ) :
        #
        ## try to use the latest availabel tags:
        #
        from Configurables import CondDB    
        CondDB ( UseLatestTags = [ options.DataType ] )
        import os 
        if os.environ.has_key('LHCBGRIDSYSROOT') :
            if os.environ.has_key('LHCBGRIDCONFIGROOT') :
                if os.path.exists ( os.environ['LHCBGRIDSYSROOT'] )  :
                    if os.path.exists ( os.environ['LHCBGRIDCONFIGROOT'] )  :
                        #
                        ## Use Oracle if possible
                        CondDB ( UseOracle = True  )


    rootInTES = ''
    if micro or options.MicroDST :
        pos       = Line.find ( '/Phys')
        rootInTES = Line[:pos]
        
    if rootInTES: 
        # ------- decoding set-up start ----------
        from MicroDSTConf.TriggerConfUtils import configureL0AndHltDecoding
        configureL0AndHltDecoding(rootInTES)
        # ------- decoding set-up end  -----------
        
    
    ## get xml-catalogs (if specified) 
    catalogs = [ options.XmlCatalogue ] if options.XmlCatalogue else []

    ## import all:
    from Bender.Main import *
    
    ## set input data
    setData ( Files , catalogs , options.Castor  )

    ## instantiate the application manager
    gaudi=appMgr ()
    
    ## Set properties of the TisTosTools
    if rootInTES: 
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
            'CheckTrg' ,
            Inputs    = [ inputLine ] ,
            PropertiesPrint  = False , 
            RootInTES = rootInTES
            ) 
    else :
        alg = Alg (
            'CheckTrg' ,
            PropertiesPrint  = False , 
            Inputs    = [ inputLine ]
            )
        
    #
    mainSeq = gaudi.algorithm ('GaudiSequencer/DaVinciUserSequence', True )
    mainSeq.Members += [ alg.name() ]

    
    
    ## initialize and read the first 1000 event
    gaudi.run( options.Nevents )

    
    # dod = gaudi.service('DataOnDemandSvc' )
    # dod.Dump = True
    
    print 'Line     : ' , Line
    print 'RootInTES: ' , rootInTES
    
    alg.trgDecs()

# =============================================================================
# The END 
# =============================================================================

