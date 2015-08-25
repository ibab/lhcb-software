import sys
args=sys.argv[:]
from Configurables import ( LHCbConfigurableUser)
class WriterConf:#(LHCbConfigurableUser):
  __slots__ = {
#       "Sequencer" : GaudiSequencer("TAlignmentSequencer")          # the sequencer to add algorithms to
#     , "TrackSelections"              : [ GoodLongTracks() ]        # input track selections for alignment
#     , "ParticleSelections"           : []                          # input particles for alignment
#     , "PVSelection"                  : None                        # primary vertices for alignment
#     , "ElementsToAlign"              : []                          # Elements to align
#     , "UseLocalFrame"                : True                        # Use local frame?
#     , "NumIterations"                : 1                           # Number of iterations
#     , "TrackLocation"                : ""                          # track container to be used for alignment
#     , "VertexLocation"               : ""                          # Location of input vertex list
#     , "DimuonLocation"               : ""                          # Location of input vertex list
#     , "ParticleLocation"             : ""                          # Location of input vertex list
#     , "UseCorrelations"              : True                        # Correlations
#     , "Constraints"                  : []                          # Specifies 'exact' (lagrange) constraints
#     , "UseWeightedAverageConstraint" : False                       # Weighted average constraint
#     , "MinNumberOfHits"              : 10                          # Min number of hits per element
#     , "Chi2Outlier"                  : 10000                       # Chi2 cut for outliers
#     , "UsePreconditioning"           : True                        # Pre-conditioning
#     , "SolvTool"                     : "DiagSolvTool"              # Solver to use
#     , "EigenValueThreshold"          : -1                          # Eigenvalue threshold for cutting out weak modes
     "WriteCondSubDetList"          : []                          # List of sub-detectors for which to write out the conditions
    , "CondFilePrefix"               : "xml/"                      # Prefix for xml file names
    , "VeloTopLevelElement"          : "/dd/Structure/LHCb/BeforeMagnetRegion/Velo"
    , "TTTopLevelElement"            : "/dd/Structure/LHCb/BeforeMagnetRegion/TT"
    , "ITTopLevelElement"            : "/dd/Structure/LHCb/AfterMagnetRegion/T/IT"
    , "OTTopLevelElement"            : "/dd/Structure/LHCb/AfterMagnetRegion/T/OT"
    , "MuonTopLevelElement"          : "/dd/Structure/LHCb/DownstreamRegion/Muon"
    , "EcalTopLevelElement"          : "/dd/Structure/LHCb/DownstreamRegion/Ecal"
    , "Precision"                    : 10                          # Set precision for conditions
#     , "OutputLevel"                  : INFO                        # Output level
#     , "LogFile"                      : "alignlog.txt"              # log file for kalman type alignment
#     , "Incident"                     : ""                          # name of handle to be executed on incident by incident server
#     , "UpdateInFinalize"             : True
#     , "OutputDataFile"               : ""
    , "DatasetName"                  : "Unknown"
#     , "OnlineMode"                   : False
#     , "OnlineAligWorkDir"            : "/group/online/AligWork/running"
  }
  def getProp( self, name ) :
      if hasattr (self, name) :
          return getattr( self, name)
      else:
          return self.getDefaultProperties()[name]

  def __init__(self,name,dets):
    ks = self.__slots__.keys()
    for k in ks:
      setattr(self,k,self.__slots__[k])
    setattr(self,"WriteCondSubDetList", dets)
#     self._name = name

  def addXmlWriter(self, alg, subdet, condname, depths ) :
    from Configurables import WriteAlignmentConditionsTool
    import getpass
    name = 'Write' + subdet + condname + 'ToXML'
    alg.addTool( WriteAlignmentConditionsTool, name )
    handle = getattr ( alg , name )
    handle.topElement = self.getProp( subdet + 'TopLevelElement' )
    handle.precision = self.getProp( "Precision" )
    handle.depths = depths
