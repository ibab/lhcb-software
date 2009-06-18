import GaudiPython
from Gaudi.Configuration import*
from Configurables import ConfigStackAccessSvc, ConfigDBAccessSvc, ConfigTarFileAccessSvc, ConfigFileAccessSvc, ConfigTreeEditor, PropertyConfigSvc

# pick the default config access svc
from Configurables import ConfigTarFileAccessSvc as ConfigAccessSvc

from TCKUtils.Sandbox import execInSandbox
from pprint import pprint

### add some decoration...
GaudiPython.gbl.Gaudi.Math.MD5.__str__ = GaudiPython.gbl.Gaudi.Math.MD5.str
GaudiPython.gbl.ConfigTreeNodeAlias.alias_type.__str__ = GaudiPython.gbl.ConfigTreeNodeAlias.alias_type.str
digest = GaudiPython.gbl.Gaudi.Math.MD5.createFromStringRep
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
    if type(x) == str and x[0:2] == '0x' :  return int(x,16)
    return int(x)

def _tck2id(x,cas = ConfigAccessSvc() ) :
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
    if type(x) is int : x = hex(x)
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
      

def _createTCKEntries(d, cas ) :
    name = cas.getFullName()
    appMgr = _appMgr()
    appMgr.createSvc(name)
    s = appMgr.service(name,'IConfigAccessSvc')
    for tck,id in d.iteritems() :
        id  = _digest(id,cas)
        tck = _tck(tck)
        print ' creating mapping:  TCK: ' + ('0x%08x'%tck) + ' -> ID: ' + str(id)
        ref = s.readConfigTreeNode( id )
        alias = TCK( ref.get(), tck )
        s.writeConfigTreeNodeAlias(alias)


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
            if k.info['id'] == id : k.update( { 'TCK' : tck } )
    for i in s.configTreeNodeAliases( alias( 'TAG/'  ) ) :
        tag = i.alias().str().split('/')[1:] 
        id  = i.ref().str()
        for k in info.values() : 
            if k.info['id'] == id : k.update( { 'TAG' : tag } ) 
    return info

def _getConfigTree( configID , cas = ConfigAccessSvc() ) :
    id = _digest(tck2id(configID,cas),cas)
    pc = PropertyConfigSvc( prefetchConfig = [ id.str() ],
                            ConfigAccessSvc = cas.getFullName() )
    name = pc.getFullName()
    appMgr = _appMgr()
    appMgr.createSvc(name)
    # init TreeNodeCache singleton
    _TreeNodeCache( svc = appMgr.service(name,'IPropertyConfigSvc') )
    return Tree(id)

def _xget( configIDs , cas = ConfigAccessSvc() ) :
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
    tables = execInSandbox( _xget, [ id ], cas )
    return _lookupProperty(tables[id],algname,property)

def _updateProperties(id, updates, label, cas  ) :
    if type(id) == str: id = digest( id )
    pc = PropertyConfigSvc( prefetchConfig = [ id.str() ],
                            ConfigAccessSvc = cas.getFullName() )
    cte = ConfigTreeEditor( PropertyConfigSvc = pc.getFullName() )
    # run program...
    appMgr = _appMgr()
    appMgr.createSvc(pc.getFullName())
    cteName = cte.name().split('.')[-1]
    ed = appMgr.toolsvc().create(cteName,interface='IConfigTreeEditor')
    if not ed : raise RuntimeWarning(' could not get tool ' + cteName )
    cf = appMgr.service(cas.getFullName(),'IConfigAccessSvc')
    if not cf : raise RuntimeWarning(' could not get service ' + cas.name() )
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
        self.info = { 'id' : alias.ref().str() , 'TCK' : '<NONE>', 'label' : '<NONE>' }
        self.info.update( zip(['release','hlttype'],alias.alias().str().split('/')[1:3]))
        x = svc.readConfigTreeNode( alias.ref() )
        label =  x.get().label();
        self.info.update( { 'label' : label } )
    def __getitem__(self,label) : 
        return self.info[label]
    def update(self,d) : 
        self.info.update( d )
    def printSimple(self,prefix='      ') : 
        tck = self.info['TCK']
        if type(tck) == int : tck = '0x%08x' % tck
        print prefix + '%10s : %s : %s'%(tck,self.info['id'],self.info['label'])
    def PVSS(self) :
        tck = self.info['TCK']
        if type(tck) == str and tck != '<NONE>' and not tck.startswith('0x') : tck = int(tck)
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
 

