#!/usr/bin/env python 
# =============================================================================
## The helper Python module for Bender application 
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
#  with the campain of Dr.O.Callot et al.: 
#  ``No Vanya's lines are allowed in LHCb/Gaudi software.''
# 
#  @date   2004-07-11
#  @author Vanya BELYAEV ibelyaev@physics.syr.edu
# =============================================================================
"""
The helper module for Bender application.
It applies some last-moment (version-dependent) fixes

This file is a part of BENDER project:
``Python-based Interactive Environment for Smart and Friendly Physics Analysis''

The project has been designed with the kind help from
Pere MATO and Andrey TSAREGORODTSEV. 

And it is based on the 
LoKi project: ``C++ ToolKit for Smart and Friendly Physics Analysis''

By usage of this code one clearly states the disagreement 
with the campain of Dr.O.Callot et al.: 
``No Vanya's lines are allowed in LHCb/Gaudi software.''

"""
# =============================================================================
__author__  = 'Vanya BELYAEV ibelyaev@physics.syr.edu'
__date__    = "2004-07-11"
__version__ = ' CVS tag $Name: not supported by cvs2svn $, version $Revision$'
__all__     = ()
# =============================================================================

## some straneglines... Refles feature ?
from GaudiPython.Bindings import gbl as cpp

print '*'*120

# =============================================================================
## "at-exit action 
def _bender_at_exit_ () :
    """
    
    At-Exit action
    
    """

    print '*'*120
    print 'Bender.Fixes:  custom "atexit" handler is being invoked'
    print '*'*120
    
    from GaudiPython.Bindings import _gaudi 
    if _gaudi :
        print 'Bender.Fixes:  AppMgr.exit() is being invoked'
        _gaudi.exit()

    print '*'*120
    print 'Bender.Fixes:  custom "atexit" handler has been invoked'
    print '*'*120


import atexit
atexit.register( _bender_at_exit_ )
print 'Bender.Fixes: - add custom "atexit" handler'


import GaudiPython.Bindings

_EvtSel = GaudiPython.Bindings.iEventSelector

if not hasattr ( _EvtSel , '_openNew_') :
    
    def _openNew_ ( self, stream, typ = 'POOL_ROOT', opt = 'READ', sel = None, fun = None, collection = None ):
        """
        Open the file(s) :

        >>> evtSel.open ( 'file.dst' )                   ## open one file 
        >>> evtSel.open ( [ 'file1.dst' , 'file2.dst'] ) ## list of files 
        >>> evtSel.open ( '/castor/.../file.dst' )       ## open castor file 
        >>> evtSel.open ( 'file.raw' )                   ## open RAW file 
        >>> evtSel.open ( 'file.mdf' )                   ## open MDF file 
        
        """
        
        if typ == 'ROOT' :
            self.g.declSvcType('RootEvtCnvSvc','DbEventCnvSvc')
            self.g.service('RootEvtCnvSvc').DbType  = 'ROOT'
            self.g.createSvc('RootEvtCnvSvc')
            self.g.service('EventPersistencySvc').CnvServices = ['RootEvtCnvSvc']
        elif typ == 'POOL_ROOT':
            cacsvc = self.g.service('PoolDbCacheSvc')
            if hasattr(cacsvc, 'Dlls') : cacsvc.Dlls += ['lcg_RootStorageSvc', 'lcg_XMLCatalog']
            else :                       cacsvc.Dlls = ['lcg_RootStorageSvc', 'lcg_XMLCatalog']
            cacsvc.OutputLevel = 4
            cacsvc.DomainOpts    = [ 'Domain[ROOT_All].CLASS_VERSION=2 TYP=int',
                                     'Domain[ROOT_Key].CLASS_VERSION=2 TYP=int',
                                     'Domain[ROOT_Tree].CLASS_VERSION=2 TYP=int' ]
            cacsvc.DatabaseOpts  = ['']
            cacsvc.ContainerOpts = ['']
            self.g.createSvc('PoolDbCacheSvc')
            cnvSvcs = [('PoolRootEvtCnvSvc',     'POOL_ROOT'),
                       ('PoolRootTreeEvtCnvSvc', 'POOL_ROOTTREE'),
                       ('PoolRootKeyEvtCnvSvc',  'POOL_ROOTKEY')]
            for svc in cnvSvcs :
                self.g.declSvcType(svc[0], 'PoolDbCnvSvc')
                cnvsvc = self.g.service(svc[0])
                cnvsvc.DbType = svc[1]
            self.g.service('EventPersistencySvc').CnvServices = [ svc[0] for svc in cnvSvcs ]
            for svc in cnvSvcs :
                self.g.createSvc(svc[0])
                
        self.g.service('EventDataSvc').RootCLID = 1
        
        if type(stream) != list : stream = [stream]
        fixpart = "TYP=\'%s\' OPT=\'%s\'" % ( typ, opt )
        if sel        : fixpart += " SEL=\'%s\'" % sel
        if fun        : fixpart += " FUN=\'%s\'" % fun
        if collection : fixpart += " COLLECTION=\'%s\'" % collection

        cstream = [] 
        for f in stream :

            mask = "DATAFILE=\'%s\' %s "
            ## prepend castor if needed 
            if 0 == f.find('/castor') : f = 'castor:' + f

            if 0 < f.find ( '.raw') or 0 < f.find ( '.mdf') :
                cstream += [ "DATA=\'%s\' SVC='LHCb::MDFSelector' " % f ]                
                if 'LHCb::RawDataCnvSvc' not in self.g.service('EventPersistencySvc').CnvServices :
                    self.g.service('EventPersistencySvc').CnvServices.append ( 'LHCb::RawDataCnvSvc')
                    self.g.createSvc('LHCb::RawDataCnvSvc')
            else: 
                cstream += ["DATAFILE=\'%s\' %s" % ( f, fixpart) ] 

        self.Input = cstream
        self.reinitialize()


    _EvtSel._openNew_ = _openNew_
    _EvtSel.open      = _openNew_
    print 'Bender.Fixes: - decorate iEventSelector to deal with RAW/MDF & CASTOR -files'

