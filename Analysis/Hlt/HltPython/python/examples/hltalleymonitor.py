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

DEBUG = True
NEVENTS = 10
MONITOR_TIME = True
MONITOR_RATE = True
MONITOR_CANDIDATES = True
MONITOR_FILTERS = True

ALLEYS = ["HadronSingle",]

BOOKINGS = {"IP":[-0.1,1.9],
            "PT":[0.,6000.],
            "Chi2":[0.,5.],
            "DOCA":[0.,1.],
            "Fraction":[0.,2.]
            }

MINVAL = 1e-16

JOB = "$HLTSYSROOT/options/HltJob.opts"
DATACARD = "$HLTSYSROOT/options/dst06_bkpi_sel.opts"
#DATACARD = "$HLTSYSROOT/options/dst06_bsmumu_sel.opts"
#DATACARD = "$opts/dst06L0_mb_1.opts"

EOPTS = []
EOPTS.append("HltSummaryWriter.SaveAll = true")
EOPTS.append("ApplicationMgr.StatusCodeCheck=false")

HIS = desktop.ROOTOBJECTS
ALGOS = {}

gaudi = GaudiPython.AppMgr(outputlevel=3)
gaudi.config(files = [JOB,DATACARD],options=EOPTS)
gaudi.initialize()

HLTSUM = gaudi.toolsvc().create("HltSummaryTool",interface="IHltConfSummaryTool")
TIMER = gaudi.toolsvc().create("SequencerTimerTool",interface="ISequencerTimerTool")

def bookRate(alley):
    """ book the time, profile and rate histograms of a sequence
    @ param the name of the alley
    """
    algos = map(lambda x: x.name,ALGOS[alley])
    xf = 20.
    n = len(algos)+1
    title = alley+"/Rate"
    h0 = TH1F(title,title,n,0.,n)
    histotools.setXLabels(h0,algos)
    desktop.register( h0 )
    return

def bookCandidates(alley):
    """ book the time, profile and rate histograms of a sequence
    @ param the name of the alley
    """
    xf = 100.
    n = len(ALGOS[alley])+1
    for algo in ALGOS[alley]:
        sel = algo.property("OutputSelection")
        if (not sel): continue
        title = algo.name+"/"+sel
        if (sel and not HIS.has_key(title)):
            hi = TH1F(title,title,100,0.,100.)
            desktop.register( hi )
    return

class hltfilter():
    def __init__(self,name,x0=0.,xf=100.):
        def hisrange(name):
            for key in BOOKINGS.keys():
                if (name.find(key)>=0):
                    return BOOKINGS[key]
            return [0.,100.]
        cromos = name.split(",")
        self.name = cromos[0]
        self.comparator = cromos[1]
        self.value = cromos[2]
        if (len(cromos)>3): self.value2 = cromos[3]
        self.id = HLTSUM.confInt("InfoID/"+name);
        self.xrange = hisrange(name)

    def vals(self,candis,fillhisto=True):
        vals = map(lambda x: x.info(self.id,MINVAL),candis)
        if (fillhisto and self.histo):
            for v in vals: self.histo.Fill(v,1.)
        return vals

    def best(self,candis,fillhisto=True):
        vals = map(lambda x: x.info(self.id,MINVAL),candis)
        if (len(vals)<=0): return 0.
        vals.sort()
        best = vals[0]
        if (self.comparator.find(">")): best=vals[-1]
        if (DEBUG): print best
        if (fillhisto and self.histobest): self.histobest.Fill(best,1.)
        return best
                
def bookFilters(alley):
    
    algos = ALGOS[alley]
    for algo in algos:
        if (algo.type.find("Filter")>0):
            filters = algo.property("FilterDescriptor")
            algo.filters = []
            if (not filters): continue
            for f in filters:
                sel0 = algo.property("InputSelection")
                sel  = algo.property("OutputSelection")
                fil = hltfilter(f)
                algo.filters.append(fil)
                title = algo.name+"/"+fil.name
                hi = TH1F(title,title,100,fil.xrange[0],fil.xrange[1])
                desktop.register( hi )
                fil.histo = hi
                title = algo.name+"Best/"+fil.name+"Best"
                hi2 = TH1F(title,title,100,fil.xrange[0],fil.xrange[1])
                desktop.register( hi2 )
                fil.histobest = hi2
    return

def bookTime(alley):
    """ book the time, profile and rate histograms of a sequence
    @ param the name of the alley
    """
    algos = map(lambda x: x.name,ALGOS[alley])
    xf = 20.
    n = len(algos)+1
    title = alley+"/Time"
    h3 = TH1F(title,title,200,0.,xf)
    desktop.register( h3 )
    title = alley+"/TimeProfile"
    h = TProfile(title,title,n,0.,1.*n,0.,xf)
    histotools.setXLabels(h,algos)
    desktop.register( h )
    for algo in algos:
        title = algo+"/Time"
        if (not HIS.has_key(title)):
            hi = TH1F(title,title,200,0.,xf/5.)
            desktop.register( hi )
    return

def iniAlley(alley):
    """ get the sequence algorithms and book histograms
    @ param alley is the name of the alley
    NOTE: it assumes that the sequencer name is 'Hlt1'+alley+'Sequence'
    """
    ALGOS[alley] = hltconf.confAlley(gaudi,"Hlt1"+alley+"Sequence")
    if (MONITOR_RATE): bookRate(alley)
    if (MONITOR_TIME): bookTime(alley)
    if (MONITOR_CANDIDATES): bookCandidates(alley)
    if (MONITOR_FILTERS): bookFilters(alley)
    
