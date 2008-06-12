#!/usr/bin/env python
# =============================================================================
## @file
#  simple PyROOT script to visualize the histograms from LoKi_Phi example
#  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
#  @date 2008-06-07
# =============================================================================


import ROOT

f = ROOT.TFile( 'PhiMC_Histos.root' )
f.ls()
f.cd('PhiMC')
f.ls()
h1 = f.Get('PhiMC/K+ K- mass')
h2 = f.Get('PhiMC/K+ K- mass, chi2_vx<49')
h3 = f.Get('PhiMC/K+ K- mass, MC-truth')

canvas = ROOT.TCanvas("canvas",'LoKiExample: LoKi_PhiMC', 1000, 1000 )

h1.SetLineColor(3)
h1.SetLineWidth(3)
h1.Draw()
h2.SetLineColor(4)
h2.SetLineWidth(3)
h2.Draw('Same')
h3.SetLineColor(2)
h3.SetLineWidth(3)
h3.Draw('Same')

canvas.Print ( 'LoKi_PhiMC.eps' )
canvas.Print ( 'LoKi_PhiMC.gif' )


    
# =============================================================================
# The end 
# =============================================================================
