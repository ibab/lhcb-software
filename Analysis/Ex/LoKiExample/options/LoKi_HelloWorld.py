#!/usr/bin/env gaudirun.py
# =============================================================================
# $Id: LoKi_HelloWorld.py,v 1.2 2008-06-26 14:33:18 ibelyaev Exp $ 
# =============================================================================
## @file
#  The configuration file to run LoKi_HelloWorld example
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
from  Configurables import LoKi__HelloWorld as HelloWorld

importOptions( "$DAVINCIROOT/options/DaVinciCommon.opts" )

## configure our own algorithm: 
alg = HelloWorld('Hello')

## confgure the application itself:
appMgr = ApplicationMgr( EvtMax = 100 , TopAlg = [ alg ] ) 

## printout frequency
EventSelector ( PrintFreq = 10 ) 

## input data:
importOptions ( os.environ['LOKIEXAMPLEDATA'] )

# =============================================================================
# The END
# =============================================================================