## --------- Analyze ----------

def anaRate(alley):
    ok = 1
    for i in range(len(ALGOS[alley])):
        if (not ok): continue
        algo = ALGOS[alley][i]
        sel = algo.property("OutputSelection")
        if (sel): ok = HLTSUM.selectionDecision(sel)
        if (ok): desktop.my(alley+"/Rate").Fill(i,1.)
        if (DEBUG):
            print " Rate: algo ",algo.name,' ? ',ok
    return 

def anaTime(alley):
    """ analize a given sequence (fill histograms of time and rate)
    @ param alley the name of the alley
    """
    t = 0.
    ok = 1
    nsteps = 0
    algos = map(lambda x: x.name,ALGOS[alley])
    sels = map(lambda x: x.property("OutputSelection"),ALGOS[alley])
    for i in range(len(algos)):
        if (ok):
            nstep = i
            algo = algos[i]
            sel  = sels[i]
            ti = TIMER.lastTime(TIMER.indexByName(algos[i]))
            if (abs(ti)<0.001): ti = 0.
            if (abs(ti)>100.): ti = 0.
            desktop.my(algo+"/Time").Fill(ti,1)
            t = t+ti
            ok = 1
            if (sel): ok = HLTSUM.selectionDecision(sel)
            desktop.my(alley+"/TimeProfile").Fill(i,t)
            if (DEBUG): print " Time: algo ",algo,",",i," ? ",ok," time (ms) ",ti," T ",t
    if (DEBUG): print " Time: total time ",t
    if (t>0. and nstep>0): desktop.my(alley+"/Time").Fill(t,1.)
    return

def getCandidates(algo,sel=None):
    can = []
    if (not sel): sel = algo.property("OutputSelection")
    if (not sel): return can
    if (algo.type.find("Track")>0):
        can = HLTSUM.selectionTracks(sel)
    if (algo.type.find("Vertex")>0):
        can = HLTSUM.selectionTracks(sel)
    return can

def anaCandidates(alley):
    for algo in ALGOS[alley]:
        sel = algo.property("OutputSelection")
        if (not sel): continue
        can = getCandidates(algo)
        title = algo.name+"/"+sel
        desktop.my(title).Fill(1.*len(can),1.)
        if (DEBUG): print " Candidates: algo",sel," Candidates ",len(can)
    return

def anaFilters(alley):
    def fill(x,histo):
        if (x>MINVAL): histo.Fill(x,1.)
    for algo in ALGOS[alley]:
        if (not algo.type.find("Filter")>0): continue
        fils = algo.filters
        sel0 = algo.property("InputSelection")
        candis = getCandidates(algo,sel0)
        for fil in fils:
            vals = fil.vals(candis,fillhisto=True)
            best = fil.best(candis,fillhisto=True)
            if (DEBUG): print " Filter: ",fil.name," vals ",vals," best ",best
    return

def go(n=1):
    for i in range(n):
        gaudi.run(1)
        for A in ALLEYS:
            if (MONITOR_RATE): anaRate(A)
            if (MONITOR_TIME): anaTime(A)
            if (MONITOR_CANDIDATES): anaCandidates(A)
            if (MONITOR_FILTERS): anaFilters(A)
    return

#
# Plotting
#---------------------------

def plotHistos(hs,alley,prefix="",endfix=""):
    n = (len(hs)/12)+1
    for i in range(n):
        n0 = 12*i
        ni = 12*(i+1)
        ni = min(ni,len(hs))
        hs2 = hs[n0:ni]
        C = PlotTools.plot(hs2)
        C.SaveAs(prefix+alley+endfix+str(i)+".ps")
    return C

def plotTime(alley):
    prefix = "Time_"
    """ plot the histograms associated to that sequence
    @ param alley the name of the alley
    """
    hs = [HIS[alley+"/Time"],HIS[alley+"/TimeProfile"]]
    C = PlotTools.plot(hs)
    C.SaveAs(prefix+alley+".ps")
    algos = map(lambda x: x.name,ALGOS[alley])
    hs = map(lambda x: HIS[x+"/Time"],algos)
    C = plotHistos(hs,alley,"Time","Algos")      
    return C

def plotRate(alley):
    prefix = "Rate_"
    hs = [HIS[alley+"/Rate"]]
    C = PlotTools.plot(hs)
    C.SaveAs(prefix+alley+".ps")
    return

def plotCandidates(alley):
    his = []
    for algo in ALGOS[alley]:
        sel = algo.property("OutputSelection")
        if (not sel): continue
        title = algo.name+"/"+sel
        if (sel): his.append(HIS[title])
    plotHistos(his,alley,"Candidates","Algos")
    return

def plotFilters(alley):
    his = []
    for algo in ALGOS[alley]:
        if (algo.type.find("Filter")>0):
            for fil in algo.filters:
                his.append(fil.histo)
                his.append(fil.histobest)
    plotHistos(his,alley,"Filters","Algos")
    return

def plotAlley(alley):
    if (MONITOR_TIME): plotTime(alley)
    if (MONITOR_RATE): plotRate(alley)
    if (MONITOR_CANDIDATES): plotCandidates(alley)
    if (MONITOR_FILTERS): plotFilters(alley)    
    return

#
#  main script
#------------------------

# if __name__ == '__main__':

def run():
    for A in ALLEYS: iniAlley(A)
    go(NEVENTS)
    for A in ALLEYS: plotAlley(A)
    gaudi.finalize()
