import GaudiPython
from Gaudi.Configuration import*
from Configurables import ConfigStackAccessSvc, ConfigDBAccessSvc, ConfigTarFileAccessSvc, ConfigZipFileAccessSvc, ConfigFileAccessSvc, PropertyConfigSvc

# pick the default config access svc
from Configurables import ConfigTarFileAccessSvc as ConfigAccessSvc

from Sandbox import execInSandbox
from pprint import pprint

### add some decoration...
GaudiPython.gbl.Gaudi.Math.MD5.__str__ = GaudiPython.gbl.Gaudi.Math.MD5.str
GaudiPython.gbl.ConfigTreeNodeAlias.alias_type.__str__ = GaudiPython.gbl.ConfigTreeNodeAlias.alias_type.str
digest = GaudiPython.gbl.Gaudi.Math.MD5.createFromStringRep
alias = GaudiPython.gbl.ConfigTreeNodeAlias.alias_type
topLevelAlias = GaudiPython.gbl.ConfigTreeNodeAlias.createTopLevel
TCK = GaudiPython.gbl.ConfigTreeNodeAlias.createTCK
vector_string = GaudiPython.gbl.std.vector('std::string')

defaultCas = ConfigZipFileAccessSvc(File = '$STRIPPINGTCKROOT/config.zip')

def _appMgr() :
    ApplicationMgr().AppName = ""
    ApplicationMgr().OutputLevel = ERROR
    appMgr = GaudiPython.AppMgr()
    appMgr.initialize()
    return appMgr

def _tck(x) :
    if type(x) == str and x[0:2].upper() == '0X' :  return int(x,16)
    return int(x)

def _tck2id(x,cas = defaultCas ) :
    name = cas.getFullName()
    appMgr = _appMgr()
    appMgr.createSvc(name)
    s = appMgr.service(name,'IConfigAccessSvc')
    for i in s.configTreeNodeAliases( alias( 'TCK/'  ) ) :
        if _tck(i.alias().str().split('/')[-1]) == _tck(x) : return i.ref().str()
    return None
    #n = s.readConfigTreeNodeAlias( alias( 'TCK/0x%08x'%tck  ) ) 
    #invalid = GaudiPython.gbl.ConfigTreeNode() 
    #val =  n.get_value_or( invalid )
    #return val.digest() if val.digest() != invalid.digest() else None

def tck2id(x,cas) :
    if type(x) is int : x = '0x%08x' % x 
    import re
    if re.compile('^[0-9a-fA-F]{32}$').match(x) :
        # x is already a configID
        return x
    elif re.compile('^0x[0-9a-fA-F]{8}$').match(x) :
        tck = execInSandbox(_tck2id,x,cas)
        if not tck : raise KeyError("not a valid TCK: "+str(x))
        return tck
    else :
        raise KeyError("not a valid TCK: "+str(x))

def _orphanScan(cas = defaultCas ) :
    treeNodeDict = dict()
    leafNodeDict = dict()
    import os
    dir = None
    if 'Directory' in cas.getProperties() : 
        dir = cas.getProp('Directory')
    if not dir  and 'Directory' in cas.getDefaultProperties():
        dir = cas.getDefaultProperty('Directory')
    for root,dirs,files in os.walk( dir ) :
        print 'checking ' + root
        import fnmatch
        for d in fnmatch.filter(dirs,'CVS') : dirs.remove(d)
        def updateDict( d, f) :
            if f not in d : d.update({f:list()})
        for f in files : 
            if 'PropertyConfigs' in root: updateDict( leafNodeDict, f )
            if 'ConfigTreeNodes' in root: updateDict( treeNodeDict, f )
    info = getConfigurations(cas)
    for (k,v) in info.iteritems():
        print '\n\n'
        print k 
        v.printSimple()
        id = v.info['id'] 
        tree =  execInSandbox( _getConfigTree, id, cas )
        for node in tree :
           def updateDict( d, id, top ) :
                if id not in d: d.update({ id: list()})
                if top not in d[ id ] : d[id] += [ top ]
           updateDict( treeNodeDict,  node.digest, k )
           if node.leaf : updateDict( leafNodeDict, node.leaf.digest, k )
    print 'leafNodes orphans: '
    for (k,v) in leafNodeDict.iteritems() :
        if not v : print k
    print 'treeNodes orphans: '
    for (k,v) in treeNodeDict.iteritems() :
        if not v : print k


