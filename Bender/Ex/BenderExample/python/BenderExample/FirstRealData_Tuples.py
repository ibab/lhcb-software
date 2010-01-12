#!/usr/bin/env python
# =============================================================================
# $Id: FirstRealData_Tuples.py,v 1.2 2010-01-12 10:58:34 ibelyaev Exp $
# =============================================================================
## @file BenderExample/FirstRealData_Tuples.py
#
#  simple example to procss tuples, prepared by FirstRealData example.
#  It is assumed that one has produces two tuples
#  - The firts  one for data with collisions
#  - The second one for data with beam-gas only
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
#  @date   2009-11-25
# =============================================================================
"""
Simple example to procss tuples, prepared by FirstRealData example.
It is assumed that one has produces two tuples
- The firts  one for data with collisions
- The second one for data with beam-gas only
"""
# =============================================================================
__author__  = "Vanya BELYAEV Ivan.Belyaev@nikhef.nl"
__version__ = "CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.2 $"
# ============================================================================
import ROOT


collisions = 'FirstRealData_BX.root'
beamgas    = 'FirstRealData_BG.root'

file1 = ROOT.TFile( collisions )
file2 = ROOT.TFile( beamgas    )

tree1 = file1.Get('RealData/RealData')
tree2 = file2.Get('RealData/RealData')

print tree1
print tree2

h1 = ROOT.TH1D('h1', 'zPoca for collisions'                  , 100 , -5000 , 5000 )
h2 = ROOT.TH1D('h2', 'zPoca for beamgas'                     , 100 , -5000 , 5000 )

h3 = ROOT.TH1D('h3', 'zPoca for collisions Et(Ecal)>3 GeV'   , 100 , -5000 , 5000 )
h4 = ROOT.TH1D('h4', 'zPoca for beamgas    Et(Ecal)>3 GeV'   , 100 , -5000 , 5000 )

h5 = ROOT.TH1D('h5', 'zPoca for collisions Et(Ecal)<1 GeV'   , 100 , -5000 , 5000 )
h6 = ROOT.TH1D('h6', 'zPoca for beamgas    Et(Ecal)<1 GeV'   , 100 , -5000 , 5000 )

h7 = ROOT.TH1D('h7', 'zPoca for collisions 1<Et(Ecal)<3 GeV' , 100 , -5000 , 5000 )
h8 = ROOT.TH1D('h8', 'zPoca for beamgas    1<Et(Ecal)<3 GeV' , 100 , -5000 , 5000 )

tree1.Draw('zPoca >> h1 ')
tree1.Draw('zPoca >> h3 ' , 'eTEcal > 3 ')
tree1.Draw('zPoca >> h5 ' , 'eTEcal < 1 ')
tree1.Draw('zPoca >> h7 ' , 'eTEcal > 1 && eTEcal <3 ')

tree2.Draw('zPoca >> h2 ')
tree2.Draw('zPoca >> h4 ' , 'eTEcal > 3 ')
tree2.Draw('zPoca >> h6 ' , 'eTEcal < 1 ')
tree2.Draw('zPoca >> h8 ' , 'eTEcal > 1 && eTEcal <3 ')

c = ROOT.TCanvas()

h1.Draw()
h2.Draw('same')

c.Print('no-cuts.gif') 
c.Print('no-cuts.eps') 

h3.Draw()
h4.Draw('same')

c.Print('etEcal>3.gif') 
c.Print('etEcal>3.eps') 

h5.Draw()
h6.Draw('same')

c.Print('etEcal<1.gif') 
c.Print('etEcal<1.eps') 

h7.Draw()
h8.Draw('same')

c.Print('1<etEcal<3.gif') 
c.Print('1<etEcal<3.eps') 





# =============================================================================
# The END 
# =============================================================================

