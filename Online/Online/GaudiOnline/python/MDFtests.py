import os, sys
import Gaudi.Configuration as CFG
import Configurables as Configs
ApplicationMgr = CFG.ApplicationMgr

base = '.'# os.getcwd()

#------------------------------------------------------------------------------------------------
def printConfiguration():
  "Print the configuration setup."
  import pprint
  pprint.pprint(CFG.configurationDict())

#------------------------------------------------------------------------------------------------
def end_config(print_config=True):
  import GaudiPython
  gaudi = GaudiPython.AppMgr()
  if print_config: printConfiguration()
  return gaudi

#------------------------------------------------------------------------------------------------
def _run(app,prt=True):
  return (app,end_config(prt))

#------------------------------------------------------------------------------------------------
def evtDataSvc():
  svc                  = CFG.EventDataSvc()
  svc.RootCLID         = 1
  svc.ForceLeaves      = 1
  svc.EnableFaultHandler = True
  return svc

#------------------------------------------------------------------------------------------------
def setData(test_castor=False):
  sel = CFG.EventSelector()
  sel.PrintFreq              = 200
  sel.Input                  = [
     "DATA='LFN:/user/frank/markus/mdf_1.dat' SVC='LHCb::MDFSelector'"
    ,"DATA='PFN:root:file://mdfData_0.dat'        SVC='LHCb::MDFSelector'"
    ,"DATA='root:file:mdfData_2.dat'              SVC='LHCb::MDFSelector'"
    ,"DATA='PFN:root:mdfData_3.dat'               SVC='LHCb::MDFSelector'"
    ,"DATA='root:mdfData_4.dat'                   SVC='LHCb::MDFSelector'"
    ,"DATA='PFN:root:file://"+base+"/mdfData_5.dat'        SVC='LHCb::MDFSelector'"
    ,"DATA='PFN:root:file:"+base+"/mdfData_6.dat'          SVC='LHCb::MDFSelector'"
    ,"DATA='PFN:root:"+base+"/mdfData_7.dat'               SVC='LHCb::MDFSelector'"
    ,"DATA='root:file://"+base+"/mdfData_8.dat'            SVC='LHCb::MDFSelector'"
    ,"DATA='root:file:"+base+"/mdfData_9.dat'              SVC='LHCb::MDFSelector'"
    ,"DATA='"+base+"/mdfData_10.dat'                       SVC='LHCb::MDFSelector'"
    ]
  if test_castor:
    sel.Input                 += [
       "DATA='root:rfio:/castor/cern.ch/user/f/frankb/mdfData_11.dat' SVC='LHCb::MDFSelector'"
      ,"DATA='root:castor:/castor/cern.ch/user/f/frankb/mdfData_12.dat' SVC='LHCb::MDFSelector'"
      ,"DATA='castor:/castor/cern.ch/user/f/frankb/mdfData_13.dat'      SVC='LHCb::MDFSelector'"
       ]
  return sel
  
#------------------------------------------------------------------------------------------------
def execute(func, numEvt):
  g = _run(func(),True)[1]
  g.start()
  print '---> Start processing events '
  g.run(numEvt)
  print '---> Finished processing events '
  g.stop()
  g.finalize()
  sys.exit(0)

#------------------------------------------------------------------------------------------------
def mdfCheck():
  app               = ApplicationMgr()
  app.AppName       = ''
  app.HistogramPersistency = "NONE"
  svc               = evtDataSvc()
  dmp               = Configs.LHCb__RawEventTestDump('Dump')
  dmp.CheckData     = 1
  dmp.CheckData     = 0
  dmp.DumpData      = 0
  dmp.FullDump      = 0
  dmp.OutputLevel   = 3
  exp               = CFG.StoreExplorerAlg('Explorer') 
  exp.Load          = 1
  exp.PrintFreq     = 0.00021
  exp.AccessForeign = True
  exp.OutputLevel   = 3
  app.TopAlg       += [dmp,exp]
  svc  = CFG.EventPersistencySvc()
  svc.CnvServices += [Configs.LHCb__RawDataCnvSvc('RawDataCnvSvc')]
  CFG.FileCatalog().Catalogs = ['xmlcatalog_file:Cnew.xml','xmlcatalog_file:C1.xml']
  CFG.IODataManager().OutputLevel = 1  
  CFG.IODataManager().AgeLimit = 1

def addWriter(name, fname, compress=None, checksum=None, md5=None):
  # To create a MDF file using the MDF writer:
  app = ApplicationMgr()
  wr  = Configs.LHCb__MDFWriter(name)
  wr.Connection       = fname
  if compress is not None:
    wr.Compress         = compress
  if checksum is not None:
    wr.ChecksumType     = checksum
  if md5 is not None:
    wr.GenerateMD5      = md5
  app.OutStream += [wr]
  return wr

