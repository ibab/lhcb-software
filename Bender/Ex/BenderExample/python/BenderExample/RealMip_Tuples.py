#!/user/bin/env python
# ============================================================================
## @file BenderExample/RealMip_Tuples.py
#  Helepr script to visualize "mip"-histograms
#  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
#  @date 2009-12-01
# =============================================================================

import ROOT
c = ROOT.TCanvas()

fh=ROOT.TFile('RealMip_Histos.root')

fh.cd('Mip')

h1 = fh.Get('Mip/e Ecal')
h2 = fh.Get('Mip/e Hcal')
h3 = fh.Get('Mip/e Prs' )

h11 = fh.Get('Mip/e1 Ecal')
h21 = fh.Get('Mip/e1 Hcal')
h31 = fh.Get('Mip/e1 Prs' )

mip = fh.Get('Mip/mip in Ecal')

mip.Draw()


ft=ROOT.TFile('RealMip.root')

tree = ft.Get('Mip/RealMip')

tree.Draw('eEcal >> h(50,0,5000)' , ' nEcal>0  && nPrs > 0 ')

tree.Draw('eEcal >> h(50,0,5000)' , ' nEcal>0  && nPrs > 0 && ePrs < 10  && sqrt(tx+tx+ty*ty)>0.080 ')
