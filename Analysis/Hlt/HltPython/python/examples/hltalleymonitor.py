#!/usr/bin/python
# =============================================================================
""" @namespace hltalleymonitor

@brief script example of how to monitor rate, time, candidates and values of an Hlt1 alley

This example monitor for a given alley
1) Rate per algorithm
2) Time and time per algorithm
3) Candidates of each selection
4) Quantities of the filters (before apply them) and the best Quantity of a given filter
5) TisTos Rate per algorithm
6) The Decision of the given alleys

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
import hltbchannels
import arguments
from ROOT import *

# Configuration
# Set the alleys to monitor, the entry po,
# Save all the HLT selections in the HltSummary


DEBUG = arguments.argument("dbg"," debug ",False)
NEVENTS = arguments.argument("n"," number of events ",10)
CHANNEL = arguments.argument("c"," channel name ","Bd2KPi")
PROD = arguments.argument("prod"," production version ","")

WRITEOPTS = True
MONITOR_TIME = arguments.argument("mtime"," monitor time ", True)
MONITOR_RATE = arguments.argument("mrate"," monitor rate ", True)
MONITOR_RATETOS = arguments.argument("mtos"," monitor TisTos ", False)
MONITOR_CANDIDATES = arguments.argument("mcan"," monitor candidates ", True)
MONITOR_FILTERS = arguments.argument("mfilter"," monitor Hlt filters ", True)
MONITOR_DECISION = arguments.argument("mdecision"," monitor alleys decision ", True)
PLOT = arguments.argument("plot"," plot and save as ps histograms ", False)
L0ENTRY = arguments.argument("l0"," l0 entry ", "L0HadronDecision")

ALLEYS = arguments.argument_list("a"," alleys list to monitor ", ["HadronSingle","HadronDi"])

JOB = "$HLTSYSROOT/options/HltJob.opts"
#DATACARD = "$HLTSYSROOT/options/dst06_bkpi_sel.opts"
#DATACARD = "$HLTSYSROOT/options/dst06_bsmumu_sel.opts"
FILES = [JOB,]


EFILES,EOPTS,EDATA = hltbchannels.configuration(CHANNEL);
print " configuration files ",EFILES
print " configuration options ",EOPTS
print " configuration data ",EDATA

#FILES = FILES+EFILES

EOPTS.append("HltSummaryWriter.SaveAll = true")
EOPTS.append("ApplicationMgr.StatusCodeCheck=false")
EOPTS.append('HistogramPersistencySvc.OutputFile = "hltalleymonitor'+CHANNEL+PROD+'.root"')
#EOPTS.append('HistogramPersistencySvc.OutputFile = "'+CHANNEL+'hltalleymonitor.root";')

HIS = desktop.ROOTOBJECTS
CHANNELPATH = hltbchannels.TESPath[CHANNEL]
ALGOS = {}

gaudi = GaudiPython.AppMgr(outputlevel=3)
gaudi.config(files = FILES, options=EOPTS)
gaudi.initialize()

TES = gaudi.evtsvc()
HLTSUM = gaudi.toolsvc().create("HltSummaryTool",interface="IHltConfSummaryTool")
TIMER = gaudi.toolsvc().create("SequencerTimerTool",interface="ISequencerTimerTool")
TISTOS = gaudi.toolsvc().create("TriggerTisTos",interface="ITriggerTisTos")

#
#  Booking
#

def ini():
    """ book and initialize
    """
    for A in ALLEYS: iniAlley(A)
    if (MONITOR_DECISION): bookDecision()

def iniAlley(alley):
    """ get the sequence algorithms and book histograms
    @ param alley is the name of the alley
    NOTE: it assumes that the sequencer name is 'Hlt1'+alley+'Sequence'
    """
    ALGOS[alley] = hltconf.confAlley(gaudi,"Hlt1"+alley+"Sequence")
    if (MONITOR_RATE): bookRate(alley)
    if (MONITOR_RATETOS): bookRateTOS(alley)
    if (MONITOR_TIME): bookTime(alley)
    if (MONITOR_CANDIDATES): bookCandidates(alley)
    if (MONITOR_FILTERS): bookFilters(alley)
    if (WRITEOPTS): writeOpts(alley)

def writeOpts(alley):
    """ write an opts file to internally monitor the alley
    """
    hltconf.writeOpts(alley,ALGOS[alley])

def bookRate(alley):
    """ book the time, profile and rate histograms of a sequence
    @ param the name of the alley: i.e HadronSingle
    """
    algos = map(lambda x: x.name,ALGOS[alley])
    xf = 20.
    n = len(algos)+1
    title = alley+":Rate"
    h0 = TH1F(title,title,n,0.,n)
    histotools.setXLabels(h0,algos)
    desktop.register( h0 )
    return

def bookRateTOS(alley):
    algos = map(lambda x: x.name,ALGOS[alley])
    xf = 20.
    n = len(algos)+1
    title = alley+":RateTOS"
    h0 = TH1F(title,title,n,0.,n)
    histotools.setXLabels(h0,algos)
    desktop.register( h0 )
    title = alley+":RateTIS"
    h1 = TH1F(title,title,n,0.,n)
    histotools.setXLabels(h1,algos)
    desktop.register( h1 )
    title = alley+":RateTISTOS"
    h2 = TH1F(title,title,n,0.,n)
    histotools.setXLabels(h2,algos)
    desktop.register( h2 )
    return    

def bookCandidates(alley):
    """ book number of candidates per algo in the alley
    @ param the name of the alley, i.e HadronSingle
    """
    xf = 100.
    n = len(ALGOS[alley])+1
    for algo in ALGOS[alley]:
        sel = algo.property("OutputSelection")
        if (not sel): continue
        title = algo.name+":"+sel
        if (sel and not HIS.has_key(title)):
            hi = TH1F(title,title,100,0.,100.)
            desktop.register( hi )
    return
         
def bookFilters(alley):
    """ book quantities of a filter
    @param: name of the alley HadronSingle
    """
    algos = ALGOS[alley]
    for algo in algos:
        if (algo.type.find("Filter")>0):
            filters = algo.property("FilterDescriptor")
            algo.filters = []
            if (not filters): continue
            for f in filters:
                sel0 = algo.property("InputSelection")
                sel  = algo.property("OutputSelection")
                fil = hltconf.hltfilter(HLTSUM,f)
                algo.filters.append(fil)
                title = algo.name+":"+fil.name
                def histo(title):
                    if (not HIS.has_key(title)):
                        hi = TH1F(title,title,100,fil.xrange[0],fil.xrange[1])
                        desktop.register( hi )
                    hi = desktop.my(title)
                    return hi
                fil.histo = histo(title)
                title = algo.name+"Best:"+fil.name+"Best"
                fil.histobest = histo(title)

    return

def bookTime(alley):
    """ book the time and profile of the algos of an alley
    @ param the name of the alley, i.e HadronSingle
    """
    algos = map(lambda x: x.name,ALGOS[alley])
    xf = 20.
    n = len(algos)+1
    title = alley+":Time"
    h3 = TH1F(title,title,200,0.,xf)
    desktop.register( h3 )
    title = alley+":TimeProfile"
    h = TProfile(title,title,n,0.,1.*n,0.,xf)
    histotools.setXLabels(h,algos)
    desktop.register( h )
    for algo in algos:
        title = algo+":Time"
        if (not HIS.has_key(title)):
            hi = TH1F(title,title,200,0.,xf/5.)
            desktop.register( hi )
    return

def bookDecision():
    """ book the decision of the list of the alleys, including the OR and AND
    """
    names = [L0ENTRY,]
    for x in ALLEYS: names.append("Hlt1"+x+"Decision")
    names.append("OR")
    names.append("AND")
    n = len(names)+1
    h = TH1F("AlleysDecision","AlleysDecision",n,0.,1.*n)
    histotools.setXLabels(h,names)
    desktop.register( h )
            
    
##
## --------- Analyze ----------
##

def go(n=1):
    for i in range(n):
        gaudi.run(1)
        desktop.unlockHistos()
        for A in ALLEYS: anaAlley(A)
        if (MONITOR_DECISION): anaDecision()
    return

def anaAlley(alley):
    if (MONITOR_RATE): anaRate(alley)
    if (MONITOR_RATETOS): anaRateTOS(alley)
    if (MONITOR_TIME): anaTime(alley)
    if (MONITOR_CANDIDATES): anaCandidates(alley)
    if (MONITOR_FILTERS): anaFilters(alley)

def anaRate(alley):
    """ ana the rate of an alley
    @param alley 
    """
    ok = 1
    for i in range(len(ALGOS[alley])):
        if (not ok): continue
        algo = ALGOS[alley][i]
        sel = algo.property("OutputSelection")
        if (sel): ok = HLTSUM.selectionDecision(sel)
        if (ok): desktop.my(alley+":Rate").Fill(i,1.)
        if (DEBUG):
            print " Rate: algo ",algo.name,' ? ',ok
    return 

def anaRateTOS(alley):
    """ ana the rate of an alley
    @param alley 
    """
    Bs = TES[CHANNELPATH]
    if (not Bs): return
    print Bs[0]
    TISTOS.addToOfflineInput(Bs[0])
    ok = 1
    for i in range(len(ALGOS[alley])):
        if (not ok): continue
        algo = ALGOS[alley][i]
        sel = algo.property("OutputSelection")
        if (sel): ok = HLTSUM.selectionDecision(sel)
        if (not ok): continue
        btistos = TISTOS.triggerTisTos(sel,TISTOS.kAllTriggerSelections)
        if (btistos.tis()):
            desktop.my(alley+":RateTIS").Fill(i,1.)
        if (btistos.tos()):
            desktop.my(alley+":RateTOS").Fill(i,1.)
        if (btistos.tis() and btistos.tos()):
            desktop.my(alley+":RateTISTOS").Fill(i,1.)
        if (DEBUG):
            print " selection ",sel," TIS ",btistos.tis(), " TOS ",btistos.tos() 
    return 

def anaTime(alley):
    """ analize a given sequence (fill histograms of time)
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
            desktop.hfill(algo+":Time",ti,1.,True)
            t = t+ti
            ok = 1
            if (sel): ok = HLTSUM.selectionDecision(sel)
            desktop.my(alley+":TimeProfile").Fill(i,t)
            if (DEBUG): print " Time: algo ",algo,",",i," ? ",ok," time (ms) ",ti," T ",t
    if (DEBUG): print " Time: total time ",t
    if (t>0. and nstep>0): desktop.my(alley+":Time").Fill(t,1.)
    return

