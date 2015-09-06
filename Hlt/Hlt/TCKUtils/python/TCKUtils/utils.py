import GaudiPython
import cppyy
from Gaudi.Configuration import *
from Configurables import ConfigStackAccessSvc, ConfigDBAccessSvc, ConfigZipFileAccessSvc, ConfigTarFileAccessSvc, ConfigFileAccessSvc, ConfigCDBAccessSvc
from Configurables import ConfigTreeEditor, PropertyConfigSvc

# pick the default config access svc
import os.path
def cdb_file(filename) :
    return os.path.join( os.environ['HLTTCKROOT'], filename )

def cdb_file_exists(filename):
    return os.path.isfile( cdb_file(filename) )

if cdb_file_exists('config.cdb') :
    from Configurables import ConfigCDBAccessSvc as ConfigAccessSvc
else :
    from Configurables import ConfigTarFileAccessSvc as ConfigAccessSvc

cdb_filename = cdb_file('config.cdb') if cdb_file_exists('config.cdb') else cdb_file('config.tar')
print 'using %s as input.' % cdb_filename

from pprint import pprint

## TCK numbering
## Bit 31 is assigned to "technical"
## Bit 30 is assigned to "for MC"
## Bit 29 is assigned to "HLT2 only"
## Bit 28 is assigned to "HLT1 only"
## Neither 30 nor 29 is "old style" Hlt1 + Hlt2

### add some decoration...
MD5 = cppyy.gbl.Gaudi.Math.MD5
MD5.__str__ = MD5.str
MD5.__hash__ = lambda x : x.str().__hash__()
cppyy.gbl.ConfigTreeNodeAlias.alias_type.__str__ = cppyy.gbl.ConfigTreeNodeAlias.alias_type.str
digest = MD5.createFromStringRep
alias = cppyy.gbl.ConfigTreeNodeAlias.alias_type
topLevelAlias = cppyy.gbl.ConfigTreeNodeAlias.createTopLevel
TCK = cppyy.gbl.ConfigTreeNodeAlias.createTCK
from os import getpid

def _appMgr() :
    #print 'starting appMgr @ pid = %s' % getpid()
    ApplicationMgr().AppName = ""
    ApplicationMgr().OutputLevel = ERROR
    appMgr = GaudiPython.AppMgr()
    appMgr.initialize()
    return appMgr

def _tck(x) :
    if type(x) == str and x[0:2].upper() == '0X' :  return int(x,16)
    return int(x)

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

