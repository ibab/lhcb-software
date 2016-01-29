#!/usr/bin/env python 
# =============================================================================
## @file Bender/Fixes_Gaudi.py
#  The helper Python module for Bender application 
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
#  @date   2011-10-21
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#
#                    $Revision$
#  Last modification $Date$
#                 by $Author$ 
# =============================================================================
""" Apply some last-moment (version-dependent) fixes

oooooooooo.                              .o8                     
`888'   `Y8b                            \"888                     
 888     888  .ooooo.  ooo. .oo.    .oooo888   .ooooo.  oooo d8b 
 888oooo888' d88' `88b `888P\"Y88b  d88' `888  d88' `88b `888\"\"8P 
 888    `88b 888ooo888  888   888  888   888  888ooo888  888     
 888    .88P 888    .o  888   888  888   888  888    .o  888     
o888bood8P'  `Y8bod8P' o888o o888o `Y8bod88P\" `Y8bod8P' d888b    

This file is a part of BENDER project:
``Python-based Interactive Environment for Smart and Friendly Physics Analysis''

The project has been designed with the kind help from
Pere MATO and Andrey TSAREGORODTSEV. 

And it is based on the 
LoKi project: ``C++ ToolKit for Smart and Friendly Physics Analysis''

By usage of this code one clearly states the disagreement 
with the smear campaign of Dr.O.Callot et al.: 
``No Vanya's lines are allowed in LHCb/Gaudi software.''

"""
# =============================================================================
__author__  = 'Vanya BELYAEV Ivan.Belyaev@itep.ru'
__date__    = "2011-10-21"
__version__ = '$Revision$'
__all__     = ()
# =============================================================================
## logging
# =============================================================================
from Bender.Logger import getLogger 
if '__main__' == __name__ : logger = getLogger ( 'BenderTools.Fixes_Gaudi' )
else                      : logger = getLogger ( __name__ )
# =============================================================================
logger.info ( '*** Fix some Gaudi features' ) 
# =============================================================================
from LoKiCore.basic import cpp 
# =============================================================================
import GaudiPython.Bindings
_EvtSel = GaudiPython.Bindings.iEventSelector

if not hasattr ( _EvtSel , '_openNew_') :
    
    def _openNew_ ( self                ,
                    stream              ,
                    typ        = 'ROOT' ,
                    opt        = 'READ' ,
                    sel        =  None  ,
                    fun        =  None  ,
                    collection =  None  ,
                    castor     =  True  ) :
        """Open the file(s) :        
        >>> evtSel.open ( 'file.dst' )                   ## open one file 
        >>> evtSel.open ( [ 'file1.dst' , 'file2.dst'] ) ## list of files 
        >>> evtSel.open ( '/castor/.../file.dst' )       ## open castor file 
        >>> evtSel.open ( 'file.raw' )                   ## open RAW file 
        >>> evtSel.open ( 'file.mdf' )                   ## open MDF file         
        """
        
        if typ == 'ROOT' and ( 'RootCnvSvc' not in self.g.ExtSvc and 'Gaudi::RootCnvSvc/RootCnvSvc' not in self.g.ExtSvc ) : 
            self.g.ExtSvc   += [ 'Gaudi::RootCnvSvc/RootCnvSvc' ]
            rsvc = self.g.service ( 'RootCnvSvc'          )
            eps  = self.g.service ( 'EventPersistencySvc' )
            eps.CnvServices += [ 'RootCnvSvc' ]
            ##
            rsvc.CacheBranches = [   ]
            rsvc.VetoBranches  = ['*']

        if isinstance ( stream , tuple ) : stream = list ( stream )
        if isinstance ( stream , str   ) : stream =      [ stream ] 

        from Bender.DataUtils import extendfile2

        files = []
        for f in stream :
            ff = extendfile2 ( f , castor )
            if sel        : ff += " SEL=\'%s\'" % sel
            if fun        : ff += " FUN=\'%s\'" % fun
            if collection : ff += " COLLECTION=\'%s\'" % collection
            files += [ ff ]
            
        self.Input = files

        ## reinitialize the service
        state_s = self._isvc.FSMState ()

        from GaudiPython.Bindings import gbl as cpp
        ## we need here 'INIITALIZED' state 
        if   cpp.Gaudi.StateMachine.OFFLINE    == state_s : 
            self.configure     ()
            self.sysInitialize ()
        elif cpp.Gaudi.StateMachine.CONFIGURED == state_s : 
            self.sysInitialize()
        elif cpp.Gaudi.StateMachine.RUNNING    == state_s : 
            self._isvc.sysStop()
            
        state_s = self._isvc.FSMState ()
        if cpp.Gaudi.StateMachine.INITIALIZED != state_s :
            logger.error( 'Invalid State of EventSelector %s ' % state_s )
            
        sc =   self._isvc.sysReinitialize ()
        return self._isvc.sysStart        ()


    _EvtSel._openNew_ = _openNew_
    _EvtSel.open      = _openNew_
    logger.debug ( 'decorate iEventSelector to deal with RAW/MDF & CASTOR -files' ) 

