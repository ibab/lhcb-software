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
StateMachine = cpp.Gaudi.StateMachine
# =============================================================================
import GaudiPython.Bindings
_EvtSel      = GaudiPython.Bindings.iEventSelector

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
        if   StateMachine.OFFLINE    == state_s : 
            self.configure     ()
            self.sysInitialize ()
        elif StateMachine.CONFIGURED == state_s : 
            self.sysInitialize()
        elif StateMachine.RUNNING    == state_s : 
            self._isvc.sysStop()
            
        state_s = self._isvc.FSMState ()
        if StateMachine.INITIALIZED != state_s :
            logger.error( 'Invalid State of EventSelector %s ' % state_s )
            
        sc =   self._isvc.sysReinitialize ()
        return self._isvc.sysStart        ()


    _EvtSel._openNew_ = _openNew_
    _EvtSel.open      = _openNew_
    logger.verbose ( 'decorate iEventSelector to deal with RAW/MDF & CASTOR -files' ) 

## Decorate iDataSvc with proper dir/ls methods '
import AnalysisPython.Dir 
logger.verbose( 'decorate iDataSvc with proper "dir/ls" methods ')

## decorate the algorithms with the proper dumpHistos methods
import GaudiPython.Bindings 
_iAlgorithm = GaudiPython.Bindings.iAlgorithm          ##    Algorithm interface
_iAlgTool   = GaudiPython.Bindings.iAlgTool            ##         Tool interface
for _t in ( _iAlgorithm , _iAlgTool ) :
    if not hasattr ( _t , 'dumpHistos' ) :
        logger.verbose ( 'decorate iAlgoritm/iAlgTool with "dumpHistos" method') 
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
    logger.verbose ( 'decorate iAlgoritm with "isEnabled" method')
    _iAlgorithm.isEnabled = lambda self : self.__call_interface_method__("_ialg","isEnabled")

if not hasattr ( _iAlgorithm , 'setEnabled' ) :
    logger.verbose ( 'decorate iAlgoritm with "setEnabled" method' )
    ## enable/disbale 
    def __set_Enabled_ ( self , value ) :
        self.Enable = True if value else False
        return self.Enable
    _iAlgorithm.setEnabled = __set_Enabled_ 

if not hasattr ( _iAlgorithm , 'sysBeginRun' ) :
    logger.verbose ( 'decorate iAlgoritm with "sysBeginRun" method')
    _iAlgorithm.sysBeginRun = lambda self : self.__call_interface_method__("_ialg","sysBeginRun")
if not hasattr ( _iAlgorithm , 'sysEndRun'   ) :
    logger.verbose ( 'decorate iAlgoritm with "sysEndRun" method')
    _iAlgorithm.sysEndRun   = lambda self : self.__call_interface_method__("_ialg","sysEndRun")


_AppMgr = GaudiPython.Bindings.AppMgr
if not hasattr ( _AppMgr , '_new_topAlg_' ) :

    ## get the list of top-level algorithms 
    def __top_Algs_ ( self ) :
        """Get the list of top-level algorithms as ``algorithms''
        >>> gaudi = appMgr()
        >>> print gaudi.topAlgs() 
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
    logger.verbose ( 'decorate AppMgr.topAlgs() ' )

##
if not hasattr ( _AppMgr , '_new_allAlg_' ) :

    ## get the list of all algorithms 
    def __all_Algs_ ( self ) :
        """Get the list of *ALL* algorithms as ``algorithms''        
        >>> gaudi = appMgr()
        >>> print gaudi.allAlgs() 
        """
        _algs = self.algorithms()

        algs  = []
        for _a in _algs :
            algs += [ self.algorithm ( _a ) ]
        return algs

    _AppMgr._new_allAlg_ = __all_Algs_
    _AppMgr.allAlgs      = __all_Algs_
    logger.verbose ( 'decorate AppMgr.allAlgs() ' ) 

##
if not hasattr ( _AppMgr , '_disable_Tops_' ) :

    ## get the list of all algorithms 
    def __disable_Top_ ( self ) :
        """ Disable all top-level algorithms,
        and return the list of disabled algorithsm
        >>> gaudi    = appMgr()
        >>> disabled = gaudi.disableTopAlgs()        
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
    logger.verbose ( 'decorate AppMgr.disableTopAlgs() ' ) 

