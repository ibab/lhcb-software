import GaudiPython
from Gaudi.Configuration import*
from Configurables import ConfigStackAccessSvc, ConfigDBAccessSvc, ConfigTarFileAccessSvc, ConfigFileAccessSvc, ConfigTreeEditor, PropertyConfigSvc

# pick the default config access svc
from Configurables import ConfigTarFileAccessSvc as ConfigAccessSvc

from TCKUtils.Sandbox import execInSandbox
from pprint import pprint

### add some decoration...
MD5 = GaudiPython.gbl.Gaudi.Math.MD5
MD5.__str__ = MD5.str
GaudiPython.gbl.ConfigTreeNodeAlias.alias_type.__str__ = GaudiPython.gbl.ConfigTreeNodeAlias.alias_type.str
digest = MD5.createFromStringRep
alias = GaudiPython.gbl.ConfigTreeNodeAlias.alias_type
topLevelAlias = GaudiPython.gbl.ConfigTreeNodeAlias.createTopLevel
TCK = GaudiPython.gbl.ConfigTreeNodeAlias.createTCK
vector_string = GaudiPython.gbl.std.vector('std::string')

def _appMgr() :
    ApplicationMgr().AppName = ""
    ApplicationMgr().OutputLevel = ERROR
    appMgr = GaudiPython.AppMgr()
    appMgr.initialize()
    return appMgr

def _tck(x) :
    if type(x) == str and x[0:2].upper() == '0X' :  return int(x,16)
    return int(x)

def _orphanScan(cas = ConfigAccessSvc() ) :
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
        tree =  getConfigTree( id, cas )
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


def _digest(x) :
    if type(x) == str : x = digest( x )
    return x

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

def dumpL0( id, cas  = ConfigAccessSvc() ) :
    tree  =  getConfigTree( id, cas )
    l0s   = [ i for i in tree if i.leaf and i.leaf.type == 'L0DUConfigProvider' ]
    for i in l0s : 
        from pprint import pprint
        print '%s TCK = %s %s' % ( 20*'*',i.leaf.props['TCK'],20 *'*' )
        print '%s Channels %s' % ( 20*'*',20 *'*' )
        pprint( _parseL0settings( eval(i.leaf.props['Channels']) ) )
        print '%s Conditions %s' % ( 19*'*',19 *'*' )
        pprint( _parseL0settings( eval(i.leaf.props['Conditions']) ) )
        print 100*'*'


def getL0Prescales( id, cas  = ConfigAccessSvc() ) :
    tree  =  getConfigTree( id, cas )
    l0s   = [ i for i in tree if i.leaf and i.leaf.type == 'L0DUConfigProvider' ]
    ret = dict()
    for i in l0s : 
        l0tck = i.leaf.props['TCK' ] 
        ret[ l0tck ]  = dict()
        settings = _parseL0settings( eval(i.leaf.props['Channels']) )
        for chan in settings.itervalues() :
            ret[ l0tck ][ chan['name'] ] = chan['rate']
    return ret


