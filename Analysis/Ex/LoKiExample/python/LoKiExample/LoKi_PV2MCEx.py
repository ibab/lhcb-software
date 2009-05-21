#!/usr/bin/env gaudirun.py
# =============================================================================
# $Id: LoKi_PV2MCEx.py,v 1.3 2009-05-21 13:58:58 ibelyaev Exp $ 
# =============================================================================
## @file
#  The configuration file to run LoKi_PV2MCEx example
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
"""
Configuration file for LoKiExample package

This file is a part of LoKi project - 
\"C++ ToolKit  for Smart and Friendly Physics Analysis\"

The package has been designed with the kind help from
Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas, 
contributions and advices from G.Raven, J.van Tilburg, 
A.Golutvin, P.Koppenburg have been used in the design.

By usage of this code one clearly states the disagreement 
with the campain of Dr.O.Callot et al.: 
\"No Vanya's lines are allowed in LHCb/Gaudi software.\"

"""
# =============================================================================
__author__  = " Vanya BELYAEV Ivan.Belyaev@nikhef.nl "
__version__ = " CVS Tag $Name: not supported by cvs2svn $, version $Revision: 1.3 $ "
# =============================================================================
from  Gaudi.Configuration import * 

## create & configure own algorithm:
from  Configurables import LoKi__Example__PV2MCEx as PV2MCEx
alg = PV2MCEx(
    'PV2MCEx'  , 
    PP2MCs = [ 'Relations/Rec/ProtoP/Charged']
    )

## PV -> MC configuration 
import LoKiPhysMC.PV2MC_Configuration 

## get input data:
from LoKiExample.Bs2Jpsiphi_mm_data import Inputs as INPUT 

## confgure the application itself:
from  Configurables import DaVinci 
DaVinci (
    DataType       = 'DC06'  , ## Data type  
    Simulation     = True    , ## Monte Carlo 
    HltType        = ''      ,
    #
    UserAlgorithms = [ alg ] , ## let DaVinci know about local algorithm
    # delegate this properties to Event Selector 
    EvtMax        = 50       ,  
    SkipEvents    = 0        ,
    Input         = INPUT      ## the list of input data files
    )



# =============================================================================
# The END
# =============================================================================
