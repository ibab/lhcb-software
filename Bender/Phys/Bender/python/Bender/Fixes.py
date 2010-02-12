#!/usr/bin/env python 
# =============================================================================
## The helper Python module for Bender application 
#
#  This file is a part of 
#  <a href="http://cern.ch/lhcb-comp/Analysis/Bender/index.html">Bender project</a>
#  <b>"Python-based Interactive Environment for Smart and Friendly 
#   Physics Analysis"</b>
#
#  The package has been designed with the kind help from
#  Pere MATO and Andrey TSAREGORODTSEV. 
#  And it is based on the 
#  <a href="http://cern.ch/lhcb-comp/Analysis/LoKi/index.html">LoKi project:</a>
#  "C++ ToolKit for Smart and Friendly Physics Analysis"
#
#  By usage of this code one clearly states the disagreement 
#  with the campain of Dr.O.Callot et al.: 
#  "No Vanya's lines are allowed in LHCb/Gaudi software."
#
#  @date   2004-07-11
#  @author Vanya BELYAEV ibelyaev@physics.syr.edu
# =============================================================================
"""
The helper module for Bender application.
It applies some last-moment (version-dependent) fixes 
"""
# =============================================================================
__author__  = 'Vanya BELYAEV ibelyaev@physics.syr.edu'
# =============================================================================
__version__ = ' CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.5 $' 
# =============================================================================
    
## "at-exit action 
def _bender_at_exit_ () :
    """
    
    At-Exit action
    
    """

    print '*'*80
    print 'Bender.Fixes:  custom "atexit" handler is being invoked'
    print '*'*80
    
    from GaudiPython.Bindings import AppMgr
    g = AppMgr()
    g.exit()

    print '*'*80
    print 'Bender.Fixes:  custom "atexit" handler has been invoked'
    print '*'*80



import atexit
atexit.register( _bender_at_exit_ )



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
                        



print '*'*100
print 'Bender.Fixes:  add custom "atexit" handler'
print 'Bender.Fixes:  decorate iEventSelector to open RAW/MDF & CASTOR -files'
print '*'*100


# =============================================================================
# The END 
# =============================================================================