## 
if not hasattr ( _AppMgr , '_disable_All_' ) :
        
    ## get the list of all algorithms 
    def __disable_All_ ( self ) :
        """ Disable ALL algorithms
        and return the list of disabled algorithsm
        >>> gaudi    = appMgr()
        >>> disabled = gaudi.disableAllAlgs()        
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
    logger.verbose ( 'decorate AppMgr.disableAllAlgs() ' ) 


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
        if self.state() == StateMachine.CONFIGURED :  self.initialize()
        svc    = iHelper.service( self._svcloc, name )
        return iIncSvc(name, svc)
    
    _AppMgr. incSvc = _incSvc_
    logger.verbose ( 'decorate AppMgr.incSvc() ' ) 

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
            """Get the current algorithm
            
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
            """Get the stack of algorithms

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
        """Get context service from Gaudi:
        
        >>> gaudi = ...
        >>> iSvc  = gaudi.cntxSvc()        
        """
        if self.state() == StateMachine.CONFIGURED :  self.initialize()
        svc  = iHelper.service( self._svcloc, name )
        return iCntxSvc( name , svc )
    
    _AppMgr.   cntxSvc = _cntxSvc_
    _AppMgr. contexSvc = _cntxSvc_
    logger.verbose ( 'decorate AppMgr.cntxSvc() ' ) 


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
    logger.verbose ( 'decorate AppMgr.evtColSvc() ' ) 

# =============================================================================
## get the links from data object  
def _links_ ( self ) :
    """Get the links from DataObject :

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

## decorate Particle Property Service 
if not hasattr ( _AppMgr , 'ppSvc' ) :
    import PartProp.Service 
    logger.verbose ( 'decorate AppMgr.ppSvc() ') 


