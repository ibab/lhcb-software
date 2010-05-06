#!/usr/bin/env gaudirun.py
# =============================================================================
# $Id: LoKi_Track2MC.py,v 1.1 2010-05-06 11:14:48 ibelyaev Exp $ 
# =============================================================================
## @file
#  The configuration file to run LoKi_Track2MC example
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
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2010-05-06
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
__author__  = " Vanya BELYAEV Ivan.Belyaev@itep.ru "
__version__ = " CVS Tag $Name: not supported by cvs2svn $, version $Revision: 1.1 $ "
__date__    = " 2010-05-06 "
# =============================================================================

from  Gaudi.Configuration import * 

## create & configure own algorithm:
from  Configurables import LoKi__Track2MCEx
alg = LoKi__Track2MCEx (
    "Track2MCEx"
    )

## configure Track<-->MC relation tables
import LoKiPhysMC.Track2MC_Configuration

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
    EvtMax        = 20       ,  
    SkipEvents    = 0        ,
    Input         = INPUT    , ## the list of input data files
    # delegate to Histogram Persistency Service
    HistogramFile = "Phi_Histos.root"
    )

# =============================================================================
# The END
# =============================================================================
