#!/usr/bin/env python 
# -*- coding: utf-8 -*-
# =============================================================================
# $Id$ 
# =============================================================================
## @file BenderTools/Seek.py
#
#  Set  of utiltiles to seek the events in DST-events 
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
#  @date   2012-06-12
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#
#                    $Revision$
#  Last modification $Date$
#                 by $Author$
# =============================================================================
""" Set  of utiltiles to seek the events in DST-events 

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

"""
# =============================================================================
__author__  = 'Vanya BELYAEV Ivan.Belyaev@itep.ru'
__date__    = "2012-06-12"
__version__ = '$Revision$'
__all__     = (
    ##
    'seekVoidDecision'  ,
    'seekAlgDecision'   ,
    'seekForData'       ,
    'seekHltDecision'   ,
    'seekStripDecision' ,
    'seekForODIN'       ,
    'seekForEvtRun'     ,
    ##
    )  
# =============================================================================
## logging
# =============================================================================
from Bender.Logger import getLogger 
if '__main__' == __name__ : logger = getLogger ( 'BenderTools.Seek' )
else                      : logger = getLogger ( __name__ )
# =============================================================================
## seek the decision for the certain void-functor
#  e.g. find event with time before June 
#  @code
#  Time = cpp.Gaudi.Time 
#  fun  =  lambda : Time( get('/Event/DAQ/ODIN').gspTime()*1000).month(local)<=Time.June
#  ok,n = seekForVoidDecision ( fun , 100000 )
#  @code 
def seekVoidDecision ( fun , EvtMax = 1000 ) :
    """
    Seek the Decision for the certain void-functor/function
    e.g. find event with time before June 
    >>> Time = cpp.Gaudi.Time   e.g. find event with time before June 
    >>> fun  =  lambda : Time( get('/Event/DAQ/ODIN').gspTime()*1000).month(local)<=Time.June 
    >>> ok,n = seekForVoidDecision ( fun , 100000 ) 
    """
    from Bender.Utils import appMgr, run 
    #
    _g   = appMgr()
    _evt = _g.evtSvc()

    evt  = 0
    ok   = False
    
    while _evt['/Event'] and evt < EvtMax :
        ok  = fun()
        if ok : return True, evt  
        run(1)
        evt += 1 
        
    return ok, evt  

# =============================================================================
## seek the decision for the certain algorithm
def seekAlgDecision ( alg , EvtMax = 1000 ) :
    """
    Seek the Decision for the certains algorithm    
    """

    fun = alg
    
    if isinstance ( alg , str ) :
        import LoKiNumbers.decorators 
        import LoKiHlt.decorators 
        from LoKiHlt.algorithms import ALG_EXECUTED, ALG_PASSED
        fun = ALG_EXECUTED  ( alg ) & ALG_PASSED ( alg )
        
    return seekVoidDecision ( fun ) 

# =============================================================================
## 
def _dataOk_ ( o ) :
    #
    if not o : return False
    #
    if hasattr ( o , 'size' ) :
        return True if 0 < o.size() else False
    #
    return True


# =============================================================================
## seek for data
def seekForData ( location       ,
                  EvtMax = 10000 ) :
    """
    Seek for existence of some (non-empty) data
    
    >>> ps , evt = seekForData ('/Event/Phys/MyDecay/Particles')
    
    """
    from Bender.Utils import appMgr, run 
    #    
    _cnt      = 0
    _g        = appMgr()
    _evt      = _g.evtSvc()

    while _evt['/Event'] and _cnt < EvtMax :        
        _obj      = _evt[ location ]
        if _dataOk_ ( _obj ) : return _obj, _cnt
        run(1)
        _cnt += 1
        
    return None, _cnt 

# =============================================================================
## seek for some ``ODIN''-expression
def seekForODIN     ( criterion                      ,
                      EvtMax     = 10000             ,
                      disableAll = False             ,
                      location   = '/Event/DAQ/ODIN' ) :
                      
    from Bender.Utils import appMgr, run, get
    #    
    _g        = appMgr()
    _evt      = _g.evtSvc()
        
    evt = 0

    ## disable the algorithms 
    _disabled = _g.disableAllAlgs() if disableAll else _g.disableTopAlgs()

    try : 
        while  _evt['/Event'] and evt < EvtMax :
            
            odin = get ( location )
            if not odin : return False , evt     ## RETURN
            ok   =  criterion ( odin )
            if ok       : return True  , evt     ## RETURN 
            run(1)
            evt += 1
            
    finally :
        for _a in _disabled :
            _a.setEnabled ( True )
        
        
    return ok, evt

# =============================================================================
## seek for some  event-run (list)
def seekForEvtRun  ( runevt_list                    ,
                     EvtMax     = 10000             ,
                     disableAll = False             ,
                     location   = '/Event/DAQ/ODIN' ) :
    """
    Seek for event in runevent list
    
    """
    from LoKiHlt.decorators import odin_runevts
    
    return seekForODIN ( odin_runevts ( runevt_list ) ,
                         EvtMax     ,
                         disableAll ,
                         location   ) 

    
# =============================================================================
## seek the decision for the certain Trigger Line 
def seekHltDecision ( expr                                 ,
                      EvtMax     = 10000                   ,
                      disableAll = False                   ,
                      location   = '/Event/Hlt/DecReports' ) :
    """
    Seek for  the event with certain Hlt-decision 
    """
    from Bender.Utils       import appMgr, run, get
    from LoKiHlt.decorators import HLT_PASS_RE 

    fun = HLT_PASS_RE ( expr ) if isinstance ( expr , str ) else expr 
        
    evt = 0

    _g        = appMgr()
    _evt      = _g.evtSvc()
    
    ## disable the algorithms 
    _disabled = _g.disableAllAlgs() if disableAll else _g.disableTopAlgs()
    
    try : 
        while  _evt['/Event'] and evt < EvtMax :
            
            hlt = get ( location )
            if not hlt : return False , evt     ## RETURN
            ok  = fun ( hlt )
            if ok      : return True  , evt     ## RETURN 
            run(1)
            evt += 1
            
    finally :
        for _a in _disabled :
            _a.setEnabled ( True )
        
        
    return ok, evt

# =============================================================================
## seek the decision for the certain stripping line 
def seekStripDecision ( expr                                        ,
                        EvtMax     = 10000                          ,
                        disableAll = False                          ,
                        location   = '/Event/Strip/Phys/DecReports' ) :
    """
    Seek the decision for the certain Stripping Line
    """
    return seekHltDecision ( expr       ,
                             EvtMax     ,
                             disableAll , 
                             location   ) 

        
# =============================================================================
if __name__ == '__main__' :

    logger.info ( 80*'*'  ) 
    logger.info ( __doc__ ) 
    logger.info ( ' Author  : %s ' %  __author__  ) 
    logger.info ( ' Version : %s ' %  __version__ ) 
    logger.info ( ' Date    : %s ' %  __date__    ) 
    logger.info ( ' Symbols : %s ' %  list ( __all__ ) ) 
    logger.info ( 80*'*'  ) 

# =============================================================================
# The END 
# =============================================================================

