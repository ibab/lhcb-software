#!/usr/bin/env gaudirun.py
# =============================================================================
# $Id: LoKi_Bs2PsiPhi_HepMC.py,v 1.2 2008-06-26 14:33:18 ibelyaev Exp $ 
# =============================================================================
## @file
#  The configuration file to run LoKi_Bs2PsiPhi example
#
#   This file is a part of LoKi project - 
#     "C++ ToolKit  for Smart and Friendly Physics Analysis"
#
#  The package has been designed with the kind help from
#  Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas, 
#  contributions and advices from G.Raven, J.van Tilburg, 
#  A.Golutvin, P.Koppenburg have been used in the design.
#
#  By usage of this code one clearly states the disagreement 
#  with the campain of Dr.O.Callot et al.: 
#  "No Vanya's lines are allowed in LHCb/Gaudi software."
#
#  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
#  @date   2008-06-07
# =============================================================================
import os 
from  Gaudi.Configuration import * 
from  Configurables import PhysDesktop
from  Configurables import LoKi__Bs2PsiPhi_NoMC     as PsiPhi
from  Configurables import LoKi__HepMCParticleMaker as HepMCMaker

importOptions( "$DAVINCIROOT/options/DaVinciCommon.opts" )

## configure our own algorithm: 
alg = PsiPhi('PsiPhi', PropertiesPrint = True )
alg.addTool ( PhysDesktop () )
alg.PhysDesktop.ParticleMakerType = 'LoKi::HepMCParticleMaker/HepMCMaker'
alg.PhysDesktop.addTool ( HepMCMaker('HepMCMaker') ) 
alg.PhysDesktop.HepMCMaker.Particles = [ "mu+"       , "K+"        ]
alg.PhysDesktop.HepMCMaker.From      = [ "J/psi(1S)" , "phi(1020)" ]
alg.PhysDesktop.HepMCMaker.MinPCharged      = -1
alg.PhysDesktop.HepMCMaker.MinPtCharged     = -1
alg.PhysDesktop.HepMCMaker.MinThetaCharged  = -1
alg.PhysDesktop.HepMCMaker.MaxThetaCharged  = -1
alg.PhysDesktop.HepMCMaker.SmearParticle = False


## configure the application itself:
appMgr = ApplicationMgr( EvtMax = 1000 )
appMgr.TopAlg += [ alg ]

## printout frequency
EventSelector ( PrintFreq = 10 ) 

## histograms:
HistogramPersistencySvc ( OutputFile = "PsiPhi_Histos.root" )

## input data:
importOptions ( os.environ['LOKIEXAMPLEDATA'] )

# =============================================================================
# The END
# =============================================================================
