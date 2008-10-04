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
from  Configurables import LoKi__GenDecayDescriptor as GenDecay

importOptions( "$DAVINCIROOT/options/DaVinciCommon.opts" )

## configure our own algorithm: 
alg = GenDecay('Gendecay')
alg.addTool ( PhysDesktop() )
alg.NTupleLUN = "GEN"
alg.PP2MCs = []

## confgure the application itself:
appMgr = ApplicationMgr( EvtMax = 10 )
appMgr.TopAlg = [ alg ]

## histograms:
HistogramPersistencySvc ( OutputFile = "GenDecay_Histos.root" )

## n-Tuples
NTupleSvc ( Output = [ "IP DATAFILE='GenDecay_Tuples.root' TYP='ROOT' OPT='NEW'" ] ) ;

## input data:
from LoKiExample.Bs2Jpsiphi_mm_data import Inputs
EventSelector ( Input     = Inputs ,
                PrintFreq = 20     ) 

# =============================================================================
# The END
# =============================================================================
