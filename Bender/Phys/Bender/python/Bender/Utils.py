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
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#
#                    $Revision$
#  Last modification $Date$
#                 by $Author$ 
# =============================================================================
"""Set of helper functions  for Bender

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
   
By usage of this code one clearly states the disagreement 
with the smear campaign of Dr.O.Callot et al.: 
   ``No Vanya's lines are allowed in LHCb/Gaudi software.''

                    $Revision$
  Last modification $Date$
                 by $Author$  
"""
# =============================================================================
__author__  = 'Vanya BELYAEV Ivan.Belyaev@itep.ru'
__date__    = "2010-00-12"
__version__ = '$Revision$'
__all__     = (
    ##
    'run'                , ## run  N events
    'run_progress'       , ## run  N events with progress bar 
    'irun'               , ## "generator" to run  N events 
    'skip'               , ## skip N events 
    'next'               , ## go to run event
    'rewind'             , ## rewind 
    'ls'                 ,
    'get'                ,
    'appMgr'             ,
    'dumpHistos'         ,
    #
    'Action'             , ## trivial context action 
    'RunAction'          , ## specialized action for  sysBeginRun/sysEndRun
    #
    'setData'            ,
    ##
    'preRun_actions'     , 
    'postRun_actions'    , 
    'preInit_actions'    , 
    'postInit_actions'   , 
    'preStart_actions'   , 
    'postStart_actions'  , 
    ## 
    'addPreRunAction'    ,
    'addPostRunAction'   ,
    'addPreInitAction'   ,
    'addPostInitAction'  ,
    'addPreStartAction'  ,
    'addPostStartAction' ,
    ##
    )
# =============================================================================
## logging
# =============================================================================
from Bender.Logger import getLogger 
if '__main__' == __name__ : logger = getLogger ( 'Bender.Utils' )
else                      : logger = getLogger ( __name__ )
## ============================================================================
from LoKiCore.basic import cpp  
SUCCESS = cpp.StatusCode(cpp.StatusCode.SUCCESS,True)
## ============================================================================
__Bender_PreRun_Actions    = [ lambda : logger.verbose('"run"    pre-action' ) ]
__Bender_PostRun_Actions   = [ lambda : logger.verbose('"run"   post-action' ) ]
__Bender_PreInit_Actions   = []
__Bender_PostInit_Actions  = []
__Bender_PreStart_Actions  = [] 
__Bender_PostStart_Actions = [] 
## ============================================================================
## post-actions() : 
def postRun_actions ( action = None ) :
    """Get the ``post-Run-actions''
    """
    global      __Bender_PostRun_Actions 
    if action  and isinstance ( action , ( list , tuple ) ) : 
        return  __Bender_PostRun_Actions +   action
    elif action : 
        return  __Bender_PostRun_Actions + [ action ]
    ##
    return list(__Bender_PostRun_Actions)
## ============================================================================
## pre-action 
def preRun_actions ( action = None ) :
    """Get the ``pre-Run-action''
    """
    global      __Bender_PreRun_Actions 
    if action  and isinstance ( action , ( list , tuple ) ) : 
        return  __Bender_PreRun_Actions +   action
    elif action : 
        return  __Bender_PreRun_Actions + [ action ]
    ##
    return list(__Bender_PreRun_Actions)
## ============================================================================
## post-action 
def postInit_actions ( action = None  ) :
    """Get the ``post-Init-action''
    """
    global      __Bender_PostIni_Actions 
    if action  and isinstance ( action , ( list , tuple ) ) : 
        return  __Bender_PostInit_Actions +   action
    elif action : 
        return  __Bender_PostInit_Actions + [ action ]
    ##
    return list(__Bender_PostInit_Actions)
## ============================================================================
## pre-action 
def preInit_actions ( action = None ) :
    """Get the ``pre-Init-action''
    """
    global      __Bender_PreIni_Actions 
    if action  and isinstance ( action , ( list , tuple ) ) : 
        return  __Bender_PreInit_Actions +   action
    elif action : 
        return  __Bender_PreInit_Actions + [ action ]
    ##
    return list(__Bender_PreInit_Actions)
