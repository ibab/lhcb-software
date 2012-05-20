#!/usr/bin/env python 
# =============================================================================
# $Id$ 
# =============================================================================
## @file Bender/Utils.py
#
#  Set of helper functions  for Bender 
#
#  This file is a part of 
#  <a href="http://cern.ch/lhcb-comp/Analysis/Bender/index.html">Bender project</a>
#  <b>``Python-based Interactive Environment for Smart and Friendly 
#   Physics Analysis''</b>
#
#  The package has been designed with the kind help from
#  Pere MATO and Andrey TSAREGORODTSEV. 
#  And it is based on the 
#  <a href="http://cern.ch/lhcb-comp/Analysis/LoKi/index.html">LoKi project:</a>
#  ``C++ ToolKit for Smart and Friendly Physics Analysis''
#
#  By usage of this code one clearly states the disagreement 
#  with the smear campaign of Dr.O.Callot et al.: 
#  ``No Vanya's lines are allowed in LHCb/Gaudi software.''
#
#  @date   2010-09-12
#  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
#
#                    $Revision$
#  Last modification $Date$
#                 by $Author$ 
# =============================================================================
"""
Set of helper functions  for Bender 

This file is a part of BENDER project:
   ``Python-based Interactive Environment for Smart and Friendly Physics Analysis''

The project has been designed with the kind help from Pere MATO and Andrey TSAREGORODTSEV. 

And it is based on the LoKi project:
   ``C++ ToolKit for Smart and Friendly Physics Analysis''
   
By usage of this code one clearly states the disagreement 
with the smear campaign of Dr.O.Callot et al.: 
   ``No Vanya's lines are allowed in LHCb/Gaudi software.''

                    $Revision$
  Last modification $Date$
                 by $Author$  
"""
# =============================================================================
__author__  = 'Vanya BELYAEV Ivan.Belyaev@nikhef.nl'
__date__    = "2010-00-12"
__version__ = '$Revision$'
__all__     = (
    ##
    'run'               , 
    'skip'              , 
    'ls'                ,
    'get'               ,
    'appMgr'            ,
    'dumpHistos'        ,
    'next'              ,
    ##
    'fireIncident'      ,
    'tagEvent'          ,    
    'markEvent'         ,
    'copyGoodEvents'    ,
    'writeEvent'        ,
    ##
    'post_Action'       , 
    'pre_Action'        , 
    'setPostAction'     , 
    'setPreAction'      ,
    'setData'           ,
    ##
    'seekForData'       ,
    'seekVoidDecision'  ,
    'seekAlgDecision'   ,
    'seekHltDecision'   ,
    'seekStripDecision' ,
    ##
    )
# =============================================================================
## logging
# =============================================================================
from Bender.Logger import getLogger 
logger = getLogger( __name__ )
## ============================================================================
## post action 
__Bender_Post_Action = None 
## pre action 
__Bender_Pre_Action  = None
## ============================================================================
## post-action 
def post_Action () :
    """
    Get the ``post-action''
    """
    return __Bender_Post_Action
## ============================================================================
## post-action 
def pre_Action () :
    """
    Get the ``pre-action''
    """
    return __Bender_Pre_Action
## ============================================================================
## set new Post-action 
def setPostAction ( action ) :
    """
    set new ``post-action'' for Bender
    """
    global __Bender_Post_Action
    __Bender_Post_Action = action
    
## ============================================================================
## set new Pre-action 
def setPreAction ( action ) :
    """
    set new ``pre-action'' for Bender
    """
    global __Bender_Pre_Action
    __Bender_Pre_Action = action
        
# =============================================================================
## run events 
def run ( nEvents     =   -1 ,
          postAction  = None ,
          preAction   = None ) :
    """
    Run gaudi

    >>> run(50)
    
    """
    ## apply "pre-action", if defined 
    if   preAction    : preAction ()
    elif pre_Action() :
        action = pre_Action()
        action() 
        
    ## get the application manager
    _g = appMgr() 
    st = _g.run ( nEvents )
    
    ## apply "post-action", if defined 
    if   postAction    : postAction ()
    elif post_Action() :
        action = post_Action()
        action() 
        
    return st 