def _updateL0TCK( id, l0tck, label, cas, extra = None ) :
    if not label : 
        print 'please provide a reasonable label for the new configuration'
        return None
    l0tck = '0x%04X'%_tck(l0tck)
    importOptions('$L0TCK/L0DUConfig.opts')
    from Configurables import L0DUMultiConfigProvider,L0DUConfigProvider
    if l0tck not in L0DUMultiConfigProvider('L0DUConfig').registerTCK :
             raise KeyError('requested L0 TCK %s is not known to TCK/L0TCK'%l0tck) 
    configProvider = L0DUConfigProvider('ToolSvc.L0DUConfig.TCK_%s'%l0tck)
    l0config = configProvider.getValuedProperties()
    l0config['TCK'] = l0tck

    pcs = PropertyConfigSvc( ConfigAccessSvc = cas.getFullName() )
    cte = ConfigTreeEditor( PropertyConfigSvc = pcs.getFullName(), 
                            ConfigAccessSvc =  cas.getFullName())

    appMgr = _appMgr()
    appMgr.createSvc(cas.getFullName())
    appMgr.createSvc(pcs.getFullName())
    cas = appMgr.service(cas.getFullName(),'IConfigAccessSvc')
    pcs = appMgr.service(pcs.getFullName(),'IPropertyConfigSvc')
    svc = AccessSvcSingleton(pcs = pcs, cas=cas )
    cte = appMgr.toolsvc().create(cte.getFullName(),interface='IConfigTreeEditor')
    id  = _digest(id)
    a = [ i.alias().str() for  i in cas.configTreeNodeAliases( alias('TOPLEVEL/') ) if i.ref() == id ]
    if len(a) != 1 : 
        print 'something went wrong: no unique toplevel match for ' + str(id)
        return
    (release,hlttype) = a[0].split('/',3)[1:3]
    mods = vector_string()
    # check L0 config in source config
    for cfg in svc.collectLeafRefs( id ) :
        #  check for either a MultiConfigProvider with the right setup,
        #  or for a template with the right TCK in it...
        if cfg.name == 'ToolSvc.L0DUConfig' : 
            if cfg.type != 'L0DUConfigProvider':
                raise KeyError("Can only update configuration which use L0DUConfigProvider, not  %s" % cfg.type )
            #  check that all specified properties exist in cfg
            for (k,v) in l0config.iteritems() :
                if k not in cfg.props : raise KeyError('Specified property %s not in store'%k)
                mods.push_back('ToolSvc.L0DUConfig.%s:%s' % (k,v) )
    if extra :
        for algname,props in extra.iteritems() :
            for k,v in props.iteritems() : 
                mods.push_back( '%s.%s:%s' %  (algname, k, v ) )
    print 'updates: %s ' % mods
    newId = cte.updateAndWrite(id,mods,label)
    noderef = cas.readConfigTreeNode( newId )
    if not noderef : print 'oops, could not find node for %s ' % newId
    top = topLevelAlias( release, hlttype, noderef.get() )
    cas.writeConfigTreeNodeAlias(top)
    print 'wrote ' + str(top.alias()) 
    return str(newId)


def _createTCKEntries(d, cas ) :
    ## first pick up all the L0 configurations during the Configurable step
    l0tcks = {}
    for tck,id in d.iteritems() :
        l0tck = tck & 0xffff
        if l0tck : l0tcks['0x%04X'%l0tck] = None
    ## find the L0 configurations if needed...   
    if l0tcks : importOptions('$L0TCK/L0DUConfig.opts')
    from Configurables import L0DUMultiConfigProvider,L0DUConfigProvider
    for l0tck in l0tcks.keys() :
        if l0tck not in L0DUMultiConfigProvider('L0DUConfig').registerTCK :
             raise KeyError('requested L0 TCK %s is not known'%l0tck) 
        configProvider = L0DUConfigProvider('ToolSvc.L0DUConfig.TCK_%s'%l0tck)
        l0tcks[l0tck] = configProvider.getValuedProperties()
    pcs = PropertyConfigSvc( ConfigAccessSvc = cas.getFullName() )
    appMgr = _appMgr()
    appMgr.createSvc(cas.getFullName())
    appMgr.createSvc(pcs.getFullName())
    cas = appMgr.service(cas.getFullName(),'IConfigAccessSvc') 
    svc = AccessSvcSingleton( pcs = appMgr.service(pcs.getFullName(),'IPropertyConfigSvc')
                            , cas = cas )
    for tck,id in d.iteritems() :
        id  = _digest(id)
        tck = _tck(tck)
        # check whether L0 part of the TCK is specified
        l0tck = tck & 0xffff
        if l0tck : 
            l0tck = '0x%04X'%l0tck
            for cfg in svc.collectLeafRefs( id ) : 
                #  check for either a MultiConfigProvider with the right setup,
                #  or for a template with the right TCK in it...
                if cfg.name == 'ToolSvc.L0DUConfig' : 
                    if cfg.type not in [ 'L0DUMultiConfigProvider', 'L0DUConfigProvider' ] :
                        raise KeyError("not a valid L0DU config provider: %s" % cfg.type )
                    if cfg.type == 'L0DUMultiConfigProvider' and l0tck not in cfg.props['registerTCK'] :
                        raise KeyError('requested L0TCK %s not known by L0DUMultiConfigProvider in config %s; known L0TCKs: %s' % ( l0tck, id, cfg.props['registerTCK'] ))
                    elif cfg.type == 'L0DUConfigProvider' and l0tck != cfg.props['TCK'] :
                        raise KeyError('requested L0TCK %s not known by L0DUConfigProvider in config %s; known L0TCK: %s' % ( l0tck, id, cfg.props['TCK'] ))
        print 'creating mapping TCK: 0x%08x -> ID: %s' % (tck,id)
        ref = cas.readConfigTreeNode( id )
        alias = TCK( ref.get(), tck )
        svc.writeConfigTreeNodeAlias(alias)


