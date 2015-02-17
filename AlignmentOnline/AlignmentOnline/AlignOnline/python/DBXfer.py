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
    if 'Velo' in listOfCondToWrite:
      self.addXmlWriter( alg, 'Velo', 'Global', [0,1] )
      self.addXmlWriter( alg, 'Velo','Modules', [2] )
      self.addXmlWriter( alg, 'Velo','Detectors', [4] )
    if 'TT' in listOfCondToWrite:
      self.addXmlWriter( alg, 'TT','Detectors', [0,1,2] )
      self.addXmlWriter( alg, 'TT','Modules', [3] )
      self.addXmlWriter( alg, 'TT','Sensors', [4,5] )
    if 'IT' in listOfCondToWrite:
      self.addXmlWriter( alg, 'IT','Detectors', [] )
    if 'OT' in listOfCondToWrite:
      self.addXmlWriter( alg, 'OT','Elements', [] )
    if 'Muon' in listOfCondToWrite:
      #self.addXmlWriter( alg, 'Muon','Detectors', [] )
      self.addXmlWriter( alg, 'Muon','Global', [0,1,2] )
    if 'Ecal' in listOfCondToWrite:
      self.addXmlWriter( alg, 'Ecal','alignment', [] )


from Gaudi.Configuration import *
from Configurables import LHCbAlgsTests__TestTimeDecoderOdin as TimeDecoder
from Configurables import (RunChangeHandlerSvc, DBXferAlg ,XmlCnvSvc, DDDBConf, CondDB, EventClockSvc, FakeEventTime)
import RunOption
import All
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
cdb.RunChangeHandlerConditions=All.ConditionMap
cdb.EnableRunChangeHandler = True
cdb.UseDBSnapshot = True
cdb.Tags = { "DDDB" : RunOption.DDDBTag,
                "LHCBCOND" : RunOption.CondDbTag,
                "ONLINE" : 'fake'}
cdb.IgnoreHeartBeat = True
dbxalg = DBXferAlg()
dbxalg.RunNumber = RunOption.RunNumber
dbxalg.RunStartTime = RunOption.RunStartTime*1000000000
dbxalg.OnlineXmlDir = RunOption.OutputDirectory
wrconf = WriterConf("wconf",["Velo","IT","TT","OT"])
wrconf.CondFilePrefix = RunOption.OutputDirectory
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
# from Configurables import XmlParserSvc
# RunChangeHandlerSvc(OutputLevel=VERBOSE)
# MessageSvc(OutputLevel=1)
EventDataSvc(ForceLeaves = True)
from GaudiPython.Bindings import AppMgr
Gaudi=AppMgr()
Gaudi.run(1)
