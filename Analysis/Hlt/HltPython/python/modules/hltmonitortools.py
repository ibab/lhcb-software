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
@date 2008-06-1@date 2008-06-16
@revision DaVinci v20r3
"""
# =============================================================================

import GaudiPython
from Gaudi.Configuration import *
from math import *
from ROOT import *
import desktop
import hltconf
from hltconf import stepname,alleySteps
import histotools

DEBUG = False


#----------------------------------------------------
class Rate(GaudiPython.PyAlgorithm):
    def book(self):
        self.steps = alleySteps(self.alley)
        self.stepsnames = map(lambda x: stepname(x),self.steps)
        xf = 20.
        n = len(self.stepsnames)+1
        title = self.alleyname+":Rate"
        h0 = TH1F(title,title,n,0.,n)
        histotools.setXLabels(h0,self.stepsnames)
        desktop.register( h0 )

    def initialize(self):
        self.nevents = 0
        return True

    def execute(self):
        self.nevents += 1
        ok = 1
        for i in range(len(self.steps)):
            if (not ok): continue
            algo = self.steps[i]
            sel = algo.property("OutputSelection")
            if (sel): ok = self.TISTOS.triggerTisTos(sel).decision()
            if (ok): desktop.my(self.alleyname+":Rate").Fill(i,1.)
            if (DEBUG):
               print " Rate: algo ",algo.name,':',stepname(algo),' ? ',ok
        return True

    def finalize(self):
        if (self.nevents>0):
            desktop.hisnorma(self.alleyname+":Rate",1./float(self.nevents))
        return True

#----------------------------------------------------
class RateTOS(GaudiPython.PyAlgorithm):

    def book(self):
        self.steps = alleySteps(self.alley)
        self.stepsnames = map(lambda x: stepname(x),self.steps)
        algos = self.stepsnames
        xf = 20.
        n = len(algos)+1
        title = self.alleyname+":RateTOS"
        h0 = TH1F(title,title,n,0.,n)
        histotools.setXLabels(h0,algos)
        desktop.register( h0 )
        title = self.alleyname+":RateTIS"
        h1 = TH1F(title,title,n,0.,n)
        histotools.setXLabels(h1,algos)
        desktop.register( h1 )
        title = self.alleyname+":RateTISTOS"
        h2 = TH1F(title,title,n,0.,n)
        histotools.setXLabels(h2,algos)
        desktop.register( h2 )
        return    
    
    def initialize(self):
        self.nevents = 0
        return True
    
    def execute(self):
        self.nevents += 1
        Bs = self.TES[self.PATH]
        if (not Bs):
            print " No B candidate "
            return True
        if (not hasattr(Bs,"__getitem__")):
            print " cannot access B candidate "
            return True
        Bs = Bs[0]
        self.TISTOS.setOfflineInput()
        n = Bs.daughters().size()
        daugs = Bs.daughters()
        daugs = map(lambda x: x.target(),daugs)
        for dau in daugs:
            self.TISTOS.addToOfflineInput(dau.clone())
        ok = True
        for i in range(len(self.steps)):
            algo = self.steps[i]
            sel = algo.property("OutputSelection")
            if (not sel): continue
            if (sel): ok = self.TISTOS.selectionTisTos(sel).decision()
            if (not ok): continue
#            btistos = self.TISTOS.triggerTisTos(sel,self.TISTOS.kAllTriggerSelections)
            btistos = self.TISTOS.triggerTisTos(sel)
            if (btistos.tis()):
                desktop.my(self.alleyname+":RateTIS").Fill(i,1.)
            if (btistos.tos()):
                desktop.my(self.alleyname+":RateTOS").Fill(i,1.)
            if (btistos.tis() and btistos.tos()):
                desktop.my(self.alleyname+":RateTISTOS").Fill(i,1.)
            if (DEBUG):
                print " selection ",sel," TIS ",btistos.tis(), " TOS ",btistos.tos() 
        return True

    def finalize(self):
        if (self.nevents>0):
            desktop.hisnorma(self.alleyname+":RateTOS",1./float(self.nevents))
            desktop.hisnorma(self.alleyname+":RateTIS",1./float(self.nevents))
            desktop.hisnorma(self.alleyname+":RateTISTOS",1./float(self.nevents))
        return True
    
#----------------------------------------------------

def typeCandidates(algo):
    """ return the type of candidates
    """
    if (algo.type.find("Track")>0): return "Track"
    if (algo.type.find("L0Calo")>0): return "Track"
    if (algo.type.find("Vert")>0): return "Vertex"
    return ""
    

def getCandidates(algo,TISTOS,isel="OutputSelection"):
    """ return the candidates of an algorithm
    @param algo, hltalgo class
    @param sel, the candidates of the selection
    """
    can = []
    sel = algo.property(isel)
    if (not sel): return can
    type = typeCandidates(algo)
    if (not type): return can
    #if (type == "Track"): return HLTSUM.selectionTracks(sel)
    objs =  TISTOS.hltSelectionObjectSummaries(sel)
    can = map(lambda x: x.summarizedObject(),objs)
    return can

def getTOSCandidates(algo,TISTOS,isel="OutputSelection"):
    """ return the candidates of an algorithm
    @param algo, hltalgo class
    @param sel, the candidates of the selection
    """
    can = []
    sel = algo.property(isel)
    if (not sel): return can
    type = typeCandidates(algo)
    if (not type): return can
    if (type == "Track"): return TISTOS.matchedTOSParticles(sel)
    if (type == "Vertex"): return TISTOS.matchedTOSVertices(sel)



#----------------------------------------------------
class Candidates(GaudiPython.PyAlgorithm):

    def book(self):
        """ book number of candidates per algo in the alley
        @ param the name of the alley, i.e HadronSingle
        """
        self.steps = alleySteps(self.alley)
        stepsnames = map(lambda x: stepname(x),self.steps)
        n = len(self.steps)+1
        title = self.alleyname+":Candidates"
        h = TProfile(title,title,n,0.,1.*n,0.,100.)
        histotools.setXLabels(h,stepsnames)
        desktop.register( h )
        xf = 100.
        for algo in self.steps:
            sel = algo.property("OutputSelection")
            title = self.alleyname+":"+algo.name+":Candidates"
            if (sel):
                hi = TH1F(title,title,100,0.,100.)
                desktop.register( hi )
        return True


    def initialize(self):
        return True

    def execute(self):
        for i in range(len(self.steps)):
            algo = self.steps[i]
            sel = algo.property("OutputSelection")
            can = getCandidates(algo,self.TISTOS)
            ncan = len(can)
            if (ncan<=0): continue
            title = self.alleyname+":"+algo.name+":Candidates"
            desktop.my(title).Fill(1.*ncan,1.)
            desktop.my(self.alleyname+":Candidates").Fill(i,1.*ncan)
            if (DEBUG): print" Candidates: algo",i,sel," Candidates ",ncan
        return True

    def finalize(self):
        return True

#----------------------------------------------------
class CandidatesTOS(GaudiPython.PyAlgorithm):

    def book(self):
        """ book number of candidates per algo in the alley
        @ param the name of the alley, i.e HadronSingle
        """
        self.steps = alleySteps(self.alley)
        stepsnames = map(lambda x: stepname(x),self.steps)
        n = len(self.steps)+1
        title = self.alleyname+":CandidatesTOS"
        h = TProfile(title,title,n,0.,1.*n,0.,100.)
        histotools.setXLabels(h,stepsnames)
        desktop.register( h )
        xf = 100.
        for algo in self.steps:
            sel = algo.property("OutputSelection")
            title = self.alleyname+":"+algo.name+":CandidatesTOS"
            if (sel):
                hi = TH1F(title,title,100,0.,100.)
                desktop.register( hi )
        return True


    def initialize(self):
        return True

    def execute(self):
        for i in range(len(self.steps)):
            algo = self.steps[i]
            sel = algo.property("OutputSelection")
            can = getTOSCandidates(algo,self.TISTOS,"InputSelection")
            ncan = len(can)
            if (ncan<=0): continue
            title = self.alleyname+":"+algo.name+":CandidatesTOS"
            desktop.my(title).Fill(1.*ncan,1.)
            desktop.my(self.alleyname+":CandidatesTOS").Fill(i,1.*ncan)
            if (DEBUG): print" Candidates TOS: algo",i,sel," Candidates ",ncan
        return True

    def finalize(self):
        return True

#----------------------------------------------------
class Time(GaudiPython.PyAlgorithm):

    def book(self):
        algos = map(lambda x: x.name,self.alley)
        xf = 20.
        n = len(algos)+1
        title = self.alleyname+":Time"
        h3 = TH1F(title,title,200,0.,xf)
        desktop.register( h3 )
        title = self.alleyname+":TimeAlley"
        h3a = TH1F(title,title,200,0.,xf)
        desktop.register( h3a )
        title = self.alleyname+":TimeProfile"
        h = TProfile(title,title,n,0.,1.*n,0.,xf)
        histotools.setXLabels(h,algos)
        desktop.register( h )
        for algo in algos:
            title = self.alleyname+":"+algo+":Time"
            hi = TH1F(title,title,200,0.,xf/5.)
            desktop.register( hi )
        return

    def initialize(self):
        return True


    def execute(self):
        t0 = self.TIMER.lastTime(self.TIMER.indexByName("Hlt1"+self.alleyname))
        if (t0>0.001):
            desktop.my(self.alleyname+":TimeAlley").Fill(t0,1.)
            #print " T0 ",self.alleyname,t0
        t = 0.
        ok = 1
        nsteps = 0
        algos = map(lambda x: x.name,self.alley)
        sels = map(lambda x: x.property("OutputSelection"),self.alley)
        for i in range(len(algos)):
            if (ok):
                nstep = i
                algo = algos[i]
                sel  = sels[i]
                ti = self.TIMER.lastTime(self.TIMER.indexByName(algos[i]))
                if (abs(ti)<0.001): ti = 0.
                if (abs(ti)>100.): ti = 0.
                title = self.alleyname+":"+algo+":Time"
                desktop.my(title).Fill(ti,1.)
                t = t+ti
                ok = 1
                if (sel): ok = self.HLTSUM.selectionDecision(sel)
                desktop.my(self.alleyname+":TimeProfile").Fill(i,t)
                if (DEBUG):
                    print " Time: algo ",algo,",",i," ? ",ok," time (ms) ",ti," T ",t
        if (DEBUG): print " Time: total time ",t
        if (t>0. and nstep>0): desktop.my(self.alleyname+":Time").Fill(t,1.)
        return True

    def finalize(self):
        return True


#----------------------------------------------------

class Filters(GaudiPython.PyAlgorithm):

    def book(self):
        for algo in self.alley:
            if (algo.type.find("Filter")>0):
                filters = algo.property("FilterDescriptor")
                algo.filters = []
                if (not filters): continue
                for f in filters:
                    sel0 = algo.property("InputSelection")
                    sel  = algo.property("OutputSelection")
                    fid = hltconf.infoID(self.ANNSVC,f) 
                    fil = hltconf.hltfilter(f,fid)
                    algo.filters.append(fil)
                    title = algo.name+":"+fil.name
                    def histo(title):
                        if (not desktop.ROOTOBJECTS.has_key(title)):
                            hi = TH1F(title,title,100,fil.xrange[0],fil.xrange[1])
                            desktop.register( hi )
                    histo(title)
                    title = algo.name+":"+fil.name+"Best"
                    histo(title)
        return 

    def execute(self):
        for algo in self.alley:
            if (not algo.type.find("Filter")>0): continue
            fils = algo.filters
            candis = getCandidates(algo,self.TISTOS,"InputSelection")
            if (len(candis) <=0): continue
            for fil in fils:
                h0 = desktop.my(algo.name+":"+fil.name)
                h1 = desktop.my(algo.name+":"+fil.name+"Best")
                vals = fil.vals(candis,h0)
                best = fil.best(candis,h1)
                if (DEBUG):
                    print " Filter: ",fil.name," vals ",vals," best ",best
        return True

#----------------------------------------------------
class FiltersTOS(GaudiPython.PyAlgorithm):

    def book(self):
        for algo in self.alley:
            if (algo.type.find("Filter")>0):
                filters = algo.property("FilterDescriptor")
                algo.filters = []
                if (not filters): continue
                for f in filters:
                    sel0 = algo.property("InputSelection")
                    sel  = algo.property("OutputSelection")
                    fid = hltconf.infoID(self.ANNSVC,f)
                    fil = hltconf.hltfilter(f,fid)
                    algo.filters.append(fil)
                    title = algo.name+":"+fil.name+"TOS"
                    def histo(title):
                        if (not desktop.ROOTOBJECTS.has_key(title)):
                            hi = TH1F(title,title,100,fil.xrange[0],fil.xrange[1])
                            desktop.register( hi )
                    histo(title)
                    title = algo.name+":"+fil.name+"Best"+"TOS"
                    histo(title)
        return 

    def execute(self):
        Bs = self.TES[self.PATH]
        if (not Bs):
            print " No B candidate "
            return True
        if (not hasattr(Bs,"__getitem__")):
            print " cannot access B candidate "
            return True
        Bs = Bs[0]
        self.TISTOS.setOfflineInput()
        n = Bs.daughters().size()
        daugs = Bs.daughters()
        daugs = map(lambda x: x.target(),daugs)
        for dau in daugs:
            self.TISTOS.addToOfflineInput(dau.clone())
        for algo in self.alley:
            if (not algo.type.find("Filter")>0): continue
            fils = algo.filters
            candis = getTOSCandidates(algo,self.TISTOS,"InputSelection")
            if (len(candis) <=0): continue
            for fil in fils:
                h0 = desktop.my(algo.name+":"+fil.name+"TOS")
                h1 = desktop.my(algo.name+":"+fil.name+"BestTOS")
                vals = fil.vals(candis,h0)
                best = fil.best(candis,h1)
                if (DEBUG):
                    print " Filter: ",fil.name," vals ",vals," best ",best
        return True