def _getConfigurations( cas = ConfigAccessSvc() ) :
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
    return info

class AccessSvcSingleton(object) :
   _pcs = None
   _cas = None
   def __init__(self,pcs=None,cas=None) :
        if cas : AccessSvcSingleton._cas = cas
        if pcs : AccessSvcSingleton._pcs = pcs
   def resolveTCK(self,tck) :
        tck = _tck(tck)
        for i in AccessSvcSingleton._cas.configTreeNodeAliases( alias( 'TCK/'  ) ) :
            if _tck(i.alias().str().split('/')[-1]) == _tck(tck) : return digest(i.ref().str())
        return None
   def _2id(self,id) :
        if type(id) is int : id = '0x%08x' % id 
        if type(id) is str and len(id)==32 : id = _digest(id)
        #  if we're not a valid id at this point, maybe we're a TCK... 
        if type(id) is not MD5 : id = self.resolveTCK(id)
        return id
   def resolveConfigTreeNode(self,id) :   
        if type(id) is not MD5 :
            id = self._2id(id)
        return AccessSvcSingleton._pcs.resolveConfigTreeNode(id) if (id and id.valid()) else None
   def resolvePropertyConfig(self,id) :
        return AccessSvcSingleton._pcs.resolvePropertyConfig(id) if (id and id.valid()) else None
   def collectLeafRefs(self,id) :
        if type(id) is not MD5 :
            id = self._2id(id)
        for ids in  AccessSvcSingleton._pcs.collectLeafRefs( id )  :
            yield PropCfg(ids)
   def resolveConfigTreeNodeAliases(self, a ) :
        if type(a) is not type(alias) : a = alias(a)
        return AccessSvcSingleton._pcs.configTreeNodeAliases( a )
   def writeConfigTreeNodeAlias(self,alias) :
        return AccessSvcSingleton._cas.writeConfigTreeNodeAlias(alias)

def _getConfigTree( id , cas = ConfigAccessSvc() ) :
    pcs = PropertyConfigSvc( ConfigAccessSvc = cas.getFullName() )
    appMgr = _appMgr()
    appMgr.createSvc(cas.getFullName())
    appMgr.createSvc(pcs.getFullName())
    AccessSvcSingleton( pcs = appMgr.service(pcs.getFullName(),'IPropertyConfigSvc')
                      , cas = appMgr.service(cas.getFullName(),'IConfigAccessSvc') )
    return Tree(id)

def _xget( ids , cas = ConfigAccessSvc() ) :
    pcs = PropertyConfigSvc( ConfigAccessSvc = cas.getFullName() )
    appMgr = _appMgr()
    appMgr.createSvc(cas.getFullName())
    appMgr.createSvc(pcs.getFullName())
    svc = AccessSvcSingleton( pcs = appMgr.service(pcs.getFullName(),'IPropertyConfigSvc')
                            , cas = appMgr.service(cas.getFullName(),'IConfigAccessSvc') )
    table = dict()
    for id in ids :
        table[id] = dict( [ ( cfg.name, cfg ) for cfg in svc.collectLeafRefs(id) ] )
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
    tables = execInSandbox( _xget, [ id ], cas )
    return _lookupProperty(tables[id],algname,property)

def _updateProperties(id, updates, label, cas  ) :
    if not label : 
        print 'please provide a reasonable label for the new configuration'
        return None
    pc = PropertyConfigSvc( ConfigAccessSvc = cas.getFullName() )
    cte = ConfigTreeEditor( PropertyConfigSvc = pc.getFullName(),
                            ConfigAccessSvc = cas.getFullName() )
    # run program...
    appMgr = _appMgr()
    cteName = cte.name().split('.')[-1]
    ed = appMgr.toolsvc().create(cteName,interface='IConfigTreeEditor')
    if not ed : raise RuntimeWarning(' could not get tool ' + cteName )
    cf = appMgr.service(cas.getFullName(),'IConfigAccessSvc')
    if not cf : raise RuntimeWarning(' could not get service ' + cas.name() )
    if type(id) == str: id = _digest( id )
    if not id.valid() : raise RuntimeWarning('not a valid id : %s' % id )
    a = [ i.alias().str() for  i in cf.configTreeNodeAliases( alias('TOPLEVEL/') ) if i.ref() == id ]
    if len(a) != 1 : 
        print 'something went wrong: no unique toplevel match for ' + str(id)
        return
    (release,hlttype) = a[0].split('/',3)[1:3]
    mods = vector_string()
    for algname,props in updates.iteritems() :
        for k,v in props.iteritems() : 
            item = algname + '.' + k + ':' + v
            print 'updating: ' + item
            mods.push_back( item )
    newId = ed.updateAndWrite(id,mods,label)
    noderef = cf.readConfigTreeNode( newId )
    top = topLevelAlias( release, hlttype, noderef.get() )
    cf.writeConfigTreeNodeAlias(top)
    print 'wrote ' + str(top.alias()) 
    return str(newId)


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


