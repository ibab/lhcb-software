
#!/usr/bin/env python
# =============================================================================
# $Id: KaliPi0_producefmDST.py$ 
# =============================================================================
## @file
#  The basic configuration for fmDST production
#  @author Daria Savrina Daria.Savrina@cern.ch
#  @date   2010-10-30
# =============================================================================
"""
The basic configuration for fmDST production for iterative pi0-calibration of
the ECAL. SDSTs are used as input.

Usage:

  > gaudirun.py KaliPi0_producefmDST.py

Or (suitable for tests)

  > python KaliPi0_producefmDST.py

"""
# =============================================================================
__author__  = " Daria Savrina Daria.Savrina@cern.ch "
__date__    = " 2009-10-30 "
__version__ = " CVS Tag $Name: not supported by cvs2svn $, version $Revision: $"
# =============================================================================
## ============= the basic import ====================
from   Gaudi.Configuration       import *
from   GaudiKernel.SystemOfUnits import MeV 
import Configurables as Configs
import Gaudi.Configuration as Gaudi

import os, sys, socket
from Paths import Paths, importOnline

from multiprocessing import Process
from multiprocessing import cpu_count as nCPUs

## =============== configuring Kali fmDST production =================
from KaliCalo.Configuration import firstPass, secondPass
import KaliCalo.ZipShelve as zs
from KaliCalo.Kali import LambdaMap

MSG_VERBOSE = 1
MSG_DEBUG   = 2
MSG_INFO    = 3
MSG_WARNING = 4
MSG_ERROR   = 5
MSG_FATAL   = 6
MSG_ALWAYS  = 7

import argparse
parser = argparse.ArgumentParser(description = "Input file delivery")
parser.add_argument("--input-file", type = str, dest = "input_file", nargs = '*', default = None)
parser.add_argument("--index"     , type = int, dest = "index"     , default = None)
parser.add_argument("--lambdas"   , type = str, dest = "lambdas"   , default = None)
args = parser.parse_args()

input_file = args.input_file
index      = args.index
lambdas    = args.lambdas
hn         = socket.gethostname()
pt         = Paths(index, hn)
directory  = pt.dst_dir
fmdst_file = os.path.join(directory, pt.fileprefix())

print lambdas

class fmProd(object):
    """
    The fmDST production task to be run on each core
    of the node. Input parameters:
    njob   - number of the current job
    nevt   - number of events to be processed
    dsname - output dataset name
    inputs - list of input files
    """
    def __init__(self, njob, nevt = -1, dsname = fmdst_file, inputs = []):
        self.__process = Process(target = self.run)
        self.__nevt    = nevt
        self.__inputs  = inputs
        self.__dsname  = dsname+'-%i'%njob

    def run(self):
        if lambdas == 'None':
            kali = firstPass (
                ##-- cuts and filters
                PtGamma           = 250 * MeV ,  ## selection cuts
                ## event multiplicity filter
                Filter            = "(650. > CONTAINS('Raw/Spd/Digits')) & ( CONTAINS('Rec/Vertex/Primary') > 0.) & ( CONTAINS('Rec/Vertex/Primary') < 5.)",
        
                ##-- output
                NTupleProduce    = True                   , # produce NTuples
                NTuple           = self.__dsname+'.root'  , # NTuple name
                Histograms       = False                  , # do not produce Histograms
                FemtoDST         = self.__dsname+'.fmDST' , # output fmDST name
        
                ##-- general (configuring DaVinci)
                DataType         = '2015'      , # data type
                EvtMax           =  self.__nevt, # number of events to run
                OutputLevel      = ERROR
             )
        else:
            kali = secondPass (
            ## 
            DestroyTES = False                 , 
            ## ``Physics''
            PtGamma          = 250 * MeV       ,
            Pi0Cut           = "PT > 550 * MeV",
            ## IO 
            NTupleProduce    = True                 , # produce NTuples
            NTuple           = self.__dsname+'.root', # NTuple name
            Histograms       = False                , # do not produce Histograms
            FemtoDST         = ""                   ,
            ## Calibration
            KaliDB           = {'name': lambdas, 'ecal': 'ecal'},
            ## general 
            DataType         = '2015'          ,
            EvtMax           =  self.__nevt    ,
            OutputLevel      = ERROR
            )

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

        ## ======== input data type - 'SDST' ============
        from Configurables import DaVinci
        DaVinci().InputType = 'DST'                            # use SDSTs as an input
        DaVinci().CondDBtag = Online.CondDBTag
        DaVinci().DDDBtag = Online.DDDBTag
    
        from GaudiConf import IOHelper
        IOHelper('ROOT').inputFiles(self.__inputs)

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

n_files   = len(input_file)
n_cpus    = nCPUs()-4

if n_files%n_cpus == 0: files_per_job = n_files/n_cpus
else                  : files_per_job = n_files/n_cpus + 1

new_list_of_files = []

while True:
    if not len(input_file) > files_per_job:
        new_list_of_files.append(input_file)
        break
    new_list_of_files.append(input_file[0:files_per_job])
    for f in input_file[0:files_per_job]:
        input_file.remove(f)

print len(new_list_of_files), len(new_list_of_files[1])

## prepare the list of tasks
writers = [fmProd(i, -1, fmdst_file, new_list_of_files[i]) for i in xrange(len(new_list_of_files))]

## run!
for w in writers:
    w.start()
for w in writers:
    w.start()
for w in writers:
    w.join()