## ============================================================================
## post-action 
def postStart_actions ( action = None ) :
    """
    Get the ``post-Start-action''
    """
    global      __Bender_PostStart_Actions 
    if action  and isinstance ( action , ( list , tuple ) ) : 
        return  __Bender_PostStart_Actions +   action
    elif action : 
        return  __Bender_PostStart_Actions + [ action ]
    ##
    return list(__Bender_PostStart_Actions)
## ============================================================================
## pre-action 
def preStart_actions ( action = None ) :
    """Get the ``pre-Start-action''
    """
    global      __Bender_PreStart_Actions 
    if action  and isinstance ( action , ( list , tuple ) ) : 
        return  __Bender_PreStart_Actions +   action
    elif action : 
        return  __Bender_PreStart_Actions + [ action ]
    ##
    return list(__Bender_PreStart_Actions)

# ============================================================================
## add new Post-action 
def addPostRunAction ( action ) :
    """ Add new ``post-run'' action for Bender
    """
    global       __Bender_PostRun_Actions
    if action :  __Bender_PostRun_Actions.append ( action )
    return tuple(__Bender_PostRun_Actions)
    
# =============================================================================
## add new Pre-action 
def addPreRunAction ( action ) :
    """ Add new ``pre-run'' action for Bender
    """
    global       __Bender_PreRun_Actions
    if action :  __Bender_PreRun_Actions.append ( action ) 
    return tuple(__Bender_PreRun_Actions)

# =============================================================================
## add new Post-action 
def addPostInitAction ( action ) :
    """ Add new ``post-start'' action for Bender
    """
    global       __Bender_PostInit_Actions
    if action :  __Bender_PostInit_Actions.append ( action ) 
    return tuple(__Bender_PostInit_Actions)

# =============================================================================
## add new Pre-action 
def addPreInitAction ( action ) :
    """ Add new ``pre-start'' action for Bender
    """
    global       __Bender_PreInit_Actions
    if action :  __Bender_PreInit_Actions.append ( action )
    return tuple(__Bender_PreInit_Actions)
    
# =============================================================================
## add new Post-action 
def addPostStartAction ( action ) :
    """ Add new ``post-start'' action for Bender
    """
    global       __Bender_PostStart_Actions
    if action :  __Bender_PostStart_Actions.append ( action ) 
    return tuple(__Bender_PostStart_Actions)

# =============================================================================
## add new Pre-action 
def addPreStartAction ( action ) :
    """ Add new ``pre-start'' action for Bender
    """
    global       __Bender_PreStart_Actions
    if action :  __Bender_PreStart_Actions.append ( action ) 
    return tuple(__Bender_PreStart_Actions)

# =============================================================================
## @class Action
#  simple ``action''/contect-manager for ``run''-fuction
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2016-02-15
class Action(object) :
    """ Simple ``action''/context-manager for ``run''-function
    """
    def __init__ ( self             ,
                   preActions  = [] ,
                   postActions = [] ) :
        
        if not isinstance (  preActions  , (list,tuple) ) :
            preActions   = [  preActions ]
        if not isinstance (  postActions , (list,tuple) ) :
            postActions  = [  postActions ]

        self. preActions = [] + [ a for a in  preActions if a ] 
        self.postActions = [] + [ a for a in postActions if a ] 
            
    ## context manager: ENTER 
    def __enter__ ( self ) :
                
        while self.preActions :
            action = self.preActions.pop(0)
            if action :
                logger.verbose ( 'Execute pre-action %s' % action)
                _r = action()
                
        return self
    
    ## context manager: EXIT
    def __exit__ ( self , *_ ) :
        
        while self.postActions :
            action = self.postActions.pop(0)
            if action :
                logger.verbose ('Execute post-action %s' % action )
                _r = action()
                