def diff( lhs, rhs , cas = ConfigAccessSvc() ) :
    (lhs,rhs) = (tck2id(lhs,cas),tck2id(rhs,cas))
    table = execInSandbox( _xget, [ lhs, rhs ] , cas ) 
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




def updateProperties(id,updates,label='', cas = ConfigAccessSvc() ) :
    return execInSandbox( _updateProperties,id,updates,label, cas )
def createTCKEntries(d, cas = ConfigAccessSvc() ) :
    return execInSandbox( _createTCKEntries, d, cas )
def copy( source = ConfigAccessSvc() , target = ConfigDBAccessSvc(ReadOnly=False) ) :
    return execInSandbox( _copy, source, target )

def listComponents( id, cas = ConfigAccessSvc() ) :
    tree = execInSandbox( _getConfigTree, id, cas )
    for i in tree : 
        if i.leaf : 
          s =  i.depth*3*' ' + i.leaf.name
          print s + (80-len(s))*' ' + str(i.leaf.digest)
def listAlgorithms( id, cas = ConfigAccessSvc() ) :
    tree =  execInSandbox( _getConfigTree, id, cas )
    for i in tree :
       if i.leaf and i.leaf.kind =='IAlgorithm':
          s =  i.depth*3*' ' + i.leaf.name
          print s + (80-len(s))*' ' + str(i.leaf.digest)
def listProperties( id, algname='',property='',cas = ConfigAccessSvc() ) :
    tree = execInSandbox( _getConfigTree, id, cas ) 
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
def orphanScan( cas = ConfigAccessSvc() ) :
    return execInSandbox(_orphanScan, cas)

def getConfigurations( cas = ConfigAccessSvc() ) :
    return execInSandbox( _getConfigurations, cas )
def getReleases( cas = ConfigAccessSvc() ) :
    return set( [ i['release']  for i in getConfigurations(cas).itervalues()  ] )
def getHltTypes( release, cas = ConfigAccessSvc() ) :
    info = execInSandbox( _getConfigurations, cas )
    return set( [ i['hlttype']  for i in info.itervalues() if i['release']==release ] )
def getTCKs( release, hlttype, cas = ConfigAccessSvc() ) :
    info = execInSandbox( _getConfigurations, cas )
    return [ ('0x%08x'%v['TCK'],v['label'])  for v in info.itervalues() if v['release']==release and v['hlttype']==hlttype] 
def getRoutingBits( id , cas = ConfigAccessSvc() ) :
    # should be a map... so we try to 'eval' it
    for p in ['RoutingBits','routingBitDefinitions'] :
        try :
            return eval(_getProperty(id,'HltRoutingBitsWriter',p,cas))
        except KeyError : 
            continue
    return None

def getHlt1Lines( id , cas = ConfigAccessSvc() ) :
    # should be a list... so we try to 'eval' it
    return eval(_getProperty(id,'Hlt1','Members',cas))
def getHlt1Decisions( id , cas = ConfigAccessSvc() ) :
    id = tck2id(id,cas)
    table = execInSandbox( _xget, [ id ], cas )[id]
    lines = eval(_lookupProperty(table,'Hlt1','Members'))
    return [ _lookupProperty(table,i.split('/')[-1],'DecisionName') for i in lines ]


def printConfigurations( info ) :
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
def listHlt1Decisions( id, cas = ConfigAccessSvc() ) :
    print getHlt1Decisions(id,cas)


######  do the actual work...

if __name__ == '__main__' :

#   cas = ConfigDBAccessSvc( ReadOnly = True , OutputLevel=DEBUG )
    listConfigurations()
#    copy()
#   createTCKEntries( { 1 : 'ecaf5768575d96fed8b54ed02dbf1496' , 
#                       2 : '9ffa18d95f9bf05421a5e6276adc8c67' } )

#    id =  'ecaf5768575d96fed8b54ed02dbf1496' 
#    listAlgorithms( id )
#    listProperties( id, 'Hlt1MuonDiMuon2L0WithIPDecision' )
#    updateProperties( id, 'Hlt1MuonDiMuon2L0WithIPDecision' , { 'FilterDescriptor' : '''[ 'VertexDimuonMass,>,1500.' , 'VertexMinIP_PV2D,||>,0.25' ]''' } )
#   listProperties( id, 'PrescaleHlt2SelB2HH' )
#   listProperties( id, 'SeqHlt2SharedD02KsPiPi' )
#   listProperties( id, 'HltPatPV3D' )
