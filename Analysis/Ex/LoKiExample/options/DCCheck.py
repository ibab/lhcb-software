#!/usr/bin/env python
# =============================================================================
## @file 
#  Simple module to test/veryfy the various Distance Calculator tools
#  @see  file LoKi_DCCheck.py
#  @see IDistanceCalculator 
#  @see LoKi::DistanceCalculator 
#  @see LoKi::TrgDistanceCalculator 
#  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
#  @date 2008-07-14
# =============================================================================
"""
Simple module to test/veryfy the various Distance Calculatro tools 
"""
# =============================================================================
__author__ = "Vanya BELYAEV Ivan.Belyaev@nikhef.nl"


import ROOT


f = ROOT.TFile( 'DCCheck_Tuples.root' )
f.ls()
f.cd('DCCheck4')
f.ls()
t = f.Get('DCCheck4/per Track')
print t

canvas = ROOT.TCanvas("canvas",'LoKiExample: LoKi_DCCheck', 1000, 1000 )
canvas.SetLogy()

h1 = ROOT.TH1D("h1","ip1-ip2/ip1+ip2", 200 , -0.1 , 0.1 )
h1.SetLineColor(2)
h1.SetLineWidth(2)

h2 = ROOT.TH1D("h2","ip1-ip2/ip1+ip2", 200 , -0.1 , 0.1 )
h2.SetLineColor(4)
h2.SetLineWidth(2)


t.Draw("(ip1-ip2)/(ip1+ip2)>>h1","signal==1")
t.Draw("(ip1-ip2)/(ip1+ip2)>>h2","signal==0","Same")


h3 = ROOT.TH1D("h3","chi2(-)/chi2(+)", 200 , -0.1 , 0.1 )
h3.SetLineColor(2)
h3.SetLineWidth(2)

h4 = ROOT.TH1D("h4","chi2(-)/chi2(+_", 200 , -0.1 , 0.1 )
h4.SetLineColor(4)
h4.SetLineWidth(2)


t2 = f.Get('DCCheck4/B')
print t2


h5 = ROOT.TH1D("h5","(vd1-vd2)/(vd1+vd2)", 200 , -0.1 , 0.1 )
h5.SetLineColor(2)
h5.SetLineWidth(2)

t2.Draw("(vd1-vd2)/(vd1+vd2)>>h5")


h5 = ROOT.TH1D("h6","(vdchi2_3-vdchi2_4)/(vdchi2_3+vdchi2_4)", 200 , -0.1 , 0.1 )
h5.SetLineColor(2)
h5.SetLineWidth(2)

t2.Draw("(vdchi2_3-vdchi2_4)/(vdchi2_3+vdchi2_4)>>h6")




# =============================================================================
# The END 
# =============================================================================