#    handle.outputFile = self.getProp('CondFilePrefix') + 'Conditions/' + subdet + '/Alignment/' +condname + '.xml'
    handle.outputFile = self.getProp('CondFilePrefix') + '/'+subdet + '/Alignment/' +condname + '.xml'
    handle.author = getpass.getuser()
    handle.desc = self.getProp('DatasetName')
    alg.XmlWriters.append("WriteAlignmentConditionsTool/" + name)

  def addXmlWriters(self,alg):
    print '================ Configuring XML writer for offline alignment ====================='
    alg.XmlWriters = []
    listOfCondToWrite = self.getProp( "WriteCondSubDetList" )
    if 'velo' in listOfCondToWrite:
      self.addXmlWriter( alg, 'Velo', 'Global', [0,1] )
      self.addXmlWriter( alg, 'Velo','Modules', [2,4] )
    if 'tt' in listOfCondToWrite:
      self.addXmlWriter( alg, 'TT','Global', [0,1,2] )
      self.addXmlWriter( alg, 'TT','Modules', [3,4] )
    if 'it' in listOfCondToWrite:
      self.addXmlWriter( alg, 'IT','Global', [0,1,2] )
      self.addXmlWriter( alg, 'IT','Modules', [3,4] )
    if 'ot' in listOfCondToWrite:
      self.addXmlWriter( alg, 'OT','Global', [0,1,2,3] )
      self.addXmlWriter( alg, 'OT','Modules', [4] )
      from Configurables import WriteOTCalibrationsTool
      alg.addTool( WriteOTCalibrationsTool, "WriteOTCalibrationsTool")
      alg.XmlWriters.append("WriteOTCalibrationsTool")
      alg.WriteOTCalibrationsTool.Directory = self.getProp('CondFilePrefix')
      alg.WriteOTCalibrationsTool.WriteGlobalCalibration = True
      alg.WriteOTCalibrationsTool.Precision = 6
    if 'muon' in listOfCondToWrite:
      #self.addXmlWriter( alg, 'Muon','Detectors', [] )
      self.addXmlWriter( alg, 'Muon','Global', [0,1,2] )
    if 'ecal' in listOfCondToWrite:
      self.addXmlWriter( alg, 'Ecal','alignment', [] )
    if ('rich1' in listOfCondToWrite) or ('rich2' in listOfCondToWrite):
      if "WriteRichCalibrationsTool" not in alg.XmlWriters:
        from Configurables import WriteRichCalibrationsTool
        alg.addTool( WriteRichCalibrationsTool, "WriteRichCalibrationsTool")
        alg.XmlWriters.append("WriteRichCalibrationsTool")
        alg.WriteRichCalibrationsTool.Directory = self.getProp('CondFilePrefix')



from Gaudi.Configuration import *
from Configurables import LHCbAlgsTests__TestTimeDecoderOdin as TimeDecoder
from Configurables import (RunChangeHandlerSvc, DBXferAlg ,XmlCnvSvc, DDDBConf, CondDB, EventClockSvc, FakeEventTime)
import RunOption
import HLT2Params
import sys
import os
def MakeTick(root,runnr):
  pth = root+"/onl/Conditions/Online/LHCb"
  if not os.access(pth, os.F_OK):
    try:
      os.makedirs(pth,0777)
    except:
      print "Cannot create directory "+pth
  else:
    print "Path "+pth+" already exists"
  xmlf = open(pth+"/RunStamp.xml","w")
  xmlf.write('<?xml version="1.0" encoding="ISO-8859-1"?>\n')
  xmlf.write('<!DOCTYPE DDDB SYSTEM "conddb:/DTD/structure.dtd">\n')
  xmlf.write('<DDDB>\n')
  xmlf.write('<condition name="RunStamp"> <param name="RunNumber" type="int">'
             +str(runnr)
             +'</param> </condition>\n')
  xmlf.write('</DDDB>\n')
  xmlf.close()

excludeFiles =[]
excludeFiles.append('online.xml')
try:
  import CondMap
