#!/usr/bin/python
# =============================================================================
""" @namespace hlttime

@brief example of how to monitor rate and time of an Hlt1 alley
@author Jose A. Hernando hernando@cern.ch
@date 2008-06-16
"""
# =============================================================================

import GaudiPython
from math import *
import desktop
import histotools
import PlotTools
import hltconf
from ROOT import *

# Configuration
# Set the alleys to monitor, the entry point,
# Save all the HLT selections in the HltSummary

DEBUG = False
NEVENTS = 10000

ALLEYS = ["HadronSingle","HadronDi"]

JOB = "$HLTSYSROOT/options/HltJob.opts"
DATACARD = "$HLTSYSROOT/options/dst06_bkpi_sel.opts"
#DATACARD = "$HLTSYSROOT/options/dst06_bsmumu_sel.opts"
#DATACARD = "$opts/dst06L0_mb_1.opts"

EOPTS = []
EOPTS.append("HltSummaryWriter.SaveAll = true")
EOPTS.append("ApplicationMgr.StatusCodeCheck=false")

HIS = desktop.ROOTOBJECTS
CONFIG = {}

gaudi = GaudiPython.AppMgr(outputlevel=3)
gaudi.config(files = [JOB,DATACARD],options=EOPTS)


HLTSUM = gaudi.toolsvc().create("HltSummaryTool",interface="IHltConfSummaryTool")
TIMER = gaudi.toolsvc().create("SequencerTimerTool",interface="ISequencerTimerTool")


def bookAlley(alley):
    """ book the time, profile and rate histograms of a sequence
    @ param the name of the alley
    """
    algos = map(lambda x: x[1],CONFIG[alley])
    xf = 20.
    n = len(algos)+1
    h0 = TH1F(alley+"_R",alley+" time ",n,0.,n)
    histotools.setXLabels(h0,algos)
    desktop.register( h0 )
    h3 = TH1F(alley+"_T",alley+" time ",200,0.,xf)
    desktop.register( h3 )
    h = TProfile(alley+"_P","time profile "+alley,n,0.,1.*n,0.,xf)
    histotools.setXLabels(h,algos)
    desktop.register( h )
    for algo in algos:
        if (not HIS.has_key(algo+"_T")):
            hi = TH1F(algo+"_T",algo+" time ",200,0.,xf/5.)
            desktop.register( hi )

def iniAlley(alley):
    """ get the sequence algorithms and book histograms
    @ param alley is the name of the alley
    NOTE: it assumes that the sequencer name is 'Hlt1'+alley+'Sequence'
    """
    CONFIG[alley] = hltconf.confAlley(gaudi,"Hlt1"+alley+"Sequence")
    bookAlley(alley)

def anaAlley(alley):
    """ analize a given sequence (fill histograms of time and rate)
    @ param alley the name of the alley
    """
    t = 0.
    ok = 1
    nsteps = 0
    algos = map(lambda x: x[1],CONFIG[alley])
    def sel(prop):
        if (prop.has_key("OutputSelection")):
            return prop["OutputSelection"]
        return None
    sels = map(lambda x: sel(x[2]),CONFIG[alley])
    for i in range(len(algos)):
        if (ok):
            nstep = i
            algo = algos[i]
            sel  = sels[i]
            ti = TIMER.lastTime(TIMER.indexByName(algos[i]))
            if (abs(ti)<0.001): ti = 0.
            if (abs(ti)>100.): ti = 0.
            desktop.my(algo+"_T").Fill(ti,1)
            t = t+ti
            ok = 1
            if (sel):
                ok = HLTSUM.selectionDecision(sel)
            if (ok): desktop.my(alley+"_R").Fill(1.*i,1)
            desktop.my(alley+"_P").Fill(i,t)
            if (DEBUG): print " algo ",algo,",",i," ? ",ok," time (ms) ",ti," T ",t
    if (DEBUG): print " total time ",t
    if (t>0. and nstep>0): desktop.my(alley+"_T").Fill(t,1.)


def go(n=1):
    for i in range(n):
        gaudi.run(1)
        for A in ALLEYS:
            anaAlley(A)

def plotAlley(alley,prefix="Time_"):
    """ plot the histograms associated to that sequence
    @ param alley the name of the alley
    """
    hs = [HIS[alley+"_T"],HIS[alley+"_P"],HIS[alley+"_R"]]
    C = PlotTools.plot(hs,"3:ly")
    C.SaveAs(prefix+alley+".ps")
    algos = map(lambda x: x[1],CONFIG[alley])
    hs = map(lambda x: HIS[x+"_T"],algos)
    n = (len(hs)/12)+1
    for i in range(n):
        n0 = 12*i
        ni = 12*(i+1)
        ni = min(ni,len(hs))
        hs2 = hs[n0:ni]
        C = PlotTools.plot(hs2)
        C.SaveAs(prefix+alley+"_algos"+str(i)+".ps")
    return C

#
#  main script
#------------------------

if __name__ == '__main__':

    gaudi.initialize()
    for A in ALLEYS: iniAlley(A)
    go(NEVENTS)
    for A in ALLEYS: plotAlley(A)
    gaudi.finalize()
