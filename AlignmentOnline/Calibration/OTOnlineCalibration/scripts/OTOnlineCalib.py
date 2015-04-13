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
    OTGaudiSeq = GaudiSequencer("OTt0OnlineClbrSeq")
    OTt0OnlineClbrAlg = OTt0OnlineClbr("OTt0OnlineClbrAlg")

    #OTt0OnlineClbrAlg.InputFiles  = [ "/hist/Savesets/2013/LHCb/Brunel/01/20/Brunel-135576-20130120T161302-EOR.root"]

    # OT T0 calibration algorithm
    OTt0OnlineClbrAlg.InputTasks  = [ "Brunel" ]
    OTt0OnlineClbrAlg.Partition = partition
    OTt0OnlineClbrAlg.ReadInitialT0FromDB = False
    OTt0OnlineClbrAlg.RunOnline = True
    OTt0OnlineClbrAlg.XMLFilePath = "/group/online/alignment/OT/Calib"
    OTt0OnlineClbrAlg.OutputLevel = MSG_DEBUG

    # Keep analysis task going.
    OTt0OnlineClbrAlg.StopAlgSequence = False
    
    OTGaudiSeq.Members += [OTt0OnlineClbrAlg]
    OTGaudiSeq.IgnoreFilterPassed = True

    ApplicationMgr().TopAlg += [ OTGaudiSeq ]
    ApplicationMgr().EvtSel  = "NONE"
    ApplicationMgr().ExtSvc += [ "LHCb::PublishSvc", "MonitorSvc" ]
    ApplicationMgr().Runable = "LHCb::OnlineRunable/Runable"

    # Online conditions
    from Configurables import LHCbApp
    LHCbApp().CondDBtag = "cond-20150119-1"
    LHCbApp().DDDBtag   = "dddb-20120831"

    from Configurables import CondDB
    conddb = CondDB()
    conddb.Tags["ONLINE"] = 'fake'
    conddb.IgnoreHeartBeat = True
    conddb.UseDBSnapshot = True
    conddb.DBSnapshotDirectory = "/group/online/hlt/conditions"
    conddb.EnableRunChangeHandler = False

    # DB patches
    db_path = os.environ['OTONLINECALIBRATIONROOT']+'/db_patches/'
    ddbs = [db_path + p for p in ("OTGeometryT0.db/DDDB", "OTCondT0.db/LHCBCOND")]

    for i, db in enumerate(ddbs):
         from Configurables import (CondDB, CondDBAccessSvc)
         OTCond = CondDBAccessSvc('OTDB' + str(i + 1))
         OTCond.ConnectionString = 'sqlite_file:' + db
         CondDB().addLayer(OTCond)

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
    msg.OutputLevel   = MSG_DEBUG # Online.OutputLevel

setup()
patchMessages()
Online.end_config(True)
