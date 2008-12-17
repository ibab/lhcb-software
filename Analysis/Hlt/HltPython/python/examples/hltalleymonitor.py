#!/usr/bin/python
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
#CHANNEL = arguments.argument("c"," channel name ","mblumi2")
CHANNEL = arguments.argument("c"," channel name ","Bs2PhiPhi")
ALLEYS = arguments.argument_list("a"," alleys list to monitor ", ["SingleHadron"])
CANDOTOS = (CHANNEL.find("mb")<0)

# define the moniroting tasks
#----------------------------------
ALGTYPES = []
TIME = arguments.argument("mtime"," monitor time ", True)
if (TIME): ALGTYPES.append([Time,"Time"])
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

importOptions("$DAVINCIROOT/options/DaVinci.py")
# import the HLT that you want to monitor!
from HltConf.Configuration import *
HltConf().oldStyle = False
HltConf().replaceL0BanksWithEmulated = True
HltConf().hltType = 'Hlt1'
HltConf().applyConf()
#--- end HLT configuration

EOPTS = []
datacards = bch.createOptLines(CHANNEL)   
print datacards
PATH = bch.TESPath[CHANNEL]
print PATH
EOPTS.append(datacards)

EOPTS.append("HltSummaryWriter.SaveAll = true")
if (CANDOTOS):
    EOPTS.append("HltSelReportsMaker.MaxCandidatesNonDecision=500");

# set the range for booking histograms variables
hltconf.VARIABLE_RANGE["L0ET"]=[1500.,5100.]

# create the application
gaudi = GaudiPython.AppMgr(outputlevel=3)
gaudi.config(options=EOPTS)

# create the moniroting tasks
for alley in ALLEYS:
    for algtype in ALGTYPES:
        alg = desktop.addAlgorithm(gaudi,algtype[0],alley+algtype[1])
        setattr(alg,"alleyname",alley)

# initialize the application and retrieve main tools
gaudi.initialize()
HIS = desktop.ROOTOBJECTS
TES = gaudi.evtsvc()
HLTSUM = gaudi.toolsvc().create("HltSummaryTool",interface="IHltConfSummaryTool")
TISTOS = gaudi.toolsvc().create("TriggerTisTos",interface="ITriggerTisTos")
TIMER = gaudi.toolsvc().create("SequencerTimerTool",interface="ISequencerTimerTool")

alley = hltconf.confAlley(gaudi,"SingleHadron")

# initialize the monitoring tasks
for alg in desktop.ALGOS:
    alleyname = alg.alleyname
    alley = hltconf.confAlley(gaudi,alleyname)
    setattr(alg,"alley",alley)
    setattr(alg,"PATH",PATH)
    setattr(alg,"HLTSUM",HLTSUM)
    setattr(alg,"TISTOS",TISTOS)
    setattr(alg,"TIMER",TIMER)
    setattr(alg,"TES",TES)
    if (getattr(alg,"book",None)): alg.book()

# run the application
gaudi.run(NEVENTS)

