#!/usr/bin/env python
# =============================================================================
# @file KaliCalo/DataUtils.py
#
# Helper module to deal with data.
# The moduel has been exported from Bender project 
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
# @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
# @date   2010-02-12
# =============================================================================
"""
Helper module to deal with data

This file is a part of BENDER project:
``Python-based Interactive Environment for Smart and Friendly Physics Analysis''

The project has been designed with the kind help from
Pere MATO and Andrey TSAREGORODTSEV. 

And it is based on the 
LoKi project: ``C++ ToolKit for Smart and Friendly Physics Analysis''


"""
# =============================================================================
__author__  = "Vanya BELYAEV Ivan.Belyaev@itep.ru"
__date__    = "2010-02-12"
__version__ = "CVS Tag $Name: not supported by cvs2svn $, version $Revision: 1.1 $"
__all__     =  ( 'setData'      ,
                 'setInputData' , 
                 'extendfile'   )
# =============================================================================
## Helper function to 'extend' the short file name
#  @author Vanya Belyaev  Ivan.Belyaev@itep.ru
#  @date 2010-02-12
def extendfile ( filename ) :
    """
    Helper function to 'extend' the short file name 
    """
    if 0 <= filename.find(' ') : return filename
    ## 
    if 0 == filename.find (  '/castor'   ) : filename = 'castor:' + filename
    if 0 == filename.find ( '//castor'   ) : filename = 'castor:' + filename
    if 0 == filename.find ( '/lhcb/data' ) : filename = 'LFN:'    + filename
    ##
    pattern = "DATAFILE='%s' %s OPT='READ'"
    typ     = "TYPE='POOL_ROOTTREE'"
    if 0 <= filename.find ( '.mdf' ) : typ = "SVC='LHCb::MDFSelector'"
    if 0 <= filename.find ( '.raw' ) : typ = "SVC='LHCb::MDFSelector'"
    return pattern %  ( filename , typ )

# =============================================================================
## Define the input data for the job:
#  Attention: the function can be used both for static and dynamic configurations
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2010-02-12
def setData ( files , catalogs = [] ) :
    """
    Define the input data for the job:
    
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
        
        if files :
            
            files = [ extendfile ( f ) for f in files ]
            
            from Gaudi.Configuration import EventSelector
            EventSelector ( Input = files )
            
        if catalogs :
            
            from Gaudi.Configuration import Gaudi__MultiFileCatalog as FileCatalog
            FileCatalog   ( Catalogs = catalogs )
            from Gaudi.Configuration import FileCatalog
            FileCatalog   ( Catalogs = catalogs )
            
    else :                        ## here we deal with the actual components
        
        if files : 
            _e = _gaudi.evtSel()
            _e.open ( files )
        if catalogs :
            _f = _gaudi.service ( 'FileCatalog' )
            _f.Catalogs = catalogs


## ditto...
setInputData = setData


import GaudiPython.Bindings

_EvtSel = GaudiPython.Bindings.iEventSelector

if not hasattr ( _EvtSel , '_openNew_') :
    
    ## Open the file(s) :
    def _openNew_ ( self , stream, typ = 'POOL_ROOT', opt = 'READ', sel = None, fun = None, collection = None ):
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

# =============================================================================
if __name__ == '__main__' :
    print '*'*120
    print                      __doc__
    print ' Author  : %s ' %   __author__    
    print ' Version : %s ' %   __version__
    print ' Date    : %s ' %   __date__
    print ' dir(%s) : %s ' % ( __name__    , dir() )
    print '*'*120


# =============================================================================
# The END 
# =============================================================================
