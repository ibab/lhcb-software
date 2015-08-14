#!/usr/bin/env python 
# -*- coding: utf-8 -*-
# =============================================================================
# $Id$ 
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
#                    $Revision$
#  Last modification $Date$
#                 by $Author$
# =============================================================================
"""Trivial Bender-based script to dump the content of (x,mu,s,r,...)DSTs

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

    dst_dump [options] file1 [ file2 [ file3 [ file4 ....'


                  $Revision$
Last modification $Date$
               by $Author$

"""
# =============================================================================
__author__  = 'Vanya BELYAEV Ivan.Belyaev@itep.ru'
__date__    = "2011-10-24"
__version__ = '$Revision$'
__all__     = ()  ## nothing to import 
__all__     =  ( 'dumpDst' , )
# =============================================================================
## logging
# =============================================================================
from Bender.Logger import getLogger
if '__main__' == __name__ : logger = getLogger ( 'BenderTools.DstDump' )
else                      : logger = getLogger ( __name__ )
    
# =============================================================================
## table format 
_fhdr_ = ' | %8s | %15s | %7s | %4s | %6s |'                 ## header  
_fmt1_ = ' | %8g | %7.1f+-%-6.1f | %-7.1f | %4d | %-6d |'  
_fmt2_ = ' | %8d | %7d%8s | %7s | %4s | %6s |'
# =============================================================================
def formatItem ( item ) : 
    if item.Min() != item.Max() or 0 != item.rms() or  0 != item.meanErr() :
        return  _fmt1_ % ( item.sum     () ,
                           item.mean    () ,
                           item.meanErr () ,
                           item.rms     () ,
                           long ( item.Min () ) ,
                           long ( item.Max () ) )
    else :
        return _fmt2_ % ( long ( item.sum  ().value() ) ,
                          long ( item.mean ().value() ) , '' , '' , '', '' ) 
# =============================================================================
## dump it ! 
#  @date   2011-10-24
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
def dumpDst ( config ) :
    ##
    logger.info ( 100*'*') 
    if config.Quiet : 
        logger.info(' Trivial Bender-based script to explore the content of (x,mu,s,r,...)DSTs ' ) 
    else :
        logger.info ( 100*'*')
        logger.info ( __doc__ ) 
        logger.info ( 100*'*')
        logger.info ( ' Author  : %s ' % __author__   ) 
        logger.info ( ' Version : %s ' % __version__  ) 
        logger.info ( ' Date    : %s ' % __date__     )
        logger.info ( 100*'*')
        
    ## The input files mus
    if not config.files : parser.error ( 'No input files are specified' )

    from BenderTools.DstExplorer import configure 
    configure ( config ) 

    from BenderTools.Utils import totalSilence
    totalSilence( dod = config.DataOnDemand )
    
    from Bender.Main import appMgr, run, cpp
    if config.Simulation : import Bender.MainMC

    root = config.RootInTES

    #
    ## instantiate the application manager
    #
    gaudi=appMgr ()

    import LHCbMath.Types 
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

    while iEvent != config.nEvents :
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
        if config.FollowLinks: 
            links = links - nodes 
            for loc in links:
            
                if config.RootInTES :
                    if 0 != loc.find ( config.RootInTES ) : continue 
                
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
        if config.DataOnDemand :
        
            for k in dodSvc.AlgMap .keys () : dods.add ( k ) 
            for k in dodSvc.NodeMap.keys () :
                obj = dodSvc.NodeMap[k]
                if 'DataObject' == obj : continue 
                dods.add ( k )
            
            dods = dods - nodes 
            dods = dods - links
        
            for loc in dods :

                if config.RootInTES :
                    if 0 != loc.find ( config.RootInTES ) : continue 
                
                if not config.Simulation :
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

    message = _fhdr_ % ( 'Total ' , '     Mean     ' , '  rms  ' ,  'min' , ' max ' ) 
    message = " | %s %s" % ( 'Location'.ljust(length) , message )

    _printMessage += [ message ] 
    _printMessage += [ lline   ]

    for loc in keys :
        item     = nObjects[loc]

        l = loc.replace('/Event/','')

        ## the actual formating 
        message = formatItem ( item )
        
        message = " | %s %s" % ( l.ljust(length) , message )
    
        _printMessage += [ message ] 
        
    _printMessage += [ lline ]     
    _printMessage += [ "   Analysed " + str(iEvent) + " events" ] 


    logger.info ( 100*'*')
    print '\n\n\n'
    ofile  = open ( config.SummaryFile , 'w' )     
    for line in _printMessage :
        print           line   
        print >> ofile, line 
    ofile.close()
    print '\n\n\n'
    logger.info ( 100*'*')

# =============================================================================
if '__main__' == __name__ :
    
    logger.info ( 100*'*')
    logger.info ( __doc__ ) 
    logger.info ( 100*'*')
    logger.info ( ' Author  : %s ' % __author__   ) 
    logger.info ( ' Version : %s ' % __version__  ) 
    logger.info ( ' Date    : %s ' % __date__     )
    logger.info ( 100*'*')
    
    dumpDst ()

    logger.info ( 100*'*')
    
# =============================================================================
# The END 
# =============================================================================

   