class AccessSvcSingleton(object) :
    __app = None
    __pcs = None
    __cas = None
    __cte = None
    def reset(self) :
        if self._app() : self._app().finalize()
        AccessSvcSingleton.__pcs = None
        AccessSvcSingleton.__cas = None
        AccessSvcSingleton.__cte = None
        AccessSvcSingleton.__app = None

    def _app(self) :
        return AccessSvcSingleton.__app
    def _pcs(self) :
        return AccessSvcSingleton.__pcs
    def _cas(self) :
        return AccessSvcSingleton.__cas
    def _cte(self) :
        if callable( AccessSvcSingleton.__cte) :
            AccessSvcSingleton.__cte = AccessSvcSingleton.__cte(self)
        return AccessSvcSingleton.__cte

    def __init__(self,create=False,createConfigTreeEditor=False,cas=ConfigAccessSvc()) :
         if create :
            if (AccessSvcSingleton.__pcs or AccessSvcSingleton.__cas) :
                   raise LogicError('re-entry of singleton creation')
            pcs = PropertyConfigSvc( ConfigAccessSvc = cas.getFullName() )
            cte = ConfigTreeEditor( PropertyConfigSvc = pcs.getFullName()
                                  , ConfigAccessSvc   = cas.getFullName() )
            AccessSvcSingleton.__app = _appMgr()
            self._app().createSvc(cas.getFullName())
            AccessSvcSingleton.__cas = self._app().service(cas.getFullName(),'IConfigAccessSvc')
            self._app().createSvc(pcs.getFullName())
            AccessSvcSingleton.__pcs = self._app().service(pcs.getFullName(),'IPropertyConfigSvc')
            AccessSvcSingleton.__cte = lambda x : self._app().toolsvc().create(cte.getFullName(),interface='IConfigTreeEditor')

    def resolveTCK(self,tck) :
         tck = _tck(tck)
         for i in self._cas().configTreeNodeAliases( alias( 'TCK/'  ) ) :
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
         return self._pcs().resolveConfigTreeNode(id) if (id and id.valid()) else None
    def writeConfigTreeNode(self,node) :
         return self._cas().writeConfigTreeNode(node)
    def resolvePropertyConfig(self,id) :
         return self._pcs().resolvePropertyConfig(id) if (id and id.valid()) else None
    def writePropertyConfig(self,node) :
         return self._cas().writePropertyConfig(node)
    def collectLeafRefs(self,id) :
         if type(id) is not MD5 :
             id = self._2id(id)
         for ids in self._pcs().collectLeafRefs( id )  :
             yield PropCfg(ids)
    def resolveConfigTreeNodeAliases(self, a ) :
         if type(a) is not type(alias) : a = alias(a)
         return self._pcs().configTreeNodeAliases( a )
    def configTreeNodeAliases( self, alias ) :
         return self._cas().configTreeNodeAliases( alias )
    def writeConfigTreeNodeAlias(self,alias) :
         return self._cas().writeConfigTreeNodeAlias(alias)
    def updateAndWrite(self, id, updates, label) :
        if not updates:
            return

        print 'updateAndWrite updates:'
        pprint(dict(updates))
        mod_map = cppyy.gbl.std.multimap('std::string', 'std::pair<std::string, std::string>')
        value_pair = cppyy.gbl.std.pair('std::string', 'std::string')
        insert_pair = cppyy.gbl.std.pair('const std::string', 'std::pair<std::string, std::string>')
        mods = mod_map()
        for algname, props in updates.iteritems() :
            for k, v in props.iteritems() :
                vp = value_pair(k, v if type(v) == str else repr(v))
                ip = insert_pair(algname, vp)
                mods.insert(ip)

        return self._cte().updateAndWrite(id,mods,label)


def createAccessSvcSingleton( cas = ConfigAccessSvc(), createConfigTreeEditor = False ) :
    return AccessSvcSingleton( create = True, createConfigTreeEditor = createConfigTreeEditor, cas = cas )


def _lookupProperty(table,algname,property) :
    if algname not in table : raise KeyError("no algorithm named %s in specified config"%algname )
    properties = table[algname].properties()
    if property not in properties: raise KeyError("could not locate property %s for algorithm %s in specified config"%(property,algname) )
    return properties[property]

def getProperty(id,algname,property, cas ) :
    tables = xget( [ id ], cas )
    return _lookupProperty(tables[id],algname,property)



### and now define the routines visible from the outside world...

class Configuration :
    " A class representing a configuration entry "
    def __init__(self,alias,svc) :
        self.info = { 'id' : alias.ref().str() , 'TCK' : [], 'label' : '<NONE>' }
        self.info.update( zip(['release','hlttype'],alias.alias().str().split('/')[1:3]))
        x = svc.resolveConfigTreeNode( alias.ref() )
        if not x :
            print 'Configuration: failed to resolve configtreenode %s - %s ' %  ( alias.alias().str(), alias.ref() )
        label =  x.label();
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
    _nreq = 0
    _noid = 0
    _cm = 0
    #  TODO: add functionality to flush _pool
    def __new__(cls, id = None) :
        TreeNode._nreq = TreeNode._nreq + 1
        if not id:
            TreeNode._noid = TreeNode._noid+1
            return object.__new__(cls)
        if type(id) != MD5 and type(id) != int :
            print id,type(id)
            id = digest(id)
        obj = TreeNode._pool.get( id )
        if not obj :
            TreeNode._cm = TreeNode._cm+1
            obj = AccessSvcSingleton().resolveConfigTreeNode(id)
            TreeNode._pool[id] = obj
        if not obj :
            print 'failed to resolve ConfigTreeNode %s'%id
        return obj