def _digest(x,cas) :
    if type(x) == str : x = digest( x )
    return x

class _TreeNodeCache:
    ncache = None
    lcache = None
    svc   = None
    def __init__(self,svc=None):
       if type(_TreeNodeCache.ncache) is type(None) :
         if type(svc) is type(None):
             raise RuntimeWarning('oops... no svc during singleton creation')
         _TreeNodeCache.ncache = dict()
         _TreeNodeCache.lcache = dict()
         _TreeNodeCache.svc = svc
       elif svc and svc != TreeNodeCache.svc :
         raise RuntimeWarning('oops... already have a different svc during singleton access ')
    def node(self,id):
       if id not in _TreeNodeCache.ncache : 
         node = _TreeNodeCache.svc.resolveConfigTreeNode( id ) 
         _TreeNodeCache.ncache[ id ] = node
       return _TreeNodeCache.ncache[ id ]
    def leaf(self,id):
       if not id.valid() : return None
       if id not in _TreeNodeCache.lcache :
          leaf = _TreeNodeCache.svc.resolvePropertyConfig( id )
          _TreeNodeCache.lcache[ id ] = leaf
       return _TreeNodeCache.lcache[id]
       
# utilities to pack and unpack L0 conditions into Condition property...
# given the 'Conditions' property of an L0DUCOnfig tool instance, 
# return a pythonized table of settings
# i.e. the inverse of 'genItems'
def _parseL0settings( settings ) :
    d = {}
    for s in  [ _parseL0setting(i) for i in settings ] :
        d[s['name']] = s
    return d
def _parseL0setting( setting ) :
    import re
    p = re.compile('([^ ]+) *= *\[(.+)\]')
    val = {}
    for s in setting :
        m = p.match(s)
        key = m.group(1) 
        value = m.group(2)
        # adapt to ideosyncracies
        if key == 'rate=' : key = 'rate'
        if key == 'conditions' :
            value = '[%s]'%value # put back the []
            # make it a list of conditions which are ANDed
            if '&&' in value : value = value.split('&&')
            else             : value = [ value ]
            #  and get rid of the [] again...
            value =  [ re.sub('\[(.*)\]',r'\1',i.strip()) for i in value ]
        val.update( { key : value } )
    return val

def dumpL0( id, cas = defaultCas ) :
    tree  =  execInSandbox( _getConfigTree, id, cas )
    l0s   = [ i for i in tree if i.leaf and i.leaf.type == 'L0DUConfigProvider' ]
    for i in l0s : 
        from pprint import pprint
        print '%s TCK = %s %s' % ( 20*'*',i.leaf.props['TCK'],20 *'*' )
        print '%s Channels %s' % ( 20*'*',20 *'*' )
        pprint( _parseL0settings( eval(i.leaf.props['Channels']) ) )
        print '%s Conditions %s' % ( 19*'*',19 *'*' )
        pprint( _parseL0settings( eval(i.leaf.props['Conditions']) ) )
        print 100*'*'


def getL0Prescales( id, cas  = defaultCas ) :
    tree  =  execInSandbox( _getConfigTree, id, cas )
    l0s   = [ i for i in tree if i.leaf and i.leaf.type == 'L0DUConfigProvider' ]
    ret = dict()
    for i in l0s : 
        l0tck = i.leaf.props['TCK' ] 
        ret[ l0tck ]  = dict()
        settings = _parseL0settings( eval(i.leaf.props['Channels']) )
        for chan in settings.itervalues() :
            ret[ l0tck ][ chan['name'] ] = chan['rate']
    return ret