if not hasattr ( _AppMgr , 'magSvc' ) :
    
    iService  = GaudiPython.Bindings.iService
    iProperty = GaudiPython.Bindings.iProperty
    iHelper   = GaudiPython.Bindings.Helper
    iCast     = GaudiPython.Bindings.InterfaceCast
    # =========================================================================
    ## @class iMagneticFieldSvc
    #  python wrapper for IMagneticFieldSvc
    #  @see IMagneticFieldSvc
    #  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
    #  @date   2016-02-17
    class iMagneticFieldSvc(iService) :
        """
        Python wrapper for IMagneticFieldSvc
        """
        def __init__( self , name , isvc ) :
            iService.__init__(self, name, isvc )
            self.__dict__['_imagsvc'] = iCast ( cpp.IMagneticFieldSvc ) ( isvc )
            print 'here3:', self._imagsvc 

        def interface        ( self ) : return self._imagsvc        
        def interface_metods ( self ) :
            """ List of major interface methods
            """
            my_methods  = [ i for i in dir (     iService   ) ]
            my_methods += [ i for i in dir (     iProperty  ) ]
            my_methods += [ i for i in dir ( cpp.IService   ) ]
            my_methods += [ i for i in dir ( cpp.IProperty  ) ]
            my_methods += [ i for i in dir ( cpp.IStateful  ) ]
            my_methods += [ i for i in dir ( cpp.IInterface ) ]
            my_methods = set( my_methods ) 
            if_methods = set()
            for m in dir( cpp.IMagneticFieldSvc ) :
                if m in my_methods : continue 
                if_methods.add ( m )
            return tuple ( if_methods )
        
        ## allow to get 'interface' methods
        def  __getattr__  ( self , attribute ) :

            if cpp.Gaudi.Utils.hasProperty ( self._isvc , attribute ) :
                return GaudiPython.Bindings.iProperty.__getattr__ ( self , attribute )

            ## use the interface method
            _if = self.interface() 
            if _if : return getattr ( _if ,  attribute )
            
            raise AttributeError ('Attribute/property %s does not exist for %s' % ( attribute , self._name ) ) 

    # ===================================================================================
    ## @class iLHCbMagnetSvc
    #  python wrapper for IMagneticFieldSvc
    #  @see ILHCbMagnetSvc
    #  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
    #  @date   2016-02-17
    class iLHCbMagnetSvc(iMagneticFieldSvc) :
        """
        Python wrapper for ILHCbMagnetSvc
        """
        def __init__( self , name , isvc ) :
            iService.__init__(self, name, isvc )
            self.__dict__['_ilhcbmagnet'] = iCast ( cpp.ILHCbMagnetSvc ) ( isvc )

        def interface        ( self ) : return self._ilhcbmagnet
        def interface_metods ( self ) :
            """ List of major interface methods
            """
            my_methods  = [ i for i in dir (     iService   ) ]
            my_methods += [ i for i in dir (     iProperty  ) ]
            my_methods += [ i for i in dir ( cpp.IService   ) ]
            my_methods += [ i for i in dir ( cpp.IProperty  ) ]
            my_methods += [ i for i in dir ( cpp.IStateful  ) ]
            my_methods += [ i for i in dir ( cpp.IInterface ) ]
            my_methods = set( my_methods ) 
            if_methods = set()
            for m in dir( cpp.ILHCbMagnetSvc ) :
                if m in my_methods : continue 
                if_methods.add ( m )
            return tuple ( if_methods )
        
        ## get the value of magnetic field at given point
        #  @param xyz Point at which magnetic field vector will be given
        #  @return fvec Maagnectic field vector.
        #  @code
        #  xyz = Gaudi.Point3D( 0 , 0 , 5.2 * meter )
        #  print svc.field() 
        #  @endcode 
        def field  ( self , xyz ) :
            """ Get the value of magnetic field at given point
            >>> xyz = Gaudi.Point3D( 0 , 0 , 5.2 * meter )
            >>> print svc.field() 
            """
            return self._ilhcbmagnet.fieldVector ( xyz )

        ## direction of magneric field 
        def isDown ( self ) :
            """ Direction of magnetic field
            >>> print svc.isDown() 
            """
            return self._ilhcbmagnet.isDown()
        ## direction of magneric field 
        def isUp   ( self ) :
            """ Direction of magnetic field
            >>> print svc.isUp () 
            """
            return not self.isDown()
        
    # =========================================================================
    ## get incident service form Gaudi 
    def _app_magFieldSvc_ ( self , name = "MagneticFieldSvc" ) :
        """Get magnetic field service from Gaudi:
        
        >>> gaudi = ...
        >>> iSvc  = gaudi.magFieldSvc()        
        """
        if self.state() < StateMachine.INITIALIZED :  self.initialize()
        if self.state() < StateMachine.RUNNING     :  self.start     ()
        print ' I am here 1 '
        svc    = iHelper.service( self._svcloc, name )
        if not svc and not name in self.ExtSvc : self.ExtSvc += [ name ]
        svc    = iHelper.service( self._svcloc, name )            
        print ' I am here 2 ', svc 
        return iMagneticFieldSvc ( name , svc )

    # =========================================================================
    ## get incident service form Gaudi 
    def _app_magSvc_ ( self , name = "MagneticFieldSvc" ) :
        """Get (LHCb) magnetic field service from Gaudi:
        
        >>> gaudi = ...
        >>> iSvc  = gaudi.magSvc()        
        """
        #
        if self.state() < StateMachine.INITIALIZED :  self.initialize()
        if self.state() < StateMachine.RUNNING     :  self.start     ()
        #
        svc    = iHelper.service( self._svcloc, name )
        if not svc and not name in self.ExtSvc : self.ExtSvc += [ name ]
        svc    = iHelper.service( self._svcloc, name )
        return iLHCbMagnetSvc ( name , svc )
    
    _AppMgr. magFieldSvc = _app_magFieldSvc_
    logger.verbose ( 'decorate AppMgr.magFieldSvc() ' ) 
    _AppMgr. magSvc      = _app_magSvc_
    logger.verbose ( 'decorate AppMgr.magSvc()      ' ) 


# =============================================================================
logger.verbose ( 'Suppress excessive prints from GaudiPython.TupleUtils') 
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


