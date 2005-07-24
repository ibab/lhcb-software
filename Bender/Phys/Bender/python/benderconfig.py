# $Id: benderconfig.py,v 1.11 2005-07-24 14:04:41 ibelyaev Exp $
# =============================================================================
# CVS version $Revision: 1.11 $ 
# =============================================================================
# CVS tag $Name: not supported by cvs2svn $ 
# =============================================================================

import gaudimodule
import os , string , tempfile 

class iJobOptSvc(gaudimodule.iService) :
    def __init__( self , svc ) :
        self.__dict__['_optsvc'] = gaudimodule.Interface(gaudimodule.gbl.IJobOptionsSvc).cast(svc)
        gaudimodule.iService.__init__( self , svc )
    def getProperties( self , name ) :
        """
        Extract *ALL* properties of the given component
        
        Usage :
        
        >>> optsvc = gaudi.optSvc() 
        >>> props = optsvc.getProperties( 'Name' )
        
        """
        props = self._optsvc.getProperties(name)
        prps = {}
        if not props : return  prps              # return
        for p in props :
            prop  = p.name () 
            value = p.value()
            if type( value ) is str :  prps[ prop  ] = eval(value)
            else :
                if len(value) > 0 and not self.isnumber(value):
                    prps[ prop ] = [ eval(i) for i in value ]
                else:
                    prps[ prop ] = [     i   for i in value ]
        return prps
        
def _optSvc_ ( self ) :
    return iJobOptSvc( self._optsvc )

gaudimodule.AppMgr.optSvc = _optSvc_

def _parse_( line , tokens = ('DATA','TYPE')) :
    """
    Trivial function to parse the line with semantics :
    " token1='MyData' token2='MyType' " 
    """
    items   = line.split()
    result = []
    for token in tokens :
        value = None 
        for item in items :
            if item.startswith( token + '=' ) : value = item[ len(token)+2:-1]
        if not value  : return None
        result += [ value ]
    return tuple(result) 
    
class iDataOnDemandSvc(gaudimodule.iService) :
    def __init__   ( self , name = 'DataOnDemandSvc' , svc = None ) :
        gaudimodule.iService.__init__( self , name , svc )
        
    def getAlgorithms ( self ) :
        _algs = self.Algorithms
        print 'native:',_algs 
        _dict = {}
        for _alg in _algs :
            item = _parse_ ( _alg )
            if not item : continue
            _dict [ item[0] ] = item[1]
        return _dict
    
    def algorithms ( self , algs  = {} ) :
        if algs :
            _dict = self.getAlgorithms()
            _dict.update ( algs )
            _list = [] 
            for key in _dict :
                _list += [ "DATA='%s' TYPE='%s'" % ( key , _dict[key] ) ]
            self.Algorithms = _list
        return self.getAlgorithms()
    
    def getNodes   ( self ) :
        _nodes = self.Nodes
        _dict = {}
        for _node in _nodes :
            item = _parse_ ( _node )
            if not item : continue
            _dict [ item[0] ] = item[1]
        return _dict
    
    def nodes      ( self , nodes = {} ) :
        if nodes :
            _dict = self.getNodes()
            _dict.update ( nodes )
            _list = [] 
            for key in _dict :
                _list += [ "DATA='%s' TYPE='%s'" % ( key , _dict[key] ) ]
            self.Nodes = _list
        return self.getNodes()
        
def _onDemandSvc_ ( self , name = 'DataOnDemandSvc' ) :
    return iDataOnDemandSvc( name ) ;

gaudimodule.AppMgr.onDemand        = _onDemandSvc_
gaudimodule.AppMgr.onDemandSvc     = _onDemandSvc_
gaudimodule.AppMgr.dataOnDemand    = _onDemandSvc_
gaudimodule.AppMgr.dataOnDemandSvc = _onDemandSvc_


def _dir_ ( self , node = None , level = -1 ) :
    """
    The simple tool to perform the inspection fo Data Store

    Usage:

    evtSvc = gaudi.evtSvc()
    evtSvc.dir('MC')
    data = evtSvc['Rec/Calo']
    evtSvc.dir( data)
    
    """
    if 0 == level          : return gaudiModule.SUCCESS ;
    if   str is type(node) : node = self.retrieveObject( node )
    elif  not node         : return self.dir('', level )
    if not node            : return gaudimodule.FAILURE
    if     hasattr ( node , 'registry'   ) : node = node.registry()
    if     hasattr ( node , 'identifier' ) :
        obj = node.object () 
        if not obj :  print  node.identifier()
        else :
            if hasattr( obj , 'size'    ) :
                size = obj.size()
                if 0 == size : print "%s (empty) %s" % (  node.identifier() , type( obj ) )
                elif hasattr ( obj , 'containedObjects' ) :
                    c = obj.containedObjects()[0]
                    print "%s container of %d %s objects " % ( node.identifier() , obj.size() , type(c))
                else : 
                    print  "%s %s " % (  node.identifier() , type( node.object()) )
            else :
                print  "%s %s " % (  node.identifier() , type( node.object()) )
    else : print "The node has no 'identifier'"
    # start the recursion 
    for l in self.leaves( node ) :
        if l : self.dir( l , level - 1 )  
    return gaudimodule.SUCCESS 
    