def _createTCKEntries(d, cas ) :
    ## first pick up all the L0 configurations during the Configurable step
#    l0tcks = {}
#    for tck,id in d.iteritems() :
#        l0tck = tck & 0xffff
#        if l0tck : l0tcks['0x%04X'%l0tck] = None
    ## find the L0 configurations if needed...   
#    if l0tcks : importOptions('$L0TCK/L0DUConfig.opts')
#    from Configurables import L0DUMultiConfigProvider,L0DUConfigProvider
#    for l0tck in l0tcks.keys() :
#        if l0tck not in L0DUMultiConfigProvider('L0DUConfig').registerTCK :
#             raise KeyError('requested L0 TCK %s is not known'%l0tck) 
#        configProvider = L0DUConfigProvider('ToolSvc.L0DUConfig.TCK_%s'%l0tck)
#        l0tcks[l0tck] = configProvider.getValuedProperties()

    pc = PropertyConfigSvc( prefetchConfig = [ _digest(id,cas).str() for (tck,id) in d.iteritems() ],
                            ConfigAccessSvc = cas.getFullName() )
    appMgr = _appMgr()
    appMgr.createSvc(cas.getFullName())
    appMgr.createSvc(pc.getFullName())
    cas = appMgr.service(cas.getFullName(),'IConfigAccessSvc')
    pc = appMgr.service(pc.getFullName(),'IPropertyConfigSvc')
    for tck,id in d.iteritems() :
        id  = _digest(id,cas)
        tck = _tck(tck)
#        # check whether L0 part of the TCK is specified
#        l0tck = tck & 0xffff
#        if l0tck : 
#            l0tck = '0x%04X'%l0tck
#            for i in pc.collectLeafRefs( id ) :
#                propConfig = pc.resolvePropertyConfig( i )
#                #  check for either a MultiConfigProvider with the right setup,
#                #  or for a template with the right TCK in it...
#                if propConfig.name() == 'ToolSvc.L0DUConfig' : 
#                    cfg = PropCfg(propConfig)
#                    if cfg.type not in [ 'L0DUMultiConfigProvider', 'L0DUConfigProvider' ] :
#                        raise KeyError("not a valid L0DU config provider: %s" % cfg.type )
#                    if cfg.type == 'L0DUMultiConfigProvider' and l0tck not in cfg.props['registerTCK'] :
#                        raise KeyError('requested L0TCK %s not known by L0DUMultiConfigProvider in config %s; known L0TCKs: %s' % ( l0tck, id, cfg.props['registerTCK'] ))
#                    elif cfg.type == 'L0DUConfigProvider' and l0tck != cfg.props['TCK'] :
#                        raise KeyError('requested L0TCK %s not known by L0DUConfigProvider in config %s; known L0TCK: %s' % ( l0tck, id, cfg.props['TCK'] ))
        print 'creating mapping TCK: 0x%08x -> ID: %s' % (tck,id)
        ref = cas.readConfigTreeNode( id )
        alias = TCK( ref.get(), tck )
        cas.writeConfigTreeNodeAlias(alias)


def _getConfigurations( cas = defaultCas ) :
    name = cas.getFullName()
    appMgr = _appMgr()
    appMgr.createSvc(name)
    s = appMgr.service(name,'IConfigAccessSvc')
    info = dict()
    for i in s.configTreeNodeAliases( alias( 'TOPLEVEL/') ) :
        x = Configuration( i,s )
        info[ i.alias().str() ] = x
    for i in s.configTreeNodeAliases( alias( 'TCK/'  ) ) :
        tck =  _tck(i.alias().str().split('/')[-1])
        id  =  i.ref().str()
        for k in info.values() :
            if k.info['id'] == id : k.info['TCK'].append(tck)
    for i in s.configTreeNodeAliases( alias( 'TAG/'  ) ) :
        tag = i.alias().str().split('/')[1:] 
        id  = i.ref().str()
        for k in info.values() : 
            if k.info['id'] == id : k.update( { 'TAG' : tag } ) 
    print info
    return info

