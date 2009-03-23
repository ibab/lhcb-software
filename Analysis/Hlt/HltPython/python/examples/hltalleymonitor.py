#! /afs/cern.ch/sw/lcg/external/Python/2.5/slc4_amd64_gcc34/bin/python
## """ !/usr/bin/python """
# =============================================================================
""" @namespace hltalleymonitor
@brief script that monitor an alley

Usage:
python hltalleymonitor.py 
note: you can use arguments: i.e python hltalleymonitor.py -c BsPhiPhi -n 1000

Inputs:
1) define the HLT configuration to run
2) define dst data cards and path where the B is located to do TOS
3) the list of alleys names to be monitored: i.e 'SingleHadron'
4) the list of monitoring tasks to perform

Monitoring tacks available:
1) Rate and RateTOS
2) Candidates and CandidatesTOS
3) Time (not working properly)
4) Variables of the filters applied (i.e PT>2500 etc), all, best and TOS

Output:
A ROOT file with a set of histograms for the monitoring tasks

@author Jose A. Hernando hernando@cern.ch
@date 2008-06-1@date 2008-06-16
@revision DaVinci v21r0
"""
# =============================================================================
import os
import GaudiPython

from math import *
import desktop
import arguments
import histotools
import PlotTools
import benchmarkchannels as bch
import hltconf

from Gaudi.Configuration import *
from ROOT import *

from hltmonitortools import Rate
from hltmonitortools import RateTOS
from hltmonitortools import Candidates
from hltmonitortools import CandidatesTOS
from hltmonitortools import Time
from hltmonitortools import Filters
from hltmonitortools import FiltersTOS

# configure channel and alleys to monitor
#----------------------------------
DEBUG = arguments.argument("dbg"," debug ",False)
NEVENTS = arguments.argument("n"," number of events ",1000)
CHANNEL = arguments.argument("c"," channel name ","Bs2PhiPhi")
HOMEDIR = str(arguments.argument("path"," path for output root files ",""))
PROD = str(arguments.argument("prod"," production name ",""))
ALLEYS = arguments.argument_list("a"," alleys list to monitor ", ["SingleHadron","DiHadron"])
CANDOTOS = ((CHANNEL.find("mb") or CHANNEL.find("Lumi"))<0)
#CANDOTOS = True 

# define the moniroting tasks
#----------------------------------
ALGTYPES = []
#TIME = arguments.argument("mtime"," monitor time ", True)
#if (TIME): ALGTYPES.append([Time,"Time"])
RATE = arguments.argument("mrate"," monitor rate ", True)
if (RATE): ALGTYPES.append([Rate,"Rate"])
RATETOS = arguments.argument("mtos"," monitor TisTos ", True)
if ((RATETOS) and CANDOTOS): ALGTYPES.append([RateTOS,"RateTOS"])
CANDIDATES = arguments.argument("mcan"," monitor candidates ", True)
if (CANDIDATES): ALGTYPES.append([Candidates,"Candidates"])
CANDIDATESTOS = arguments.argument("mcantos"," monitor candidates TOS ", True)
if (CANDIDATESTOS and CANDOTOS): ALGTYPES.append([CandidatesTOS,"CandidatesTOS"])
FILTERS = arguments.argument("mfilters"," monitor filters ", True)
if (FILTERS): ALGTYPES.append([Filters,"Filters"])
FILTERSTOS = arguments.argument("mfilterstos"," monitor filters ", True)
if ((FILTERSTOS) and CANDOTOS):
    ALGTYPES.append([FiltersTOS,"FiltersTOS"])

# import the HLT that you want to monitor!
from Configurables import DaVinci
from Configurables import HltConf

#from HltConf.HltMuonLines import HltMuonLinesConf

HltConf().L0TCK = "0xFFFF"
DV = DaVinci()
DV.ReplaceL0BanksWithEmulated = True
DV.DataType = "DC06"
#DV.DataType = "2008"
DV.HistogramFile = "hltalleymonitor_"+CHANNEL+".root"
#DV.HistogramFile = "DVHistos_1.root"
#DV.HltType = 'MU'
DV.HltType = 'MU+HA+PH+EL'
DV.PrintFreq = 100
DV.applyConf()
#--- end HLT configuration

EOPTS = []
datacards = bch.createOptLines(CHANNEL)   
print datacards
PATH = bch.TESPath[CHANNEL]
print PATH
EOPTS.append(datacards)

from Configurables import HltSelReportsMaker
HltSelReportsMaker().MaxCandidatesNonDecision = 500

#EOPTS.append("HltSummaryWriter.SaveAll = true")
#if (CANDOTOS):
    #EOPTS.append("HltSelReportsMaker.MaxCandidatesNonDecision=500");



# set the range for booking histograms variables
hltconf.VARIABLE_RANGE["L0ET"]=[1500.,5100.]

# create the application
gaudi = GaudiPython.AppMgr(outputlevel=3)
gaudi.config(options=EOPTS)
# To fix the problem of L0 order of running
topalg = gaudi.TopAlg
topalg.remove('GaudiSequencer/L0DUBankSwap')
topalg.insert(1,'GaudiSequencer/L0DUBankSwap') 
gaudi.TopAlg = topalg

# CUANTION1  -----------
# trick for DaVinci v22r0p2
topalg = gaudi.TopAlg
topalg.remove('GaudiSequencer/L0DUBankSwap')
topalg.insert(1,'GaudiSequencer/L0DUBankSwap')
gaudi.TopAlg = topalg
#------------------------

# create the moniroting tasks
for alley in ALLEYS:
    print 'Line - (1) ', alley
    for algtype in ALGTYPES:
        alg = desktop.addAlgorithm(gaudi,algtype[0],alley+algtype[1])
        setattr(alg,"alleyname",alley)

# initialize the application and retrieve main tools
gaudi.initialize()
HIS = desktop.ROOTOBJECTS
TES = gaudi.evtsvc()
ANNSVC = gaudi.service("HltANNSvc",IANNSvc)
#HLTSUM = gaudi.toolsvc().create("HltSummaryTool",interface="IHltSummaryTool")
TISTOS = gaudi.toolsvc().create("TriggerTisTos",interface="ITriggerTisTos")
TIMER = gaudi.toolsvc().create("SequencerTimerTool",interface="ISequencerTimerTool")

# initialize the monitoring tasks
for alg in desktop.ALGOS:
    alleyname = alg.alleyname
    print 'Line - (2) ', alleyname
    alley = hltconf.confAlley(gaudi,alleyname)
    setattr(alg,"alley",alley)
    setattr(alg,"PATH",PATH)
    #setattr(alg,"HLTSUM",HLTSUM)
    setattr(alg,"TISTOS",TISTOS)
    setattr(alg,"TIMER",TIMER)
    setattr(alg,"ANNSVC",ANNSVC)
    setattr(alg,"TES",TES)
    if (getattr(alg,"book",None)): alg.book()

# run the application
gaudi.run(NEVENTS)

# end gaudi
gaudi.exit()

# copying the root files to the path directory
if (HOMEDIR!=""):
    PWD = os.environ['PWD']
    print 'mv '+PWD+"/*.root "+HOMEDIR+'/.'
    os.system('mv '+PWD+"/*.root "+HOMEDIR+'/.')
