import os, sys, PyLCGDict
PyLCGDict.loadDict('GaudiKernelDict')
gbl = PyLCGDict.makeNamespace('')
Interface      = gbl.GaudiPython.Interface
Helper         = gbl.GaudiPython.Helper
StringProperty = gbl.SimpleProperty('std::basic_string<char>','BoundedVerifier<std::basic_string<char> > ')
DataObject     = gbl.DataObject
PropertyRefs = []

print ' ATTENTION: Local version (from $BENDERPYTHON) of gaudimodule is loaded '

#----enable tab completion------------------------------
try:
  import rlcompleter,readline    
  readline.parse_and_bind("tab: complete")
except:
  pass

class iProperty(object) :
  def __init__(self, name, ip = None) :
    if ip : self.__dict__['_ip'] = Interface(gbl.IProperty).cast(ip)
    else  : self.__dict__['_ip'] = None
    self.__dict__['_svcloc'] = gbl.Gaudi.svcLocator()
    optsvc = Helper.service(self._svcloc,'JobOptionsSvc')
    if optsvc : self.__dict__['_optsvc'] = Interface(gbl.IJobOptionsSvc).cast(optsvc)
    else      : self.__dict__['_optsvc'] = None
    self.__dict__['_name'] = name
  def getInterface(self) :
    if not self._ip : self.retrieveInterface()
    return self._ip
  def retrieveInterface(self) :
    pass
  def __setattr__(self, name, value):
    ip = self.getInterface()
    if ip :
      prop = ip.getProperty(name)
      if self.isvector(prop) :
        v = prop.value()
        v.clear()
        map(v.push_back,value)
        prop.setValue(v)
      else :
        prop.setValue(value)
    else :
      sprop = StringProperty(name,str(value))
      self._optsvc.addPropertyToCatalogue(self._name, sprop)
      PropertyRefs.append(sprop)
  def __getattr__(self, name ):
    ip = self.getInterface()
    if ip :
      prop = ip.getProperty(name)
      if self.isvector(prop) : return map(None,prop.value())
      else                   : return prop.value()
    else :
      prop = self._optsvc.getProperties(self._name)
      for p in prop :
        if p.name() == name :
          value = p.value()
          if type(value) is str :
            if value[0] != '[' and not self.isnumber(value) : return value  
            else : return eval(value)
          else :  # then it is a vector<string>
            if len(value) > 0 and not self.isnumber(value[0]):
              return [eval(i) for i in value]
            else:
              return [i for i in value]
      return None
  def properties(self):
    dct = {}
    if self._ip :
      for p in self._ip.getProperties() : 
        if self.isvector(p) : dct[p.name()] = map(None, p.value())
        else                : dct[p.name()] = p.value()
    else :
      for p in  self._optsvc.getProperties( self._name ) : dct[p.name()] = p.value()
    return dct
  def isvector(self, prop) :
    return type(prop).__name__.find('vector') != -1
  def isnumber(self, num) :
    if type(num) is not str : return False
    for c in num :
      if c not in '0123456789.+-eE' : return False
    return True
  def name(self) :
    return self._name

class iService(iProperty) :
  def __init__(self, name, isvc = None ) :
    iProperty.__init__(self, name, isvc )
    if isvc : self.__dict__['_isvc'] = Interface(gbl.IService).cast(isvc)
    else    : self.__dict__['_isvc'] = None
  def retrieveInterface(self) :
    isvc = Helper.service(self._svcloc,self._name)
    if isvc : self.__init__(self._name, isvc)
  def initialize(self) :
    return self._isvc.initialize()
  def reinitialize(self) :
    return self._isvc.reinitialize()
  def finalize(self) :
    return self._isvc.finalize()

class iAlgorithm(iProperty) :
  def __init__(self, name, ialg = None ) :
    iProperty.__init__(self, name, ialg )
    if ialg : self.__dict__['_ialg'] = Interface(gbl.IAlgorithm).cast(ialg)
    else    : self.__dict__['_ialg'] = None
  def retrieveInterface(self) :
    ialg = Helper.algorithm(Interface(gbl.IAlgManager).cast(self._svcloc),self._name)
    if ialg : self.__init__(self._name, ialg)
  def initialize(self) :
    return self._ialg.initialize()
  def execute(self) :
    return self._ialg.execute()
  def finalize(self) :
    return self._ialg.finalize()

