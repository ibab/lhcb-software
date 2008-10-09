#!/usr/bin/env python
# =============================================================================
## @file
#  simple PyROOT script to visualize the histograms from Bs2PsiPhi example
#  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
#  @date 2008-10-09
# =============================================================================
"""
Simple PyROOT script to visualize the histograms from 'Bs2PsiPhi' example
"""
# =============================================================================
__author__  = " Vanya BELYAEV Ivan.Belyaev@nikhef.nl "
__version__ = " CVS Tag $Name: not supported by cvs2svn $, version $Revision: 1.1 $ "
# =============================================================================


import ROOT

f = ROOT.TFile( 'Bs2PsiPhi_Tuples.root' )
f.ls()
f.cd('Bs2PsiPhi')
f.ls()


dimuons = f.Get('Bs2PsiPhi/NTuple for dimuons')

canvas_2m = ROOT.TCanvas("dimuons",'BenderExample: Bs2PsiPhi.py', 1000, 1000 )
canvas_2m.SetLogy ()

h1=ROOT.TH1D('h1','mu+mu- mass',200,2.5,3.5)
h2=ROOT.TH1D('h2','mu+mu- mass',200,2.5,3.5)

dimuons.Draw('m>>h1')
dimuons.Draw('m>>h2','1==mc1*mc2*mc')

h1.SetLineColor(2)
h1.SetLineWidth(3)
h1.Draw()
h2.SetLineColor(4)
h2.SetLineWidth(3)
h2.Draw('Same')

canvas_2m.Print ( 'Bender_Bs2PsiPhi_dimuons.eps' )
canvas_2m.Print ( 'Bender_Bs2PsiPhi_dimuons.gif' )

dikaons = f.Get('Bs2PsiPhi/NTuple for dikaons')
canvas_2k = ROOT.TCanvas("dikaons",'BenderExample: Bs2PsiPhi.py', 1000, 1000 )
canvas_2k.SetLogy ()


h3=ROOT.TH1D('h3','K+ K- mass',200,1.0,1.1)
h4=ROOT.TH1D('h4','K+ K- mass',200,1.0,1.1)

dikaons.Draw('m>>h3')
dikaons.Draw('m>>h4','1==mc1*mc2*mc')

h3.SetLineColor(2)
h3.SetLineWidth(3)
h3.Draw()
h4.SetLineColor(4)
h4.SetLineWidth(3)
h4.Draw('Same')

canvas_2k.Print ( 'Bender_Bs2PsiPhi_dikaons.eps' )
canvas_2k.Print ( 'Bender_Bs2PsiPhi_dikaons.gif' )


f = ROOT.TFile( 'Bs2PsiPhi_Histos.root' )
f.ls()
f.cd('Bs2PsiPhi')
f.ls()



# =============================================================================
# The end 
# =============================================================================
