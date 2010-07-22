from ROOT import TFile, TGraph, TCanvas

###################################################
##
## python / ROOT macro to produce a trend plot for 
## HPD image centres from multiple input files.
##
## author: T. Blake
##
###################################################


import sys
import os

from pyHistoParsingUtils import hpdLocalOffset

def usage(args):
    print 'Invalid arguments, correct usage is:'
    print 'python -i ' + args[0] + ' <copy-nr> <rootfile-0> .. <rootfile-n>'


if len(sys.argv) < 3 or 'root' in sys.argv[1]:
    usage(sys.argv)
    sys.exit(2)
    
    
hpdcopynr = int(sys.argv[1])

grx0 = TGraph()
gry0 = TGraph()

ipoint = 0

for rootfilenr in range(2,len(sys.argv)):
    x0 = 0
    y0 = 0
    rootfilename = sys.argv[rootfilenr]
    if not os.path.exists(rootfilename):
        continue
    else:
        rootfile = TFile(rootfilename)
        try:
            x0, y0 = hpdLocalOffset(rootfile, hpdcopynr)
        except ZeroDivisionError:
            print hpdcopynr, ' has no valid centre'
        except:
            print 'Unexpected error has occured'
        else:
            grx0.SetPoint(ipoint,ipoint,x0)
            gry0.SetPoint(ipoint,ipoint,y0)
            ipoint += 1

# publish result

can_trend = TCanvas('can_trend')
can_trend.cd()
grx0.Draw('ap')
grx0.GetXaxis().SetTitle('file nr in sequence')
grx0.GetYaxis().SetTitle('offset')
grx0.SetMaximum(2.0)
grx0.SetMinimum(-2.0)
gry0.Draw('p+')
gry0.SetMarkerColor(2)
gry0.SetLineColor(2)
can_trend.Update()
        