class iDataSvc(object) :
  def __init__(self,idp) :
    self._idp = Interface(gbl.IDataProviderSvc).cast(idp)
    self._idm = Interface(gbl.IDataManagerSvc).cast(idp)
  def registerObject(self, path, obj) :
    return self._idp.registerObject(path,obj)
  def unregisterObject(self, path) :
    return self._idp.unregisterObject(path)
  def retrieveObject(self, path) :
    return Helper.dataobject(self._idp, path)
  def __getitem__(self, path) :
    return Helper.dataobject(self._idp, path)
  def __setitem__(self, path, obj) :
    return self._idp.registerObject(path,obj)
  def __delitem__(self, path) :
    return self._idp.unregisterObject(path)
  def leaves(self, node=None ) :
    if not node : node = self.retrieveObject('')
    ll = gbl.std.vector('IRegistry*')()
    if type(node) is str : obj = self.retrieveObject(node)
    else                 : obj = node
    if self._idm.objectLeaves(node, ll).isSuccess() : return ll
  def dump(self, node=None) :
    if not node : 
      root = self.retrieveObject('')
      if root : node = root.registry()
      else : return
    print node.identifier()
    if node.object() :
      for l in self.leaves(node) : self.dump(l)
  def setRoot(self, name, obj): 
    return self._idm.setRoot(name,obj)
  def clearStore(self): 
    return self._idm.clearStore()

class iHistogramSvc(iDataSvc) :
  def __init__(self,ihs) :
    self._ihs = Interface(gbl.IHistogramSvc).cast(ihs)
    iDataSvc.__init__(self, ihs)
  def retrieve1D(self, path) :
    return Helper.histo1D(self._ihs, path)
  def retrieve2D(self, path) :
    return Helper.histo2D(self._ihs, path)
  def retrieve3D(self, path) :
    return Helper.histo3D(self._ihs, path)
  def retrieve(self,path):
    h = self.retrieve1D(path)
    if not h : h = self.retrieve2D(path)
    if not h : h = self.retrieve3D(path)
    return h
  def book(self, *args) :
    return apply(self._ihs.book,args)  
  
class EventSelector(iService):
  def __init__(self):
    iService.__init__(self, 'EventSelector', Helper.service(gbl.Gaudi.svcLocator(),'EventSelector'))
    self.__dict__['g'] = AppMgr()
  def open(self, stream, typ = 'ROOT', opt = 'READ' ):
    if typ == 'ROOT' :
      self.g.DLLs   += [ 'GaudiDb', 'GaudiRootDb']
      self.g.declSvcType('RootEvtCnvSvc','DbEventCnvSvc')
      self.g.service('RootEvtCnvSvc').DbType  = 'ROOT'
      self.g.createSvc('RootEvtCnvSvc')
      self.g.service('EventPersistencySvc').CnvServices = ['RootEvtCnvSvc']
    elif typ == 'POOL_ROOT':
      self.g.DLLs   += [ 'GaudiDb', 'GaudiPoolDb']
      cacsvc = self.g.service('PoolDbCacheSvc')
      cacsvc.Dlls += ['lcg_RootStorageSvc', 'lcg_XMLCatalog']
      cacsvc.OutputLevel = 4
      cacsvc.DomainOpts    = [ 'Domain[ROOT_All].CLASS_VERSION=2 TYP=int',
                               'Domain[ROOT_Key].CLASS_VERSION=2 TYP=int',
                               'Domain[ROOT_Tree].CLASS_VERSION=2 TYP=int' ]
      cacsvc.DatabaseOpts  = ['']
      cacsvc.ContainerOpts = ['']
      self.g.createSvc('PoolDbCacheSvc')
      for svc in [('PoolRootEvtCnvSvc',     'POOL_ROOT'),
                  ('PoolRootTreeEvtCnvSvc', 'POOL_ROOTTREE'),
                  ('PoolRootKeyEvtCnvSvc',  'POOL_ROOTKEY')]:
        self.g.declSvcType(svc[0], 'PoolDbCnvSvc')
        cnvsvc = self.g.service(svc[0])
        cnvsvc.DbType = svc[1]
        self.g.service('EventPersistencySvc').CnvServices += [svc[0]]
        self.g.createSvc(svc[0])
    self.g.service('EventDataSvc').RootCLID = 1
    if type(stream) != list : stream = [stream]
    cstream = ["DATAFILE=\'%s\' TYP=\'%s\' OPT=\'%s\'"%(s,typ,opt) for s in stream]
    self.Input = cstream
  def rewind(self):
    self.finalize()
    self.initialize()