## Decorate iDataSvc with proper dir/ls methods '
import AnalysisPython.Dir 
logger.debug ( 'decorate iDataSvc with proper "dir/ls" methods ')

## decorate the algorithms with the proper dumpHistos methods
import GaudiPython.Bindings 
_iAlgorithm = GaudiPython.Bindings.iAlgorithm          ##    Algorithm interface
_iAlgTool   = GaudiPython.Bindings.iAlgTool            ##         Tool interface
for _t in ( _iAlgorithm , _iAlgTool ) :
    if not hasattr ( _t , 'dumpHistos' ) :
        logger.debug ( 'decorate iAlgoritm/iAlgTool with "dumpHistos" method') 
        ## dump all histogram from the given component 
        def _dumpHistos ( cmp , *args ) :
            """ASCII-Dump for 1D-histograms and 1D-profiles
            >>> cmp = ...  # get the component
            >>> cmp.dumpHistos ()            
            """
            _histos = cmp.Histos()
            for _k in _histos :
                _h = _histos[_k]
                if hasattr ( _h , 'dump' ) :print _h.dump( *args )
        
        _t.dumpHistos = _dumpHistos 

if not hasattr ( _iAlgorithm , 'isEnabled' ) :
    logger.debug ( 'decorate iAlgoritm with "isEnabled" method')
    _iAlgorithm.isEnabled = lambda self : self.__call_interface_method__("_ialg","isEnabled")

if not hasattr ( _iAlgorithm , 'setEnabled' ) :
    logger.debug ( 'decorate iAlgoritm with "setEnabled" method' )
    ## enable/disbale 
    def __set_Enabled_ ( self , value ) :
        self.Enable = True if value else False
        return self.Enable
    _iAlgorithm.setEnabled = __set_Enabled_ 

# =============================================================================
## decorate stat-entity 
#_SE = cpp.StatEntity
#_iadd_old_ = _SE.__iadd__
#def _iadd_new_ (s,v) :
#    print 'SE:', type(v), v 
#    if isinstance ( v , (int,long) ) : v = float(v) 
#    return _iadd_old_(s,v)
#_SE.__iadd__ = _iadd_new_
#_SE.__str__  = _SE.toString 
#logger.debug ( 'decorate StatEntity operator += ')

_AppMgr = GaudiPython.Bindings.AppMgr
if not hasattr ( _AppMgr , '_new_topAlg_' ) :

    ## get the list of top-level algorithms 
    def __top_Algs_ ( self ) :
        """
        
        Get the list of top-level algorithms as ``algorithms''
        
        """
        _algs = self.TopAlg

        def _alg_name_ ( _n  ):

            _p = _n.rfind('/')
            if 0 > _p : return _n
            return _n[_p:]

        def _pyAlg     ( _n ) :
            for _a in self.pyalgorithms :
                if _n == _a.name() : return _a
            return None 
            
        algs = [] 
        for _a in _algs :
            # get the proper name 
            _n  = _alg_name_ ( _a )
            # check if it is pyalgorithm:
            _pa = _pyAlg     ( _n )
            if _pa :
                algs += [ _pa ]
            else   :
                _alg  = self.algorithm ( _a , True )
                algs += [ _alg ]
                
        return algs

    _AppMgr._new_topAlg_ = __top_Algs_
    _AppMgr.topAlgs      = __top_Algs_
    logger.debug ( 'decorate AppMgr.topAlgs() ' )