# =============================================================================
## run N events
#  @code
#  run(10)
#  @endcode 
def run ( nEvents       =   -1  ,
          postAction    = None  ,
          preAction     = None  ,
          with_progress = False ) :
    """Run gauidi 
    >>> run(50)
    """
    if 0 < nEvents and with_progress :
        return run_progress ( nEvents , postAction , preAction )

    with Action ( preAction , postAction ) :
        
        ##  get the application manager
        _g = appMgr() 
        sc = _g.run ( nEvents )
        if sc.isSuccess() and not get('/Event') :
            logger.warning ( 'No more events in event selection...' )
            sc.setCode ( 2 )
            
    return sc

# =============================================================================
## helper context manager to emulate IEventProcessor::executeRun
#  @see AppMgr.run
#  @see IEventProcessor::executeRun
class RunAction(object) :
    """ Helper context manager to emulate IEventProcessor::executeRun
    >>> with RunAction() :
    ...      do something  here ... 
    """
    def __init__   ( self , g = None ) :
        self._gaudi = g
        self._ialgs = []

    ## context manager: ENTER 
    def __enter__  ( self ) :

        if not self._gaudi : self._gaudi = appMgr ()
        sc = self._gaudi.start()
        if sc.isFailure() :
            logger.fatal ( "Can't start AppMgr %s" % sc ) 
        self._algs = self._gaudi.topAlgs()
        for _a in self._algs :
            sc = _a.sysBeginRun()
            if sc.isFailure() :
                logger.error( "Error  from %s:sysBeginRun %s" % ( _a.name() , sc ) )
                
        return self
    
    ## context manager: EXIT  
    def __exit__ ( self , *_ ) :

        while self._ialgs :
            _i = self._ialgs.pop(0)
            sc = _i.sysEndRun()
            if sc.isFailure() :
                logger.error( "Error  from %s:sysEndRun %s" % ( _a.name() , sc ) )         

# =============================================================================
def _next_event_ ( nevt = 1 ) :
    _g = appMgr()
    return  _g._evtpro.nextEvent( nevt )
    
# =============================================================================
## run N events with progress bar 
#  @code
#  run_progress (10)
#  @endcode 
def run_progress ( nEvents           ,
                   postAction = None ,
                   preAction  = None ) : 
    """Run gaudi showing the progress bar  
    >>> run_progress ( 50 )
    """
    if nEvents <= 0 :
        return run ( nEvents , postAction , preAction )
    ##
    sc = SUCCESS
    
    with Action ( preAction , postAction ) :

        _g = appMgr()
        sc = _g.start()
        if sc.isFailure() : return sc 

        with Action ( preRun_actions() , postRun_actions() ) :

            with  RunAction () : 
            
                from Ostap.progress_bar import progress_bar
                for i in progress_bar ( xrange ( nEvents ) ) :
                    sc = _next_event_  ( 1 )
                    if sc.isFailure() :
                        logger.error('Error from run %s' % sc)
                        return sc                 
    return sc

