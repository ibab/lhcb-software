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
    'seekHlt1Decision'  ,
    'seekHlt2Decision'  ,
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
def seekVoidDecision ( fun                ,
                       EvtMax     = 1000  ,
                       postAction = None  ,
                       preAction  = None  ,
                       progress   = False ) :
    """Seek the Decision for the certain void-functor/function
    e.g. find event with time before June
    ##
    >>> Time = cpp.Gaudi.Time   e.g. find event with time before June 
    >>> fun  = lambda : get('DAQ/ODIN').eventTime().month(local) <= Time.June 
    >>> ok,n = seekForVoidDecision ( fun , 100000 ) 
    """
    from    Bender.Utils import irun

    for i in irun ( 1                  ,
                    fun                ,
                    postAction         ,
                    preAction          ,
                    EvtMax             ,
                    running = progress ) :

        evnt, iev, nev = i 
        return 1==iev, nev

    return False , EvtMax  

# =============================================================================
## seek the decision for the certain algorithm
def seekAlgDecision ( alg                ,
                      EvtMax     = 1000  ,
                      postAction = None  ,
                      preAction  = None  ,
                      progress   = False ) :
    """Seek the Decision for the certain algorithm
    >>> alg = 'GOODKAON'
    >>> seekAlgDecision ( alg ) 
    """
    
    fun = alg
    
    if isinstance ( alg , str ) :
        import LoKiNumbers.decorators 
        import LoKiHlt.decorators 
        from LoKiHlt.algorithms import ALG_EXECUTED, ALG_PASSED
        fun = ALG_EXECUTED  ( alg ) & ALG_PASSED ( alg )
        
    return seekVoidDecision ( fun        ,
                              EvtMax     ,
                              postAction ,
                              preAction  ,
                              progress   ) 

# =============================================================================
## @class ValidData
#  helper object to find non-empty data
class ValidData(object) :
    """Helper object to find no-empty data
    """
    
    def __init__ ( self , location ) :
        self.location = location
        from Bender.Utils import appMgr 
        _g          = appMgr()
        self.evtSvc = _g.evtSvc()
        
    def __call__ ( self ) :
        # 
        self.data = self.evtSvc[ self.location ]
        #
        if    not self.data                     : return False
        elif  hasattr ( self.data , '__len__' ) : return 0 < len( self.data ) 
        elif  hasattr ( self.data , 'size'    ) : return 0 < self.data.size()
        #
        return True
    
    def __del__ ( self ) :  
        self.data    = None
        self.evtSvc  = None
        del self.data
        del self.evtSvc 

# =============================================================================
## @class GoodData
#  helper object to find good data 
class GoodData(ValidData) :
    """Helper object to find food data  data
    """
    def __init__ ( self , criterion , location ) :
        self.good     = criterion
        ValidData.__init__ ( self , location )
        
    def __call__ ( self ) :
        # 
        self.data = self.evtSvc[ self.location ]
        #
        if not self.data : return False
        return self.good ( self.data )

    def __del__ ( self ) :  
        self.good    = None 
        del self.good 

# =============================================================================
## seek for data
def seekForData ( location           ,
                  EvtMax     = 10000 ,
                  postAction = None  ,
                  preAction  = None  ,
                  progress   = False ) :
    """Seek for existence of some (non-empty) data
    >>> ps , evt = seekForData ('Phys/MyDecay/Particles')    
    """
    dataOK    = ValidData( location )
    ok , nev  = seekVoidDecision ( dataOK     ,
                                   EvtMax     ,
                                   postAction ,
                                   preAction  ,
                                   progress   ) 
    ##
    if ok : ok = dataOK.data
    else  : ok = None
    ##
    return ok, nev