except:
  print "Cannot Import CondMap. Trying All"
  try:
    import All as CondMap
  except:
    print "Cannot Import All either. Exiting with status code 2"
    sys.exit(2)
ks = CondMap.ConditionMap.keys()
dets = []
for i in range(len(ks)):
  k = ks[i]
  excl=False;
  for j in range(len(excludeFiles)):
    if k.find(excludeFiles[j]) >= 0:
      excl = True
      break
  if excl :
    continue
  detxml = k[k.rfind('/')+1:]
  det = detxml[:detxml.rfind('.xml')]
  det = det.lower()
  dets.append(det)
print "Moving to Offline DB the following detector Data ",dets
ecs = EventClockSvc()
ecs.InitialTime = RunOption.RunStartTime*1000000000
ecs.addTool(FakeEventTime,"EventTimeDecoder")
ecs.EventTimeDecoder.StartTime = ecs.InitialTime
ecs.EventTimeDecoder.TimeStep = 10

#xmlCnvSvc = XmlCnvSvc(AllowGenericConversion = True)
DDDBConf()
#detDataSvc = DetectorDataSvc()
#DetectorPersistencySvc( CnvServices = [ xmlCnvSvc ] )
cdb = CondDB()
cdb.RunChangeHandlerConditions=CondMap.ConditionMap
cdb.EnableRunChangeHandler = True
cdb.EnableRunStampCheck=False
cdb.UseDBSnapshot = True
cdb.Tags = { "DDDB" : RunOption.DDDBTag,
                "LHCBCOND" : RunOption.CondDbTag,
                "ONLINE" : 'fake'}
cdb.IgnoreHeartBeat = True
dbxalg = DBXferAlg()
dbxalg.RunNumber = RunOption.RunNumber
dbxalg.RunStartTime = RunOption.RunStartTime*1000000000
dbxalg.OnlineXmlDir = RunOption.OutputDirectory
wrconf = WriterConf("wconf",dets)#["Velo","IT","TT","OT"])
wrconf.CondFilePrefix = RunOption.OutputDirectory+"/offl/Conditions/Online"
from Configurables import WriteMultiAlignmentConditionsTool
xmlwriter = WriteMultiAlignmentConditionsTool()#"AlignWriterTool")
wrconf.addXmlWriters(xmlwriter)
app = ApplicationMgr()
app.TopAlg = [ dbxalg ]
app.addTool(xmlwriter)
#app.ExtSvc += [ detDataSvc]#, rch ]
app.EvtSel = "NONE"
app.EvtMax = 1
app.OutputLevel = INFO
RunStart = HLT2Params.RunStartTime*1000000000
RunEnd =  (HLT2Params.RunStartTime+HLT2Params.RunDuration)*1000000000
print args
if len(args)>1:
  lstrun = args[1]
  if lstrun == 'LastRun':
    RunEnd = None
# from Configurables import XmlParserSvc
#RunChangeHandlerSvc(OutputLevel=VERBOSE)
#MessageSvc(OutputLevel=1)
EventDataSvc(ForceLeaves = True)
from GaudiPython.Bindings import AppMgr
Gaudi=AppMgr()
print "===================== Initializing the XML conversion ========================"
stat = Gaudi.initialize()
print "===================== Running the XML conversion ========================"
stat = Gaudi.run(1)
print "============= Status :",stat
if stat.isFailure():
  sys.exit(44)
print "============= Gaudi.ReturnCode :",Gaudi.ReturnCode
if Gaudi.ReturnCode != 0:
  sys.exit(44)
print "===================== Stopping the XML conversion ========================"
stat = Gaudi.stop()
print "============= Status :",stat
if stat.isFailure():
  sys.exit(44)
print "===================== Finalizing the XML conversion ========================"
stat = Gaudi.finalize()
print "============= Status :",stat
if stat.isFailure():
  sys.exit(44)
print "===================== Exiting the XML conversion ========================"
stat = Gaudi.exit()
print "============= Status :",stat
if stat.isFailure():
  sys.exit(44)