def _getConfigTree( configID , cas = defaultCas ) :
    id = _digest(tck2id(configID,cas),cas)
    pc = PropertyConfigSvc( prefetchConfig = [ id.str() ],
                            ConfigAccessSvc = cas.getFullName() )
    name = pc.getFullName()
    appMgr = _appMgr()
    appMgr.createSvc(name)
    # init TreeNodeCache singleton
    _TreeNodeCache( svc = appMgr.service(name,'IPropertyConfigSvc') )
    return Tree(id)

# TODO: caching should be done seperately for each cas instance... 
def xget( ids , cas = ConfigAccessSvc() ) : 
    if 'forest' not in dir(xget) : xget.forest = dict() 
    fetch = [ id for id in ids if id not in xget.forest.keys() ] 
    if fetch : 
	xget.forest.update( execInSandbox( _xget, fetch, cas ) ) 
    forest = dict() 
    for id in ids : forest[id] = xget.forest[id] 
    return forest 

def _xget( configIDs , cas = defaultCas ) :
    ids = [ _digest(i,cas) for i in configIDs ]
    pc = PropertyConfigSvc( prefetchConfig = [ id.str() for id in ids ],
                            ConfigAccessSvc = cas.getFullName() )
    appMgr = _appMgr()
    appMgr.createSvc(pc.getFullName())
    svc = appMgr.service(pc.getFullName(),'IPropertyConfigSvc')
    table = dict()
    for id in ids :
        table[id.str()] = dict()
        for i in svc.collectLeafRefs( id ) :
            propConfig = svc.resolvePropertyConfig( i )
            if propConfig.name() in table[id.str()].keys() : raise KeyError("Already in list for %s: '%s'"%(id.str(),propConfig.name()))
            table[id.str()][propConfig.name()] = PropCfg( propConfig )
    return table 



def _copyTree(svc,nodeRef,prefix) :
    node = svc.readConfigTreeNode(nodeRef)
    leafRef = node.leaf()
    if (leafRef.valid()) : 
        leaf = svc.readPropertyConfig(leafRef)
        print prefix + leaf.name()
        newRef = svc.writePropertyConfig(leaf.get())
        # TODO: check validity...
    for i in node.nodes() : _copyTree(svc,i,prefix+'   ')
    svc.writeConfigTreeNode(node.get())

def _copy( source , target ) :
    csvc = ConfigStackAccessSvc( ConfigAccessSvcs = [ target.getFullName(), source.getFullName() ], OutputLevel=DEBUG )
    appMgr = _appMgr()
    appMgr.createSvc(csvc.getFullName())
    s = appMgr.service(csvc.getFullName(),'IConfigAccessSvc')
    for label in [ 'TOPLEVEL/','TCK/','ALIAS/' ] :
        for i in s.configTreeNodeAliases( alias(label) ) :
            print '\n\n copying tree ' + str(i.alias()) + '\n\n'
            _copyTree(s,i.ref(),' ')
            print '\n\n writing alias ' + str(i.alias()) + '\n\n'
            s.writeConfigTreeNodeAlias(i)
    print 'done copying...'


def _lookupProperty(table,algname,property) :
    if algname not in table : raise KeyError("no algorithm named %s in specified config"%algname )
    properties = table[algname].properties()
    if property not in properties: raise KeyError("could not locate property %s for algorithm %s in specified config"%(property,algname) )
    return properties[property]

def _getProperty(id,algname,property, cas ) :
    id = tck2id(id,cas)
#    tables = execInSandbox( _xget, [ id ], cas )
    tables = xget( [ id ], cas )
    return _lookupProperty(tables[id],algname,property)


### and now define the routines visible from the outside world...