gaudimodule.iDataSvc.dir = _dir_
gaudimodule.iDataSvc.ls  = _dir_

def _config_( self , **args ) :
    """  
    Simple utility to perform the configuration of Gaudi application.
    It reads the set of input job-options files, and set few
    additional parameters 'options' through the usage of temporary *.opts file
    
    Usage:
    
    gaudi.config( files   = [ '$GAUSSOPTS/Gauss.opts'                       ,
                              '$DECFILESROOT/options/10022_010.0GeV.opts' ] ,
                  options = [ 'EventSelector.PrintFreq   =   5  '         ] )
    
    """
    files  = args.get('files',[])
    for file in files :
      sc = self.readOptions( file   )
      if sc.isFailure() : raise RuntimeError , ' Unable to read file "' + file +'" '
    options  = args.get('options',None)
    if options :
      import tempfile
      tmpfilename = tempfile.mktemp()
      tmpfile     = open( tmpfilename, 'w' )
      tmpfile.write ( '#pragma print on  \n' ) 
      tmpfile.write ( '/// File         "' + tmpfilename+'" generated by GaudiPython \n\n' ) 
      for opt in options :
        if type(options) is dict :
          tmpfile.write( ' \t ' + opt + ' = '+ options[opt]+ ' ;  // added by GaudiPython \n' )
        else :
          tmpfile.write( ' \t ' + opt + ' ;  // added by GaudiPython \n' )
      tmpfile.write ( '/// End of  file "' + tmpfilename+'" generated by GaudiPython \n\n' ) 
      tmpfile.close() 
      sc = self.readOptions( tmpfilename )
      if sc.isFailure() : raise RuntimeError , ' Unable to read file "' + tmpfilename +'" '
      os.remove( tmpfilename )
    # We need to make sure that the options are taken by the ApplicationMgr
    props = self.optSvc().getProperties('ApplicationMgr')
    # adjust DLLs 
    for dll in props.get ( 'DLLs'  , [] ) :
        if dll not in self.DLLs   : self.DLLs   += [dll]
    if props.has_key('DLLs') : del props['DLLs']
    # adjust Services 
    for svc in props.get ( 'ExtSvc', [] ) :
        if svc not in self.ExtSvc : self.ExtSvc += [svc]
    if props.has_key('ExtSvc') : del props['ExtSvc']
    for key in props.keys() : self.__setattr__( key , props[key] ) 
    return gaudimodule.SUCCESS

gaudimodule.AppMgr.config = _config_

def _histofile_( self , name , Type  = None ) :
    """
    The setter for output histogram name
    
    Usage:
    
    hsvc = gaudi.histoSvc()
    hsvc.defineOutput( 'myfile', Type = 'HBOOK' )
    
    """
    _g = gaudimodule.AppMgr()
    hsvc = _g.service('HistogramPersistencySvc')
    hsvc.OutputFile = name
    # redefine the persistency 
    if Type : _g.HistogramPersistency = Type
    return gaudimodule.SUCCESS

gaudimodule.AppMgr.histoSvc             = gaudimodule.AppMgr.histSvc
gaudimodule.iHistogramSvc.defineOutput  = _histofile_
gaudimodule.iHistogramSvc.setOutputFile = _histofile_
gaudimodule.iHistogramSvc.setOutput     = _histofile_

#def _ntuplesvc_( self , name = 'NTupleSvc' ) :
#    if 'NTupleSvc' not in self.ExtSvc : self.ExtSvc += ['NTupleSvc']
#    svc = gaudimodule.Helper.service( self._svcloc, name, True )
#    return gaudimodule.iNTupleSvc(name, svc)
#
# gaudimodule.AppMgr.ntuplesvc            = _ntuplesvc_

gaudimodule.AppMgr.nTupleSvc            = gaudimodule.AppMgr.ntuplesvc
gaudimodule.AppMgr.nTupSvc              = gaudimodule.AppMgr.ntuplesvc
gaudimodule.AppMgr.ntupSvc              = gaudimodule.AppMgr.ntuplesvc


# =============================================================================
# $Log: not supported by cvs2svn $
# Revision 1.10  2005/05/20 10:55:19  ibelyaev
#  prepare for v4r8
#
# Revision 1.9  2005/02/08 20:31:51  ibelyaev
#  bug fix and some performace optimisation
#
# Revision 1.8  2005/01/24 17:44:39  ibelyaev
#  v4r5
#
# =============================================================================
# The END 
# =============================================================================