class TreeNode(object) :
    " A class representing a ConfigTreeNode "
    #  use flyweight pattern, and use digest to identify objects...
    import weakref
    _pool = weakref.WeakValueDictionary()
    def __new__(cls, id = None) :
        if not id: return object.__new__(cls)
        obj = TreeNode._pool.get( id )
        if not obj :
            obj = AccessSvcSingleton().resolveConfigTreeNode(id)
            TreeNode._pool[id] = obj
        return obj


class PropCfg(object) :
    " A class representing a PropertyConfig "
    #  use flyweight pattern, and use digest to identify objects...
    import weakref
    _pool = weakref.WeakValueDictionary()
    #  TODO: make a singleton svc which we use to resolve IDs if not existent yet...
    def __new__(cls, id = None) :
        if not id : return object.__new__(cls) # added to make it possible to recv in parent process...
        obj = PropCfg._pool.get( id )
        if not obj :
            x = AccessSvcSingleton().resolvePropertyConfig(id)
            obj = object.__new__(cls)
            obj.name = x.name()
            obj.type = x.type()
            obj.kind = x.kind()
            obj.fullyQualifiedName = x.fullyQualifiedName()
            obj.digest = id
            obj.props  = dict( [ (i.first,i.second) for i in x.properties() ] )
            PropCfg._pool[id] = obj
        return obj
    def properties(self) : return self.props
    def fqn(self) : return  self.fullyQualifiedName + ' ('+self.kind+')'
    def fmt(self) :
        return [  "'"+k+"':"+v+'\n'  for k,v in self.props.iteritems() ]

class Tree(object):
    def __init__(self,id,parent=None ) :
        self.parent = parent
        self.depth = self.parent.depth+1  if self.parent else 0
        node = TreeNode(id)
        self.digest = node.digest().str()
        self.label = node.label()
        leaf =  node.leaf() 
        self.leaf = PropCfg( leaf ) if leaf.valid() else None
        self.nodes = [ Tree(id=id,parent=self) for id in node.nodes() ]
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

def diff( lhs, rhs , cas = ConfigAccessSvc() ) :
    table = execInSandbox( _xget, [ lhs, rhs ] , cas ) 
    setl = set( table[lhs].keys() )
    setr = set( table[rhs].keys() )
    onlyInLhs = setl - setr
    if len(onlyInLhs)>0 : 
        print 'only in %s: ' % lhs
        for i in onlyInLhs : print '   ' + i
    onlyInRhs = setr - setl
    if len(onlyInRhs)>0 : 
        print 'only in %s:'  % rhs
        for i in onlyInRhs : print '   ' + i
    for i in setl & setr :
        (l,r) = ( table[lhs][i], table[rhs][i] )
        if l.digest != r.digest : 
            from difflib import unified_diff
            print ''.join( unified_diff(l.fmt(), r.fmt(), 
                                        l.fqn(), r.fqn(),
                                        lhs, rhs, n=0) )

def updateProperties(id,updates,label='', cas = ConfigAccessSvc() ) :
    return execInSandbox( _updateProperties,id,updates,label, cas )
def updateL0TCK(id, l0tck, label='', cas = ConfigAccessSvc(), extra = None ) :
    return execInSandbox( _updateL0TCK, id, l0tck, label, cas = cas, extra = extra)
def createTCKEntries(d, cas = ConfigAccessSvc() ) :
    return execInSandbox( _createTCKEntries, d, cas )
def copy( source = ConfigAccessSvc() , target = ConfigDBAccessSvc(ReadOnly=False) ) :
    return execInSandbox( _copy, source, target )

def listComponents( id, cas = ConfigAccessSvc() ) :
    tree = getConfigTree( id, cas )
    for i in tree : 
        if i.leaf : 
          s =  i.depth*3*' ' + i.leaf.name
          print s + (80-len(s))*' ' + str(i.leaf.digest)
