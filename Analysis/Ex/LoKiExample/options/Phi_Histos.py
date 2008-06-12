#!/usr/bin/env python
# =============================================================================
## @file
#  simple PyROOT script to visualize the histograms from LoKi_Phi example
#  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
#  @date 2008-06-07
# =============================================================================


import ROOT

f = ROOT.TFile( 'Phi_Histos.root' )
f.ls()
f.cd('Phi')
f.ls()
h1 = f.Get('Phi/K+ K- mass')
h2 = f.Get('Phi/K+ K- mass, chi2_vx<49')
print h1
print h2

canvas = ROOT.TCanvas("canvas",'LoKiExample: LoKi_Phi', 1000, 1000 )

h1.SetLineColor(2)
h1.SetLineWidth(3)
h1.Draw()
h2.SetLineColor(4)
h2.SetLineWidth(3)
h2.Draw('Same')

canvas.Print ( 'LoKi_Phi.eps' )
canvas.Print ( 'LoKi_Phi.gif' )


    
# =============================================================================
# The end 
# =============================================================================