def getCandidates(algo,sel=None):
    """ return the candidates of an algorithm
    @param algo, hltalgo class
    @param sel, the candidates of the selection
    """
    can = []
    if (not sel): sel = algo.property("OutputSelection")
    if (not sel): return can
    if (algo.type.find("Track")>0):
        can = HLTSUM.selectionTracks(sel)
    if (algo.type.find("Vertex")>0):
        can = HLTSUM.selectionTracks(sel)
    return can

def anaCandidates(alley):
    """ ana the candidates
    @param: name of the alley
    """
    for algo in ALGOS[alley]:
        sel = algo.property("OutputSelection")
        if (not sel): continue
        can = getCandidates(algo)
        if (len(can)<=0): return
        title = algo.name+":"+sel
        desktop.hfill(title,1.*len(can),1.,True)
        if (DEBUG): print " Candidates: algo",sel," Candidates ",len(can)
    return

def anaFilters(alley):
    """ ana the filters
    """
    for algo in ALGOS[alley]:
        if (not algo.type.find("Filter")>0): continue
        fils = algo.filters
        sel0 = algo.property("InputSelection")
        candis = getCandidates(algo,sel0)
        for fil in fils:
            if (fil.histobest.look): continue
            vals = fil.vals(candis,fillhisto=True)
            best = fil.best(candis,fillhisto=True)
            fil.histo.lock = True
            fil.histobest.lock = True
            if (DEBUG): print " Filter: ",fil.name," vals ",vals," best ",best
    return