class PropCfg(object) :
    " A class representing a PropertyConfig "
    #  use flyweight pattern, and use digest to identify objects...
    import weakref
    _pool = weakref.WeakValueDictionary()
    _nreq = 0
    _noid = 0
    _cm = 0
    #  TODO: make a singleton svc which we use to resolve IDs if not existent yet...
    #  TODO: add functionality to flush _pool
    def __new__(cls, id = None) :
        PropCfg._nreq = PropCfg._nreq + 1
        if not id :
            PropCfg._noid = PropCfg._noid +1
            return object.__new__(cls) # added to make it possible to recv in parent process...
        if type(id) != MD5 :
            print id,type(id)
            id = digest(id)
        obj = PropCfg._pool.get( id )
        if not obj :
            PropCfg._cm = PropCfg._cm + 1
            x = AccessSvcSingleton().resolvePropertyConfig(id)
            if not x :
                print 'failed to resolve PropertyCondif %s'%id
            obj = object.__new__(cls)
            obj.name = x.name()
            obj.type = x.type()
            obj.kind = x.kind()
            obj.fullyQualifiedName = x.fullyQualifiedName()
            obj.digest = id
            obj.props  = dict( (i.first,i.second) for i in x.properties() )
            PropCfg._pool[id] = obj
        if not obj :
                print 'invalid PropCfg %s'%id
        return obj
    def properties(self) : return self.props
    def fqn(self) : return  self.fullyQualifiedName + ' ('+self.kind+')'
    def fmt(self) :
        return [  "'%s':%s\n"%(k,v)  for k,v in self.props.iteritems() ]

class Tree(object):
    def __init__(self,id,parent=None ) :
        self.parent = parent
        self.depth = self.parent.depth+1  if self.parent else 0
        node = TreeNode(id)
        if not node : print 'invalid TreeNode for %s' % id
        self.digest = node.digest().str()
        self.label = node.label()
        leaf =  node.leaf()
        self.leaf = PropCfg( leaf ) if leaf.valid() else None
        self.nodes = [ Tree(id=id,parent=self) for id in node.nodes() ]
        #TODO: add direct access to leafs 'in' this tree by name
        #      i.e. notify 'top level parent' of 'my' leafs...
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
    def iterleafs(self) :
        for i in self._inorder() : # this _can_ give duplicate leafs!!!
            if i.leaf is None : continue
            yield i.leaf
    def leafs(self) :
        d = dict()
        for i in self.iterleafs() :
            if i.name not in d : d[ i.name ] = i
        return d


    #TODO: add direct access to leafs 'in' this tree by name
    # and use it to implement an efficient __contains__


# TODO: rewrite in terms of trees...
#       that should make it a lot faster for almost identical
#       trees...
def diff( lhs, rhs , cas = ConfigAccessSvc() ) :
    table = xget( [ lhs, rhs ] , cas )
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

def copy( source = ConfigAccessSvc() , target = ConfigDBAccessSvc(ReadOnly=False), glob = None ) :
    if source == target :
        print 'WARNING: source and target are the same -- no-op ...'
        return
    r = AccessProxy().access( cas = ConfigStackAccessSvc( ConfigAccessSvcs = [ target.getFullName(), source.getFullName() ]
                                                        , OutputLevel=DEBUG )
                            ).rcopy(glob)
    AccessProxy().flush()
    return r

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
        matchNode = lambda x : reqNode.match(x.leaf.fullyQualifiedName)
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

