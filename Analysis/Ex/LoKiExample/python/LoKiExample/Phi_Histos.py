#!/usr/bin/env python
# =============================================================================
## @file
#  simple PyROOT script to visualize the histograms from LoKi_Phi example
#  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
#  @date 2008-06-07
# =============================================================================
"""

Simple PyROOT script to visualize the histograms from LoKi_Phi example

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
