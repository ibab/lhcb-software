import GaudiPython
from Gaudi.Configuration import*
from Configurables import ConfigStackAccessSvc, ConfigDBAccessSvc,ConfigFileAccessSvc, ConfigTreeEditor, PropertyConfigSvc

from Moore.Sandbox import execInSandbox
from pprint import pprint

### add some decoration...
GaudiPython.gbl.Gaudi.Math.MD5.__str__ = GaudiPython.gbl.Gaudi.Math.MD5.str
GaudiPython.gbl.ConfigTreeNodeAlias.alias_type.__str__ = GaudiPython.gbl.ConfigTreeNodeAlias.alias_type.str
digest = GaudiPython.gbl.Gaudi.Math.MD5.createFromStringRep
alias = GaudiPython.gbl.ConfigTreeNodeAlias.alias_type
topLevelAlias = GaudiPython.gbl.ConfigTreeNodeAlias.createTopLevel
TCK = GaudiPython.gbl.ConfigTreeNodeAlias.createTCK
vector_string = GaudiPython.gbl.std.vector('std::string')

def _tck(x) :
    if type(x) == str and x[0:2] == '0x' :  return int(x,16)
    return int(x)


def _createTCKEntries(d, cas ) :
    ApplicationMgr().OutputLevel = ERROR
    appMgr = GaudiPython.AppMgr()
    appMgr.createSvc(cas.getFullName())
    s = appMgr.service(cas.getFullName(),'IConfigAccessSvc')
    for tck,id in d.iteritems() :
        if type(id) == str : id = digest( id )
        print ' creating mapping:  TCK: ' + str(tck) + ' -> ID: ' + str(id)
        ref = s.readConfigTreeNode( id )
        alias = TCK( ref.get(), tck )
        s.writeConfigTreeNodeAlias(alias)


def _getConfigurations( cas = ConfigFileAccessSvc() ) :
    ApplicationMgr().OutputLevel = ERROR
    appMgr = GaudiPython.AppMgr()
    appMgr.createSvc(cas.getFullName())
    s = appMgr.service(cas.getFullName(),'IConfigAccessSvc')
    info = dict()
    for i in s.configTreeNodeAliases( alias( 'TOPLEVEL/') ) :
        info[ i.ref().str() ] = Configuration( i,s )
    for i in s.configTreeNodeAliases( alias( 'TCK/'  ) ) :
        info[ i.ref().str() ].update( { 'TCK' : _tck(i.alias().str().split('/')[-1]) } )
    for i in s.configTreeNodeAliases( alias( 'TAG/'  ) ) :
        info[ i.ref().str() ].update( { 'TAG' : i.alias().str().split('/')[1:] } )
    return info


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
    # run program...
    ApplicationMgr().OutputLevel = ERROR
    appMgr = GaudiPython.AppMgr()
    appMgr.initialize()
    appMgr.createSvc(csvc.getFullName())
    s = appMgr.service(csvc.getFullName(),'IConfigAccessSvc')
    for label in [ 'TOPLEVEL/','TCK/','ALIAS/' ] :
        for i in s.configTreeNodeAliases( alias(label) ) :
            print '\n\n copying tree ' + str(i.alias()) + '\n\n'
            _copyTree(s,i.ref(),' ')
            print '\n\n writing alias ' + str(i.alias()) + '\n\n'
            s.writeConfigTreeNodeAlias(i)
    print 'done copying...'

def _showAlgorithms(id, cas ) :
    if type(id) == str: id = digest( id )
    pc = PropertyConfigSvc( prefetchConfig = [ id.str() ],
                            ConfigAccessSvc = cas.getFullName() )
    cte = ConfigTreeEditor( PropertyConfigSvc = pc.getFullName() )
    # run program...
    ApplicationMgr().OutputLevel = ERROR
    appMgr = GaudiPython.AppMgr()
    appMgr.initialize()
    appMgr.createSvc(pc.getFullName())
    x = appMgr.toolsvc().create(cte.name(),interface='IConfigTreeEditor')
    print '\n\n   List of algorithms for ' + str(id)
    x.printAlgorithms(id)


def _showProperties(id,algname,property, cas ) :
    if type(id) == str: id = digest( id )
    pc = PropertyConfigSvc( prefetchConfig = [ id.str() ],
                            ConfigAccessSvc = cas.getFullName() )
    cte = ConfigTreeEditor( PropertyConfigSvc = pc.getFullName() )
    # run program...
    ApplicationMgr().OutputLevel = ERROR
    appMgr = GaudiPython.AppMgr()
    appMgr.initialize()
    appMgr.createSvc(pc.getFullName())
    x = appMgr.toolsvc().create(cte.name(),interface='IConfigTreeEditor')
    x.printProperties(id,algname,property)