print "===================== Updating the Database ========================"
import CondDBUI
import CondDBUI.Admin
DBString = "CondDBOnline(owner)/ONLINE" # use the central Oracle Database...
####DBString = "sqlite_file:./CONDDB.db/ONLINE"
db = CondDBUI.CondDB(DBString, create_new_db = False, readOnly=False)
import time
if RunEnd == None:
  etime =  HLT2Params.RunStartTime+86400*365*100
else:
  etime = RunEnd/1000000000
print "Operating on Database "+DBString
print "IoV is ["+str(RunStart)+","+str(RunEnd)+"], ["+time.ctime(HLT2Params.RunStartTime)+","+time.ctime(etime)+"]"
status = 0
status = CondDBUI.Admin.MakeDBFromFiles(RunOption.OutputDirectory+"/offl", db,
                                   includes = [], excludes = [],
                                   verbose = True,
                                   since = RunStart,
                                   until = RunEnd,
                                   writeDuplicate = False
                                    )
RunEnd =  (HLT2Params.RunStartTime+HLT2Params.RunDuration)*1000000000
print "Reopening Database in Read-Only mode"
db.closeDatabase()
db = CondDBUI.CondDB(DBString, create_new_db = False, readOnly=True)
print "Checking Database Contents against entered files"
Fails = CondDBUI.Admin.CompareDBToFiles(RunOption.OutputDirectory+"/offl", db,
                                   includes = [], excludes = [],
                                   verbose = True,
                                   since = RunStart,
                                   until = RunEnd
                                    )
from pprint import pprint
if Fails:
  print "Readback check on DB failed\n"
  pprint(Fails)
  sys.exit(44)
MakeTick(RunOption.OutputDirectory,RunOption.RunNumber)
RunEnd =  (HLT2Params.RunStartTime+HLT2Params.RunDuration)*1000000000
print "Adding the RunTick to the Database"
print "IoV is ["+str(RunStart)+","+str(RunEnd)+"]"+", ["+time.ctime(HLT2Params.RunStartTime)+","+time.ctime(RunEnd/1000000000)+"]"
db = CondDBUI.CondDB(DBString, create_new_db = False, readOnly=False)
status = CondDBUI.Admin.MakeDBFromFiles(RunOption.OutputDirectory+"/onl", db,
                                   includes = [], excludes = [],
                                   verbose = True,
                                   since = RunStart,
                                   until = RunEnd,
                                   writeDuplicate = False
                                    )
db.closeDatabase()
db = CondDBUI.CondDB(DBString, create_new_db = False, readOnly=True)
print "Checking the Run Tick"
Fails = CondDBUI.Admin.CompareDBToFiles(RunOption.OutputDirectory+"/onl", db,
                                   includes = [], excludes = [],
                                   verbose = True,
                                   since = RunStart,
                                   until = RunEnd
                                    )
if Fails:
  print "Readback check on DB failed\n"
  pprint(Fails)
  sys.exit(44)
db.closeDatabase()
DBString = "CondDBOnline/ONLINE" # use the central Oracle Database...
db = CondDBUI.CondDB(DBString, create_new_db = False, readOnly=True)
print "Checking Database Contents against entered files with account "+DBString
Fails = CondDBUI.Admin.CompareDBToFiles(RunOption.OutputDirectory+"/offl", db,
                                   includes = [], excludes = [],
                                   verbose = True,
                                   since = RunStart,
                                   until = RunEnd
                                    )
if Fails:
  print "Readback check on DB failed\n"
  pprint(Fails)
  sys.exit(44)
print "Checking The Run Tick contents with account "+DBString
Fails = CondDBUI.Admin.CompareDBToFiles(RunOption.OutputDirectory+"/onl", db,
                                   includes = [], excludes = [],
                                   verbose = True,
                                   since = RunStart,
                                   until = RunEnd
                                    )
if Fails:
  print "Readback check on DB failed\n"
  pprint(Fails)
  sys.exit(44)

print "===================== Updated the Database ======================== Status = ",status
