import time
import os, Gaudi.Configuration
from Configurables import GaudiSequencer, ApplicationMgr, LHCb__FmcMessageSvc
from Configurables import OTt0OnlineClbr
import OnlineConfig as Online

MSG_VERBOSE = 1
MSG_DEBUG   = 2
MSG_INFO    = 3
MSG_WARNING = 4
MSG_ERROR   = 5
MSG_FATAL   = 6
MSG_ALWAYS  = 7

#
partition = os.environ['PARTITION'] if 'PARTITION' in os.environ else os.environ['PARTITION_NAME']

def setup():
    initialTime = long(time.time() * 1e9)

    OTGaudiSeq = GaudiSequencer("OTt0OnlineClbrSeq")
    OTt0OnlineClbrAlg = OTt0OnlineClbr("OTt0OnlineClbrAlg")
    #OTt0OnlineClbrAlg.InputFiles  = [ "/hist/Savesets/2013/LHCb/Brunel/01/20/Brunel-135576-20130120T161302-EOR.root"]

    # OT T0 calibration algorithm
    OTt0OnlineClbrAlg.InputTasks  = [ "Brunel" ]
    OTt0OnlineClbrAlg.Partition = partition
    OTt0OnlineClbrAlg.ReadInitialT0FromDB = False
    OTt0OnlineClbrAlg.RunOnline = True
    OTt0OnlineClbrAlg.CheckDataT0 = True
    OTt0OnlineClbrAlg.PublishedName = "OT/Calib"
    OTt0OnlineClbrAlg.XMLFilePath = "/group/online/alignment/OT/Calib"
    OTt0OnlineClbrAlg.OutputLevel = MSG_INFO
    OTt0OnlineClbrAlg.UseClockPhase = (partition != "FEST")
    OTt0OnlineClbrAlg.InitialTime = initialTime
    OTt0OnlineClbrAlg.Threshold = 0.04
    # Keep analysis task going.
    OTt0OnlineClbrAlg.StopAlgSequence = False

    OTGaudiSeq.Members += [OTt0OnlineClbrAlg]
    OTGaudiSeq.IgnoreFilterPassed = True

    ApplicationMgr().TopAlg += [ OTGaudiSeq ]
    ApplicationMgr().EvtSel  = "NONE"
    ApplicationMgr().ExtSvc += [ "LHCb::PublishSvc", "MonitorSvc", "IncidentSvc"]
    ApplicationMgr().Runable = "LHCb::OnlineRunable/Runable"

    from Configurables import CondDB
    conddb = CondDB()
    conddb.Tags["ONLINE"] = 'fake'
    conddb.IgnoreHeartBeat = True
    conddb.UseDBSnapshot = True
    conddb.DBSnapshotDirectory = "/group/online/hlt/conditions"

    from Configurables import EventClockSvc, FakeEventTime, EventDataSvc
    ecs = EventClockSvc()
    ecs.InitialTime = initialTime
    ecs.addTool(FakeEventTime, "EventTimeDecoder")
    ecs.EventTimeDecoder.StartTime = initialTime
    ecs.EventTimeDecoder.TimeStep = 10
    EventDataSvc().ForceLeaves = True

    # Configure DB tags and per-run conditions to be used to be the same as what
    # the HLT1 reconstruction farm uses. This is done by directly importing the
    # python file to ensure the script can also start when LHCb is running
    # passthrough.
    conddb.EnableRunChangeHandler = True
    conddb.RunChangeHandlerConditions = {'LHCb/2015/%d/ot.xml' : ["Conditions/Calibration/OT/CalibrationGlobal"]}
    
    from Configurables import LHCbApp
    LHCbApp().CondDBtag = 'cond-20150409-2'
    LHCbApp().DDDBtag   = 'dddb-20150119-3'
    LHCbApp().DataType = '2015'

#============================================================================================================
def patchMessages():
    """
          Messages in the online get redirected.
          Setup here the FMC message service

          @author M.Frank
    """
    app=ApplicationMgr()
    #Configs.AuditorSvc().Auditors = []
    app.MessageSvcType = 'LHCb::FmcMessageSvc'
    if Gaudi.Configuration.allConfigurables.has_key('MessageSvc'):
      del Gaudi.Configuration.allConfigurables['MessageSvc']
    msg = LHCb__FmcMessageSvc('MessageSvc')
    msg.fifoPath      = os.environ['LOGFIFO']
    msg.LoggerOnly    = True
    msg.doPrintAlways = False
    msg.OutputLevel   = MSG_INFO # Online.OutputLevel

setup()
patchMessages()
Online.end_config(True)