class Configuration :
    " A class representing a configuration entry "
    def __init__(self,alias,svc) :
        self.info = { 'id' : alias.ref().str() , 'TCK' : [], 'label' : '<NONE>' }
        self.info.update( zip(['release','hlttype'],alias.alias().str().split('/')[1:3]))
        x = svc.readConfigTreeNode( alias.ref() )
        label =  x.get().label();
        self.info.update( { 'label' : label } )
    def __getitem__(self,label) : 
        return self.info[label]
    def update(self,d) : 
        self.info.update( d )
    def printSimple(self,prefix='      ') : 
        if not self.info['TCK'] :
            print prefix + '%10s : %s : %s'%('<NONE>',self.info['id'],self.info['label'])
        else :
            for tck in self.info['TCK'] :
                if type(tck) == int : tck = '0x%08x' % tck
                print prefix + '%10s : %s : %s'%(tck,self.info['id'],self.info['label'])
    def PVSS(self) :
        for tck in self.info['TCK'] :
            if type(tck) == str and not tck.startswith('0x') : tck = int(tck)
            if type(tck) != str : tck = '0x%08x' % tck
            return  '%20s : %10s : %s : %s\n'%(self.info['hlttype'],tck,self.info['id'],self.info['label'])

class PropCfg :
    " A class representing a PropertyConfig "
    def __init__(self, x) :
        self.name = x.name()
        self.type = x.type()
        self.kind = x.kind()
        self.fullyQualifiedName = x.fullyQualifiedName()
        self.digest = x.digest().str()
        self.props = dict()
        for i in x.properties() : self.props.update( { i.first: i.second } )
    def properties(self) : return self.props
    def fqn(self) : return  self.fullyQualifiedName + ' ('+self.kind+')'
    def fmt(self) :
        return [  "'"+k+"':"+v+'\n'  for k,v in self.props.iteritems() ]

class Tree(object):
    def __init__(self, id, parent = None):
        cache = _TreeNodeCache()
        self.parent = parent
        self.depth = self.parent.depth+1  if self.parent else 0
        node = cache.node(id)
        self.digest = node.digest().str()
        self.label = node.label()
        leaf = cache.leaf( node.leaf() ) 
        self.leaf = PropCfg( leaf ) if leaf else None
        self.nodes = [ Tree(i,self) for i in node.nodes() ]
    def prnt(self):
        s = ' --> ' + str(self.leaf.digest) if self.leaf else ''
        indent = self.depth*'   '
        print indent + str(self.digest) + (30-len(indent))*' ' +  s
        for i in self.nodes: i.prnt()
    # define in order traversal
    def __iter__(self):
        return self._inorder()
    def _inorder(self) :
        yield self
        for i in self.nodes: 
           for x in i._inorder() : yield x
 

def diff( lhs, rhs , cas = defaultCas ) :
    (lhs,rhs) = (tck2id(lhs,cas),tck2id(rhs,cas))
#    table = execInSandbox( _xget, [ lhs, rhs ] , cas ) 
    table = xget( [ lhs, rhs ] , cas ) 
    setl = set( table[lhs].keys() )
    setr = set( table[rhs].keys() )
    onlyInLhs = setl - setr
    if len(onlyInLhs)>0 : 
        print 'only in ' + lhs + ': '
        for i in onlyInLhs : print '   ' + i
    onlyInRhs = setr - setl
    if len(onlyInRhs)>0 : 
        print 'only in ' + rhs + ': ' 
        for i in onlyInRhs : print '   ' + i
    for i in setl & setr :
        (l,r) = ( table[lhs][i], table[rhs][i] )
        if l.digest != r.digest : 
            from difflib import unified_diff
            print ''.join( unified_diff(l.fmt(), r.fmt(), 
                                        l.fqn(), r.fqn(),
                                        lhs, rhs, n=0) )


def createTCKEntries(d, cas = defaultCas ) :
    return execInSandbox( _createTCKEntries, d, cas )

def copy( source = defaultCas , target = ConfigDBAccessSvc(ReadOnly=False) ) :
    return execInSandbox( _copy, source, target )