# ==============================================================================
## Run over events till certain number of good events is found
#  @code
#  def accept_function () : ...
#  ...
#  for i in irun ( 100 , accept_function ) :
#         hdr = get('/Event/Rec/Header')
#         print hdr.runNumber(), hdr.eventNumber() 
#  @endcode
#  The command can be useful in scripts 
def irun ( nEvents                    ,
           accept     = lambda : True , 
           postAction = None          ,
           preAction  = None          ,
           EvtMax     = -1            ,
           progress   = False         ,
           running    = False         ) : 
    """Run over events till certain number of good events is found
    >>> def accept_function () : 
    >>> for i in irun ( 100 , accept_function ) :
    ...         hdr = get('/Event/Rec/Header')
    ...         print hdr.runNumber(), hdr.eventNumber()

    e.g. one can select events with at least 2 J/psi:
    good_events = lambda : 2<= len(get('/Event/PSIX/Phys/SelDetachedPsisForBandQ/Particles'))
    
    The command can be useful in scripts 
    """
    with Action ( preAction , postAction ) :
        
        ## get application manager:        
        _g   = appMgr() 
        sc   = SUCCESS
        evnt = True 
        iev  = 0
        nev  = 0
        
        from Ostap.progress_bar import ProgressBar, RunningBar 
        
        with Action ( preRun_actions() , postRun_actions() ) :

            with  RunAction () : 

                use_rbar = running
                use_pbar = progress and 1  < nEvents and not running 
                
                pbar = ProgressBar ( max_value = nEvents , silent = not use_pbar ) 
                rbar = RunningBar  (                       silent = not use_rbar ) 
                
                while sc.isSuccess() and evnt and ( nEvents < 0 or iev < nEvents ) :
                    
                    sc      = _next_event_ ( 1 )
                    if sc.isFailure() : break   ## BREAK 
                    evnt    = get('/Event')
                    if not evnt :
                        if sc.isSuccess() : sc.setCode(2)
                        break                     ## BREAK! 

                    pbar  += 1 
                    rbar  += 1
                    nev   += 1                    ## total number of event
                    
                    if 0 < EvtMax < nev : break  ## BREAK 
                    
                    if   0 == nev % 1000000 : 
                        logger.fatal   ('irun: run over %d, found %s ' % ( nev , iev ) )
                    elif 0 == nev %  100000 : 
                        logger.error   ('irun: run over %d, found %s ' % ( nev , iev ) ) 
                    elif 0 == nev %   10000 : 
                        logger.warning ('irun: run over %d, found %s ' % ( nev , iev ) ) 
                    elif 0 == nev %    5000 : 
                        logger.info    ('irun: run over %d, found %s ' % ( nev , iev ) )                 
                    elif 0 == nev %    1000 : 
                        logger.debug   ('irun: run over %d, found %s ' % ( nev , iev ) )                 
                    elif 0 == nev %     500 : 
                        logger.verbose ('irun: run over %d, found %s ' % ( nev , iev ) )                 
                
                    ## check
                    if accept() :              ## GOOD EVENT!  
                        iev  += 1
                        yield evnt,iev,nev     ## retun 


# =============================================================================
## go to next  event
#  @code 
#  next()
#  @endcode 
def next ( num = 1 )  :
    """Go to next event
    >>> next()
    """
    return run ( num )

# ==============================================================================
## define the input data for Bender job
def setData ( files            ,
              catalogs = []    ,
              castor   = False ,
              grid     = None  ) :
    """ Define the input data for Bender job:    
    >>> files    = [ 'file1.dst' , 'file2.dst' ]    
    >>> catalogs = ....    
    >>> import USERSCRIPT    
    >>> USERSCRIPT.setData ( files , catalogs )    
    """
    
    if   type ( files    ) is str   : files    =      [ files    ]
    elif type ( files    ) is tuple : files    = list ( files    ) 
    if   type ( catalogs ) is str   : catalogs =      [ catalogs ]    
    elif type ( catalogs ) is tuple : catalogs = list ( catalogs )
    
    if not issubclass ( type ( files    ) , list ) :
        files    = [ f for f in files    ] 
    if not issubclass ( type ( catalogs ) , list ) :
        catalogs = [ c for c in catalogs ] 
    
    from GaudiPython.Bindings import _gaudi
    if not _gaudi :               ## here we deal with configurables!

        from Configurables import Gaudi__RootCnvSvc
        rcnv = Gaudi__RootCnvSvc ( 'RootCnvSvc' )
        rcnv.CacheBranches = [   ]
        rcnv.VetoBranches  = ['*']    

        if files :
            
            from Bender.DataUtils import extendfile2
            files = [ extendfile2 ( f , castor , grid ) for f in files ]

            from Gaudi.Configuration import EventSelector
            inpts  = EventSelector().Input
            inpts += files  
            EventSelector ( Input = inpts )
            
        if catalogs :

            from Gaudi.Configuration import Gaudi__MultiFileCatalog as FileCatalog
            ctlgs  = FileCatalog().Catalogs

            cc     = [] 
            for c in catalogs :
                if c in cc : continue
                if 1<=c.find('.xml') : 
                    import os
                    if   os.path.exists ( c ) or 0 != c.find('xmlcatalog_file:') :
                        logger.debug ( 'Prepend catalog with protocol: %s' % c )
                        c = 'xmlcatalog_file:' + c
                if c in cc : continue
                cc.append ( c )
                
            logger.debug ( 'The catalogs: %s' % cc )
            ctlgs += cc 

            FileCatalog ( Catalogs = ctlgs )
            logger.debug ( 'FileCatalog:\n %s' % FileCatalog() )
        
            ## from Gaudi.Configuration import FileCatalog
            
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
    """Get the application manager
    >>> gaudi = appMgr() 
    """
    import GaudiPython.Bindings
    _g = GaudiPython.Bindings.AppMgr()
    if not 'LoKiSvc' in _g.ExtSvc :
        logger.debug ('appMgr: add LoKiSvc into the list of services')
        _g.ExtSvc += [ 'LoKiSvc']
    return _g

