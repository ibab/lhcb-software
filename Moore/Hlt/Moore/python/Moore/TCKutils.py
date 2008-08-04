import GaudiPython
from Gaudi.Configuration import*
from Configurables import ConfigStackAccessSvc, ConfigDBAccessSvc,ConfigFileAccessSvc, ConfigTreeEditor, PropertyConfigSvc

from Moore.Sandbox import execInSandbox

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


def _getConfigurations( cas ) :
    ApplicationMgr().OutputLevel = ERROR
    appMgr = GaudiPython.AppMgr()
    appMgr.createSvc(cas.getFullName())
    s = appMgr.service(cas.getFullName(),'IConfigAccessSvc')
    d = {}
    x = set()
    for i in ['TOPLEVEL','TCK','TAG' ] :
        d[i] = s.configTreeNodeAliases( alias( i+'/'  ) ) 
#        x.append( [ j.ref() for j in d[i] ) # get the label of this config...
    info = {} 
    for i in d['TOPLEVEL'] : 
        info[ i.ref().str()  ] = dict(zip(  ['release','runtype'],  i.alias().str().split('/')[1:3]))
        ref = s.readConfigTreeNode( i.ref() )
        info[ i.ref().str() ].update( { 'label' : ref.get().label() } )
    for i in d['TCK'] :      info[ i.ref().str()  ].update( { 'TCK' : _tck(i.alias().str().split('/')[-1]) } )
    for i in d['TAG'] :      info[ i.ref().str()  ].update( { 'TAG' : i.alias().str().split('/')[1:] } )
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
    for label in [ 'TOPLEVEL','TCK','ALIAS' ] :
        for i in s.configTreeNodeAliases( alias(label+'/') ) : 
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
    print "got release='"+release+"', runtype='"+runtype+"'"
    updates = vector_string()
    print "Properties:" 
    from  pprint import pprint
    pprint( props )
    for k,v in props.iteritems() : 
        updates.push_back( algname + '.' + k + ': ' + v)
    newId = ed.updateAndWrite(id,updates)
    noderef = cf.readConfigTreeNode( newId )
    top = topLevelAlias( release, runtype, noderef.get() )
    cf.writeConfigTreeNodeAlias(top)
    print 'wrote ' + str(top.alias())


### and now define the routines visible from the outside world...
def updateProperties(id,algname,properties, cas = ConfigFileAccessSvc() ) :
    return execInSandbox( _updateProperties, id,algname,property,cas )
def createTCKEntries(d, cas = ConfigFileAccessSvc() ) :
    return execInSandbox( _createTCKEntries, d, cas )
def listConfigurations( cas = ConfigFileAccessSvc() ) :
    info = execInSandbox( _getConfigurations, cas )
    print '\n\n   List of configurations'
    for release in set( [ i['release'] for i in info.itervalues()  ] ) : 
        print release
        for runtype in set( [ i['runtype'] for i in info.itervalues() if i['release']==release ] ) :
            print '    ' + runtype
            for c in [ k for k,v in info.iteritems() if v['release']==release and v['runtype']==runtype] :
                tck = '  <NONE>  '
                if 'TCK' in info[c] : tck = '0x%08x' % info[c]['TCK']
                print '       %s : %s : %s' % ( tck , c, info[c]['label'] )
    return info

def getReleases( cas = ConfigFileAccessSvc() ) :
    info = execInSandbox( _getConfigurations, cas )
    rel = set( [ i['release']  for i in info.itervalues()  ] )
    from pprint import pprint
    pprint(rel)
    return rel

def getRunTypes( release, cas = ConfigFileAccessSvc() ) :
    info = execInSandbox( _getConfigurations, cas )
    rt = set( [ i['runtype']  for i in info.itervalues() if i['release']==release ] )
    from pprint import pprint
    pprint(rt)
    return rt

def getTCKs( release, runtype, cas = ConfigFileAccessSvc() ) :
    info = execInSandbox( _getConfigurations, cas )
    rt = [ (k,v['TCK'])  for k,v in info.iteritems() if v['release']==release and v['runtype']==runtype] 
    from pprint import pprint
    pprint(rt)
    return rt

def copy( source = ConfigFileAccessSvc() , target = ConfigDBAccessSvc(ReadOnly=False) ) :
    return execInSandbox( _copy, source, target )
    
                               

def showAlgorithms( id, cas = ConfigFileAccessSvc() ) :
    return execInSandbox( _showAlgorithms, id, cas )
def showProperties( id, algname,property='',cas = ConfigFileAccessSvc() ) :
    return execInSandbox( _showProperties, id,algname,property,cas )


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
