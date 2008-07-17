# =============================================================================
""" @namespace hltconf
@brief get the configuration of an Hlt alley, its filters
@author Jose A. Hernando hernando@cern.ch
@date 2008-06-16
"""
# =============================================================================

import GaudiPython
from math import *

DEBUG = False

HLTPROPERTIES = ["InputSelection","OutputSelection","InputSelection2",
                 "FilterDescriptor","RecoName","MatchName","HistoDescriptor"]

VARIABLE_RANGE = {"IP":[-0.1,1.9],
                  "PT":[0.,6000.],
                  "Chi2":[0.,5.],
                  "DOCA":[0.,1.],
                  "Fraction":[0.,2.],
                  "Dz":[0.,5.],
                  "Pointing":[0.,1.],
                  "Match":[0.,2.]
                  }

VARIABLE_DEFAULT = 1e16


def defineRange(name,x0,xf):
    VARIABLE_RANGE[name] = [x0,xf]

def confAlley(gaudi,sequencer):
    """ return the list of algos in a sequencer name with is properties
    It enters in the sub-sequencer giving the final list of 
    (type of algorithm, istance name of algorithm)
    """
    algos = []
    confSequencer(gaudi,sequencer,algos)
    if (DEBUG):
        for algo in algos: print algo
    return algos

class hltalgo():
    """ A class to hold the important information of an HltAlgorithm
    (type,name, properties)
    """
    def __init__(self,type,name,props):
        self.type = type
        self.name = name
        self.props = props

    def property(self,name):
        if (not self.props.has_key(name)): return None
        return self.props[name]

    def __repr__(self):
        c = " type: "+str(self.type)+" name: "+str(self.name)
        return c

def confSequencer(gaudi,sequencer,algos):
    """ add in the list of algos objects of 'hltalgo' with the relevant properties
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
        sel  = None
        i0 = member.find("/")
        if (i0>0):
            type = member[:i0]
            algo = member[i0+1:]
        if (type != "GaudiSequencer"):
            props = gaudi.algorithm(algo).properties()
            hltprops = {}
            for hltprop in HLTPROPERTIES:
                if (props.has_key(hltprop)):
                    value = gaudi.algorithm(algo).properties()[hltprop].value()
                    hltprops[hltprop] = value
                    if (DEBUG): print hltprop," = ",value
            algos.append(hltalgo(type,algo,hltprops))
        else:
            confSequencer(gaudi,algo,algos)

def hisrange(name):
    """ return a range for a given name of the title
    """
    for key in VARIABLE_RANGE.keys():
        if (name.find(key)>=0):
            return VARIABLE_RANGE[key]
    return [0.,100.]

    
def writeOpts(alley,alleyConf):
    """ write an opts file with the histogram of the internal monitoring of the ally
    """
    ss = []
    for a in alleyConf:
        def hopt(title):
            ran = hisrange(title)
            s = a.name+'.HistoDescriptor += {"'+title+'":("'+title+'",'+str(ran[0])+','+str(ran[1])+',100)};\n'
            ss.append(s)
        if (a.type.find("Hlt")<0): continue
        s = a.name+".HistogramUpdatePeriod = 1;\n"
        ss.append(s)
        sel = a.property("OutputSelection")
        if (sel): hopt(sel)
        isFilter = (a.type.find("Filter")>0)
        isMaker = (a.type.find("Maker")>0)
        if (isFilter or isMaker):
            filters = a.property("FilterDescriptor")
            if (not filters): continue
            for f in filters:
                cromos = f.split(",")
                filter = cromos[0]
                hopt(filter)
                hopt(filter+"Best")
    f = open("Hlt1"+alley+"InternalMonitor.opts","w")
    for s in ss: f.write(s)
    f.close()


class hltfilter():
    """ class that holds the information of a filter
    passing some candidates return the quantities associated to the filter
    """ 
    def __init__(self,HLTSUM,name,x0=0.,xf=100.):
        cromos = name.split(",")
        self.name = cromos[0]
        self.comparator = cromos[1]
        self.value = cromos[2]
        self.id = int(HLTSUM.confInt("InfoID/"+self.name));
        if (DEBUG): print " filter ID ",name,self.id
        if (len(cromos)>3): self.value2 = cromos[3]
        self.xrange = hisrange(name)

    def vals(self,candis,fillhisto=False):
        vals0 = map(lambda x: x.info(self.id,VARIABLE_DEFAULT),candis)
        vals = filter(lambda x : x!=VARIABLE_DEFAULT, vals0)
        if (fillhisto and self.histo):
            for v in vals: self.histo.Fill(v,1.)
        return vals

    def best(self,candis,fillhisto=False):
        vals = map(lambda x: x.info(self.id,VARIABLE_DEFAULT),candis)
        if (len(vals)<=0): return 0.
        vals.sort()
        best = vals[0]
        if (self.comparator.find(">")>=0): best=vals[-1]
        if (DEBUG): print best
        if (fillhisto and self.histobest): self.histobest.Fill(best,1.)
        return best
