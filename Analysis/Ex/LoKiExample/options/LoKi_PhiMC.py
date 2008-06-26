#!/usr/bin/env gaudirun.py
# =============================================================================
# $Id: LoKi_PhiMC.py,v 1.2 2008-06-26 14:33:19 ibelyaev Exp $ 
# =============================================================================
## @file
#  The configuration file to run LoKi_PhiMC example
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
from  Configurables import LoKi__PhiMC as PhiMC

importOptions( "$DAVINCIROOT/options/DaVinciCommon.opts" )
importOptions( "$COMMONPARTICLESROOT/options/StandardKaons.opts" )

## configure our own algorithm: 
phi = PhiMC('PhiMC')
phi.addTool ( PhysDesktop() )
phi.PhysDesktop.InputLocations = ['Phys/StdTightKaons']
phi.PP2MCs = [ 'Relations/Rec/ProtoP/Charged']

## confgure the application itself:
appMgr = ApplicationMgr( EvtMax = 1000 )
appMgr.TopAlg += [ phi ]

## printout frequency
EventSelector ( PrintFreq = 100 ) 

## histograms:
HistogramPersistencySvc ( OutputFile = "PhiMC_Histos.root" )

## input data:
importOptions ( os.environ['LOKIEXAMPLEDATA'] )

# =============================================================================
# The END
# =============================================================================
