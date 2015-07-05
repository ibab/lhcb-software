"""
     Run Data Quality application in the online environment

     @author M.Frank
"""
__version__ = "$Id: DataQuality.py,v 1.25 2010/11/09 12:20:55 frankb Exp $"
__author__  = "Markus Frank <Markus.Frank@cern.ch>"

import os, sys
import Configurables as Configs
import Gaudi.Configuration as Gaudi
import GaudiKernel
from Configurables import GaudiSequencer, RichPIDQCConf
from Configurables import MuEffMonitor, TrackMasterExtrapolator, MuonFastPosTool
from Configurables import CondDB, EventPersistencySvc, HistogramPersistencySvc
from Configurables import EventLoopMgr
from GaudiKernel.ProcessJobOptions import PrintOff,InstallRootLoggingHandler,logging

#PrintOff(999)
InstallRootLoggingHandler(level=logging.CRITICAL)

GaudiKernel.ProcessJobOptions._parser._parse_units(os.path.expandvars("$STDOPTS/units.opts"))
GaudiKernel.ProcessJobOptions.printing_level=999
requirement = None

def dummy(*args,**kwd): pass

MSG_VERBOSE = 1
MSG_DEBUG   = 2
MSG_INFO    = 3
MSG_WARNING = 4
MSG_ERROR   = 5
MSG_FATAL   = 6
MSG_ALWAYS  = 7

def configureForking(appMgr):
  import OnlineEnv
  from Configurables import LHCb__CheckpointSvc
  numChildren = os.sysconf('SC_NPROCESSORS_ONLN')
  if os.environ.has_key('NBOFSLAVES'):
    numChildren = int(os.environ['NBOFSLAVES'])

  print '++++ configure Brunel for forking with ',numChildren,' children.'
  sys.stdout.flush()
  forker = LHCb__CheckpointSvc("CheckpointSvc")
  forker.NumberOfInstances   = numChildren
  forker.Partition           = OnlineEnv.PartitionName
  forker.TaskType            = os.environ['TASK_TYPE']
  forker.UseCores            = False
  forker.ChildSessions       = False
  forker.FirstChild          = 0
  # Sleep in [ms] for each child in batches of 10:
  forker.ChildSleep          = 500;
  forker.UtgidPattern        = "%UTGID%02d";
  forker.PrintLevel          = 3  # 1=MTCP_DEBUG 2=MTCP_INFO 3=MTCP_WARNING 4=MTCP_ERROR
  forker.OutputLevel         = 4  # 1=VERBOSE 2=DEBUG 3=INFO 4=WARNING 5=ERROR 6=FATAL
  appMgr.ExtSvc.insert(0,forker)
  sys.stdout.flush()

