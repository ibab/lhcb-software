#!/usr/bin/env python
# =============================================================================
## @file
#  simple PyROOT script to visualize the histograms 
#  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
#  @date 2008-06-07
# =============================================================================
"""

Simple PyROOT script to visualize the histograms 

Configuration file for LoKiExample package

This file is a part of LoKi project - 
\"C++ ToolKit  for Smart and Friendly Physics Analysis\"

The package has been designed with the kind help from
Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas, 
contributions and advices from G.Raven, J.van Tilburg, 
A.Golutvin, P.Koppenburg have been used in the design.

By usage of this code one clearly states the disagreement 
with the campain of Dr.O.Callot et al.: 
\"No Vanya's lines are allowed in LHCb/Gaudi software.\"

"""
# =============================================================================
__author__  = " Vanya BELYAEV Ivan.Belyaev@nikhef.nl "
__version__ = " CVS Tag $Name: not supported by cvs2svn $, version $Revision: 1.1 $ "
# =============================================================================
import ROOT,math


def convert( h ) :
    g=h.Clone()
    g.Reset()
    nb = h.GetNbinsX()
    total=h.Integral() 
    for i in range(1,nb+1) :
        v=h.Integral(1,i+1)
        g.SetBinContent( i , ( total - v )  / total )
        n1 = max(v,1.0)
        n2 = max(total-v,1.0)
        n1 = min(n1 , total-1)
        n2 = min(n2 , total-1)
        err = math.sqrt(n1*n2/total)/total
        g.SetBinError( i , err  )
    return g

    
def graph ( h1 , h2 ) :
    
    g1 = convert ( h1 )
    g2 = convert ( h2 )

    nb = g1.GetNbinsX()
    x  = ROOT.TVectorD(nb)
    y  = ROOT.TVectorD(nb)
    xe = ROOT.TVectorD(nb)
    ye = ROOT.TVectorD(nb)
    for i in range(1,nb) :
        x  [ i - 1 ] = g1.GetBinContent (i)
        y  [ i - 1 ] = g2.GetBinContent (i)
        xe [ i - 1 ] = g1.GetBinError   (i)
        ye [ i - 1 ] = g2.GetBinError   (i)
        
    return ROOT.TGraphErrors( x , y , xe , ye ) 

        
def plot ( h1 , h2 , fname = "fig") :
    
    canvas = ROOT.TCanvas( fname , fname , 1000, 1000 )
    canvas.SetLogy ()
    canvas.SetGrid ()
    
    f1 = h1.Clone ()
    f2 = h2.Clone () 

    print 'integrals: %s & %s'% ( h1.Integral() , h2.Integral() ) 
    f1.Scale(1.0/h1.Integral())
    f2.Scale(1.0/h2.Integral())
    
    f1.SetLineColor (2)
    f1.SetLineWidth (3) 
    
    f2.SetLineColor (4)
    f2.SetLineWidth (3) 
    
    f2.Draw() 
    f1.Draw('same') 

    canvas.Print( fname + '.eps')
    canvas.Print( fname + '.gif')


f=ROOT.TFile('IPCheck_Tuples.root')
t=f.Get('IPCheck/IP')


h1=ROOT.TH1D('h1','tanh(dip) (signal)',100,-1,1)
h2=ROOT.TH1D('h2','tanh(dip) (pv'     ,100,-1,1)

t.Draw('tanh(dip)>>h1','1==signal && pt>0.5')
t.Draw('tanh(dip)>>h2','0==signal && pt>0.5')

plot ( h1 , h2 , 'dip' )


h3=ROOT.TH1D('h3','tanh(dop) (signal)',100,-1,1)
h4=ROOT.TH1D('h4','tanh(dop) (pv'     ,100,-1,1)

t.Draw('tanh(dop)>>h3','1==signal && pt>0.5')
t.Draw('tanh(dop)>>h4','0==signal && pt>0.5')

plot ( h3 , h4 , 'dop' )


h5=ROOT.TH1D('h5','tanh(ip) (signal)',101,0,1.01)
h6=ROOT.TH1D('h6','tanh(ip) (pv'     ,101,0,1.01)

t.Draw('tanh(ip)>>h5','1==signal &&pt>0.5')
t.Draw('tanh(ip)>>h6','0==signal &&pt>0.5')

plot ( h5 , h6 , 'ip' )


h7=ROOT.TH1D('h7','100*tanh(ipChi2/100) (signal)',201,0,101)
h8=ROOT.TH1D('h8','100*tanh(ipChi2/100) (pv'     ,201,0,101)

t.Draw('100*tanh(ipChi2/100)>>h7','1==signal && pt>0.5 ')
t.Draw('100*tanh(ipChi2/100)>>h8','0==signal && pt>0.5')

plot ( h7 , h8 , 'ipChi2' )

h9=ROOT.TH1D('h9' ,'400*tanh(ipChi2/400) (signal)',201,0,401)
h10=ROOT.TH1D('h10','400*tanh(ipChi2/400) (pv'    ,201,0,401)

t.Draw('400*tanh(ipChi2/400)>>h9','1==signal && pt>0.5 ')
t.Draw('400*tanh(ipChi2/400)>>h10','0==signal && pt>0.5')

plot ( h9 , h10 , 'ipChi2_400' )

h11=ROOT.TH1D('h11','50*tanh(ipChi2/50) (signal)',201,0,51)
h12=ROOT.TH1D('h12','50*tanh(ipChi2/50) (pv'     ,201,0,51)

t.Draw('50*tanh(ipChi2/50)>>h11','1==signal && pt>0.5 ')
t.Draw('50*tanh(ipChi2/50)>>h12','0==signal && pt>0.5')

plot ( h11 , h12 , 'ipChi2_50' )


g1=graph(h1,h2)
#g2=graph(h3,h4)
g3=graph(h5,h6)
g4=graph(h7,h8)
g5=graph(h9,h10)
g6=graph(h11,h12)

g1.SetMarkerColor(2)
g3.SetMarkerColor(3)
g4.SetMarkerColor(4)
g5.SetMarkerColor(5)
g6.SetMarkerColor(6)

g1.SetLineColor(2)
g3.SetLineColor(3)
g4.SetLineColor(4)
g5.SetLineColor(5)
g6.SetLineColor(6)

g1.SetMarkerStyle(21)
g3.SetMarkerStyle(22)
g4.SetMarkerStyle(23)
g5.SetMarkerStyle(23)
g6.SetMarkerStyle(23)


canvas = ROOT.TCanvas( 'graph' , 'graph' , 1000, 1000 )
canvas.SetLogy()
#canvas.SetLogx()
canvas.SetGridx()
canvas.SetGridy()

g1.Draw('AP')
g3.Draw('P')
g4.Draw('P')
g5.Draw('P')
g6.Draw('P')

    
# =============================================================================
# The end 
# =============================================================================
