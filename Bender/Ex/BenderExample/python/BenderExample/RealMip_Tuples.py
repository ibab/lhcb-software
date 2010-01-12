#!/user/bin/env python
# ============================================================================
## @file BenderExample/RealMip_Tuples.py
#
#  Helper script to visualize "mip"-histograms
#
#  This file is a part of 
#  <a href="http://cern.ch/lhcb-comp/Analysis/Bender/index.html">Bender project</a>
#  <b>"Python-based Interactive Environment for Smart and Friendly 
#   Physics Analysis"</b>
#
#  The package has been designed with the kind help from
#  Pere MATO and Andrey TSAREGORODTSEV. 
#  And it is based on the 
#  <a href="http://cern.ch/lhcb-comp/Analysis/LoKi/index.html">LoKi project:</a>
#  "C++ ToolKit for Smart and Friendly Physics Analysis"
#
#  By usage of this code one clearly states the disagreement 
#  with the campain of Dr.O.Callot et al.: 
#  "No Vanya's lines are allowed in LHCb/Gaudi software."
#
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