def getTCKInfo(x) :
    for (i,j) in getConfigurations().iteritems() :
        if x in j['TCK'] : return (j['hlttype'],j['release'])
    return None

def getReleases( cas = ConfigAccessSvc() ) :
    return set( [ i['release']  for i in getConfigurations(cas).itervalues()  ] )
def getHltTypes( release, cas = ConfigAccessSvc() ) :
    info = getConfigurations( cas )
    return set( [ i['hlttype']  for i in info.itervalues() if i['release']==release ] )
def getTCKs( release = None, hlttype = None, cas = ConfigAccessSvc() ) :
    info = getConfigurations( cas )
    pred = lambda x : x['TCK'] and ( not release or x['release'] == release ) and ( not hlttype or x['hlttype'] == hlttype )
    result = []
    for i in [ x for x in info.itervalues() if pred(x) ]:
            for tck in i['TCK'] :
                result.append( ('0x%08x'%tck,i['label'])  )
    return result
def getTCKList( cas = ConfigAccessSvc() ) :
    info = getConfigurations( cas )
    result = []
    for i in info.itervalues() :
            for tck in i['TCK'] : result.append( '0x%08x'%tck  )
    return result
def getRoutingBits( id , cas = ConfigAccessSvc() ) :
    # should be a map... so we try to 'eval' it
    for p in ['RoutingBits','routingBitDefinitions'] :
        try :
            return eval(getProperty(id,'Hlt1RoutingBitsWriter',p,cas))
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

def dump( id, properties = None,  lines = None, file = None, cas = ConfigAccessSvc() ) :
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

    def prettyPrintDict(code,trItem) :
        try :
            cuts = eval(code) # should be a dict
            return "{ "+ ('\n' + (_tab+25+18)*' ' + ', ' ).join(  [ trItem(k,v) for (k,v) in cuts.iteritems() ] ) + '\n'+(_tab+25+18)*' '+"}"
        except :
            return code

    def prettyPrintList(code,trItem = None, skipEmpty = True) :
        try :
            l = eval(code)
            if skipEmpty and not l : return ''
            if len(l)<2 : return code
            if trItem :
                l = [ trItem(i) for i in l ]
            return "[ "+('\n' + (_tab+25+18)*' ' + ', ' ).join( l )+'\n'+(_tab+25+18)*' '+']'
        except :
            return code

    trtable = { 'Code' : prettyPrintStreamer
              , 'DaughtersCuts' : lambda x : prettyPrintDict(x, lambda k,v : "'%s' : '%s'"%(k,v) )
              , 'Inputs' : prettyPrintList
              , 'InputLocations' : prettyPrintList
              , 'Preambulo' : prettyPrintList
              , 'FilterDescriptor' : lambda x : prettyPrintList(x,lambda y : "'%s'"%y, True)
              , 'RoutingBits' : lambda x : prettyPrintDict(x, lambda k,v : "%2d : \"%s\""%(k,v) )
              }

    import re
    show = not lines
    from sys import stdout
    file = open(file,'w') if file else sys.stdout
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
           if v : line += '%-15s : %s' % ( k, v)
       file.write(line+'\n')