#============================================================================================================
def patchBrunel():
  """
        Instantiate the options to run Brunel with raw data

        @author M.Frank
  """
  import GaudiConf.DstConf
  import Brunel.Configuration
  import OnlineEnv

  brunel = Brunel.Configuration.Brunel()
  brunel.OnlineMode = True
  try:
    brunel.DDDBtag    = OnlineEnv.DDDBTag
  except:
    print "DDDBTag not found, use default"

  try:
    brunel.CondDBtag = OnlineEnv.CondDBTag
  except:
    print "CondDBTag not found, use default"


  ##print '[ERROR]', OnlineEnv.DDDBTag, OnlineEnv.CondDBTag

  conddb = CondDB()
  conddb.IgnoreHeartBeat = True
  #
  # Adjust to pickup the proper online conditions
  #
  import ConditionsMap
  conddb.setProp('RunChangeHandlerConditions', ConditionsMap.RunChangeHandlerConditions)
  conddb.setProp('EnableRunChangeHandler', True)

  brunel.DataType = "2015"
  brunel.UseDBSnapshot = True  # Try it
  brunel.WriteFSR = False     # This crashes Jaap's stuff

  conddb = CondDB()
  conddb.Online = True
  #
  # Adjust to pickup the proper online conditions from ConditionsMap
  #
  conddb.RunChangeHandlerConditions = ConditionsMap.RunChangeHandlerConditions
  conddb.setProp('EnableRunChangeHandler', True)

  # Enabled data-on-demand
  Gaudi.ApplicationMgr().ExtSvc += [ "DataOnDemandSvc" ]

  ################################################################################
  #                                                                              #
  # Set up PID monitoring sequence                                               #
  #                                                                              #
  ################################################################################

  # The sequencer to run all the monitoring in
  seq = GaudiSequencer("PIDMoniSeq")

  pidSeq = GaudiSequencer("RichPIDSelections")

  brunel.setOtherProps( RichPIDQCConf(), ['OutputLevel','Context'] )
  RichPIDQCConf().setProp("CalibSequencer",pidSeq)

  seq.Members += [ pidSeq ]

  ################################################################################
  #                                                                              #
  # Configure the muon efficiency monitor                                        #
  #                                                                              #
  ################################################################################

  muEffMoni = MuEffMonitor("MuEffMonitor")

  muEffMoni.addTool(TrackMasterExtrapolator, name = "MuEffExtrap")
  muEffMoni.Extrapolator = muEffMoni.MuEffExtrap

  muEffMoni.MuEffExtrap.ApplyMultScattCorr  = True
  muEffMoni.MuEffExtrap.ApplyEnergyLossCorr = True
  muEffMoni.MuEffExtrap.MaterialLocator     = "SimplifiedMaterialLocator" 
  muEffMoni.MuEffExtrap.OutputLevel         = 6

  muEffMoni.nSigma1X = [11., 8., 7., 7.]
  muEffMoni.nSigma1Y = [ 6., 5., 5., 5.]
  muEffMoni.nSigmaX  = [ 5., 5., 5., 5.]
  muEffMoni.nSigmaY  = [ 5., 5., 5., 5.]

  muEffMoni.RequiredStations = 4
  muEffMoni.MomentumCut      = 3000.0
  muEffMoni.nSigmaFidVol     = 3.0

  muEffMoni.UseCalo  = True
  muEffMoni.EecalMax = 1500.0
  muEffMoni.EhcalMax = 5000.0
  muEffMoni.EhcalMin = 1000.0
        
  muEffMoni.Chi2ProbTrMin =  0.01
  muEffMoni.Chi2MuMin     = 10.0
  muEffMoni.nSigmaXother  =  2.0
  muEffMoni.nSigmaYother  =  2.0
  
  muEffMoni.HistoLevel = "OfflineFull"

  seq.Members += [ muEffMoni ]
  
  ################################################################################
  #                                                                              #
  # Configure the muon pid monitor                                               #
  #                                                                              #
  ################################################################################
  
  from Configurables          import MuIDMonitor, CombineParticles
  from Configurables          import FilterDesktop
  from StandardParticles      import StdNoPIDsPions, StdNoPIDsProtons, StdNoPIDsMuons
  from PhysSelPython.Wrappers import Selection, SelectionSequence, DataOnDemand

  MuPidMoniSeq_Lambda = GaudiSequencer("MuPidMoniSeq_Lambda")
  MuPidMoniSeq_Jpsi   = GaudiSequencer("MuPidMoniSeq_Jpsi")

  #
  # Make pions and protons
  #

  PionsFilter      = FilterDesktop("PionsFilter")
  PionsFilter.Code = "(P>3000*MeV) & (PT>100*MeV) & (TRCHI2DOF<2) & (ISLONG) & (MIPCHI2DV(PRIMARY)>9)"
  PionsFilterSel   = Selection("PionsFilterSel",
                               Algorithm          = PionsFilter,
                               RequiredSelections = [StdNoPIDsPions]
                               )

  ProtonsFilter      = FilterDesktop("ProtonsFilter")
  ProtonsFilter.Code = "(P>3000*MeV) & (PT>100*MeV) & (TRCHI2DOF<2) & (ISLONG) & (MIPCHI2DV(PRIMARY)>9)"
  ProtonsFilterSel   = Selection("ProtonsFilterSel",
                                 Algorithm          = ProtonsFilter,
                                 RequiredSelections = [StdNoPIDsProtons]
                                 )
  #
  # Make Lambda_0
  #

  LambdaMonitor                 = CombineParticles("LambdaMonitor")
  LambdaMonitor.DecayDescriptor = "[Lambda0 -> p+ pi-]cc" ;
  LambdaMonitor.CombinationCut  = "(ADAMASS('Lambda0')<10*MeV)"
  LambdaMonitor.MotherCut       = "(50<BPVVDZ) & (600>BPVVDZ) & (BPVDIRA>0.9999995)  & (CHILDCUT((PT>0.2*GeV),1) | CHILDCUT((PT>0.2*GeV),2)) & (ADWM('KS0', WM('pi+', 'pi-')) > 20*MeV)"

  LambdaMonitorSel = Selection("LambdaMonitorSel",
                               Algorithm          = LambdaMonitor,
                               RequiredSelections = [PionsFilterSel, ProtonsFilterSel])

  LambdaMonitorSeq = SelectionSequence("LambdaMonitorSeq",
                                       TopSelection = LambdaMonitorSel)
  
  
  MuPidMoniSeq_Lambda.Members += [LambdaMonitorSeq.sequence()]

  #
  # Make muons and J/psi
  #

  mucocut = '(0.5<PPINFO(LHCb.ProtoParticle.InAccMuon,-1)) & (P>3*GeV) & (PT>800*MeV) & (TRCHI2DOF<3) & (ISLONG)'

  tag1cuts = " (CHILDCUT(ISMUON,1)) & (CHILDCUT((P>6*GeV),1)) & (CHILDCUT((PT>1.5*GeV),1)) "
  tag2cuts = " (CHILDCUT(ISMUON,2)) & (CHILDCUT((P>6*GeV),2)) & (CHILDCUT((PT>1.5*GeV),2)) "
  
  probe2cuts = " ( (CHILDCUT((PPINFO(LHCb.ProtoParticle.CaloEcalE,-10000)<1000*MeV),2)) & (CHILDCUT((PPINFO(LHCb.ProtoParticle.CaloHcalE,-10000)<4000*MeV),2)) & (CHILDCUT((PPINFO(LHCb.ProtoParticle.CaloEcalE,-10000)>-10*MeV),2)) & (CHILDCUT((PPINFO(LHCb.ProtoParticle.CaloHcalE,-10000)>1000*MeV),2)) ) "
  probe1cuts = " ( (CHILDCUT((PPINFO(LHCb.ProtoParticle.CaloEcalE,-10000)<1000*MeV),1)) & (CHILDCUT((PPINFO(LHCb.ProtoParticle.CaloHcalE,-10000)<4000*MeV),1)) & (CHILDCUT((PPINFO(LHCb.ProtoParticle.CaloEcalE,-10000)>-10*MeV),1)) & (CHILDCUT((PPINFO(LHCb.ProtoParticle.CaloHcalE,-10000)>1000*MeV),1)) )  "

  child1cuts = tag1cuts + " & " + probe2cuts

  child2cuts = tag2cuts + " & " + probe1cuts


  MuonsFilter      = FilterDesktop("MuonsFilter")
  MuonsFilter.Code = mucocut
  MuonsFilterSel   = Selection("MuonsFilterSel",
                               Algorithm          = MuonsFilter,
                               RequiredSelections = [StdNoPIDsMuons]
                               )

  JpsiMonitor                 = CombineParticles("JpsiMonitor")
  JpsiMonitor.DecayDescriptor = "J/psi(1S) -> mu+ mu-" ;
  JpsiMonitor.CombinationCut  = "(ADAMASS('J/psi(1S)')<300*MeV)"
  JpsiMonitor.MotherCut       = "(VFASPF(VCHI2/VDOF)<20) & ( " + child1cuts + " | " + child2cuts + " ) "

  JpsiMonitor.OutputLevel    = 6
  
  JpsiMonitorSel = Selection("JpsiMonitorSel",
                             Algorithm          = JpsiMonitor,
                             RequiredSelections = [MuonsFilterSel])
  
  JpsiMonitorSeq = SelectionSequence("JpsiMonitorSeq",
                                     TopSelection = JpsiMonitorSel)
  
  
  MuPidMoniSeq_Jpsi.Members += [JpsiMonitorSeq.sequence()]
  
  #
  # Monitoring muon mis-id with Lambda_0
  #
  
  MuIDLambdaPlot             = MuIDMonitor("MuIDLambdaPlot")
  MuIDLambdaPlot.Inputs      = [LambdaMonitorSel.outputLocation()]
  MuIDLambdaPlot.OutputLevel = 6

  MuIDLambdaPlot.MassMean       = 1115.68
  MuIDLambdaPlot.MassWindow     =   20.
  MuIDLambdaPlot.EffMassWin     =    2.;
  
  MuIDLambdaPlot.JpsiAnalysis   = 0
  MuIDLambdaPlot.LambdaAnalysis = 1
  MuIDLambdaPlot.HitInFoi       = 1
  
  
  MuIDLambdaPlot.PreSelMomentum = 3000.              # MuonID preselection momentum (MeV/c)
  MuIDLambdaPlot.MomentumCuts   = [ 6000. , 10000. ] # MuonID momentum cut ranges (MeV/c)
  
  # MuonID FOI parameters
  
  MuIDLambdaPlot.FOIfactor      = 1.
  
  MuIDLambdaPlot.XFOIParameter1 = [5.5, 4.0, 3.3, 2.8,
                                   5.2, 3.6, 2.4, 2.4,
                                   5.7, 4.4, 2.8, 2.3,
                                   5.1, 3.1, 2.3, 2.1,
                                   5.8, 3.4, 2.6, 2.8]

  MuIDLambdaPlot.XFOIParameter2 = [11., 3., 1., 1.,
                                   31., 28., 21., 17.,
                                   30., 31., 27., 22.,
                                   28., 33., 35., 47.,
                                   31., 39., 56., 151.]

  MuIDLambdaPlot.XFOIParameter3 = [0.20, 0.08, 0.03, 0.1,
                                   0.06, 0.08, 0.10, 0.15,
                                   0.04, 0.06, 0.09, 0.12,
                                   0.08, 0.15, 0.23, 0.36,
                                   0.07, 0.14, 0.24, 0.49]

  MuIDLambdaPlot.YFOIParameter1 = [2.8, 1.7, -153., 1.9,
                                   3.3, 2.1, 1.7, 1.6,
                                   3.6, 2.8, 1.9, 1.8,
                                   4.4, 3.3, 2.2, 2.2,
                                   4.8, 3.9, 2.6, 2.3]
  
  MuIDLambdaPlot.YFOIParameter2 = [3., 2., 156., 0.,
                                   17., 15., 9., 5.,
                                   26., 25., 16., 15.,
                                   30., 49., 57., 92.,
                                   32., 55., 96., 166.]
  
  MuIDLambdaPlot.YFOIParameter3 = [0.03, 0.02, 0.00, 0.09,
                                   0.13, 0.19, 0.19, 0.24, 
                                   0.11, 0.19, 0.21, 0.32,
                                   0.10, 0.22, 0.30, 0.52,
                                   0.08, 0.20, 0.34, 0.52]


  #
  # Parameters of the Landau functions
  #
  
  MuIDLambdaPlot.distMuon  = [0.311, 1.349, 0.524, 0.0020, 17., 10.6, 0.04, 4.1, 1.64]
  MuIDLambdaPlot.distPion  = [11., -12., 0.2029, -0.026, 0.06, 0.59, 0.008, -29., 41.]
  
  MuPidMoniSeq_Lambda.Members += [MuIDLambdaPlot]
  
  #
  # Monitoring muon id with J/psi
  #
  
  MuIDJpsiPlot             = MuIDMonitor("MuIDJpsiPlot")
  MuIDJpsiPlot.Inputs      = [JpsiMonitorSel.outputLocation()]
  MuIDJpsiPlot.OutputLevel = 6
  
  MuIDJpsiPlot.MassMean   = 3096.91
  MuIDJpsiPlot.MassWindow = 300.
  MuIDJpsiPlot.EffMassWin = 20.;
  
  MuIDJpsiPlot.JpsiAnalysis   = 1
  MuIDJpsiPlot.LambdaAnalysis = 0
  MuIDJpsiPlot.HitInFoi       = 1
  
  MuIDJpsiPlot.PreSelMomentum = MuIDLambdaPlot.PreSelMomentum
  MuIDJpsiPlot.MomentumCuts   = MuIDLambdaPlot.MomentumCuts
  
  # MuonID FOI parameters
  
  MuIDJpsiPlot.FOIfactor      = MuIDLambdaPlot.FOIfactor
  MuIDJpsiPlot.XFOIParameter1 = MuIDLambdaPlot.XFOIParameter1
  MuIDJpsiPlot.XFOIParameter2 = MuIDLambdaPlot.XFOIParameter2
  MuIDJpsiPlot.XFOIParameter3 = MuIDLambdaPlot.XFOIParameter3
  MuIDJpsiPlot.YFOIParameter1 = MuIDLambdaPlot.YFOIParameter1
  MuIDJpsiPlot.YFOIParameter2 = MuIDLambdaPlot.YFOIParameter2
  MuIDJpsiPlot.YFOIParameter3 = MuIDLambdaPlot.YFOIParameter3
  
  #
  # Parameters of the Landau functions
  #
  
  MuIDJpsiPlot.distMuon = MuIDLambdaPlot.distMuon
  MuIDJpsiPlot.distPion = MuIDLambdaPlot.distPion
  
  MuPidMoniSeq_Jpsi.Members += [MuIDJpsiPlot]
  MuPidMoniSeq_Jpsi.IgnoreFilterPassed = True
  MuPidMoniSeq_Lambda.IgnoreFilterPassed = True

  seq.Members += [MuPidMoniSeq_Lambda, MuPidMoniSeq_Jpsi]

  # set the options

  class __MonAdd:
    def __init__(self,s):
      self.seq = s
    def addMonitors(self):
      # Append to processing
      GaudiSequencer("PhysicsSeq").Members += [ self.seq ]

  mon = __MonAdd(seq)
  Gaudi.appendPostConfigAction(mon.addMonitors)

  EventLoopMgr().OutputLevel = MSG_DEBUG #ERROR
  EventLoopMgr().Warnings    = False

  brunel.UseDBSnapshot = True     # try it
  Brunel.Configuration.Brunel.configureOutput = dummy
  HistogramPersistencySvc().OutputFile = ""
  HistogramPersistencySvc().OutputLevel = MSG_ERROR
  print brunel
  return brunel

