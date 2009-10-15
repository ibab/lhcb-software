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
from  Gaudi.Configuration import * 



## configure our own algorithm: 
from  Configurables import LoKi__CCTest as CCTest
alg = CCTest( 'CCTest'           ,
              Decays = [
    "pi+" ,
    "K+pi+++" ,
    "cc nosos " ,
    "some line which contains B0/B~0 and other K*(892)0mesons Meson"
    ] )

## get input data:
from LoKiExample.Bs2Jpsiphi_mm_data import Inputs as INPUT 

## confgure the application itself:
from  Configurables import DaVinci 
DaVinci (
    DataType       = 'DC06'  , ## Data type  
    Simulation     = True    , ## Monte Carlo 
    Hlt            = False      ,
    #
    UserAlgorithms = [ alg ] , ## let DaVinci know about local algorithm
    # delegate this properties to Event Selector 
    EvtMax        = 10       ,  
    SkipEvents    = 0        ,
    Input         = INPUT      ## the list of input data files
    )



# =============================================================================
# The END 
# =============================================================================