def listComponents( id, cas = defaultCas ) :
    tree = getConfigTree( id, cas )
    for i in tree : 
        if i.leaf : 
          s =  i.depth*3*' ' + i.leaf.name
          print s + (80-len(s))*' ' + str(i.leaf.digest)

def getAlgorithms( id, cas = defaultCas ) :
    tree = getConfigTree( id, cas )
    x = ''
    for i in tree :
       if i.leaf and i.leaf.kind =='IAlgorithm':
          s =  i.depth*3*' ' + i.leaf.name
          x = x + s + (80-len(s))*' ' + str(i.leaf.digest) + '\n'
    return x

def listAlgorithms( id, cas = defaultCas ) :
    print getAlgorithms(id,cas)

def getStreams( id, cas = defaultCas ) : 
    tree =  getConfigTree( id, cas )
    x = ''
    stream_prefix = 'StrippingSequenceStream'
    for i in tree :
       if hasattr(i,'leaf') and i.leaf : 
          if i.leaf.type == 'GaudiSequencer' : 
             if i.leaf.name.startswith(stream_prefix) : 
                stream_name = i.leaf.name[len(stream_prefix):]
                x = x + '   ' + stream_name + '\n'
    return x

def listStreams( id, cas = defaultCas ) :
    print getStreams(id,cas)

def getLines( id, stream = None, cas = defaultCas) :
    tree = getConfigTree( id, cas )
    x = ''
    stream_prefix = 'StrippingSequenceStream'
    stream_lines = []
    stream_name = ''
    for i in tree :
       if hasattr(i,'leaf') and i.leaf : 
          if i.leaf.type == 'GaudiSequencer' : 
             if i.leaf.name.startswith(stream_prefix) : 
                stream_name = i.leaf.name[len(stream_prefix):]
                if not stream or stream_name == stream : 
                   x = x + 'Stream ' + stream_name + '\n'
                   stream_lines = []
          if ( not stream or stream == stream_name ) and i.leaf.type == 'StrippingAlg' :
             if not i.leaf.name.startswith('StrippingStream'+ stream_name) and i.leaf.name not in stream_lines : 
                s = i.leaf.name
                x = x + '   ' + s + (80-len(s))*' ' + '\n'
                stream_lines.append(i.leaf.name)
    return x

def listLines( id, stream = None, cas = defaultCas ) :
    print getLines(id, stream, cas)

def getLineProperties( id, linename='', cas = defaultCas ) : 
    tree =  getConfigTree( id, cas )
    matchleaf = None
    for i in tree :
       if i.leaf and i.leaf.type == 'StrippingAlg' :
          if i.leaf.name == linename : 
              matchleaf = i.leaf
    if matchleaf : 
       prescaler  = matchleaf.properties()['Prescale']
       postscaler = matchleaf.properties()['Postscale']
       filterseq  = matchleaf.properties()['Filter1']
       outputloc  = matchleaf.properties()['OutputLocation']
       hltfilt    = matchleaf.properties()['HLT']
       l0dufilt   = matchleaf.properties()['L0DU']
       odinfilt   = matchleaf.properties()['ODIN']

       prescale = None
       postscale = None
       filtermembers = None

       for i in tree : 
          if i.leaf and i.leaf.fullyQualifiedName == prescaler : 
             prescale = i.leaf.properties()['AcceptFraction']
       for i in tree : 
          if i.leaf and i.leaf.fullyQualifiedName == postscaler : 
             postscale = i.leaf.properties()['AcceptFraction']
       for i in tree : 
          if i.leaf and i.leaf.fullyQualifiedName == filterseq : 
             filtermembers = i.leaf.properties()['Members']

       x  = "   Properties for line " + linename + "\n"
       x += "      Prescale       : " + prescale + "\n"
       if odinfilt and odinfilt != '' : 
          x += "      ODIN           : " + odinfilt + "\n"
       if l0dufilt and l0dufilt != '' : 
          x += "      L0DU           : " + l0dufilt + "\n"
       if hltfilt and hltfilt != '' : 
          x += "      HLT            : " + hltfilt + "\n"
       x += "      Filter members : " + filtermembers + "\n"
       x += "      Postscale      : " + postscale + "\n"
       x += "      Output location: '" + outputloc + "'\n"

       return x