if not hasattr ( GaudiPython.Bindings.iAlgTool , '_old_init_' ) :
    
    ##  Get list of methods from abstract iAglTool interface
    #   @code  
    #   tool = ...
    #   for m in tool.interface_methods() : print m
    #   @endcode
    def _interface_methods_ ( self ) :
        """ Get list of methods from abstract iAglTool interface
        >>> tool = ...
        >>> for m in tool.interface_methods() : print m 
        """
        
        if not self._itool : return tuple()
        
        my_methods = list ( dir ( GaudiPython.Bindings.iAlgTool ) )
        my_methods               += [ i for i in dir ( cpp.IAlgTool   ) ] 
        my_methods               += [ i for i in dir ( cpp.IInterface ) ]
        my_methods               += [ i for i in dir ( cpp.IStateful  ) ]
        if self._ip : my_methods += [ i for i in dir ( self._ip       ) ]
        my_methods = set( my_methods )
        if_methods = set()  
        for i in dir( self._itool ) :
            if i in my_methods    : continue
            if_methods.add ( i )
            
        return tuple( if_methods )
    
    _old_iAlgTool_init_ = GaudiPython.Bindings.iAlgTool.__init__
    ## initialize iAlgTool and extend vizibel interface 
    def _new_iAlgTool_init_ ( self , name , itool = None ) :

        ##  invoke 'old' method 
        _old_iAlgTool_init_ ( self , name , itool )
        
        for i in self.interface_methods() : 
            if i in self.__dict__ : continue 
            self.__dict__ [ i ] = getattr ( self._itool , i )
                
             
    ## allow to get 'interface' methods for iAlgTool (experimental)
    def  _itool_getattrib_  ( tool , attribute ) :
        """ Get 'interface' method ofr iAlgTool
        
        """
        if cpp.Gaudi.Utils.hasProperty ( tool._itool , attribute ) :
            return GaudiPython.Bindings.iProperty.__getattr__ ( tool , attribute )
        
        if tool._itool :
            return getattr  ( tool._itool , attribute ) 
        
        raise AttributeError ('Attribute/property %s does not exist for %s' % ( attribute , tool ) ) 

    GaudiPython.Bindings.iAlgTool._old_init_        = _old_iAlgTool_init_
    GaudiPython.Bindings.iAlgTool._new_init_        = _new_iAlgTool_init_
    GaudiPython.Bindings.iAlgTool.__init__          = _new_iAlgTool_init_
    GaudiPython.Bindings.iAlgTool.interface_methods = _interface_methods_  
    GaudiPython.Bindings.iAlgTool.__getattr__       = _itool_getattrib_   
    
    logger.verbose ( 'more decoration for iAlgTool: add interface methods') 



##  print ut for tthe tools  
def _itool_str_ ( self ) :

    if not self._itool :
        tname = self._name 
        if 0 == tname.find('ToolSvc.') : tname = tname[8:]
        return "iAlgTool('%s')" % tname 

    tname = self._itool.name()
    if 0 == tname.find('ToolSvc.') : tname = tname[8:]
    ttype = self._itool.type()
    iif   = type( self._itool ).__name__ 
    if 0 == iif.find ( 'cppyy.' ) : iif = iif [6:] 
    if 0 == iif.find ( 'cpp.'   ) : iif = iif [4:] 

    if tname == ttype : 
        return "iAlgTool('%s',interface=%s)" % ( tname , iif ) 
    return "iAlgTool('%s/', interface=%s)" % ( ttype , tname , iif ) 
        
GaudiPython.Bindings.iAlgTool. __str__  = _itool_str_
GaudiPython.Bindings.iAlgTool. __repr__ = _itool_str_

if not hasattr ( GaudiPython.Bindings.iToolSvc , 'tool' ) :

    ## get the tool from tool service 
    def _its_tool_ ( self ,  typename , name = None , interface = None , createIf = True , parent = None   ) :
        """ Get the tool form Tool Service
        >>> toolSvc = ...
        >>> dvt = toolSvc.tool('DaVinci::Transporter/QUQU', interface = cpp.IParticleTransporter )  
        """
        if not name :
            t,s,n = typename.rpartition('/')
            if t and s and n :
                typename = t
                name     = n
            else : name = typename
            
        p1 = typename.find ( ':PUBLIC' )
        if 0 < p1 and  p1 + 6 == len(typename) :
            typename = typename [:p1]
                
        itool = GaudiPython.Bindings.Helper.tool ( self._its , typename , name  , parent , createIf ) 
        if itool and  interface :
            iif = GaudiPython.Bindings.InterfaceCast(interface)( itool )
            if not iif : logger.warning("Can't retrieve proepr interface %s  for %s" % ( interface , itool ) )
            return GaudiPython.Bindings.iAlgTool  ( itool.name() , iif )
        elif not itool : logger.warning("Can't retrieve the tool  %s'%s" % ( typename , name ) )
    
        return GaudiPython.Bindings.iAlgTool  ( name , itool  )
    
    GaudiPython.Bindings.iToolSvc.tool = _its_tool_
    
    logger.verbose ( 'Decoration for iToolSvc.tool') 
    

