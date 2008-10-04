#!/usr/bin/env gaudirun.py
# =============================================================================
# $Id: LoKi_Tracks.py,v 1.1 2008-10-04 16:14:48 ibelyaev Exp $ 
# =============================================================================
## @file
#  The configuration file to run LoKi_Phi example
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
__version__ = " CVS Tag $Name: not supported by cvs2svn $, version $Revision: 1.1 $ "
# =============================================================================
import os 
from  Gaudi.Configuration import * 
from  Configurables import PhysDesktop
from  Configurables import LoKi__SelectTracks          as SelectTracks 
from  Configurables import LoKi__Hybrid__TrackSelector as Selector 

importOptions( "$DAVINCIROOT/options/DaVinciCommon.opts" )

appMgr = ApplicationMgr ( TopAlg = [] ,
                          EvtMax = 50 )

## configure our own algorithm:
alg = SelectTracks ('ALL')
alg.addTool ( Selector  ( Code = "TrALL" ) ) 

appMgr.TopAlg += [ alg ]

## configure our own algorithm:
alg = SelectTracks ('FASTandGOOD')
alg.addTool ( Selector  ( Code = "(TrP>5000)&(TrCHI2<100)" ) )  

appMgr.TopAlg += [ alg ]

## configure our own algorithm:
alg = SelectTracks ('FASTandPOS')
alg.addTool ( Selector  ( Code = "(TrP>10000)&(0<TrQ)" ) )  

appMgr.TopAlg += [ alg ]

## input data:
from LoKiExample.Bs2Jpsiphi_mm_data import Inputs
EventSelector ( Input     = Inputs ,
                PrintFreq = 10     ) 

# =============================================================================
# The END
# =============================================================================
