#!/usr/bin/env gaudirun.py
# =============================================================================
## @file
#  The configuration file to run LoKi_CCTest example
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
#  @date   2008-08-04
# =============================================================================
"""
The configuration file to run LoKi_CCTest example

This file is a part of LoKi project - 
'C++ ToolKit  for Smart and Friendly Physics Analysis'

The package has been designed with the kind help from
Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas, 
contributions and advices from G.Raven, J.van Tilburg, 
A.Golutvin, P.Koppenburg have been used in the design.

By usage of this code one clearly states the disagreement 
with the campain of Dr.O.Callot et al.: 
'No Vanya's lines are allowed in LHCb/Gaudi software'.
"""
# =============================================================================
__author__ = "Vanya BELYAEV Ivan.Belyaev@nikhef.nl"
# =============================================================================


import os 
from  Gaudi.Configuration import * 
from  Configurables import PhysDesktop
from  Configurables import LoKi__CCTest as CCTest

importOptions( "$DAVINCIROOT/options/DaVinciCommon.opts" )

## configure our own algorithm: 
alg = CCTest( 'CCTest'           ,
              Decays = [ "pi+" ,
                         "K+pi+++" ,
                         "some line which contains B0abd other K*(892)0mesons Meson"
                         ] )

ApplicationMgr ( EvtMax = 5       ,
                 TopAlg = [ alg ] ) 

## input data:
from LoKiExample.Bs2Jpsiphi_mm_data import Inputs
EventSelector ( Input = Inputs ) 

# =============================================================================
# The END 
# =============================================================================