##
if not hasattr ( _AppMgr , '_new_allAlg_' ) :

    ## get the list of all algorithms 
    def __all_Algs_ ( self ) :
        """
        
        Get the list of all algorithms as ``algorithms''
        
        """
        _algs = self.algorithms()

        algs  = []
        for _a in _algs :
            algs += [ self.algorithm ( _a ) ]
        return algs

    _AppMgr._new_allAlg_ = __all_Algs_
    _AppMgr.allAlgs      = __all_Algs_
    logger.debug ( 'decorate AppMgr.allAlgs() ' ) 

##
if not hasattr ( _AppMgr , '_disable_Tops_' ) :

    ## get the list of all algorithms 
    def __disable_Top_ ( self ) :
        """
        Disable all top-level algorithms
        """
        _tops     = self.topAlgs()
        _disabled = []
        for _a in _tops :
            if _a.isEnabled() :
                _a.setEnabled ( False )
                _disabled += [ _a ]
             
        return _disabled 

    _AppMgr._disable_Tops_   = __disable_Top_ 
    _AppMgr.disableTopAlgs   = __disable_Top_ 
    logger.debug ( 'decorate AppMgr.disableTopAlgs() ' ) 

## 
if not hasattr ( _AppMgr , '_disable_All_' ) :
        
    ## get the list of all algorithms 
    def __disable_All_ ( self ) :
        """        
        Disable all algorithms
        """
        _tops      = self.allAlgs()
        _disabled = []
        for _a in _tops :
            if _a.isEnabled() :
                _a.setEnabled ( False )
                _disabled += [ _a ]
             
        return _disabled 

    _AppMgr._disable_All_   = __disable_All_ 
    _AppMgr.disableAllAlgs  = __disable_All_ 
    logger.debug ( 'decorate AppMgr.disableAllAlgs() ' ) 


## decorate Incident Service 
if not hasattr ( _AppMgr , 'incSvc' ) : 
    # =========================================================================
    iService = GaudiPython.Bindings.iService
    iHelper  = GaudiPython.Bindings.Helper
    iCast    = GaudiPython.Bindings.InterfaceCast
    # =========================================================================
    ## @class iIncSvc
    #  python wrapper for IIncidentSvc
    #  @see IIncidentSvc
    #  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
    #  @date   2011-12-16
    class iIncSvc(iService) :
        """
        Python wrapper for IIncidentSvc 
        """
        def __init__( self , name , isvc ) :
            iService.__init__(self, name, isvc )
            self.__dict__['_iincs'] = iCast ( cpp.IIncidentSvc ) ( isvc )
            
        ## fire the incident 
        def fireIncident ( self , name , source = "<unknown source>") :
            """
            Fire the incident 
            """
            if not self._iincs :
                raise RuntimeError , " Invalid pointer to IIncidentSvc "
            incident = cpp.Incident( source , name ) 
            return self._iincs.fireIncident ( incident ) 
    
    # =========================================================================
    ## get incident service form Gaudi 
    def _incSvc_ ( self , name = "IncidentSvc" ) :
        """
        Get incident service form Gaudi:
        
        >>> gaudi = ...
        >>> iSvc  = gaudi.incSvc()
        
        """
        if self.state() == cpp.Gaudi.StateMachine.CONFIGURED :  self.initialize()
        svc    = iHelper.service( self._svcloc, name )
        return iIncSvc(name, svc)
    
    _AppMgr. incSvc = _incSvc_
    logger.debug ( 'decorate AppMgr.incSvc() ' ) 

