from Gaudi.Configuration import *
from Configurables import LHCbApp, CondDB
import socket, os, sys

import Configurables as Configs
import Gaudi.Configuration as Gaudi

from Paths import Paths, importOnline
#from Paths import Paths, importOnline

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
args = parser.parse_args()

input_files= args.input_files
index      = args.index
hn         = socket.gethostname()
pt         = Paths(index, hn)
directory  = pt.dst_dir
dst_file   = os.path.join(directory, pt.fileprefix())
#directory  = os.path.dirname(input_files[0])
#pt         = Paths(input_files[0], index, hn)

from Configurables import Brunel
Brunel().Detectors   = ['Spd', 'Prs', 'Ecal', 'Hcal' ]
Brunel().DataType    = "2015"
Brunel().InputType   = "MDF"
Brunel().OutputType  = "DST"
Brunel().EvtMax      = 1000
Brunel().WriteFSR    = False
Brunel().Histograms  = "None"
Brunel().OnlineMode  = True
Brunel().PrintFreq   = 1000
Brunel().DatasetName = dst_file
Brunel().CondDBtag   = 'cond-20150828'

if '__main__' == __name__ :
    """
    Messages in the online get redirected.
    Setup here the FMC message service
    
    @author M.Frank
    """

    from Gaudi.Configuration import *
    from Configurables import CondDB
    #cdb = CondDB()
    #cdb.PartitionConnectionString["LHCBCOND"] = "sqlite_file:$HOME/cmtuser/data/tryLHCBCOND.db/LHCBCOND"
    #cdb.Tags["LHCBCOND"] = ""

    #Online = importOnline()

    app=Gaudi.ApplicationMgr()
    #Configs.AuditorSvc().Auditors = []
    #app.MessageSvcType = 'LHCb::FmcMessageSvc'
    #if Gaudi.allConfigurables.has_key('MessageSvc'):
    #    del Gaudi.allConfigurables['MessageSvc']
    #msg = Configs.LHCb__FmcMessageSvc('MessageSvc')
    #msg.fifoPath      = os.environ['LOGFIFO']
    #msg.LoggerOnly    = True
    #msg.doPrintAlways = False
    #msg.OutputLevel   = MSG_INFO # Online.OutputLevel

    from GaudiConf import IOHelper
    IOHelper('MDF').inputFiles(input_files)
    #IOHelper('MDF').inputFiles(['/home/dsavrina/cmtuser/data/154030_0000000173_07.raw'])

    from GaudiPython.Bindings import AppMgr    
    gaudi = AppMgr()

    gaudi.run(1000)