# =============================================================================
## seek for some ``ODIN''-expression
def seekForODIN     ( criterion               ,
                      EvtMax     = 10000      ,
                      disableAll = False      ,
                      location   = 'DAQ/ODIN' ,
                      postAction = None       ,
                      preAction  = None       ,
                      progress   = False      ) :
    
    from Bender.Utils import DisablesAlgos 
    #    
    with DisabledAlgos( disableAll ) :
        
        odinGOOD   = GoodData( criterion , location )
        odin , nev = seekVoidDecision ( odinGOOD   ,
                                        EvtMax     ,
                                        postAction ,
                                        preAction  ,
                                        progress   ) 
        if odin : odin = odinGOOD.data
        else    : odin = None
    ##
    return odin, nev 

# =============================================================================
## seek for some  event-run (list)
def seekForEvtRun  ( runevt_list             ,
                     EvtMax     = 10000      ,
                     disableAll = False      ,
                     location   = 'DAQ/ODIN' ,
                     postAction = None       ,
                     preAction  = None       ,
                     progress   = False      ) :
    
    """Seek for event in runevent list
    """
    from LoKiHlt.decorators import odin_runevts
    ## 
    return seekForODIN ( odin_runevts ( runevt_list ) ,
                         EvtMax     ,
                         disableAll ,
                         location   ,
                         postAction ,
                         preAction  ,
                         progress   ) 

# =============================================================================
## seek the decision for the certain Trigger Line 
def seekHltDecision ( expr                ,
                      EvtMax              ,  
                      disableAll          ,  
                      location            , 
                      postAction = None   ,
                      preAction  = None   ,
                      progress   = False  ) :
    """
    Seek for  the event with certain Hlt1-decision 
    """
    from LoKiHlt.decorators import HLT_PASS_RE 
    
    fun = HLT_PASS_RE ( expr ) if isinstance ( expr , str ) else expr 
    
    from Bender.Utils import DisabledAlgos
    
    with DisabledAlgos( disableAll ) :
        
        hltGOOD    = GoodData( fun , location )
        hlt , nev  = seekVoidDecision ( hltGOOD    ,
                                        EvtMax     ,  
                                        postAction ,
                                        preAction  ,
                                        progress   ) 
        if hlt  : hlt = hltGOOD.data
        else    : hlt = None
    ##
    return hlt, nev 

# =============================================================================
## seek the decision for the certain Trigger Line 
def seekHlt1Decision ( expr                           ,
                       EvtMax     = 10000             ,
                       disableAll = False             ,
                       location   = 'Hlt1/DecReports' , 
                       postAction = None              ,
                       preAction  = None              ,
                       progress   = False             ) :
    """
    Seek for  the event with certain Hlt1-decision 
    """
    return seekHltDecision ( expr       ,
                             EvtMax     ,
                             disableAll ,
                             location   ,
                             postAction ,
                             preAction  ,
                             progress   ) 

# =============================================================================
## seek the decision for the certain Trigger Line 
def seekHlt2Decision ( expr                           ,
                       EvtMax     = 10000             ,
                       disableAll = False             ,
                       location   = 'Hlt2/DecReports' ,
                       postAction = None              ,
                       preAction  = None              ,
                       progress   = False             ) :
    """
    Seek for  the event with certain Hlt1-decision 
    """
    return seekHltDecision ( expr       ,
                             EvtMax     ,
                             disableAll ,
                             location   ,
                             postAction ,
                             preAction  ,
                             progress   ) 

# =============================================================================
## seek the decision for the certain stripping line 
def seekStripDecision ( expr                                 ,
                        EvtMax     = 10000                   ,
                        disableAll = False                   ,
                        location   = 'Strip/Phys/DecReports' ,
                        postAction = None                    ,
                        preAction  = None                    ,
                        progress   = False                   ) :
    """
    Seek the decision for the certain Stripping Line
    """
    return seekHltDecision ( expr       ,
                             EvtMax     ,
                             disableAll , 
                             location   ,
                             postAction ,
                             preAction  ,
                             progress   ) 

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

