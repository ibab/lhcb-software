#############################################################
##
##  pyROOT macro to plot HPD image size
##
##  Author: T. Blake
##
#############################################################

from ROOT import TFile, TH2, TEllipse, TCanvas, gStyle, TH1F, gPad
from math import sqrt

import os, sys

from pyHPDXMLParsingUtils import *
from rootStyle import applyRootStyle 
applyRootStyle() 


def drawHPDImageRadius( xmllist ):
    histrad = TH1F("histrad","",100,10,16)
    histrad.Sumw2()
    histrad.SetXTitle("radius [pixels]")
    counterdict = countersFromXMLFileList( xmllist )
    for hpd in range(0,484):
        rad = hpdRadiusInPixels( hpd, counterdict )
        if rad > 0:
            histrad.Fill(rad)
    canrad = TCanvas("canrad","canrad",400,400)
    canrad.cd()
    histrad.Draw()
    histrad.Fit("gaus")
    canrad.Update()
    rep = ''
    while not rep in [ 'q', 'Q' ]:
        rep = raw_input( 'enter "q" to quit: ' )
        if 1 < len(rep):
            rep = rep[0]


if len(sys.argv) < 2:
    usage( sys.argv )
else:
    xmllist = [] 
    for arg in sys.argv:
        if '.xml' in arg:
            if os.path.exists(arg):
                xmllist += [arg]
    if len(xmllist) > 0:
        drawHPDImageRadius(xmllist) 
        
