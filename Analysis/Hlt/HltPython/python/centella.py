#! /usr/bin/env python
from math import *
from ROOT import TH1F,TFile

DEBUG = False

HISTOS = {}
COUNTERS = {}
TOOLS = {}

def book1D(name,nbins,x0,xf,title=""):
    """ book 1D histogram and keep it in HISTOS dictionary
    """
    if (title == ""): title = name 
    HISTOS[name] = TH1F("h1d"+name,title,nbins,x0,xf)

def fill1D(name,x,w=1.):
    """ fill histogram stored in HISTOS dictionary
    """
    if (not HISTOS.has_key(name)): return
    HISTOS[name].Fill(x,w)

def draw(name):
    HISTOS[name].Draw()

def initCounter(name, n = 0):
    """ init counter
    """
    COUNTERS[name] = n

def increaseCounter(name,n=1):
    """ increase counter
    """
    COUNTERS[name] = COUNTERS[name] + n 

def tool(gaudi,toolname,toolinterface):
    """ retrieve a tool
    """
    if (TOOLS.has_key(toolname)): return TOOLS[toolname]
    TOOLS[toolname] = gaudi.toolsvc().create(toolname,interface=toolinterface)
    return TOOLS[toolname]

def saveHistos(fname="centella.root"):
    """ save histos in HISTOS in a root file
    """ 
    file = TFile(name,"w")
    for name in HISTOS.keys():
        HISTOS[name].save()
    
    