def getAlgorithms( id, cas = ConfigAccessSvc() ) :
    tree =  getConfigTree( id, cas )
    x = ''
    for i in tree :
       if i.leaf and i.leaf.kind =='IAlgorithm':
          s =  i.depth*3*' ' + i.leaf.name
          x = x + s + (80-len(s))*' ' + str(i.leaf.digest) + '\n'
    return x

def listAlgorithms( id, cas = ConfigAccessSvc() ) :
    print getAlgorithms(id,cas)

def getProperties( id, algname='',property='',cas = ConfigAccessSvc() ) :
    retlist= dict()
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
    # generate a unique ID for this traversal, so that we do not
    # repeat leafs...
    import uuid
    sig = uuid.uuid4().hex
    for i in tree :
       if not i.leaf or (matchNode and not matchNode(i)) : continue
       if hasattr(i.leaf,sig) : continue
       setattr(i.leaf,sig,True)
       pdict = dict()
       for (k,v) in i.leaf.properties().iteritems() :
          if matchProp and not matchProp(k) : continue
          pdict[k] = v
       if pdict : retlist[  i.leaf.name ] = pdict
    return retlist

def listProperties( id, algname='',property='',cas = ConfigAccessSvc() ) :
    for (c,d) in getProperties(id,algname,property,cas).iteritems() :
         print '\n   Requested Properties for %s' % c
         for k,v in d.iteritems() :
            print "      '%s':%s" % (k,v) 

def orphanScan( cas = ConfigAccessSvc() ) :
    return execInSandbox(_orphanScan, cas)

def getConfigurations( cas = ConfigAccessSvc() ) :
    return execInSandbox( _getConfigurations, cas )

def getTCKInfo(x) :
    for (i,j) in getConfigurations().iteritems() :
        if x in j['TCK'] : return (j['hlttype'],j['release'])
    return None

def getReleases( cas = ConfigAccessSvc() ) :
    return set( [ i['release']  for i in getConfigurations(cas).itervalues()  ] )
def getHltTypes( release, cas = ConfigAccessSvc() ) :
    info = execInSandbox( _getConfigurations, cas )
    return set( [ i['hlttype']  for i in info.itervalues() if i['release']==release ] )
def getTCKs( release = None, hlttype = None, cas = ConfigAccessSvc() ) :
    info = execInSandbox( _getConfigurations, cas )
    pred = lambda x : x['TCK'] and ( not release or x['release'] == release ) and ( not hlttype or x['hlttype'] == hlttype )
    result = []
    for i in [ x for x in info.itervalues() if pred(x) ]:
            for tck in i['TCK'] :
                result.append( ('0x%08x'%tck,i['label'])  )
    return result
def getTCKList( cas = ConfigAccessSvc() ) :
    info = execInSandbox( _getConfigurations, cas )
    result = []
    for i in info.itervalues() :
            for tck in i['TCK'] : result.append( '0x%08x'%tck  )
    return result
def getRoutingBits( id , cas = ConfigAccessSvc() ) :
    # should be a map... so we try to 'eval' it
    for p in ['RoutingBits','routingBitDefinitions'] :
        try :
            return eval(_getProperty(id,'HltRoutingBitsWriter',p,cas))
        except KeyError : 
            continue
    return None

## TODO:  is a string the best thing to return???
def getAlgorithms( id, cas = ConfigAccessSvc() ) :
    tree =  getConfigTree( id, cas )
    tempstr = ''
    for i in tree :
       if i.leaf and i.leaf.kind =='IAlgorithm':
          s =  i.depth*3*' ' + i.leaf.name
          tempstr = tempstr + s + (80-len(s))*' ' + str(i.leaf.digest) + '\n'
    return tempstr

