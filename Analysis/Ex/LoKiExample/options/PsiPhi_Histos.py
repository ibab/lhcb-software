#!/usr/bin/env python
# =============================================================================
## @file
#  simple PyROOT script to visualize the histograms from LoKi_Phi example
#  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
#  @date 2008-06-07
# =============================================================================


import ROOT

f = ROOT.TFile( 'PsiPhi_Histos.root' )
f.ls()
f.cd('PsiPhi')
f.ls()

canvas = ROOT.TCanvas("canvas1",'LoKiExample: LoKi_PsiPhi : Phi ', 1000, 1000 )
h1 = f.Get('PsiPhi/K+K- mass')
h2 = f.Get('PsiPhi/K+K- mass, chi2_vx < 49')
h3 = f.Get('PsiPhi/K+K- mass (MC)')

h1.SetLineColor(3)
h1.SetLineWidth(3)
h1.Draw()
h2.SetLineColor(4)
h2.SetLineWidth(3)
h2.Draw('Same')
h3.SetLineColor(2)
h3.SetLineWidth(3)
h3.Draw('Same')

canvas.Print ( 'LoKi_PsiPhi_Phi.eps' )
canvas.Print ( 'LoKi_PsiPhi_Phi.gif' )

canvas = ROOT.TCanvas("canvas2",'LoKiExample: LoKi_PsiPhi : Psi ', 1000, 1000 )
h1 = f.Get('PsiPhi/ mu mu  mass')
h2 = f.Get('PsiPhi/ mu mu  mass, chi2_vx < 49')
h3 = f.Get('PsiPhi/ mu mu  mass (MC)')

h1.SetLineColor(3)
h1.SetLineWidth(3)
h1.Draw()
h2.SetLineColor(4)
h2.SetLineWidth(3)
h2.Draw('Same')
h3.SetLineColor(2)
h3.SetLineWidth(3)
h3.Draw('Same')

canvas.Print ( 'LoKi_PsiPhi_Psi.eps' )
canvas.Print ( 'LoKi_PsiPhi_Psi.gif' )


canvas = ROOT.TCanvas("canvas3",'LoKiExample: LoKi_PsiPhi : Bs ', 1000, 1000 )
h1 = f.Get('PsiPhi/ psi phi mass')
h2 = f.Get('PsiPhi/ psi phi mass, chi2_vx < 49')
h3 = f.Get('PsiPhi/ psi(MC) phi     mass')
h4 = f.Get('PsiPhi/ psi     phi(MC) mass')
h5 = f.Get('PsiPhi/ psi(MC) phi(MC) mass')

h1.SetLineColor(1)
h1.SetLineWidth(3)
h1.Draw()
h3.SetLineColor(3)
h3.SetLineWidth(3)
h3.Draw('Same')
h4.SetLineColor(4)
h4.SetLineWidth(3)
h4.Draw('Same')
h5.SetLineColor(2)
h5.SetLineWidth(3)
h5.Draw('Same')

canvas.Print ( 'LoKi_PsiPhi_Bs.eps' )
canvas.Print ( 'LoKi_PsiPhi_Bs.gif' )






    
# =============================================================================
# The end 
# =============================================================================
