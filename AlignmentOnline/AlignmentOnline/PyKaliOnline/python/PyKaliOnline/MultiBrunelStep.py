## modules for splitting the job between several
## cores of each node
from multiprocessing import Process
from multiprocessing import cpu_count as nCPUs

## general Gaudi
from Gaudi.Configuration import *
from Configurables import LHCbApp, CondDB
import Configurables as Configs
import Gaudi.Configuration as Gaudi

import socket, os, sys

## input and output files paths and
## online configuration
from PyKaliOnline.Paths import Paths, importOnline

MSG_VERBOSE = 1
MSG_DEBUG   = 2
MSG_INFO    = 3
MSG_WARNING = 4
MSG_ERROR   = 5
MSG_FATAL   = 6
MSG_ALWAYS  = 7

## get input arguments
import argparse
parser = argparse.ArgumentParser(description = "Input file delivery")
parser.add_argument("--input-files", type = str, dest = "input_files", nargs = '*', default = None) ## list of input files
parser.add_argument("--index"      , type = int, dest = "index"      , default = 0)                 ## analyzer number
parser.add_argument("--run-prefix" , type = str, dest = "run_prefix" , default = '')                ## run numbers range
args = parser.parse_args()

input_files= args.input_files
index      = args.index
run_prefix = args.run_prefix
hn         = socket.gethostname() ## node name

## create the name of output file
pt         = Paths(index, hn)
directory  = pt.dst_dir                                ## working directory
dst_file   = os.path.join(directory, pt.fileprefix())  ## output dst name, includes the host name,
dst_file   = dst_file+'-Runs'+run_prefix               ## analyzer number and run numbers range

class BrunelSplit(object):
    """
    The Brunel reconstruction task to be run on each core
    of the node. Input parameters:
    njob   - number of the current job
    nevt   - number of events to be processed
    dsname - output dataset name
    inputs - list of input files
    """
    def __init__(self, njob, nevt = -1, dsname = dst_file, inputs = []):
        self.__process = Process(target = self.run)
        self.__nevt    = nevt
        self.__inputs  = inputs
        self.__dsname  = dsname+'-%i'%njob

    def run(self):
        from Configurables import Brunel
    
        ## Brunel parameters
        Brunel().Detectors   = ['Spd', 'Prs', 'Ecal', 'Hcal' ]
        Brunel().DataType    = "2015"
        Brunel().InputType   = "MDF"
        Brunel().OutputType  = "DST"
        Brunel().EvtMax      = self.__nevt
        Brunel().WriteFSR    = False
        Brunel().Histograms  = "Online"
        Brunel().OnlineMode  = True
        Brunel().PrintFreq   = 1000
        Brunel().DatasetName = self.__dsname

        ## use the conditions database
        ## configuration for online
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
        Brunel().DDDBtag   = Online.DDDBTag

        """
        Messages in the online get redirected.
        Setup here the FMC message service
    
        @author M.Frank
        """
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
        IOHelper('MDF').inputFiles(self.__inputs)

        from GaudiPython.Bindings import AppMgr    
        gaudi = AppMgr()
        gaudi.initialize()
        gaudi.run(self.__nevt)
        gaudi.stop()
        gaudi.finalize()
        gaudi.exit()

    def process(self):
        return self.__process

    def start( self ):
        self.__process.start()

    def join( self ):
        self.__process.join()

## split the list of input files between
## by number of CPUs
n_files   = len(input_files)
n_cpus    = nCPUs()-4

if n_files%n_cpus == 0: files_per_job = n_files/n_cpus
else                  : files_per_job = n_files/n_cpus + 1

new_list_of_files = []

while True:
    if not len(input_files) > files_per_job:
        new_list_of_files.append(input_files)
        break
    new_list_of_files.append(input_files[0:files_per_job])
    for f in input_files[0:files_per_job]:
        input_files.remove(f)

print len(new_list_of_files), len(new_list_of_files[1])

## prepare the list of tasks
writers = [BrunelSplit(i, -1, dst_file, new_list_of_files[i]) for i in xrange(len(new_list_of_files))]

## run!
for w in writers:
    w.start()
for w in writers:
    w.join()

#if '__main__' == __name__ :
#
#
#    gaudi.run(-1)