def dump( id, properties = None,  lines = None, cas = ConfigAccessSvc() ) :
    if not properties : 
        properties = [ 'RoutingBits', 'AcceptFraction', 'FilterDescriptor'
                     , 'Preambulo', 'Code', 'InputLocations','Input','Inputs'
                     , 'DaughtersCuts', 'CombinationCut', 'MotherCut', 'DecayDescriptor'
                     , 'OutputSelection', 'Output' ]
    tree =  getConfigTree( id, cas )
    def len1(line):
        _i = line.rfind('\n')
        return len(line) if _i<0 else len(line)-(_i+1)

    def prettyPrintStreamer(code) :
        code.translate(None,'\n')
        return ('\n' + (_tab+25+18)*' ' + '>> ' ).join( [ i.strip() for i in code.split('>>') ] )

    def prettyPrintDaughtersCuts(code) :
        try :
            cuts = eval(code) # should be a dict
            return "{ "+ ('\n' + (_tab+25+18)*' ' + ', ' ).join(  [ "'%s' : '%s'"%(k,v) for (k,v) in cuts.iteritems() ] ) + '\n'+(_tab+25+18)*' '+"}"
        except : 
            return code

    def prettyPrintList(code) :
        try :
            l = eval(code)
            if len(l)<2 : return code
            return "[ "+('\n' + (_tab+25+18)*' ' + ', ' ).join( l )+'\n'+(_tab+25+18)*' '+']'
        except :
            return code

    trtable = { 'Code' : prettyPrintStreamer
              , 'DaughtersCuts' : prettyPrintDaughtersCuts
              , 'Inputs' : prettyPrintList
              , 'Preambulo' : prettyPrintList
              # , 'FilterDescriptor' : prettyPrintList
              }

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
           if k in trtable.keys() : v = trtable[k](v)
           line += '%-15s : %s' % ( k, v)
       print line

def getConfigTree(id, cas = ConfigAccessSvc()):
    #return  _getConfigTree( id, cas )
    return execInSandbox( _getConfigTree, id, cas )

def getHlt1Lines( id , cas = ConfigAccessSvc() ) :
    # should be a list... so we try to 'eval' it
    return eval(_getProperty(id,'Hlt1','Members',cas))
def getHlt2Lines( id , cas = ConfigAccessSvc() ) :
    # should be a list... so we try to 'eval' it
    return eval(_getProperty(id,'Hlt2','Members',cas))
def getHlt1Decisions( id , cas = ConfigAccessSvc() ) :
    table = execInSandbox( _xget, [ id ], cas )[id]
    lines = eval(_lookupProperty(table,'Hlt1','Members'))
    return [ _lookupProperty(table,i.split('/')[-1],'DecisionName') for i in lines ]

def _sortReleases( release ):
    version = release.split('_')[ 1 ]
    import re
    m = re.compile('v(\d+)r(\d+)(?:p(\d+))?').match( version )
    return [ int( x ) if x else 0 for x in m.groups() ]

def _sortConfigs( config ):
    return config[ 'TCK' ] if config[ 'TCK'] else []

def printConfigurations( info ) :
    for release in sorted(set( [ i['release'] for i in info.itervalues()  ] ), key = _sortReleases ) : 
        print release
        confInRelease = [ i for i in info.itervalues() if i['release']==release ]
        for hlttype in sorted(set( [ i['hlttype'] for i in confInRelease ] ) ) :
            confInHltType = sorted( [ i for i in confInRelease if i['hlttype']==hlttype ], key = _sortConfigs )
            print '    ' + hlttype
            [ i.printSimple('      ') for i in confInHltType ]

def dumpForPVSS( info, root ) :
    if not os.access(root,os.F_OK) : os.makedirs(root)    
    for release in sorted(set( [ i['release'] for i in info.itervalues()  ] ) ) : 
        f=open( root + '/' + release,  'w')
        [ f.write( i.PVSS() ) for i in info.itervalues() if i['release']==release ]
        f.close()

def printReleases( rel ) : pprint(rel)
def printHltTypes( rt ) : pprint(rt)
def printTCKs( tcks ) : pprint(tcks)

def listConfigurations( cas = ConfigAccessSvc() ) :
    return printConfigurations( getConfigurations(cas) )
def listReleases( cas = ConfigAccessSvc() ) :
    return printReleases( getReleases() ) 
def listHltTypes( release, cas = ConfigAccessSvc() ) :
    return printHltTypes( getHltTypes(release) ) 
def listTCKs( release, hlttype, cas = ConfigAccessSvc() ) :
    return printTCKs( getTCKs(release,hlttype) ) 
def listRoutingBits( id, cas = ConfigAccessSvc() ) :
    print getRoutingBits(id,cas)
def listHlt1Lines( id, cas = ConfigAccessSvc() ) :
    print getHlt1Lines(id,cas)
def listHlt2Lines( id, cas = ConfigAccessSvc() ) :
    print getHlt2Lines(id,cas)
def listHlt1Decisions( id, cas = ConfigAccessSvc() ) :
    print getHlt1Decisions(id,cas)


######  do the actual work...

if __name__ == '__main__' :
    listConfigurations()
