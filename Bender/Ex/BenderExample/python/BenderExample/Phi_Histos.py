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

f = ROOT.TFile( 'Phi_Histos.root' )
f.ls()
f.cd('Phi')
f.ls()
h1 = f.Get('Phi/K+ K- mass')
h2 = f.Get('Phi/K+ K- mass, chi2_vx<49')
print h1
print h2

canvas = ROOT.TCanvas("canvas",'BenderExample: Phi.py', 1000, 1000 )

h1.SetLineColor(2)
h1.SetLineWidth(3)
h1.Draw()
h2.SetLineColor(4)
h2.SetLineWidth(3)
h2.Draw('Same')

canvas.Print ( 'Bender_Phi.eps' )
canvas.Print ( 'Bender_Phi.gif' )


    
# =============================================================================
# The end 
# =============================================================================
