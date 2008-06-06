#! /usr/bin/env python
# =============================================================================
""" 
@brief example of how to monitor time and rate of an HLT1 alley
@author Jose A. Hernando hernando@cern.ch
@date 2008-06-66
"""
# =============================================================================

import gaudimodule
from math import *
import desktop
import histotools
import PlotTools
from ROOT import *

HIS = desktop.ROOTOBJECTS
DEBUG = False

JOB = "$HLTSYSROOT/options/HltJob.opts"
DATACARD = "$hlttime/opts/dst06_bkpi_sel.opts"
#DATACARD = "$dev/opts/dst06_bsmumu_sel.opts"
#DATACARD = "$hlttime/opts/dst06L0_mb_1.opts"

EOPTS = []
EOPTS.append("HltSummaryWriter.SaveAll = true")
EOPTS.append("ApplicationMgr.StatusCodeCheck=false")

gaudi = gaudimodule.AppMgr(outputlevel=3)
gaudi.config(files = [JOB,DATACARD],options=EOPTS)

HLTSUM = gaudi.toolsvc().create("HltSummaryTool",interface="IHltConfSummaryTool")
TIMER = gaudi.toolsvc().create("SequencerTimerTool",interface="ISequencerTimerTool")


ENTRY = "L0TriggerHadron"

ALLEYS = [ "Hlt1HadronSingle",
           "Hlt1HadronDi"]

SEQUENCES = {}
ALGOS = {}

def getAlgos(algos,sequencer):
    """ return the list of algorithms in a sequencer name.
    It enters in the sub-sequencer giving the final list of 
    (type of algorithm, istance name of algorithm)
    """
    seq = gaudi.algorithm(sequencer)
    if (not seq):
        print "ERROR : no sequencer named ", sequencer
    props = seq.properties()
    membersprop = props["Members"]
    members = membersprop.value() 
    for member in members:
        type = member
        algo = member
        i0 = member.find("/")
        if (i0>0):
            type = member[:i0]
            algo = member[i0+1:]
        if (type != "GaudiSequencer"):
            algos.append((type,algo))
        else:
            getAlgos(algos,algo)
    if (DEBUG):
        print " sequencer ",sequencer
        print" algos ",algos

def getSequence(name,sequence):
    """ fill the dictionary ALGOS with the list of (algorithm,HLT-selection)
    @ param name of the alley
    @ param sequence the main name of the sequencer
    """
    algos = []
    getAlgos(algos,sequence)
    SEQUENCES[name] = algos
    def Algos(key):
        ns = []
        algs = SEQUENCES[key]
        for alg in algs:
            a = alg[1]
            sel = None
            ok = gaudi.algorithm(a).properties().has_key("OutputSelection")
            if (ok):
                sel = gaudi.algorithm(a).properties()["OutputSelection"].value()
            ns.append((a,sel))
            if (DEBUG):
                print " algo ",a," isHLT? ",sel
            if (DEBUG): print ns
        return ns
    ALGOS[name] = Algos(name)
        
def bookSequence(key):
    """ book the time, profile and rate histograms of a sequence
    @ param the name of the alley
    """
    algos = map(lambda x: x[0],ALGOS[key])
    xf = 20.
    n = len(algos)+1
    h0 = TH1F(key+"_R",key+" time ",n,0.,n)
    histotools.setXLabels(h0,algos)
    desktop.register( h0 )
    h3 = TH1F(key+"_T",key+" time ",200,0.,xf)
    desktop.register( h3 )
    h = TProfile(key+"_P","time profile "+key,n,0.,1.*n,0.,xf)
    histotools.setXLabels(h,algos)
    desktop.register( h )

def iniSequence(key):
    """ get the sequence algorithms and book histograms
    @ param key is the name of the alley
    NOTE: it assumes that the sequencer name is key+'Sequence'
    """
    getSequence(key,key+"Sequence")
    bookSequence(key)

def anaSequence(key,entry):
    """ analoce a given sequence (fill histograms of time-rate)
    @ param key the name of the alley
    @ param entry the name of the selection to enter in the alley
    """
    t = 0
    ok = HLTSUM.selectionDecision(entry)
    if (not ok): return
    algos = map(lambda x: x[0],ALGOS[key])
    sels = map(lambda x: x[1],ALGOS[key])
    for i in range(len(algos)):
        if (ok):
            algo = algos[i]
            sel  = sels[i]
            ti = TIMER.lastTime(TIMER.indexByName(algos[i]))
            if (abs(ti)<0.001): ti = 0.
            if (abs(ti)>100.): ti = 0.
            t = t+ti
            gaudi.algorithm(algo)
            ok = 1
            if (sel):
                ok = HLTSUM.selectionDecision(sel)
            if (ok): desktop.my(key+"_R").Fill(1.*i,1)
            desktop.my(key+"_P").Fill(i,t)
            if (DEBUG): print " algo ",algo,",",i," ? ",ok," time (ms) ",ti," T ",t
    if (DEBUG): print " total time ",t
    if (t>0.): desktop.my(key+"_T").Fill(t,1.)


gaudi.initialize()
for A in ALLEYS:
    iniSequence(A)

def go(n=1):
    for i in range(n):
        gaudi.run(1)
        for A in ALLEYS:
            anaSequence(A,ENTRY)

def plotSequence(key):
    """ plot the histograms associated to that sequence
    @ param key the name of the alley
    """
    hs = [HIS[key+"_T"],HIS[key+"_P"],HIS[key+"_R"]]
    C = PlotTools.plot(hs,"3:ly")
    C.SaveAs(key+".ps")
    return C