class RemoteAccess(object) :
    _svc = None
    def __init__( self, cas ) :
        #print 'remote(%s) created at pid=%s and cas %s' % (self,getpid(),cas.getType())
        RemoteAccess._svc = createAccessSvcSingleton( cas = cas )
    def __del__( self ) :
        RemoteAccess._svc.reset()
    def rgetConfigTree( self, id ) :
        # maybe prefetch all leafs by invoking
        #RemoteAccess._svc.collectLeafRefs(id)
        # benchmark result: makes no difference whatsoever...
        #from time import clock
        #x = clock()
        t = Tree(id)
        #print '<< remote(%s) at pid=%s: rgetConfigTree(%s) : lookup time: %s s.' % (self,getpid(),id, clock()-x)
        return t
    def rgetConfigurations( self ) :
        #print 'remote(%s) at pid=%s: rgetConfigurations()' % (self,getpid())
        svc = RemoteAccess._svc
        info = dict()
        #print 'reading TOPLEVEL'
        for i in svc.configTreeNodeAliases( alias( 'TOPLEVEL/') ) :
            x = Configuration( i,svc )
            info[ i.alias().str() ] = x
        #print 'reading TCK'
        for i in svc.configTreeNodeAliases( alias( 'TCK/'  ) ) :
            tck =  _tck(i.alias().str().split('/')[-1])
            id  =  i.ref().str()
            for k in info.values() :
                if k.info['id'] == id : k.info['TCK'].append(tck)
        #print 'reading TAG'
        for i in svc.configTreeNodeAliases( alias( 'TAG/'  ) ) :
            tag = i.alias().str().split('/')[1:]
            id  = i.ref().str()
            for k in info.values() :
                if k.info['id'] == id : k.update( { 'TAG' : tag } )
        return info
    def rupdateProperties(self, id, updates, label ) :
        if not label :
            print 'please provide a reasonable label for the new configuration'
            return None
        svc = RemoteAccess._svc
        #  either we got (some form of) an ID, and we reverse engineer back the alias (provided it is unique!)
        #  or we got an alias directly...

        if type(id) == cppyy.gbl.ConfigTreeNodeAlias :
            a = id.alias().str()
            id = id.ref()
            # this is fine for TOPLEVEL, but not TCK...
        else :
            id = svc._2id(id)
            if not id.valid() : raise RuntimeWarning('not a valid id : %s' % id )
            a = [ i.alias().str() for  i in svc.configTreeNodeAliases( alias('TOPLEVEL/') ) if i.ref() == id ]
            if len(a) != 1 :
                print 'something went wrong: no unique toplevel match for ' + str(id)
                return
            a = a[0]
        (release,hlttype) = a.split('/',3)[1:3]
        newId = svc.updateAndWrite(id, updates, label)
        noderef = svc.resolveConfigTreeNode( newId )
        top = topLevelAlias( release, hlttype, noderef )
        svc.writeConfigTreeNodeAlias(top)
        print 'wrote ' + str(top.alias())
        return str(newId)
    def rupdateL0TCK(self, id, l0config, label, extra ) :
        svc = RemoteAccess._svc
        id  = svc._2id(id)
        if not id.valid() : raise RuntimeWarning('not a valid id : %s' % id )
        a = [ i.alias().str() for  i in svc.configTreeNodeAliases( alias('TOPLEVEL/') ) if i.ref() == id ]
        if len(a) != 1 :
            print 'something went wrong: no unique toplevel match for ' + str(id)
            return
        (release,hlttype) = a[0].split('/',3)[1:3]

        from collections import defaultdict
        updates = defaultdict(dict)
        # check L0 config in source config
        for cfg in svc.collectLeafRefs( id ):
            #  check for either a MultiConfigProvider with the right setup,
            #  or for a template with the right TCK in it...
            if cfg.name == 'ToolSvc.L0DUConfig' :
                if cfg.type != 'L0DUConfigProvider':
                    raise KeyError("Can only update configuration which use L0DUConfigProvider, not  %s" % cfg.type )
                #  check that all specified properties exist in cfg
                for (k,v) in l0config.iteritems() :
                    if k not in cfg.props : raise KeyError('Specified property %s not in store'%k)
                    updates['ToolSvc.L0DUConfig'].update({k : v})

        if extra:
            updates.update(extra)

        newId = svc.updateAndWrite(id, updates, label)
        noderef = svc.resolveConfigTreeNode( newId )
        if not noderef : print 'oops, could not find node for %s ' % newId
        top = topLevelAlias( release, hlttype, noderef )
        svc.writeConfigTreeNodeAlias(top)
        print 'wrote ' + str(top.alias())
        return str(newId)
    def rresolveTCK(self, tck ) :
        svc = RemoteAccess._svc
        return svc.resolveTCK(tck)

    def rcreateTCKEntries(self, d ) :
        svc = RemoteAccess._svc
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
            ref = svc.resolveConfigTreeNode( id )
            alias = TCK( ref, tck )
            svc.writeConfigTreeNodeAlias(alias)

    def rcopyTree( self, nodeRef ) :
        svc = RemoteAccess._svc
        def __nodes( n ) :
            node = svc.resolveConfigTreeNode( n )
            from itertools import chain
            # depth first traversal -- this insures we do not write
            # incomplete data objects...
            for j in chain.from_iterable( __nodes(i) for i in node.nodes() ) :
                yield j
            yield node
        for i in __nodes( nodeRef ) :
            leafRef = i.leaf()
            if leafRef.valid() :
                leaf = svc.resolvePropertyConfig(leafRef)
                assert leaf
                newRef = svc.writePropertyConfig(leaf)
                assert leafRef == newRef
            n = svc.writeConfigTreeNode(i)
            assert n == i.digest()

    def rcopy( self, glob = None  ) :
        svc = RemoteAccess._svc
        from copy import deepcopy
        from itertools import chain
        aliases = [ deepcopy(i)  for label in ( 'TOPLEVEL/','TCK/' ) for i in svc.configTreeNodeAliases( alias(label) ) ]
        if glob :
             from fnmatch import fnmatch
             aliases = filter( lambda i : fnmatch(i.alias().str(),glob), aliases)
        # Now, split them into
        #    TOPLEVEL (just copy)
        top   = filter( lambda i : i.alias().str().startswith('TOPLEVEL/'), aliases )
        #    The rest: find corresponding TOPLEVEL, add it to the toplevel list, re-create alias afterwards
        other = filter( lambda i : not i.alias().str().startswith('TOPLEVEL/'), aliases )
        assert len(top)+len(other) == len(aliases)
        toplevelaliases = svc.configTreeNodeAliases( alias('TOPLEVEL/') )
        for i in other :
            top += [ deepcopy(j) for j in toplevelaliases if j.ref() == i.ref() and j not in top ]
        l = len(top)
        print '# of TOPLEVEL items to copy: %s' % l
        for k,i in enumerate( sorted( top, key = lambda x : x.alias().str() ), 1 ):# TODO: sort on the (integer) numbers appearing in the string...
            print '[%s/%s] copying tree %s' % (k,l, i.alias())
            empty = dict()
            node = svc.resolveConfigTreeNode( i.ref() )
            newid = self.rupdateProperties(i,empty,node.label())
            assert newid == i.ref().str()
        for i in  other:
            print 'copying alias %s -> %s ' % (i.alias().str(),i.ref() )
            svc.writeConfigTreeNodeAlias( i )
        print 'done copying... '


