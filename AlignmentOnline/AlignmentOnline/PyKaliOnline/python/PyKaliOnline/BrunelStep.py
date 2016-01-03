from Gaudi.Configuration import *
from Configurables import LHCbApp, CondDB
import socket, os, sys

import Configurables as Configs
import Gaudi.Configuration as Gaudi

from PyKaliOnline.Paths import Paths, importOnline

MSG_VERBOSE = 1
MSG_DEBUG   = 2
MSG_INFO    = 3
MSG_WARNING = 4
MSG_ERROR   = 5
MSG_FATAL   = 6
MSG_ALWAYS  = 7

import argparse
parser = argparse.ArgumentParser(description = "Input file delivery")
parser.add_argument("--input-files", type = str, dest = "input_files", nargs = '*', default = None)
parser.add_argument("--index"      , type = int, dest = "index"      , default = None)
parser.add_argument("--run-prefix" , type = str, dest = "run_prefix" , default = None)
args = parser.parse_args()

input_files= args.input_files
index      = args.index
run_prefix = args.run_prefix
hn         = socket.gethostname()
pt         = Paths(index, hn)
directory  = pt.dst_dir
dst_file   = os.path.join(directory, pt.fileprefix())
dst_file   = dst_file+'-Runs'+run_prefix

if '__main__' == __name__ :
    """
    Messages in the online get redirected.
    Setup here the FMC message service
    
    @author M.Frank
    """

    from Configurables import Brunel
    
    Brunel().Detectors   = ['Spd', 'Prs', 'Ecal', 'Hcal' ]
    Brunel().DataType    = "2015"
    Brunel().InputType   = "MDF"
    Brunel().OutputType  = "DST"
    Brunel().EvtMax      = -1
    Brunel().WriteFSR    = False
    Brunel().Histograms  = "Online"
    Brunel().OnlineMode  = True
    Brunel().PrintFreq   = 1000
    Brunel().DatasetName = dst_file

    from Configurables import CondDB
    CondDB().Simulation = False
    CondDB().UseDBSnapshot = True
    CondDB().DBSnapshotDirectory = "/group/online/hlt/conditions"
    CondDB().EnableRunChangeHandler = True
    CondDB().Tags["ONLINE"] = 'fake'
    CondDB().setProp("IgnoreHeartBeat", True)
    CondDB().Online = True

    try:
        import AllHlt1
    except ImportError:
        rd = '/group/online/hlt/conditions/RunChangeHandler'
        sys.path.append(rd)
        import AllHlt1

    import AllHlt1
    CondDB().RunChangeHandlerConditions = AllHlt1.ConditionMap

    Online = importOnline()
    Brunel().CondDBtag = Online.CondDBTag
    Brunel().DDDBtag = Online.DDDBTag

    app=Gaudi.ApplicationMgr()
    #Configs.AuditorSvc().Auditors = []
    app.MessageSvcType = 'LHCb::FmcMessageSvc'
    if Gaudi.allConfigurables.has_key('MessageSvc'):
       del Gaudi.allConfigurables['MessageSvc']
    msg = Configs.LHCb__FmcMessageSvc('MessageSvc')
    msg.fifoPath      = os.environ['LOGFIFO']
    msg.LoggerOnly    = True
    msg.doPrintAlways = False
    msg.OutputLevel   = MSG_INFO # Online.OutputLevel

    from GaudiConf import IOHelper
    IOHelper('MDF').inputFiles(input_files)

    from GaudiPython.Bindings import AppMgr    
    gaudi = AppMgr()

    gaudi.run(-1)
