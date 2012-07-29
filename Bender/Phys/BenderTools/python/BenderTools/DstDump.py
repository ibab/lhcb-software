#!/usr/bin/env python 
# =============================================================================
# $Id: DstDump.py 138072 2012-04-04 14:41:10Z ibelyaev $ 
# =============================================================================
## @file BenderTools/DstDump.py
#
#  Trivial Bender-based script to dump the content of (x,mu,s,r,...)DSTs
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
#    dst_dump [options] file1 [ file2 [ file3 [ file4 ....'
#
#  @endcode 
#
#  @date   2011-10-24
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#
#                    $Revision: 138072 $
#  Last modification $Date: 2012-04-04 16:41:10 +0200 (Wed, 04 Apr 2012) $
#                 by $Author: ibelyaev $
# =============================================================================

"""
Trivial Bender-based script to dump the content of (x,mu,s,r,...)DSTs

This file is a part of BENDER project:

  ``Python-based Interactive Environment for Smart and Friendly Physics Analysis''

The project has been designed with the kind help from Pere MATO and Andrey TSAREGORODTSEV. 

And it is based on the LoKi project:
 
   ``C++ ToolKit for Smart and Friendly Physics Analysis''

By usage of this code one clearly states the disagreement with the smear campaign of Dr.O.Callot et al.:

   ``No Vanya's lines are allowed in LHCb/Gaudi software''

Usage:

    dst_dump [options] file1 [ file2 [ file3 [ file4 ....'


                  $Revision: 138072 $
Last modification $Date: 2012-04-04 16:41:10 +0200 (Wed, 04 Apr 2012) $
               by $Author: ibelyaev $

"""
# =============================================================================
__author__  = 'Vanya BELYAEV Ivan.Belyaev@cern.ch'
__date__    = "2011-10-24"
__version__ = '$Revision: 138072 $'
__all__     = ()  ## nothing to import 
__usage__   = 'dst_dump [options] file1 [ file2 [ file3 [ file4 ....'
# =============================================================================
## logging
# =============================================================================
from Bender.Logger import getLogger 
logger = getLogger( __name__ )
# =============================================================================
from BenderTools.Parser      import makeParser
from BenderTools.DstExplorer import configure 
# =============================================================================
if '__main__' == __name__ :
    
    print 120*'*'
    print ' Author  : ', __author__ 
    print ' Version : ', __version__ 
    print ' Date    : ', __date__ 

    parser = makeParser  ( __usage__ , __version__ )
    
    parser.add_option (
        '-n'                          ,
        '--events'                    ,
        dest    = 'nEvents'           ,
        type    = 'int'               , 
        help    = "Number of events to process " ,
        default = -1    
        )

    parser.add_option (
        '-z'                         ,
        '--summary'                  ,
        dest    = 'SummaryFile'      ,
        help    = "Output file with dst-summary" ,
        type    = 'str'              , 
        default = 'dst_summary.txt'    
        )

    parser.add_option (
        '-f'                         ,
        '--follow'                   ,
        dest    = 'FollowLinks'      ,
        help    = "Flag to follow links, useful for packed (u)DST (*)" ,
        action  = "store_true"       ,
        default = False    
        )
    
    parser.add_option (
        '-d'                         ,
        '--dod'                   ,
        dest    = 'DataOnDemand'      ,
        help    = "Dump the known DOD-locations (fragile!), (+)" ,
        action  = "store_true"       ,
        default = False    
        )
    
    options , arguments = parser.parse_args() 
    
    print 120*'*'
    if options.Quiet :
        print ' Trivial Bender-based script to explore the content of (x,mu,s,r,...)DSTs '
    else :
        print __doc__

    
    ## The input files must be specified!
    if not arguments : parser.error ( 'No input files are specified' ) 
    
    options.Quiet = True 
    configure ( options , arguments ) 

    from Configurables import MessageSvc, DataOnDemandSvc, ToolSvc 
    from Configurables import Gaudi__RootCnvSvc    as RootCnvSvc 
    from Configurables import Gaudi__IODataManager as IODataManager
    from Configurables import LHCb__RawDataCnvSvc  as RawDataCnvSvc 
        
    msg = MessageSvc()
    msg.OutputLevel = 5
    
    ToolSvc           (                                  OutputLevel = 6 )
    RootCnvSvc        ( "RootCnvSvc"                   , OutputLevel = 6 )
    RawDataCnvSvc     (                                  OutputLevel = 6 )
    
    IODataManager     ( 'IODataManager'                , OutputLevel = 6 ,
                        AgeLimit = 1 , UseGFAL = False )
    
    if options.DataOnDemand :
        DataOnDemandSvc   ( Dump = True  )
    else :
        DataOnDemandSvc   ( Dump = False , OutputLevel = 6 )
        msg.setFatal += [ 'DataOnDemandSvc' ] 
        
    msg.setFatal += [ 'RootCnvSvc'          ,
                      'IOManagerSvc'        ,
                      'RootHistSvc'         ,
                      'LHCb::RawDataCnvSvc' ]
    
    if options.Simulation : from Bender.MainMC   import *
    else                  : from Bender.Main     import *



    root = options.RootInTES
    ## guess a bit about structure 
    ##if not options.RootInTES :
    ##    if hasInFile ( arguments , 'EW.DST' ) :
            
    #
    ## instantiate the application manager
    #
    gaudi=appMgr ()
    
    #
    ## copy the lines from Juan's script check_dst_contents.py
    #
    evtSvc = gaudi.evtSvc()
    SE     = cpp.StatEntity 
    def addEntry ( dct , key , val ) :
        if not dct.has_key(key) : dct[key] = SE()             
        dct[key] += float(val)
            
    dodSvc = gaudi.service('DataOnDemandSvc')
    
    nSelEvents = {}
    nObjects   = {}

    iEvent = 0

    while iEvent != options.nEvents :
        #
        sc = run(1)
        if sc.isFailure()       : break
        #
        if not evtSvc['/Event'] : break
        ##
        iEvent += 1
        #
        nodes = evtSvc.nodes ( node      = root ,
                               forceload = True )
        if not nodes :
            logger.warning ( "No nodes are selected for Root:'%s'" % root  )

        nodes = set ( nodes ) 
        links = set ()
        dods  = set ()

        #
        ## explore the regular nodes
        #
        for loc in nodes :
            data = evtSvc[loc]
            loc = loc[:7] + ' ' + loc[7:] 
            if not data :
                addEntry ( nSelEvents , loc , 0 )
                addEntry ( nObjects   , loc , 0 )
            elif type( data ) == cpp.DataObject  : continue 
            else :
                #
                lnks = data.links()
                for l in lnks : links.add ( l )
                #
                if   hasattr ( data , 'size'   ) : addEntry ( nObjects , loc , data.size()  )
                elif hasattr ( data , '__len__') : addEntry ( nObjects , loc , len ( data ) )
                else                             : addEntry ( nObjects , loc , 1            )
                
        #
        ## follow the links? Useful for packed (u)DST
        #
        if options.FollowLinks: 
            links = links - nodes 
            for loc in links:

                if options.RootInTES :
                    if 0 != loc.find ( options.RootInTES ) : continue 
                
                data = evtSvc[loc]
                loc = loc[:7] + '*' + loc[7:] 
                if not data :
                    addEntry ( nSelEvents , loc , 0 )
                    addEntry ( nObjects   , loc , 0 )
                elif type( data ) == cpp.DataObject : continue 
                else :
                    #
                    if   hasattr ( data , 'size'   ) : addEntry ( nObjects , loc , data.size()  )
                    elif hasattr ( data , '__len__') : addEntry ( nObjects , loc , len ( data ) )
                    else                             : addEntry ( nObjects , loc , 1            )
                    
        #
        ## explore locations known for DOD
        #
        if options.DataOnDemand :
            
            for k in dodSvc.AlgMap .keys () : dods.add ( k ) 
            for k in dodSvc.NodeMap.keys () :
                obj = dodSvc.NodeMap[k]
                if 'DataObject' == obj : continue 
                dods.add ( k )
            
            dods = dods - nodes 
            dods = dods - links
            
            for loc in dods :

                if options.RootInTES :
                    if 0 != loc.find ( options.RootInTES ) : continue 
                
                if not options.Simulation :
                    if 0 <= loc.find ( 'MC'   ) : continue
                    if 0 <= loc.find ( 'Prev' ) : continue
                    if 0 <= loc.find ( 'Next' ) : continue
                    
                data = evtSvc[loc]
                loc = loc[:7] + '+' + loc[7:] 
                if not data :
                    addEntry ( nSelEvents , loc , 0 )
                    addEntry ( nObjects   , loc , 0 )
                elif type( data ) == cpp.DataObject : continue 
                else :
                    #
                    if   hasattr ( data , 'size'   ) : addEntry ( nObjects , loc , data.size()  )
                    elif hasattr ( data , '__len__') : addEntry ( nObjects , loc , len ( data ) )
                    else                             : addEntry ( nObjects , loc , 1            )

                
    keys = nObjects.keys()
    keys.sort()

    length  = 25
    for key in keys : length = max ( length , len  ( key ) ) 
    length += 2-7
        
    _printMessage = []
    
    lline   = ' +' + ( length + 58 ) * '-' + '+'
    _printMessage += [ lline ] 
    
    message = ' | %8s | %15s | %7s | %4s | %6s |' % ( 'Total ' , '     Mean     ' , '  rms  ' ,  'min' , ' max ' ) 
    message = " | %s %s" % ( 'Location'.ljust(length) , message )
    
    _printMessage += [ message ] 
    _printMessage += [ lline   ]
    
    for loc in keys :
        item     = nObjects[loc]

        l = loc.replace('/Event/','')
        
        if item.Min() != item.Max() or 0 != item.rms() or  0 != item.meanErr() : 
            message = ' | %8g | %7.1f+-%-6.1f | %-7.1f | %4d | %-6d |' % ( item.sum     () ,
                                                                           item.mean    () ,
                                                                           item.meanErr () ,
                                                                           item.rms     () ,
                                                                           long ( item.Min () ) ,
                                                                           long ( item.Max () ) )
        else :
            message = ' | %8d | %7d%8s | %7s | %4s | %6s |' % ( long ( item.sum  ()  ) ,
                                                                long ( item.mean ()  ) , '' , '' , '', '' ) 
            
        message = " | %s %s" % ( l.ljust(length) , message )

        _printMessage += [ message ] 
        
    _printMessage += [ lline ]     
    _printMessage += [ "   Analysed " + str(iEvent) + " events" ] 

    
    print '\n\n\n'
    ofile  = open ( options.SummaryFile , 'w' )     
    for line in _printMessage :
        print           line   
        print >> ofile, line 
    ofile.close()
    print '\n\n\n'
    
    
# =============================================================================
# The END 
# =============================================================================

   
