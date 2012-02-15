#!/usr/bin/env python 
# =============================================================================
# $Id$ 
# =============================================================================
## @file Bender/DstDump.py
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
#                    $Revision$
#  Last modification $Date$
#                 by $Author$
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


                  $Revision$
Last modification $Date$
               by $Author$

"""
# =============================================================================
__author__  = 'Vanya BELYAEV Ivan.Belyaev@cern.ch'
__date__    = "2011-10-24"
__version__ = '$Revision$'
__all__     = ()  ## nothing to import 
__usage__   = 'dst_dump [options] file1 [ file2 [ file3 [ file4 ....'
# =============================================================================
from Bender.DstExplorer import makeParser, configure 

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
        '-o'                         ,
        '--output'                   ,
        dest    = 'OutputFile'       ,
        help    = "Output file with dst-summary" ,
        type    = 'str'              , 
        default = 'dst_summary.txt'    
        )
    
    options , arguments = parser.parse_args() 
    
    print 120*'*'
    if options.Quiet :
        print ' Trivial Bender-based script to explore the content of (x,mu,s,r,...)DSTs '
    else :
        print __doc__

    
    ## Files must be specfied are mandatory!
    if not arguments : parser.error ( 'No input files are specified' ) 
    
    options.Quiet = True 
    configure ( options , arguments ) 

    from Configurables import MessageSvc, DataOnDemandSvc
    from Configurables import Gaudi__RootCnvSvc    as RootCnvSvc 
    from Configurables import Gaudi__IODataManager as IODataManager
    
    msg = MessageSvc()
    msg.OutputLevel = 5
    
    RootCnvSvc        ( "RootCnvSvc"    , OutputLevel = 6 )
    DataOnDemandSvc   ( Dump = False    , OutputLevel = 6 )
    IODataManager     ( 'IODataManager' , OutputLevel = 6 , AgeLimit = 1 , UseGFAL = False ) 
    
    msg.setFatal += ['RootCnvSvc' , 'DataOnDemandSvc' , 'IOManagerSvc']
    
    if options.Simulation : from Bender.MainMC   import *
    else                  : from Bender.Main     import *

    
    if options.RootInTES and 0 != options.RootInTES.find ( '/Event/' ) :
        options.RootInTES = '/Event/' + options.RootInTES 
        
    ## instantiate the application manager 
    gaudi=appMgr ()
    
    #
    ## copy the lines from Juan's script check_dst_contents.py
    #
    evtSvc = gaudi.evtSvc()
    SE     = cpp.StatEntity 
    def addEntry ( dct , key , val ) :
        if not dct.has_key(key) : dct[key] = SE()             
        dct[key] += float(val)
            

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
        nodes = evtSvc.nodes ( node      = options.RootInTES ,
                               forceload = True              )
        if not nodes :
            print "warning: no nodes are selected for Root:'%s'" % options.RootInTES
            
        for loc in nodes :
            data = evtSvc[loc]
            if not data :
                addEntry ( nSelEvents , loc , 0 )
                addEntry ( nObjects   , loc , 0 )
            else :
                addEntry ( nSelEvents , loc , 1 )
                if   hasattr ( data , 'size'   ) : addEntry ( nObjects , loc , data.size()  )
                elif hasattr ( data , '__len__') : addEntry ( nObjects , loc , len ( data ) )
                else                             : addEntry ( nObjects , loc , 1            )


    keys = nObjects.keys()
    keys.sort()
    
    length  = 25
    for key in keys : length = max ( length , len  ( key ) ) 
    length += 2-7
        
    _printMessage = []
    outputFile    = open ( options.OutputFile, 'w')
    
    lline   = ' +' + ( length + 58 ) * '-' + '+'
    outputFile.write(lline + '\n')
    _printMessage += [ lline ] 
    
    message = ' | %8s | %15s | %7s | %4s | %6s |' % ( 'Total ' , '     Mean     ' , '  rms  ' ,  'min' , ' max ' ) 
    message = " | %s %s" % ( 'Location'.ljust(length) , message )
    
    outputFile.write(message + '\n')
    _printMessage += [ message ] 
    outputFile.write(lline   + '\n')
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

        outputFile.write(message + '\n')
        _printMessage += [ message ] 
        
    outputFile.write(lline + '\n')
    _printMessage += [ lline ] 
    
    outputFile.write(  "   Analysed " + str(iEvent) + " events")
    _printMessage += [ "   Analysed " + str(iEvent) + " events" ] 


    print '\n\n\n'
    for m in _printMessage : print m 
    # print _printMessage
    print '\n\n\n'

    
 
# =============================================================================
# The END 
# =============================================================================

   
