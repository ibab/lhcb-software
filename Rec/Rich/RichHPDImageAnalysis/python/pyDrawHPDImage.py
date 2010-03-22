#############################################################
##
##  pyROOT macro to plot HPD image with overlaid fit result
##
##  Usage :
##    python pyPublisHPDImage.py <root-file> <xmlfile 0> ... <xml file n> <hpd>
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

def usage(argv):
    print 'python '+argv[0]+' <root-file> <xml file 0> ... <xml file n> <ID>'
    sys.exit(2)
    
def createEllipse( x0, y0, rad ):
    ellipse = TEllipse( x0, y0, rad, rad ) 
    ellipse.SetFillStyle(0)
    ellipse.SetLineWidth(2)
    return ellipse

def drawHPDImage( rootfilename, hpd , x0, y0, rad ):
    canvas = TCanvas("HPDImage_"+hpd,"HPDImage",500,500)    
    rootfile = TFile( rootfilename )
    histstr = 'RICH/RichHPDImageSummary/Rich_HPD_' + hpd + '_Image'
    hist = rootfile.Get( histstr )
    hist.Draw('colz')
    ellipse = createEllipse( x0, y0, rad )
    ellipse.Draw('same')
    canvas.Update()
    rep = ''
    while not rep in [ 'q', 'Q' ]:
        rep = raw_input( 'enter "q" to quit: ' )
        if 1 < len(rep):
            rep = rep[0]


if len(sys.argv) < 4:
    usage( sys.argv )
else:
    rootfilename = sys.argv[1]
    if not os.path.exists( rootfilename ):
        sys.exit(2)
    xmllist = [] 
    for arg in sys.argv:
        if '.xml' in arg:
            if os.path.exists(arg):
                xmllist += [arg]
    if len(xmllist) == 0:
        sys.exit(2)
    hpd = sys.argv[-1]
    counterdict = countersFromXMLFileList( xmllist )
    isvalidcentre, col0, row0 = hpdCentreInPixels( hpd, counterdict )
    rad = hpdRadiusInPixels( hpd, counterdict )
    if isvalidcentre:
        drawHPDImage( rootfilename, hpd, col0,row0,rad ) 
        