# =============================================================================
## colors 
BLACK, RED, GREEN, YELLOW, BLUE, MAGENTA, CYAN, WHITE = range(8)
# =============================================================================
## printout of status code
def _sc_print_ ( sc ) :
    """Print the Status Code
    >>> st = ...
    >>> print st
    """
    from Bender.Logger import colored_string 
    if   sc.isSuccess     () : return colored_string( 'SUCCESS'     , WHITE , GREEN  , True ) 
    elif sc.isRecoverable () : return colored_string( 'RECOVERABLE' , RED   , YELLOW , True ) 
    elif 0 != sc.getCode  () :
        return colored_string('FAILURE[%d]' % sc.getCode() , YELLOW , RED , True ) 
    return colored_string('FAILURE' , YELLOW , RED , True ) 

cpp.StatusCode .__repr__ = _sc_print_
cpp.StatusCode .__str__  = _sc_print_

SUCCESS = cpp.StatusCode(cpp.StatusCode.SUCCESS,True) 

if not hasattr ( _AppMgr , '_old_initialize_' ) :
    
    _old_appmgr_initialize_ = _AppMgr.initialize
    def _new_initialize_ ( self ) :
        """Initialize application manager 
        """
        
        ## already initialized 
        if self.FSMState() >= StateMachine.INITIALIZED  : return SUCCESS
        
        import Bender.Utils as BU
        with BU.Action ( BU. preInit_actions() , BU.postInit_actions() ) :
            sc = self._old_initialize_ ()
            if sc.isFailure() :
                logger.error('Error from AppMgr.initialize %s' % sc )
                return sc
            
        return sc 
        
    _AppMgr.initialize       =        _new_initialize_ 
    _AppMgr._old_initialize_ = _old_appmgr_initialize_ 

if not hasattr ( _AppMgr , '_old_start_' ) :
    
    _old_appmgr_start_      = _AppMgr.start    
    def _new_start_ ( self ) :
        """Start application manager 
        """
        ## already running 
        if self.FSMState() >= StateMachine.RUNNING : return SUCCESS
        
        if self.FSMState() < StateMachine.INITIALIZED :
            sc = self.initialize()
            if sc.isFailure() : return sc
            
        import Bender.Utils as BU
        with BU.Action ( BU. preStart_actions() , BU.postStart_actions() ) :
            sc = self._old_start_ ()
            if sc.isFailure() :
                logger.error('Error from AppMgr.start %s' % sc )
                return sc
            
        return sc
        
    _AppMgr.start            =        _new_start_ 
    _AppMgr._old_start_      = _old_appmgr_start_ 


if not hasattr ( _AppMgr , '_old_run_' ) :
    
    _old_appmgr_run_      = _AppMgr.run    
    def _new_run_ ( self , nevt = -1 ) :
        """Run ``nevt'' events
        """
        if self.FSMState() < StateMachine.RUNNING :
            sc = self.start()
            if sc.isFailure() : return sc
        
        import Bender.Utils as BU
        with BU.Action ( BU. preRun_actions() , BU.postRun_actions() ) :
            sc = self._old_run_ ( nevt )
            if sc.isFailure() :
                logger.error('Error from AppMgr.run %s' % sc )
                return sc
            
        return sc
    
    _AppMgr.run            =        _new_run_ 
    _AppMgr._old_run_      = _old_appmgr_run_ 


## def _new_run_( self ,  n ) :
    
##     if self.FSMState() == StateMachine.CONFIGURED :    
##         sc = self.initialize()
##         if sc.isFailure() or self.ReturnCode != 0 :
##             return sc
##     if self.FSMState() == StateMachine.INITIALIZED :
##         sc = self.start()
##         if sc.isFailure() or self.ReturnCode != 0:
##             return sc
##     return self._evtpro.executeRun(n)

if hasattr ( cpp.LoKi , 'Print' )  and hasattr ( cpp.LoKi.Print , 'print_string' ) :

    _print_func_ = cpp.LoKi.Print.print_string
    _MS          = cpp.MsgStream


    def _ms_enter_ ( self ) : return self
    def _ms_exit_  ( self, *_ ) :
        if self.isActive() :
            self.doOutput()
    def _ms_lshift_ ( self,  data ) :
        if self.isActive() :
            if not isinstance ( data , str ) : data = str ( data )
            if data : _print_func_ (  self , data )
        return self
    def _ms_rrshift_ ( self,  data ) :
        return _ms_lshift_ ( self , data )

    _MS.__enter__   = _ms_enter_
    _MS.__exit__    = _ms_exit_
    _MS.__lshift__  = _ms_lshift_
    _MS.__rrshift__ = _ms_rrshift_
    _MS.__nonzero__ = lambda s : s.isActive()
    
    logger.debug ( 'Decorate MsgStream' )
    
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
