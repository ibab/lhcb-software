#!/usr/bin/env python
# =============================================================================
## @file
#  simple PyROOT script to visualize the histograms from LoKi_Phi example
#  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
#  @date 2008-06-07
# =============================================================================
"""
Simple PyROOT script to visualize the histograms from LoKi_Phi example

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


import ROOT

f = ROOT.TFile( 'MCMuon_Tuples.root' )
f.ls()
f.cd('MCMuon')
f.ls()
t = f.Get('MCMuon/Muons')
print t

canvas = ROOT.TCanvas("canvas",'LoKiExample: LoKi_MCMuon', 1000, 1000 )
canvas.SetLogy()

h1 = ROOT.TH1D("h1","DLL(mu-pi)", 100 , 0 , 10 )
h1.SetLineColor(2)
h1.SetLineWidth(3)

h2 = ROOT.TH1D("h2","DLL(mu-pi)", 100 , 0 , 10 )
h2.SetLineColor(4)
h2.SetLineWidth(3)


t.Draw("min(PT,9.999)>>h2","mc1==1")
t.Draw("min(PT,9.999)>>h1","mc1==0","Same")


canvas.Print ( 'LoKi_MCMuon.eps' )
canvas.Print ( 'LoKi_MCMuon.gif' )


    
# =============================================================================
# The end 
# =============================================================================
