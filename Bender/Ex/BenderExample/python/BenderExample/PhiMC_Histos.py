#!/usr/bin/env python
# =============================================================================
## @file
#  simple PyROOT script to visualize the histograms from Phi example
#  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
#  @date 2008-10-09
# =============================================================================
"""
Simple PyROOT script to visualize the histograms from 'Phi'example
"""
# =============================================================================
__author__  = " Vanya BELYAEV Ivan.Belyaev@nikhef.nl "
__version__ = " CVS Tag $Name: not supported by cvs2svn $, version $Revision: 1.1 $ "
# =============================================================================


import ROOT

f = ROOT.TFile( 'PhiMC_Histos.root' )
f.ls()
f.cd('PhiMC')
f.ls()
h1 = f.Get('PhiMC/K+ K- mass')
h2 = f.Get('PhiMC/K+ K- mass,0<chi2<49')
h3 = f.Get('PhiMC/K+ K- mass,mctruth')
print h1
print h2
print h3

canvas = ROOT.TCanvas("canvas",'BenderExample: PhiMC.py', 1000, 1000 )

h1.SetLineColor(2)
h1.SetLineWidth(3)
h1.Draw()
h2.SetLineColor(4)
h2.SetLineWidth(3)
h2.Draw('Same')
h3.SetLineColor(6)
h3.SetLineWidth(3)
h3.Draw('Same')

canvas.Print ( 'Bender_PhiMC.eps' )
canvas.Print ( 'Bender_PhiMC.gif' )


    
# =============================================================================
# The end 
# =============================================================================