# ==============================================================================
## define the input data for Bender job
def setData ( files , catalogs = [] , castor = False ) :
    """
    Define the input data for Bender job:
    
    >>> files    = [ 'file1.dst' , 'file2.dst' ]
    
    >>> catalogs = ....
    
    >>> import USERSCRIPT
    
    >>> USERSCRIPT.setData ( files , catalogs )
    
    """

    from GaudiPython.Bindings import _gaudi
        
    if   type ( files    ) is str   : files    =      [ files    ]
    elif type ( files    ) is tuple : files    = list ( files    ) 
    if   type ( catalogs ) is str   : catalogs =      [ catalogs ]    
    elif type ( catalogs ) is tuple : catalogs = list ( catalogs )
    
    if not issubclass ( type ( files    ) , list ) :
        files    = [ f for f in files    ] 
    if not issubclass ( type ( catalogs ) , list ) :
        catalogs = [ c for c in catalogs ] 
    
    if not _gaudi :               ## here we deal with configurables!

        from Configurables import Gaudi__RootCnvSvc
        rcnv = Gaudi__RootCnvSvc ( 'RootCnvSvc' )
        rcnv.CacheBranches = [   ]
        rcnv.VetoBranches  = ['*']    

        if files :
            
            from Bender.DataUtils import extendfile2
            files = [ extendfile2 ( f , castor ) for f in files ]

            from Gaudi.Configuration import EventSelector
            EventSelector ( Input = files )
            
        if catalogs :
            
            from Gaudi.Configuration import Gaudi__MultiFileCatalog as FileCatalog
            FileCatalog   ( Catalogs = catalogs )
            from Gaudi.Configuration import FileCatalog
            FileCatalog   ( Catalogs = catalogs )
            
    else :                        ## here we deal with the actual components


        rcnv = _gaudi.service('Gaudi::RootCnvSvc/RootCnvSvc')
        rcnv.CacheBranches = [   ]
        rcnv.VetoBranches  = ['*']    
        
        if files :
            
            from Bender.DataUtils import extendfile1
            files = [ extendfile1 ( f , castor ) for f in files ]
        
            _e = _gaudi.evtSel()
            _e.open ( files )
            
        if catalogs :
            _f = _gaudi.service ( 'FileCatalog' )
            _f.Catalogs = catalogs


## ditto...
setInputData = setData

## ============================================================================
## Get the application manager 
def appMgr( *varg , **kwarg ) :
    """
    Get the application manager 
    """
    from GaudiPython.Bindings import AppMgr 
    _g = AppMgr()
    if not 'LoKiSvc' in _g.ExtSvc : _g.ExtSvc += [ 'LoKiSvc']
    return _g

# =============================================================================
## define "easy"-function to browse TES
def ls  ( *args , **kwargs ) :
    """
    Browse the Transient Store

    >>> run(1)
    >>> ls()

    >>> ls('/Event/Strip')
    
    """
    _g  = appMgr()
    _es = _g.evtSvc()
    
    return _es.ls ( *args , **kwargs ) 

# =============================================================================
## define "easy-get"-function to get data for TES 
def get  ( path , selector = lambda s : s ) :
    """
    Get object from  the Transient Store
    
    >>> run(1)
    
    >>> o = get('/Event/Strip')
    
    """
    _g  = appMgr()
    _es = _g.evtSvc()
    data = _es[path]
    #
    try : ## try to use the selector 
        #
        return selector ( data )
        # 
    except:
        logger.error('Unable to use selector %s ' % selector )

    return data 

# =============================================================================
## dump all histogram from the given component 
def dumpHistos ( o , *args ) :
    """
    Dump all histogram from the given component

    >>> dump( 'MakeKs' )

    >>> alg = ...
    >>> dumpHistos ( alg , 30 , 20 )
    """
    if isinstance ( o , str ) :
        _g = appMgr()
        o  = _g.algorithm( o )
    return o.dumpHistos ( *args ) 

# =============================================================================
## seek the decision for the certain void-functor 
def seekVoidDecision ( fun , EvtMax = 1000 ) :
    """
    Seek the Decision for the certains algorithm    
    """

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
        from LoKiHlt.algorithms import ALG_EXECUTED, ALG_PASSED
        fun = ALG_EXECUTED  ( alg ) & ALG_PASSED ( alg )
        
    return seekVoidDecision ( fun ) 


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
## seek the decision for the certain Trigger Line 
def seekHltDecision ( expr                                 ,
                      EvtMax     = 10000                   ,
                      disableAll = False                   ,
                      location   = '/Event/Hlt/DecReports' ) :
    
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
            _a.setEnable ( True )
        
        
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
## skip the events 
def skip ( nEvents ) : 
    """
    Skip events
    
    >>> skip ( 50 )
    
    """
    _g = appMgr()

    ## disable all algorithms
    _disabled = _g.disableAllAlgs()

    try:
        st = run ( nEvents )
    finally:
        for _a in _disabled :
            _a.setEnabled ( True )

    return st 