def print_header(typ,name):
  print '+----------------------------------------------------------------------------+'
  print '|      Executing %-12s  %-25s                     |'%(typ,name,)
  print '+----------------------------------------------------------------------------+'

#------------------------------------------------------------------------------------------------
def _createMDF(test_castor=None):
  print_header('MDF','CreateMDF')
  app  = ApplicationMgr()
  app.EvtSel = 'NONE'
  app.TopAlg = [Configs.LHCb__RawEventTestCreator('Creator')]
  mdfCheck()
  # To create a MDF file using the standard Gaudi output stream:
  wr = CFG.OutputStream('Writer_0')
  wr.Output   = "DATAFILE='PFN:root:file://mdfData_0.dat' SVC='LHCb::RawDataCnvSvc' OPT='REC'"
  wr.ItemList = ["/Event#1"]
  app.OutStream += [wr]
  addWriter('Writer_1','LFN:/user/frank/markus/mdf_1.dat',0,0,True)
  addWriter('Writer_2','LFN:/user/frank/markus/mdf_2.dat',0,1,True)
  addWriter('Writer_3','PFN:root:mdfData_3.dat',2,1,True)
  addWriter('Writer_4','root:mdfData_4.dat',2,0,True)
  addWriter('Writer_5','PFN:root:file://'+base+'/mdfData_5.dat')
  addWriter('Writer_6','PFN:root:file://'+base+'/mdfData_6.dat')
  addWriter('Writer_7','PFN:'+base+'/mdfData_7.dat')
  addWriter('Writer_8','root:file://'+base+'/mdfData_8.dat')
  addWriter('Writer_9','root:file:'+base+'/mdfData_9.dat')
  addWriter('Writer_10',''+base+'/mdfData_10.dat')
  if test_castor is not None:
    addWriter('Writer_11','root:rfio:/castor/cern.ch/user/f/frankb/mdfData_11.dat')
    addWriter('Writer_12','root:castor:/castor/cern.ch/user/f/frankb/mdfData_12.dat')
    addWriter('Writer_13','castor:/castor/cern.ch/user/f/frankb/mdfData_13.dat')
  CFG.MessageSvc().OutputLevel = 3
  return app

#------------------------------------------------------------------------------------------------
def _readMDF(test_castor=None):
  print_header('MDF','ReadMDF')
  app  = ApplicationMgr()
  setData(test_castor)
  mdfCheck()
  return app

def _createMIF(test_castor=None):
  print_header('MDF','CreateMIF')
  app  = ApplicationMgr()
  setData(test_castor)
  mdfCheck()
  wr = Configs.LHCb__MIFWriter('Writer')
  wr.Connection = 'file://mifData.dat'
  app.TopAlg += [wr]
  return app

def _readMIF(test_castor=None):
  print_header('MDF','ReadMIF')
  mdfCheck()
  CFG.EventSelector().PrintFreq              = 1000
  CFG.EventSelector().Input                  = [ "DATA='file://mifData.dat' SVC='LHCb::MIFSelector'" ]
  CFG.IODataManager().AgeLimit               = 5
  return ApplicationMgr()
  
def _createPOOL(test_castor=None):
  print_header('MDF','CreatePOOL')
  app  = ApplicationMgr()
  setData(test_castor)
  mdfCheck()
  mini = CFG.OutputStream('POOLMini')
  mini.ItemList          = ["/Event#1", "/Event/DAQ#1"]
  mini.Output            = "DATAFILE='PFN:mdfPOOL.dat' TYP='POOL_ROOTTREE' OPT='RECREATE'"
  app.OutStream  += [mini]
  CFG.importOptions('$GAUDIPOOLDBROOT/options/GaudiPoolDbRoot.opts')
  cache = CFG.PoolDbCacheSvc()
  cache.OutputLevel = 1
  return app

def _readPOOL(test_castor=None):
  print_header('MDF','ReadPOOL')
  mdfCheck()
  CFG.importOptions('$GAUDIPOOLDBROOT/options/GaudiPoolDbRoot.opts')
  sel = CFG.EventSelector()
  sel.PrintFreq  = 200
  sel.Input      = ["DATA='PFN:mdfPOOL.dat' TYP='POOL_ROOTTREE' OPT='OLD'"]
  CFG.IODataManager().AgeLimit = 5
  return ApplicationMgr()
  
if __name__ == "__main__":
  tt = sys.argv[1]
  res = 1
  fun = 'execute(_'+tt+',1000000)'
  if tt == 'createMDF':
    res =  execute(_createMDF,2000)
    sys.exit(0)
  res =  eval('execute(_'+tt+',1000000)')
  sys.exit(0)
