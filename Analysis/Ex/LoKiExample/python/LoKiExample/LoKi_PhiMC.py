#!/usr/bin/env gaudirun.py
# =============================================================================
# $Id: LoKi_PhiMC.py,v 1.3 2009-10-15 09:28:59 pkoppenb Exp $ 
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
from  Configurables import LoKi__PhiMC as PhiMC
alg = PhiMC (
    "PhiMC"                                             , ## the instance name 
    InputLocations = [ 'StdTightKaons' ]                , ##   input particles 
    PP2MCs         = [ 'Relations/Rec/ProtoP/Charged' ] , ##   PP -> MC tables 
    )

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
    EvtMax        = 500      ,  
    SkipEvents    = 0        ,
    Input         = INPUT    , ## the list of input data files
    # delegate to Histogram Persistency Service
    HistogramFile = "PhiMC_Histos.root"
    )


# =============================================================================
# The END
# =============================================================================
