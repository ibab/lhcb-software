#############################################################
##
##  pyROOT macro to plot HPD image with overlaid fit result
##
##  Author: T. Blake
##
##  
#############################################################

from ROOT import TFile, TH2, TEllipse, TCanvas, gStyle, TH1F, gPad
from math import sqrt

import os, sys

from pyHPDXMLParsingUtils import *

from rootStyle import applyRootStyle 
applyRootStyle() 

richcopydict = { 'R1U' : 0 , 'R1D' : 98 , 'R2A' : 196, 'R2C' : 340 }
richhpdsdict = { 'R1U' : 14 , 'R1D' : 14, 'R2A' : 16, 'R2D' : 16 }
 

def usage(argv):
    print 'python '+argv[0]+' <root-file> <xml 0> ... <xml n> <ID> <rich=1 or 2> <box=0 or 1> <col=0 ..>'
    sys.exit(2)

def getHistogramFromFile( rootfile, hpd ):
    histstr = 'RICH/RichHPDImageSummary/Rich_HPD_' + str(hpd) + '_Image'
    hist = rootfile.Get(histstr)
    return hist 

def createEllipse( x0, y0, rad ):
    ellipse = TEllipse( x0, y0, rad, rad ) 
    ellipse.SetFillStyle(0)
    ellipse.SetLineWidth(2)
    return ellipse

def getHistsForDisplay( rootfile, hpdFirst, hpdLast ):
    hists = []
    for hpd in range( hpdFirst, hpdLast ):
        hists += [ getHistogramFromFile(rootfile,hpd) ] 
    return hists

def getEllipsesForOverlay( xmlfilelist, hpdFirst, hpdLast ):
    counterdict = countersFromXMLFileList( xmllist )
    ellipses = []
    for hpd in range( hpdFirst, hpdLast ):
        isvalidcentre, col0, row0 = hpdCentreInPixels( hpd, counterdict )
        rad = hpdRadiusInPixels( hpd, counterdict )
        ellipses += [ createEllipse(col0, row0, rad) ]
    return ellipses 
    
def drawHPDColumn( xmlfilelist, rootfilename, firstHPD, lastHPD ):
    rootfile = TFile( rootfilename )
    hists = getHistsForDisplay(rootfile,firstHPD,lastHPD)
    ellipses = getEllipsesForOverlay(xmlfilelist,firstHPD,lastHPD) 
    canvas = TCanvas("SumamryCanvas","SummaryCanvas",1000,400)
    canvas.Divide( int(len(hists)/2), 2 ) 
    for i in range(0, len(hists)):
        canvas.cd( i+1 )
        gPad.SetLogz()
        hists[i].Draw('colz')
        ellipses[i].Draw("same")
        canvas.Update()
    rep = ''
    while not rep in [ 'q', 'Q' ]:
        rep = raw_input( 'enter "q" to quit: ' )
        if 1 < len(rep):
            rep = rep[0]

if len(sys.argv) < 5:
    usage( sys.argv )
else:
    rootfilename = sys.argv[1]
    if not os.path.exists( rootfilename ):
        sys.exit(2)
    xmllist = []
    for arg in sys.argv:
        if '.xml' in arg:
            if os.path.exists(arg):
                xmllist += [ arg ]
    if 0 == len(xmllist):
        sys.exit(2)
    box = sys.argv[len(sys.argv)-2]
    col = sys.argv[len(sys.argv)-1]
    firstHPD = richcopydict[box] + int(col)*richhpdsdict[box]
    lastHPD  = firstHPD + richhpdsdict[box]  
    drawHPDColumn( xmllist, rootfilename, firstHPD, lastHPD )

    
  