def listLineProperties( id, linename='', cas = defaultCas ) : 
    print getLineProperties(id, linename, cas)

def getProperties( id, algname='',property='',cas = defaultCas ) :
    retlist=[]
    tree = getConfigTree ( id, cas )
    import re
    if algname :
        reqNode = re.compile(algname)
        matchNode = lambda x : reqNode.match(x.leaf.name)
    else :
        matchNode = None
    if property :
        reqProp = re.compile(property)
        matchProp = lambda x : reqProp.match(x)
    else :
        matchProp = None
    for i in tree :
       if not i.leaf or (matchNode and not matchNode(i)) : continue
       identLine =  i.leaf.fullyQualifiedName
       for (k,v) in i.leaf.properties().iteritems() :
          if matchProp and not matchProp(k) : continue
          retlist.append((identLine, v))
    return retlist

def listProperties( id, algname='',property='',cas = defaultCas ) :
    tree = getConfigTree( id, cas ) 
    import re
    if algname : 
        reqNode = re.compile(algname)
        matchNode = lambda x : reqNode.match(x.leaf.name)
    else :
        matchNode = None
    if property :
        reqProp = re.compile(property)
        matchProp = lambda x : reqProp.match(x)
    else :
        matchProp = None
    for i in tree :
       if not i.leaf or (matchNode and not matchNode(i)) : continue
       first = True
       for (k,v) in i.leaf.properties().iteritems() :
          if matchProp and not matchProp(k) : continue
          if first : 
            print '\n   Requested Properties for ' + i.leaf.fullyQualifiedName
            first = False
          print "      '" + k + "\':" + v

def orphanScan( cas = defaultCas ) :
    return execInSandbox(_orphanScan, cas)

def getConfigurations( cas = defaultCas ) :
    return execInSandbox( _getConfigurations, cas )

def getTCKInfo(x) :
    for (i,j) in getConfigurations().iteritems() :
        if x in j['TCK'] : return (j['hlttype'],j['release'])
    return None

def getReleases( cas = defaultCas ) :
    return set( [ i['release']  for i in getConfigurations(cas).itervalues()  ] )

def getHltTypes( release, cas = defaultCas ) :
    info = execInSandbox( _getConfigurations, cas )
    return set( [ i['hlttype']  for i in info.itervalues() if i['release']==release ] )

def getTCKs( release = None, hlttype = None, cas = defaultCas ) :
    info = execInSandbox( _getConfigurations, cas )
    pred = lambda x : x['TCK'] and ( not release or x['release'] == release ) and ( not hlttype or x['hlttype'] == hlttype )
    result = []
    for i in [ x for x in info.itervalues() if pred(x) ]:
            for tck in i['TCK'] :
                result.append( ('0x%08x'%tck,i['label'])  )
    return result

def getTCKList( cas = defaultCas ) :
    info = execInSandbox( _getConfigurations, cas )
    result = []
    for i in info.itervalues() :
            for tck in i['TCK'] : result.append( '0x%08x'%tck  )
    return result

def getRoutingBits( id , cas = defaultCas ) :
    # should be a map... so we try to 'eval' it
    for p in ['RoutingBits','routingBitDefinitions'] :
        try :
            return eval(_getProperty(id,'HltRoutingBitsWriter',p,cas))
        except KeyError : 
            continue
    return None

## TODO:  is a string the best thing to return???
def getAlgorithms( id, cas = defaultCas ) :
    tree =  execInSandbox( _getConfigTree, id, cas )
    tempstr = ''
    for i in tree :
       if i.leaf and i.leaf.kind =='IAlgorithm':
          s =  i.depth*3*' ' + i.leaf.name
          tempstr = tempstr + s + (80-len(s))*' ' + str(i.leaf.digest) + '\n'
    return tempstr