## Decorate iDataSvc with proper dir/ls methods '
import AnalysisPython.Dir 
print 'Bender.Fixes: - decorate iDataSvc with proper "dir/ls" methods '

## decorate the algorhtms with the proper dumpHistos methods
import GaudiPython.Bindings 
_iAlgorithm = GaudiPython.Bindings.iAlgorithm          ##    Algorithm interface
_iAlgTool   = GaudiPython.Bindings.iAlgTool            ##         Tool interface
for _t in ( _iAlgorithm , _iAlgTool ) :
    if not hasattr ( _t , 'dumpHistos' ) :
        print 'Bender.Fixes: - decorate iAlgoritm/iAlgTool with "dumpHistos" method'
        ## dump all histogram from the given component 
        def _dumpHistos ( cmp , *args ) :
            """
            ASCII-Dump for 1D-histograms and 1D-profiles

            >>> cmp = ...  # get the component
            >>> cmp.dumpHistos ()
            
            """
            _histos = cmp.Histos()
            for _k in _histos :
                _h = _histos[_k]
                if hasattr ( _h , 'dump' ) :print _h.dump( *args )
        
        _t.dumpHistos = _dumpHistos 

if not hasattr ( _iAlgorithm , 'isEnabled' ) :
    print 'Bender.Fixes: - decorate iAlgoritm with "isEnabled" method'
    _iAlgorithm.isEnabled = lambda self : self.__call_interface_method__("_ialg","isEnabled")

if not hasattr ( _iAlgorithm , 'setEnabled' ) :
    print 'Bender.Fixes: - decorate iAlgoritm with "setEnabled" method'
    ## enable/disbale 
    def __set_Enabled_ ( self , value ) :
        self.Enable = True if value else False
        return self.Enable
    _iAlgorithm.setEnabled = __set_Enabled_ 

# =============================================================================
## decorate stat-entity 
_SE = cpp.StatEntity
_iadd_old_ = _SE.__iadd__
def _iadd_new_ (s,v) :
    if isinstance ( v , (int,long) ) : v = float(v) 
    return _iadd_old_(s,v)
_SE.__iadd__ = _iadd_new_
_SE.__str__  = _SE.toString 
print 'Bender.Fixes: - decorate StatEntity operator += '


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
    print 'Bender.Fixes: - decorate AppMgr.topAlgs() '

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
    print 'Bender.Fixes: - decorate AppMgr.allAlgs() '

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
                _a.setEnable ( False )
                _disabled += [ _a ]
             
        return _disabled 

    _AppMgr._disable_Tops_   = __disable_Top_ 
    _AppMgr.disableTopAlgs   = __disable_Top_ 
    print 'Bender.Fixes: - decorate AppMgr.disableTopAlgs() '

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
                _a.setEnable ( False )
                _disabled += [ _a ]
             
        return _disabled 

    _AppMgr._disable_All_   = __disable_All_ 
    _AppMgr.disableAllAlgs  = __disable_All_ 
    print 'Bender.Fixes: - decorate AppMgr.disableAllAlgs() '



# =============================================================================
## decorate the ranges
# =============================================================================

_rb = cpp.Gaudi.RangeBase_

if not hasattr ( _rb , '__iter__' ) or not hasattr ( _rb , '__getslice__' )    :
    
    import LoKiCore.decorators
    
    if not hasattr ( _rb , '__iter__' ) or not hasattr ( _rb , '__getslice__' )    :
        
        _rb .__iter__     = LoKiCore.decorators._iter_1_  
        _rb .__getslice__ = LoKiCore.decorators._slice_ 
        print 'Bender.Fixes: - Fix the problem with Gaudi.RangeBase_ '
    
print '*'*120

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