# =============================================================================
## go to next  event 
def next ( num = 1 )  :
    """
    Go to next event

    >>> next()
    
    """
    return run ( num )


# ===========================================================================
## fire the incident with given name
def fireIncident ( name   = "GoodEvent" ,
                   source = "Bender"    ) :
    """
    Fire the incident
    
    >>> fireIncident()
    
    """
    _g  = appMgr    ()
    _is = _g.incSvc ()
    _is.fireIncident ( name , source )

# =============================================================================
## fire the incident with the given name    
tagEvent  = fireIncident
markEvent = fireIncident

# =============================================================================
## helper function to write interesint event into the separate stream
#
#  It is assumed that the proper configuration is done with
#   "copyGoodEvent"  function  ( static CONFIGURABLE)
#
#  @code
#
#  #
#  ##Configurables:
#  #
#  from Bender.Utils import copyGoodEvents
#  copyGoodEvents ( filename = 'SelectedEvents' )
#
#  ...
#
#  #
#  ## run-time command line: 
#  #
#  >>> writeEvent () 
#
#  #
#  ## run-time (algorithm)
#  #
#          ...
#          writeEvent()
#          ...
#  @endcode 
#
#  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
#  @date   2011-12-17
#
#  @see copyGoodEvents
#
def writeEvent ( name = "GoodEvent" ) :
    """
    Helper function to write interesint event into the separate stream

    It is assumed that the proper configuration is done with
    copyGoodEvent  function  ( static CONFIGURABLE)
    
    #
    ##Configurables:
    #
    from Bender.Utils import copyGoodEvents
    copyGoodEvents ( filename = 'SelectedEvents' )
    
    ...
    
    #
    ## run-time command line: 
    #
    >>> writeEvent () 
    
    #
    ## run-time (algorithm)
    #
                  ...

                  writeEvent()
                  ...
    
    """
    _g     = appMgr()      
    _icntx = _g.cntxSvc ()
    #
    ## get the "current" algorithm from ContextService 
    _curr  = _icntx.current()
    if not _curr :
        ## we are out of event-scope ?
        #  try to execute manually the writer 
        writer = _g.algorithm ( name )
        if not writer._ialg : writer.retrieveInterface()
        if not writer._ialg :
            raise RuntimeError , 'Unable to find writer "%s"' % name
        # run it! 
        return writer.sysExecute()
    #
    ## otherwise fire the incident
    #
    fireIncident ( name )
    #
    from GaudiPython.Bindings import SUCCESS
    #
    return SUCCESS 
    
# ============================================================================
## Utility to copy selected events to some file
#  It is based on configurables,
#  thus it needs to be invoked BEFORE GaudiPython
#
#  @code
#
#  #
#  ##Configurables:
#  #
#  from Bender.Utils import copyGoodEvents
#  copyGoodEvents ( filename = 'SelectedEvents' )
#
#  @endcode 
#
#  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
#  @date 2011-12-16
#  @see InputCopyStream
#  @see IOHelper
#  @see writeEvents
#  @see Gaudi::IncidentFilter
#  @see fireIncident
def copyGoodEvents ( filename                 ,
                     extraitems = []          ,
                     incident   = 'GoodEvent' ) :
    """
    Utitity to copy ``good/tagged'' events

    It is based on configurables,
    thus it needs to be invoked BEFORE GaudiPython
    
    #
    ##Configurables:
    #
    from Bender.Utils import copyGoodEvents
    copyGoodEvents ( filename = 'SelectedEvents' )
    
    """
    from GaudiConf import IOHelper
    ioh   = IOHelper       ( 'ROOT' , 'ROOT' ) 
    algs  = ioh.outputAlgs ( filename , 'InputCopyStream/%s' % incident )
    
    ioh.setupServices()
    from Configurables import Gaudi__IncidentFilter as Tagger    
    tag   = Tagger ( "%sInc"   % incident , Incidents = [ incident] ) 
    
    from Configurables import GaudiSequencer
    seq   = GaudiSequencer ( '%sSeq' % incident        ,
                             Members = [ tag ] +  algs ) 

    from Configurables import ApplicationMgr
    AM    = ApplicationMgr ()
    
    if not AM.OutStream : AM.OutStream =[]
    
    AM.OutStream.append ( seq )
    
    
# =============================================================================
if __name__ == '__main__' :
    print '*'*120
    print                      __doc__
    print ' Author  : %s ' %   __author__    
    print ' Version : %s ' %   __version__
    print ' Date    : %s ' %   __date__
    print '*'*120
    
# =============================================================================
# The END 
# =============================================================================