def dump( id, properties = None,  lines = None, cas = defaultCas ) :
    if not properties : 
        properties = [ 'RoutingBits', 'AcceptFraction', 'FilterDescriptor'
                     , 'Code', 'InputLocations'
                     , 'DaughtersCuts', 'CombinationCut', 'MotherCut', 'DecayDescriptor'
                     , 'OutputSelection' ]
    tree =  execInSandbox( _getConfigTree, id, cas )
    def len1(line):
        _i = line.rfind('\n')
        return len(line) if _i<0 else len(line)-(_i+1)

    import re
    show = not lines
    for i in tree :
       if not i.leaf or i.leaf.kind != 'IAlgorithm' : continue
       if lines and i.leaf.type in [ 'Hlt::Line', 'HltLine' ] :
           show =  re.match(lines,i.leaf.name) 
       if not show : continue
       _tab = 50
       line =  i.depth * '   ' + i.leaf.name
       if len1(line)>( _tab-1): line +=  '\n'+ _tab*' '
       else :                   line +=  (_tab-len1(line))*' '
       line +=  '%-25.25s'%i.leaf.type
       for k,v in [ (k,v) for k,v in i.leaf.props.iteritems() if k in properties and v ]:
           if _tab+25 < len1(line) : line+= '\n'+(_tab+25)*' '
           line += '%-15s : %s' % ( k, v)
       print line

def getConfigTree(id, cas = defaultCas ):
#    return execInSandbox( _getConfigTree, id, cas )
    if 'forest' not in dir(getConfigTree) : getConfigTree.forest = dict() 
    if id not in getConfigTree.forest : 
	getConfigTree.forest[id] = execInSandbox( _getConfigTree, id, cas ) 
    return getConfigTree.forest[id] 

def getHlt1Lines( id , cas = defaultCas ) :
    # should be a list... so we try to 'eval' it
    return eval(_getProperty(id,'Hlt1','Members',cas))

def getHlt2Lines( id , cas = defaultCas ) :
    # should be a list... so we try to 'eval' it
    return eval(_getProperty(id,'Hlt2','Members',cas))

def getHlt1Decisions( id , cas = defaultCas ) :
    id = tck2id(id,cas)
    table = execInSandbox( _xget, [ id ], cas )[id]
    lines = eval(_lookupProperty(table,'Hlt1','Members'))
    return [ _lookupProperty(table,i.split('/')[-1],'DecisionName') for i in lines ]


def printConfigurations( info ) :
    for i in info.itervalues() : print i.info
    for release in sorted(set( [ i['release'] for i in info.itervalues()  ] )) : 
        print release
        confInRelease = [ i for i in info.itervalues() if i['release']==release ]
        for hlttype in sorted(set( [ i['hlttype'] for i in confInRelease ] ) ) :
            print '    ' + hlttype
            [ i.printSimple('      ') for i in confInRelease if i['hlttype']==hlttype ] 

def dumpForPVSS( info, root ) :
    if not os.access(root,os.F_OK) : os.makedirs(root)    
    for release in sorted(set( [ i['release'] for i in info.itervalues()  ] ) ) : 
        f=open( root + '/' + release,  'w')
        [ f.write( i.PVSS() ) for i in info.itervalues() if i['release']==release ]
        f.close()

def printReleases( rel ) : pprint(rel)

def printHltTypes( rt ) : pprint(rt)

def printTCKs( tcks ) : pprint(tcks)

def listConfigurations( cas = defaultCas ) :
    return printConfigurations( getConfigurations(cas) )

def listReleases( cas = defaultCas ) :
    return printReleases( getReleases() ) 

def listTCKs( release, hlttype, cas = defaultCas ) :
    return printTCKs( getTCKs(release,hlttype) ) 


######  do the actual work...

if __name__ == '__main__' :
    listConfigurations()
