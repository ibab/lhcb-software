#!/usr/bin/env gaudirun.py
# =============================================================================
## @file
#  The configuration file to run LoKi_GenDecayexample
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
from  Configurables import LoKi__DecayDescriptor as Decay


importOptions('$LOKIEXAMPLEOPTS/LoKi_Bs2PsiPhi.py')


## configure our own algorithm: 
alg = Decay('decay')
alg.addTool ( PhysDesktop() )
alg.PhysDesktop.InputLocations = [ "Phys/PsiPhi" ] 
alg.NTupleLUN = "RC"

## confgure the application itself:
appMgr = ApplicationMgr( EvtMax = 20 )
appMgr.TopAlg += [ alg ]

## printout frequency
EventSelector ( PrintFreq = 100 ) 

## histograms:
HistogramPersistencySvc ( OutputFile = "Decay_Histos.root" )


# =============================================================================
# The END
# =============================================================================
