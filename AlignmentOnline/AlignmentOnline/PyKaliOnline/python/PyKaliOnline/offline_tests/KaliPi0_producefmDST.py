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

input_files= args.input_file
index      = args.index
lambdas    = args.lambdas
hn         = socket.gethostname()
#directory  = os.path.dirname(input_file)
#pt         = Paths(input_file, index, hn)
pt         = Paths(index, hn)
directory  = pt.dst_dir
tup_file   = os.path.join(directory, pt.fileprefix())+'.root'
fmdst_file = os.path.join(directory, pt.fileprefix())+'.fmDST'

print lambdas

if lambdas == 'None':
    kali = firstPass (
        ##-- cuts and filters
        PtGamma           = 250 * MeV ,  ## selection cuts
        ## event multiplicity filter
        Filter            = "(650. > CONTAINS('Raw/Spd/Digits')) & ( CONTAINS('Rec/Vertex/Primary') > 0.) & ( CONTAINS('Rec/Vertex/Primary') < 5.)",
        
        ##-- output
        NTupleProduce    = True        , # produce NTuples
        NTuple           = tup_file    , # NTuple name
        Histograms       = False       , # do not produce Histograms
        FemtoDST         = fmdst_file  , # output fmDST name
        
        ##-- general (configuring DaVinci)
        DataType         = '2015'      , # data type
        EvtMax           =  -1         , # number of events to run
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
        NTupleProduce    = True            , # produce NTuples
        NTuple           = tup_file        , # NTuple name
        Histograms       = False           , # do not produce Histograms
        FemtoDST         = ""              ,
        ## Calibration
        KaliDB           = {'name': lambdas, 'ecal': 'ecal'},
        ## general 
        DataType         = '2015'          ,
        EvtMax           =  -1             ,
        OutputLevel      = ERROR
        )

## ======== input data type - 'SDST' ============
from Configurables import DaVinci
DaVinci().InputType = 'DST'                            # use SDSTs as an input
DaVinci().CondDBtag = 'cond-20150828'

if '__main__' == __name__ :

    """
    Messages in the online get redirected.
    Setup here the FMC message service
    
    @author M.Frank
    """
    #Online = importOnline()

    #from Gaudi.Configuration import *
    #from Configurables import CondDB
    
    #cdb = CondDB()
    #cdb.PartitionConnectionString["LHCBCOND"] = "sqlite_file:$HOME/cmtuser/data/tryLHCBCOND.db/LHCBCOND"
    #cdb.Tags["LHCBCOND"] = ""
    
    #app=Gaudi.ApplicationMgr()
    #Configs.AuditorSvc().Auditors = []
    #app.MessageSvcType = 'LHCb::FmcMessageSvc'
    #if Gaudi.allConfigurables.has_key('MessageSvc'):
    #    del Gaudi.allConfigurables['MessageSvc']
    #msg = Configs.LHCb__FmcMessageSvc('MessageSvc')
    #msg.fifoPath      = os.environ['LOGFIFO']
    #msg.LoggerOnly    = True
    #msg.doPrintAlways = False
    #msg.OutputLevel   = MSG_INFO # Online.OutputLevel

    ## make printout of the own documentations
    print '*'*120
    print                      __doc__
    print ' Author  : %s ' %   __author__
    print ' Version : %s ' %   __version__
    print ' Date    : %s ' %   __date__
    print '*'*120

    from GaudiConf import IOHelper
    IOHelper('ROOT').inputFiles(input_files)

    from GaudiPython.Bindings import AppMgr    
    gaudi = AppMgr()

    gaudi.run(-1)

## THE END