from multiprocessing.managers import BaseManager
class AccessMgr(BaseManager):
    pass

AccessMgr.register('Access',RemoteAccess)


class AccessProxy( object ) :
    _manager = None
    _access = None
    _cas = None
    def __init__( self ) :
        #print 'creating proxy in pid = %s' % os.getpid()
        pass
    # TODO: access should be seperately for each cas instance...
    #  worse: since Configurables are singletons, they may have
    #         changed since the last time used. Hence, have to
    #         check that that hasn't happend, so we need a local
    #         copy of the state of cas, and compare to that....
    #         and if different, let's just shutdown the remote
    #         and start again...
    #         (shouldn't have to flush PropertyConfig/ConfigTreeNode)
    def access( self, cas ) :
        if not self._valid(cas) : self.flush()
        if not AccessProxy._manager :
            AccessProxy._manager  = AccessMgr()
            AccessProxy._manager.start()
            #print 'proxy started manager'
        if not AccessProxy._access :
            #print 'proxy requesting access to remote -- cas = %s'%(cas.getType())
            AccessProxy._access = AccessProxy._manager.Access( cas )
            AccessProxy._cas = cas
            AccessProxy._properties = cas.getProperties()

        return AccessProxy._access
    def _valid( self, cas ) :
        if not AccessProxy._access or not AccessProxy._cas: return True
        if cas != AccessProxy._cas : return False # different configurable!
        return cas.getProperties() == AccessProxy._properties
    def flush( self ) : # make flush visible such that eg. createTCKEntries can flush the remote and force re-reading...
        AccessProxy._cas = None
        AccessProxy._properties = None
        AccessProxy._access = None
        AccessProxy._manager.shutdown()
        AccessProxy._manager = None


