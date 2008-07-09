# =============================================================================
""" @namespace hltconf

@brief given alley name return the list of algorithms are in the alley
[type,name,hlt configuration], where the hlt configuration is a dictionary with the properties of the hlt algorithm (i.e Inputselection or OutputSelection)
@author Jose A. Hernando hernando@cern.ch
@date 2008-06-16
"""
# =============================================================================

import GaudiPython
from math import *

DEBUG = False

HLTPROPERTIES = ["InputSelection","OutputSelection","InputSelection2",
                 "FilterDescriptor","RecoName","MatchName","HistoDescriptor"]

def confAlley(gaudi,sequencer):
    """ return the list of algorithms in a sequencer name with is properties
    It enters in the sub-sequencer giving the final list of 
    (type of algorithm, istance name of algorithm)
    """
    algos = []
    confSequencer(gaudi,sequencer,algos)
    if (DEBUG):
        for algo in algos: print algo
    return algos

class hltalgo():
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

    
