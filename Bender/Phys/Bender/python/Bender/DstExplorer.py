#!/usr/bin/env python 
# =============================================================================
# $Id: DstExplorer.py,v 1.1 2010-09-10 13:11:31 ibelyaev Exp $ 
# =============================================================================
# $URL$
# =============================================================================
## @file Bender/DstExplorer.py
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
#  with the campain of Dr.O.Callot et al.: 
#  ``No Vanya's lines are allowed in LHCb/Gaudi software''
#
# Usage:
#
# @code
#
#    dst_explorer [options] file1 [ file2 [ file3 [ file4 ....'
#
#       >>> ls('/Event')
#       >>> ls('/Event/MicroDST')
#       >>> ls('/Event/MicroDST/Dimuon')
#       >>> ls('/Event/MicroDST/Dimuon/Phys')
#       >>> ls('/Event/MicroDST/Dimuon/Phys/SelBs2KKMUMU')
#       >>> bs = get('/Event/MicroDST/Dimuon/Phys/SelBs2KKMUMU/Particles')
#       >>> for b in bs : print b.decay()
#       >>> run(1)
#       >>> ls('/Event')
#
#  @endcode 
#
#  @date   2010-09-10
#  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
#
#  Last modification $Date: 2010-09-10 13:11:31 $
#                 by $Author: ibelyaev $
# =============================================================================
"""
Trivial Bender-based script to explore the content of (x,mu,s,r,...)DSTs

This file is a part of BENDER project:

  ``Python-based Interactive Environment for Smart and Friendly Physics Analysis''

The project has been designed with the kind help from Pere MATO and Andrey TSAREGORODTSEV. 

And it is based on the LoKi project:
 
   ``C++ ToolKit for Smart and Friendly Physics Analysis''

By usage of this code one clearly states the disagreement with the campain of Dr.O.Callot et al.:

   ``No Vanya's lines are allowed in LHCb/Gaudi software''

Usage:

  dst_explorer [options] file1 [ file2 [ file3 [ file4 ....'

       >>> ls('/Event')
       >>> ls('/Event/MicroDST')
       >>> ls('/Event/MicroDST/Dimuon')
       >>> ls('/Event/MicroDST/Dimuon/Phys')
       >>> ls('/Event/MicroDST/Dimuon/Phys/SelBs2KKMUMU')
       >>> bs = get('/Event/MicroDST/Dimuon/Phys/SelBs2KKMUMU/Particles')
       >>> for b in bs : print b.decay()
       >>> run(1)
       >>> ls('/Event') 

Last modification $Date: 2010-09-10 13:11:31 $
               by $Author: ibelyaev $

"""
# =============================================================================
__author__  = 'Vanya BELYAEV Ivan.Belyaev@nikhef.nl'
__date__    = "2010-09-10"
__version__ = '$Revision: 1.1 $'
__all__     = ()  ## nothing to import 
__usage__   = 'dst_explorer [options] file1 [ file2 [ file3 [ file4 ....'
# =============================================================================

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
        default = '2010'  
        )
    ## 
    parser.add_option (
        '-s'                          ,
        '--simulation'                ,
        action  = "store_true"        ,
        dest    = 'Simulation'        ,
        help    = "``Simulation''  flag to be propagated to DaVicnci" ,
        default = False   
        )
    ## 
    parser.add_option (
        '-l'                          ,
        '--level'                     ,
        type    = 'int'               , 
        dest    = 'OutputLevel'       ,
        help    = "``OutputLevel'' attribute for ApplicationMgr/MessageSvc [default : %default]" ,
        default = 3                  
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

    (options,arguments) = parser.parse_args() 

    ## redefine output level for 'quiet'-mode
    if options.OutputLevel > 5 : options.OutputLevel = 5
    if options.OutputLevel < 0 : options.OutputLevel = 0
    if options.Quiet and 4 > options.OutputLevel :
        options.OutputLevel = 4
        
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
    if not arguments : parser.error('No input files are specified') 

    #
    ## start the actual action:
    #

    from Bender.Main         import *
    from Gaudi.Configuration import * 
    from Configurables       import DaVinci
    
    daVinci = DaVinci (
        DataType   = options.DataType   ,
        Simulation = options.Simulation ,
        Lumi       = False              
        )

           
    ## Reset all DaVinci sequences 
    def _action ( ) :
        """ Reset all DaVinci sequences """
        for seq in ( 'DaVinciInitSeq'      ,
                     'DaVinciMainSequence' ,
                     'DaVinciSequence'     ,
                     'MonitoringSequence'  ) :
            
            cSeq = getConfigurable( seq )
            if cSeq and hasattr ( cSeq , 'Members' ) : cSeq.Members = []

            ## reset the list of top-level algorithms 
            from Configurables import ApplicationMgr
            a = ApplicationMgr()
            a.TopAlg = []
            a.OutputLevel = options.OutputLevel
            
            from Configurables import MessageSvc
            m = MessageSvc ( OutputLevel = options.OutputLevel )
            

    appendPostConfigAction ( _action )
    
    ## instantiate the application manager 
    gaudi=appMgr()

    ## set input data 
    setData ( arguments , [] )

    ## initialize and read the first event
    gaudi.run(1)

    
    
# =============================================================================
# The END 
# =============================================================================