#============================================================================================================
def setupOnline():
  """
        Setup the online environment: Buffer managers, event serialisation, etc.

        @author M.Frank
  """
  import OnlineEnv

  buffs = ['Events']
  sys.stdout.flush()
  app=Gaudi.ApplicationMgr()
  app.AppName = ''
  app.HistogramPersistency = 'ROOT'
  app.SvcOptMapping.append('LHCb::OnlineEvtSelector/EventSelector')
  app.SvcOptMapping.append('LHCb::FmcMessageSvc/MessageSvc')
  mep = OnlineEnv.mepManager(OnlineEnv.PartitionID,OnlineEnv.PartitionName,buffs,True)
  mep.ConnectWhen = "start";
  sel = OnlineEnv.mbmSelector(input=buffs[0],type='ONE',decode=False,event_type=2)
  sel.REQ1 = "EvType=2;TriggerMask=0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF;VetoMask=0,0x2,0,0;MaskType=ANY;UserType=ONE;Frequency=PERC;Perc=100.0"
  app.EvtSel  = sel
  app.Runable = OnlineEnv.evtRunable(mep)
  app.Runable.NumErrorToStop = -1;
  app.ExtSvc.append(mep)
  app.ExtSvc.append(sel)
  app.AuditAlgorithms = False
  app.TopAlg.insert(0,"UpdateAndReset")
  Configs.MonitorSvc().OutputLevel = MSG_ERROR
  Configs.MonitorSvc().UniqueServiceNames = 1
  Configs.RootHistCnv__PersSvc("RootHistSvc").OutputLevel = MSG_ERROR
  app.OutputLevel = OnlineEnv.OutputLevel
  configureForking(app)
  sys.stdout.flush()

#============================================================================================================
def patchMessages():
  """
        Messages in the online get redirected.
        Setup here the FMC message service

        @author M.Frank
  """
  import OnlineEnv
  app=Gaudi.ApplicationMgr()
  Configs.AuditorSvc().Auditors = []
  app.MessageSvcType = 'LHCb::FmcMessageSvc'
  if Gaudi.allConfigurables.has_key('MessageSvc'):
    del Gaudi.allConfigurables['MessageSvc']
  msg = Configs.LHCb__FmcMessageSvc('MessageSvc')
  msg.fifoPath      = os.environ['LOGFIFO']
  msg.LoggerOnly    = True
  msg.doPrintAlways = False
  msg.OutputLevel   = OnlineEnv.OutputLevel

#============================================================================================================
def start():
  """
        Finish configuration and configure Gaudi application manager

        @author M.Frank
  """
  import OnlineEnv
  OnlineEnv.end_config(False)
  #OnlineEnv.end_config(True)

def run():
  try:
    br = patchBrunel()
    setupOnline()
    patchMessages()
    start()
  except Exception,X:
    print '[ERROR] Exception:',str(X)
    sys.exit(1)