class AppMgr(iService) :
  def __init__(self, outputlevel = 3, joboptions = None) :
    self.__dict__['_appmgr'] = gbl.Gaudi.createApplicationMgr()
    self.__dict__['_svcloc'] = gbl.Gaudi.svcLocator()
    self.__dict__['_algmgr'] = Interface(gbl.IAlgManager).cast(self._appmgr)
    self.__dict__['_evtpro'] = Interface(gbl.IEventProcessor).cast(self._appmgr)
    self.__dict__['_svcmgr'] = Interface(gbl.ISvcManager).cast(self._appmgr)
    self.__dict__['pyalgorithms'] = []
    iService.__init__(self, 'ApplicationMgr', self._appmgr )
    #------python specific initialization-------------------------------------
    if self._isvc.state() == 0 :  # Not yet configured
      if not joboptions : self.JobOptionsType = 'NONE'
      else : self.JobOptionsPath = joboptions
      self.OutputLevel = outputlevel
    # IB   self.configure()
    # IB self.service('MessageSvc').OutputLevel = outputlevel
    # IB if self._isvc.state() == 1 :  # Configured state
    # IB  self.initialize()
  def loaddict(self, dict) :
    try: 
      PyLCGDict.loadDict(dict)
    except: 
      if Helper.loadDynamicLib(dict) == 1 : return
      else: raise ImportError, 'Error loading dictionary library'
  def service(self, name) :
    svc = Helper.service( self._svcloc, name )
    return iService(name, svc )
  def declSvcType(self, svcname, svctype ) :
    self._svcmgr.declareSvcType(svcname, svctype)
  def createSvc(self, name ) :
    return Helper.service( self._svcloc, name, True )
  def services(self) :
    l = self._svcloc.getServices()
    nl = l.__class__(l)  # get a copy
    s = []
    for i in range(l.size()) :
      s.append(nl.front().name())
      nl.pop_front()
    return s
  def algorithm(self, name ) :
    alg = Helper.algorithm( self._algmgr, name )
    return iAlgorithm(name, alg )
  def algorithms(self) :
    l = self._algmgr.getAlgorithms()
    nl = l.__class__(l)  # get a copy
    s = []
    for i in range(l.size()) :
      s.append(nl.front().name())
      nl.pop_front()
    return s
  def property( self , name ) :
    if name in self.algorithms() : return self.algorithm( name )
    elif name in self.services() : return self.service(name )
    else :                         return iProperty( name )
  def datasvc(self, name) :
    svc = Helper.service( self._svcloc, name )
    return iDataSvc(svc)
  def evtsvc(self) :
    return self.datasvc('EventDataSvc')
  def detsvc(self) :
    return self.datasvc('DetectorDataSvc')
  def evtsel(self):
    if not hasattr(self,'_evtsel') : self.__dict__['_evtsel'] = EventSelector()
    return self._evtsel
  def histsvc(self, name='HistogramDataSvc') :
    svc = Helper.service( self._svcloc, name )
    return iHistogramSvc(svc)
  def partsvc(self ) :
    svc = Helper.service( self._svcloc, 'ParticlePropertySvc' )
    return Interface(gbl.IParticlePropertySvc).cast(svc)
  def readOptions(self, file) :
    optsvc = Interface(gbl.IJobOptionsSvc).cast(Helper.service(self._svcloc,'JobOptionsSvc'))
    return optsvc.readOptions(file)
  def addAlgorithm(self, alg) :
    self.pyalgorithms.append(alg)
    self.topAlg += [alg.name()]
  def removeAlgorithm(self, alg) :
    tmp = self.topAlg
    tmp.remove(alg.name())
    self.topAlg = tmp
    self.pyalgorithms.remove(alg)
  def config(self) :
    return self._appmgr.configure()
  def configure(self) :
    return self._appmgr.configure()
  def run(self, n) :
    if self._isvc : self._isvc.reinitialize()
    else          : raise RuntimeError, 'internal problem'
    self._evtpro.executeRun(n)
  def executeEvent(self) :
    self._evtpro.executeEvent()
  def execute(self) :
    self._evtpro.executeEvent()
  def exit(self) :
    self._appmgr.finalize()
    sc = self._appmgr.terminate()
    #if sc.isSuccess() : os._exit(0)
    #else              : os._exit(1)