# =============================================================================
## define "easy"-function to browse TES
#  @code
#  ls('/Event')
#  ls('/Event/MC', forceload = True )
#  @endcode 
def ls  ( *args , **kwargs ) :
    """Browse the Transient Store
    >>> run(1)
    >>> ls()
    >>> ls('/Event/MC')
    >>> ls('/Event/MC', forceload = True)    
    """
    _g  = appMgr()
    _es = _g.evtSvc()
    return _es.ls ( *args , **kwargs ) 

# =============================================================================
## define "easy-get"-function to get data for TES 
#  @code
#  run(1)    
#  o = get('/Event/MC')
#  o = get('/Event/MC/Particles' , MCPT > 1 )
#  @endcode 
def get  ( path , selector = lambda s : s ) :
    """Get object from  the Transient Store
    >>> o = get('/Event/MC')
    >>> o = get('/Event/MC/Particles' , MCPT > 1 )
    """
    _g  = appMgr()
    _es = _g.evtSvc()
    data = _es[path]
    #
    try : ## try to use the selector 
        return selector ( data )
    except:
        pass
    
    try : ## try to use selector 
        return filter ( selector , data )
    except :
        pass 
    
    return data 

# =============================================================================
## dump all histogram from the given component 
#  @code
#  dump( 'MakeKs' )
#  alg = ...
#  dumpHistos ( alg , 30 , 20 )
#  @endcode 
def dumpHistos ( o , *args ) :
    """Dump all histogram from the given component:
    >>> dump( 'MakeKs' )
    >>> alg = ...
    >>> dumpHistos ( alg , 30 , 20 )
    """
    if isinstance ( o , str ) :
        _g = appMgr()
        o  = _g.algorithm( o )
    return o.dumpHistos ( *args ) 

# =============================================================================
## Temporarily disable all algorithms
#  @code
#  with DisableAlgos() :
#       ... do something 
#  @endocde 
class DisabledAlgos(object) :
    """Temporarily disable all algorithms
    >>> with DisableAlgos() :
    >>> ... do something 
    """
    def __init__  ( self , disableAll = True )  :
        self._disabled     = ()
        self._disable_all  = disableAll 
        
    ## enter context: disable everything 
    def __enter__ ( self ) :
        _g  = appMgr()
        if self._disable_all : self._disabled = tuple( _g.disableAllAlgs() ) 
        else                 : self._disabled = tuple( _g.disableTopAlgs() ) 
        return self._disabled

    ## exit the context: reenable algorithms 
    def __exit__ ( self , *_ ) :
        for _a in self._disabled : _a.setEnabled ( True ) 
        del self._disabled    


# =============================================================================
## skip the events
#  @code
#  skip ( 10 ) 
#  @endcode 
def skip ( nEvents ) : 
    """Skip N-events
    >>> skip ( 50 )    
    """
    st = SUCCESS 
    with DisabledAlgos() :
        st = run ( nEvents )
        
    return st 

# =============================================================================
## rewind
#  @code
#  rewind()
#  run(1)   ##   needed  to actually get the 1st event after rewind 
#  @endcode
#  @thanks Thomas RUF 
def rewind ( ) :
    """Rewind:
    >>> rewind() 
    >>> run(1)    ## needed to actually get the 1st event after rewind 
    """
    _g =    appMgr()
    with DisabledAlgos () :
        _g.evtSel().rewind() 
    
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