def resolveTCK( tck, cas=ConfigAccessSvc()) :
    return AccessProxy().access(cas).rresolveTCK( tck )

def getConfigTree(id, cas = ConfigAccessSvc()):
    if 'forest' not in dir(getConfigTree) : getConfigTree.forest = dict()
    if id not in getConfigTree.forest :
        tree = AccessProxy().access(cas).rgetConfigTree( id )
        getConfigTree.forest[id] = tree
        if tree.digest != id :
            # in case we got a TCK, the remote side resolves this to an ID
            # and we mark this ID in our cache. Unfortunately, it doesn't work
            # the other way around (i.e. we first get an ID, then a TCK )
            # and we must rely on the remote to cache as much as possible...
            getConfigTree.forest[tree.digest] = tree
    return getConfigTree.forest[id]

def getConfigurations( cas = ConfigAccessSvc() ) :
    return AccessProxy().access(cas).rgetConfigurations()

def updateProperties(id,updates,label='', cas = ConfigAccessSvc() ) :
    ret = AccessProxy().access(cas).rupdateProperties( id,updates,label )
    if ret : AccessProxy().flush() # explicit flush in case we wrote something
    return ret

def updateL0TCK(id, l0tck, label='', cas = ConfigAccessSvc(), extra = None ) :
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
    ret = AccessProxy().access(cas).rupdateL0TCK(id,l0config,label,extra)
    if ret : AccessProxy().flush()
    return ret


def createTCKEntries(d, cas = ConfigAccessSvc() ) :
    ret = AccessProxy().access(cas).rcreateTCKEntries(d)
    AccessProxy().flush()
    return ret


def xget( ids , cas = ConfigAccessSvc() ) :
    if 'forest' not in dir(xget) : xget.forest = dict()
    fetch = [ id for id in ids if id not in xget.forest.keys() ]
    if fetch :
        for t in fetch : xget.forest[t] = getConfigTree(t).leafs()
    forest = dict()
    for id in ids : forest[id] = xget.forest[id]
    return forest


def getHlt1Lines( id , cas = ConfigAccessSvc() ) :
    # should be a list... so we try to 'eval' it
    return eval(getProperty(id,'Hlt1','Members',cas))
def getHlt2Lines( id , cas = ConfigAccessSvc() ) :
    # should be a list... so we try to 'eval' it
    return eval(getProperty(id,'Hlt2','Members',cas))
def getHltLines( id , cas = ConfigAccessSvc() ) :
    return getHlt1Lines(id,cas) + getHlt2Lines(id,cas)

def getHlt1Decisions( id , cas = ConfigAccessSvc() ) :
    table = xget( [ id ], cas )[id]
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
    pprint(getRoutingBits(id,cas))
def listHlt1Lines( id, cas = ConfigAccessSvc() ) :
    pprint(getHlt1Lines(id,cas))
def listHlt2Lines( id, cas = ConfigAccessSvc() ) :
    pprint(getHlt2Lines(id,cas))
def listHlt1Decisions( id, cas = ConfigAccessSvc() ) :
    pprint(getHlt1Decisions(id,cas))


######  do the actual work...

if __name__ == '__main__' :
    listConfigurations()