## decorate Context Service 
if not hasattr ( _AppMgr , 'cntxSvc' ) : 
    # =========================================================================
    iService   = GaudiPython.Bindings.iService
    iAlgorithm = GaudiPython.Bindings.iAlgorithm
    iCast      = GaudiPython.Bindings.InterfaceCast
    iHelper    = GaudiPython.Bindings.Helper
    # =========================================================================
    ## @class iCntxSvc
    #  python wrapper for IAlgContextSvc
    #  @see IIncidentSvc
    #  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
    #  @date   2011-12-16
    class iCntxSvc(iService) :
        """
        Python wrapper for IAlgContextSvc 
        """
        def __init__( self , name , isvc ) :
            iService.__init__(self, name, isvc )
            self.__dict__['_ictxs'] = iCast( cpp.IAlgContextSvc )( isvc )

        ## get the current algorithm
        def current ( self ) :
            """
            Get the current algorithm
            
            >>> icntx = ...
            >>> alg = icntx.current()
            
            """
            if not self._ictxs :
                raise RuntimeError , " Invalid pointer to IIncidentSvc "
            # 
            curr = self._ictxs.currentAlg()
            if not curr : return None
            # 
            return iAlgorithm ( curr.name() , curr )
        
        ## get he stack of algorithms 
        def stack   ( self ) :
            """
            Get the stack of algorithms

            >>> icntx = ...
            >>> algs  = icntx.stack()
            
            """
            if not self._ictxs :
                raise RuntimeError , " Invalid pointer to IIncidentSvc "
            # 
            algs = self._ictxs.algorithms()
            #
            result = []
            for a in algs : result.append ( iAlgorithm ( a.name() , a ) ) 
            # 
            return result 
            
    # =========================================================================
    ## get incident service form Gaudi 
    def _cntxSvc_ ( self , name = "AlgContextSvc" ) :
        """
        Get context service from Gaudi:
        
        >>> gaudi = ...
        >>> iSvc  = gaudi.cntxSvc()
        
        """
        if self.state() == cpp.Gaudi.StateMachine.CONFIGURED :  self.initialize()
        svc  = iHelper.service( self._svcloc, name )
        return iCntxSvc( name , svc )
    
    _AppMgr.   cntxSvc = _cntxSvc_
    _AppMgr. contexSvc = _cntxSvc_
    logger.debug ( 'decorate AppMgr.cntxSvc() ' ) 


## decorate Event Collection service
if not hasattr ( _AppMgr , 'evtColSvc' ) :
    
    iNTupleSvc = GaudiPython.Bindings.iNTupleSvc 
    iHelper    = GaudiPython.Bindings.Helper
    
    def _evtcolsvc_( self , name='EvtTupleSvc' ) :
        #
        if   name                         in self.ExtSvc : pass 
        elif 'TagCollectionSvc/%s'% name  in self.ExtSvc : pass
        else : self.ExtSvc += [ 'TagCollectionSvc/%s'% name ]
        #
        svc = iHelper.service ( self._svcloc , name , False )
        return iNTupleSvc ( name , svc )

    _AppMgr. evtcolSvc = _evtcolsvc_
    _AppMgr. evtColSvc = _evtcolsvc_
    logger.debug ( 'decorate AppMgr.evtColSvc() ' ) 

# =============================================================================
## get the links from data object  
def _links_ ( self ) :
    """
    Get the links from DataObject :

    >>> obj = ...
    >>> links = obj.links()
    
    """
    lnks = set() 
    lm = self.linkMgr()
    if not lm : return lnks
    nl = lm.size() 
    for l in range(0,nl) :
        lnks.add ( lm.link(l).path() )
    return lnks

cpp.DataObject.links = _links_
# =============================================================================

# =============================================================================
logger.debug ( 'Suppress excessive prints from GaudiPython.TupleUtils') 
# =============================================================================
import GaudiPython.TupleUtils as _TU 

_TU.__nTuple__           =  _TU.nTuple

## silent version 
def _silent_ntuple_ ( s , *args ) :
    """ Retrive N-tuple ( book on demand )  """
    from Ostap.Utils import mute_py 
    with mute_py() : 
        tup = _TU.__nTuple__(s , *args )
    logger.info ( 'Booked n-tuple %s' % tup )
    return tup

_silent_ntuple_.__doc__ += '\n' + _TU._nTuple_.__doc__
_TU.nTuple           = _silent_ntuple_


_TU._release_tuples_ = _TU.releaseTuples

## silent version 
def _silent_release_tuples_ () :
    "Silent verison of release-tuples"
    from Ostap.Utils import mute_py
    with mute_py() : 
        return _TU._release_tuples_()
    
_silent_release_tuples_.__doc__ += '\n' +  _TU.releaseTuples.__doc__
    
_TU.releaseTuples                = _silent_release_tuples_

## print for n-tuple object  
def _nt_print_ ( self ) :
    if not self or not self.valid() : return 'Tuples::Tuple()'
    _t = self.nTuple()
    if not _t : return str(type(self))
    _r = _t.registry()
    if not _r : return 'Tuples::Tuple(%s)' % _t.name() 
    return 'Tuples::Tuple(%s)' % _r.identifier()

cpp.Tuples.Tuple.__repr__ = _nt_print_
cpp.Tuples.Tuple.__str__  = _nt_print_

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