def _updateProperties(id,algname,props, cas  ) :
    if type(id) == str: id = digest( id )
    pc = PropertyConfigSvc( prefetchConfig = [ id.str() ],
                            ConfigAccessSvc = cas.getFullName() )
    cte = ConfigTreeEditor( PropertyConfigSvc = pc.getFullName() )
    # run program...
    ApplicationMgr().OutputLevel = ERROR
    appMgr = GaudiPython.AppMgr()
    appMgr.initialize()
    appMgr.createSvc(pc.getFullName())
    ed = appMgr.toolsvc().create(cte.name(),interface='IConfigTreeEditor')
    cf = appMgr.service(cas.getFullName(),'IConfigAccessSvc')
    a = filter( lambda  i: i.ref() == id , cf.configTreeNodeAliases( alias('TOPLEVEL/') ) )
    if len(a) != 1 : 
        print 'something went wrong: no unique toplevel match for ' + str(id)
        return
    (release,runtype) = a[0].alias().str().split('/',3)[1:3]
    updates = vector_string()
    for k,v in props.iteritems() : 
        updates.push_back( algname + '.' + k + ': ' + v)
    newId = ed.updateAndWrite(id,updates)
    noderef = cf.readConfigTreeNode( newId )
    top = topLevelAlias( release, runtype, noderef.get() )
    cf.writeConfigTreeNodeAlias(top)
    print 'wrote ' + str(top.alias())


### and now define the routines visible from the outside world...

class Configuration :
    " A class representing a configuration entry "
    def __init__(self,alias,svc) :
        self.info = { 'id' : alias.ref().str() , 'TCK' : '<NONE>', 'label' : '<NONE>' }
        self.info.update( zip(['release','runtype'],alias.alias().str().split('/')[1:3]))
        self.info.update( { 'label' :svc.readConfigTreeNode( alias.ref() ).get().label() } )
    def __getitem__(self,label) : 
        return self.info[label]
    def update(self,d) : self.info.update( d )
    def printSimple(self,prefix='      ') : 
        tck = self.info['TCK']
        if type(tck) == int : tck = '0x%08x' % tck
        print prefix + '%10s : %s : %s'%(tck,self.info['id'],self.info['label'])
        


def updateProperties(id,algname,properties, cas = ConfigFileAccessSvc() ) :
    return execInSandbox( _updateProperties, id,algname,properties,cas )
def createTCKEntries(d, cas = ConfigFileAccessSvc() ) :
    return execInSandbox( _createTCKEntries, d, cas )
def copy( source = ConfigFileAccessSvc() , target = ConfigDBAccessSvc(ReadOnly=False) ) :
    return execInSandbox( _copy, source, target )
def showAlgorithms( id, cas = ConfigFileAccessSvc() ) :
    return execInSandbox( _showAlgorithms, id, cas )
def showProperties( id, algname,property='',cas = ConfigFileAccessSvc() ) :
    return execInSandbox( _showProperties, id,algname,property,cas )

def getConfigurations( cas = ConfigFileAccessSvc() ) :
    return execInSandbox( _getConfigurations, cas )
def getReleases( cas = ConfigFileAccessSvc() ) :
    return set( [ i['release']  for i in getConfigurations(cas).itervalues()  ] )
def getRunTypes( release, cas = ConfigFileAccessSvc() ) :
    info = execInSandbox( _getConfigurations, cas )
    return set( [ i['runtype']  for i in info.itervalues() if i['release']==release ] )
def getTCKs( release, runtype, cas = ConfigFileAccessSvc() ) :
    info = execInSandbox( _getConfigurations, cas )
    return [ ('0x%08x'%v['TCK'],v['label'])  for v in info.itervalues() if v['release']==release and v['runtype']==runtype] 

def printConfigurations( info ) :
    for release in set( [ i['release'] for i in info.itervalues()  ] ) : 
        print release
        confInRelease = [ i for i in info.itervalues() if i['release']==release ]
        for runtype in set( [ i['runtype'] for i in confInRelease ] ) :
            print '    ' + runtype
            [ i.printSimple('      ') for i in confInRelease if i['runtype']==runtype ] 
def printReleases( rel ) : pprint(rel)
def printRunTypes( rt ) : pprint(rt)
def printTCKs( tcks ) : pprint(tcks)

def listConfigurations( cas = ConfigFileAccessSvc() ) :
    return printConfigurations( getConfigurations(cas) )
def listReleases( cas = ConfigFileAccessSvc() ) :
    return printReleases( getReleases() ) 
def listRunTypes( release, cas = ConfigFileAccessSvc() ) :
    return printRunTypes( getRunTypes(release) ) 
def listTCKs( release, runtype, cas = ConfigFileAccessSvc() ) :
    return printTCKs( getTCKs(release,runtype) ) 


######  do the actual work...

if __name__ == '__main__' :

#   cas = ConfigDBAccessSvc( ReadOnly = True , OutputLevel=DEBUG )
    listConfigurations()
#    copy()
#   createTCKEntries( { 1 : 'ecaf5768575d96fed8b54ed02dbf1496' , 
#                       2 : '9ffa18d95f9bf05421a5e6276adc8c67' } )

#    id =  'ecaf5768575d96fed8b54ed02dbf1496' 
#    showAlgorithms( id )
#    showProperties( id, 'Hlt1MuonDiMuon2L0WithIPDecision' )
#    updateProperties( id, 'Hlt1MuonDiMuon2L0WithIPDecision' , { 'FilterDescriptor' : '''[ 'VertexDimuonMass,>,1500.' , 'VertexMinIP_PV2D,||>,0.25' ]''' } )
#   showProperties( id, 'PrescaleHlt2SelB2HH' )
#   showProperties( id, 'SeqHlt2SharedD02KsPiPi' )
#   showProperties( id, 'HltPatPV3D' )
