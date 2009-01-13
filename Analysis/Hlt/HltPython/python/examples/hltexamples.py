#! /usr/bin/env python
# =============================================================================
""" @namespace hltexamples
@brief example of how to get decision, candidates and info from the Hlt1
@author Jose Angel Hernando jose.angel.hernando-morata@cern.ch
@date 2007-07-09
"""
# =============================================================================

import GaudiPython
from math import *
import desktop
import arguments
import benchmarkchannels as bch
import hltconf
import hltmonitortools
from ROOT import *

# select the channel
CHANNEL = arguments.argument("c"," channel name ","Bs2PhiPhi")


# HLT configuration
from Configurables import DaVinci
DV = DaVinci()
DV.DataType = "DC06"
DV.ReplaceL0BanksWithEmulated = True
DV.HltType = 'Hlt1'
DV.applyConf()

# configure the application and the datacards to run on
EOPTS = []
datacards = bch.createOptLines(CHANNEL)
print datacards
EOPTS.append(datacards)

EOPTS.append("HltSummaryWriter.SaveAll = true")
EOPTS.append("HltSelReportsMaker.MaxCandidatesNonDecision=500");

# create the application
gaudi = GaudiPython.AppMgr(outputlevel=3)
gaudi.config(options=EOPTS)

# initialize the application and retrieve main tools
gaudi.initialize()
HIS = desktop.ROOTOBJECTS
TES = gaudi.evtsvc()
HLTSUM = gaudi.toolsvc().create("HltSummaryTool",interface="IHltSummaryTool")
TISTOS = gaudi.toolsvc().create("TriggerTisTos",interface="ITriggerTisTos")
TIMER = gaudi.toolsvc().create("SequencerTimerTool",interface="ISequencerTimerTool")
ANNSVC = gaudi.service("HltANNSvc",IANNSvc)

def infoID(filtername):
    """ returns a function to retrieve the info from the object
    @ param name of the quantity, i.e. 'PT','IP_PV2D'
    """
    return hltconf.infoID(ANNSVC,filtername)

def candidates(algo):
    return hltmonitortoos.getCandidates(algo,HLTSUM)

def l0():
    """ how to get the l0 decision?
        how to get the l0 decision per channel (i.e hadron high threshold)
    """
    l0 = TES["Trig/L0/L0DUReport"]
    print " L0 decision ",l0                 


def l0_candidates():
    """ hot to get the l0 candidates?
    """
    calos = TES["Trig/L0/FullCalo"]
    if (calos):
        hadcalos = filter(lambda calo: (calo.type() == 2),calos)
        print " L0 hadron calo candidates ",len(hadcalos)
        
    muons = TES["Trig/L0/Muon"]
    if (muons):
        print " L0 muon candidates ",muons.size()

def hlt():
    """ how to get the hlt decision?
        how to get the decision of a selection?
        how to get the selections that an event has passed?
    """
    print " HLT decision ", HLTSUM.decision()
    names = HLTSUM.selections()
    for name in names:
        print " \t",name," ? ",HLTSUM.selectionDecision(name)

def hlt_alley_configuration(alleyname):
    """ how to get the filters applied in one selection?
        how to get the input selection used for a selection?
        @ param name of the selection, i.e Hlt1HadronSingleDecision 
    """
    alley = hltconf.confAlley(gaudi,alleyname)
    for algo in alley: print algo
    return

def hlt_candidates_info(alleyname = "SingleHadron",
                        selection="Hlt1SingleHadronDecision",
                        infos=["PT","IP_PV2D"]):
    """ how to get the info of the candidates of a selection?
    i.e what is the PT,IP value of the tracks of the HadPreTrigger selection?
    @ param name of the selection, i.e Hlt1HadronSingleDecision
    """
    alley = hltconf.confAlley(gaudi,alleyname)
    algo = hltconf._produceSelection(alley,selection)
    if (not algo): return
    candis = hltmonitortools.getCandidates(algo,HLTSUM)
    for can in candis:
        for info in infos:
            id = hltconf.infoID(ANNSVC,info)
            print "candidate ",selection,info,can.info(id,-1)

def run(n=10):
    """ run 10 events and print info about L0 and HLT
    @param n number of events to run
    """
    print " *** configuration  *** "
    hlt_alley_configuration("SingleHadron")
    ok = True
    while (ok):
        gaudi.run(1)
        l0()
        ok = not HLTSUM.selectionDecision("Hlt1SingleHadronDecision")
    hlt_candidates_info("SingleHadron","Hlt1SingleHadronDecision")
    
if __name__ == "__main__":
    run()