def getComponentProperties( name ):
  if Helper.loadDynamicLib(name) != 1 :
    raise ImportError,  'Error loading component library '+ name
  properties = {}
  g = AppMgr(outputlevel=7) 
  svcloc    = gbl.Gaudi.svcLocator()
  dummysvc  = gbl.Service('DummySvc',svcloc)
  factories = gbl.FactoryTable.instance().getEntries()
  for i in range(factories.size()) :
    factory = factories.front()
    factories.pop_front()
    if    Interface(gbl.IAlgFactory).cast(factory) : ctype = 'Algorithm'
    elif  Interface(gbl.ISvcFactory).cast(factory) : ctype = 'Service'
    elif  Interface(gbl.IToolFactory).cast(factory) : ctype = 'AlgTool'
    else :  ctype = 'Unkown'
    cname = factory.ident().split()[-1]
    if ctype in ('Algorithm','Service', 'AlgTool') :
      try :
        if ctype == 'AlgTool' :
          obj = factory.instantiate(dummysvc)
        else :
          obj = factory.instantiate(svcloc)
      except RuntimeError, text :
        print 'Error instantiating', cname, ' from ', name
        print text
        continue
      prop = iProperty('dummy', obj)
      properties[cname] = [ctype, prop.properties()]
      try:  obj.release()
      except: pass
  return properties


#----PyAlgorithm----------------------------------
PyAlgorithm = gbl.GaudiPython.PyAlgorithmWrap
PyAlgorithm_init_orig = PyAlgorithm.__init__
def PyAlgorithm_init(self,name=None) :
  if not name : name = self.__class__.__name__
  PyAlgorithm_init_orig(self, self, name)
  self._svcloc = gbl.Gaudi.svcLocator()
  algmgr = Interface(gbl.IAlgManager).cast(self._svcloc)
  sc = algmgr.addAlgorithm(self)
  if sc.isFailure() : raise RuntimeError, 'Unable to add Algorithm'
def PyAlgorithm_execute(self) : return 1
def PyAlgorithm_initialize(self) : return 1
def PyAlgorithm_finalize(self) : return 1
PyAlgorithm.__init__   = PyAlgorithm_init
PyAlgorithm.initialize = PyAlgorithm_initialize
PyAlgorithm.finalize   = PyAlgorithm_finalize
PyAlgorithm.execute    = PyAlgorithm_execute

#----Adding extra functionality to C++ raw classes
def printHisto1D(h) :
  x = h.axis()
  return  'Histogram 1D "%s" %d bins [%f,%f]' % (h.title(), x.bins(), x.lowerEdge(), x.upperEdge())
def contentsHisto1D(h) :
  x = h.axis()
  return  map(h.binEntries, range(x.bins()))
def printHisto2D(h) :
  x,y = h.xAxis(),h.yAxis()
  return  'Histogram 2D "%s" %d xbins [%f,%f], %d ybins [%f,%f]' % \
  (h.title(), x.bins(), x.lowerEdge(), x.upperEdge(), y.bins(), y.lowerEdge(), y.upperEdge() )
def printStatusCode(s) :
  if s.isSuccess() : return 'SUCCESS'
  else             : return 'FAILURE'

gbl.AIDA.IHistogram1D.__repr__ = printHisto1D
gbl.AIDA.IHistogram1D.contents = contentsHisto1D
gbl.AIDA.IHistogram2D.__repr__ = printHisto2D
gbl.StatusCode.__repr__ = printStatusCode
gbl.StatusCode.SUCCESS = 1 
gbl.StatusCode.FAILURE = 0 