def anaDecision():
    names = [L0ENTRY,]
    for a in ALLEYS: names.append("Hlt1"+a+"Decision")
    okOR = false
    okAND = true
    n = len(names)
    for i in range(n):
        name = names[i]
        ok = HLTSUM.selectionDecision(name)
        if (i>0): okOR = okOR or ok
        okAND = okAND and ok
        if (ok): desktop.hfill("AlleysDecision",i,1.,False)
    if (okOR): desktop.hfill("AlleysDecision",n,1.,False)
    if (okAND): desktop.hfill("AlleysDecision",n+1,1.,False)
    if (DEBUG): print " Decision OR ",okOR," Decision AND ",okAND

#
# finalize
#---------------------------
def fin():
    if (MONITOR_DECISION): finDecision()
    for alley in ALLEYS: finAlley(alley)

def finAlley(alley):
    if (MONITOR_RATE): finRate(alley)
    if (MONITOR_RATETOS): finRateTOS(alley)

def hisnorma(title,norma):
    h = desktop.my(title)
    h.Scale(norma)

def finDecision():
    hisnorma("AlleysDecision",1./float(NEVENTS))

def finRate(alley):
    hisnorma(alley+":Rate",1./float(NEVENTS))

def finRateTOS(alley):
    hisnorma(alley+":RateTOS",1./float(NEVENTS))
    hisnorma(alley+":RateTIS",1./float(NEVENTS))
    hisnorma(alley+":RateTISTOS",1./float(NEVENTS))
    
#
# Plotting
#---------------------------

def plot():
    if (not PLOT): return
    for alley in ALLEYS: plotAlley(alley)
    plotDecision()

def plotAlley(alley):
    if (not PLOT): return
    if (MONITOR_TIME): plotTime(alley)
    if (MONITOR_RATE): plotRate(alley)
    if (MONITOR_RATETOS): plotRateTOS(alley)
    if (MONITOR_CANDIDATES): plotCandidates(alley)
    if (MONITOR_FILTERS): plotFilters(alley)    
    return

def plotHistos(hs,name="",prefix="",endfix=""):
    n = (len(hs)/12)+1
    for i in range(n):
        n0 = 12*i
        ni = 12*(i+1)
        ni = min(ni,len(hs))
        hs2 = hs[n0:ni]
        C = PlotTools.plot(hs2)
        end2fix = ""
        if (i>0): end2fix =str(i)
        C.SaveAs(CHANNEL+prefix+name+endfix+end2fix+".ps")
    return C

def plotTime(alley):
    """ plot the histograms associated to that sequence
    @ param alley the name of the alley
    """
    hs = [HIS[alley+":Time"],HIS[alley+":TimeProfile"]]
    C = plotHistos(hs,alley,prefix="Time")
    algos = map(lambda x: x.name,ALGOS[alley])
    hs = map(lambda x: HIS[x+":Time"],algos)
    C = plotHistos(hs,name=alley,prefix="Time",endfix="Algos")      
    return C

def plotRate(alley):
    hs = [HIS[alley+":Rate"]]
    C = plotHistos(hs,name=alley,prefix="Rate")
    return

def plotRateTOS(alley):
    hs = [HIS[alley+":RateTOS"],HIS[alley+":RateTIS"],HIS[alley+":RateTISTOS"]]
    C = plotHistos(hs,name=alley,prefix="RateTOS")
    return

def plotCandidates(alley):
    his = []
    for algo in ALGOS[alley]:
        sel = algo.property("OutputSelection")
        if (not sel): continue
        title = algo.name+":"+sel
        if (sel): his.append(HIS[title])
    plotHistos(his,name=alley,prefix="Candidates",endfix="Algos")
    return

def plotFilters(alley):
    his = []
    for algo in ALGOS[alley]:
        if (algo.type.find("Filter")>0):
            for fil in algo.filters:
                his.append(fil.histo)
                his.append(fil.histobest)
    plotHistos(his,name=alley,prefix="Filters",endfix="Algos")
    return

def plotDecision():
    h = desktop.my("AlleysDecision")
    plotHistos([h,],"AlleysDecision")

#
#  main script
#------------------------

def run():
    ini()
    go(NEVENTS)
    fin()
    plot()
    gaudi.exit()

if __name__ == '__main__':
    run()

