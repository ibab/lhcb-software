#!/usr/bin/env gaudirun.py
# =============================================================================
# $Id: CheckMC_Bs2PsiPhi.py,v 1.1 2008-10-04 16:14:48 ibelyaev Exp $ 
# =============================================================================
## @file
#  The configuration file to run CheckMC_Bs2PsiPhi example
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
The configuration file to run CheckMC_Bs2PsiPhi example

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
from  Configurables import LoKi__Bs2PsiPhi         as PsiPhi
from  Configurables import LoKi__CheckMC_Bs2PsiPhi as CheckMC

importOptions( "$DAVINCIROOT/options/DaVinciCommon.opts" )
importOptions( "$COMMONPARTICLESROOT/options/StandardKaons.opts" )
importOptions( "$COMMONPARTICLESROOT/options/StandardMuons.opts" )

## confgure the application itself:
appMgr = ApplicationMgr( EvtMax = 1000 )

## configure our own algorithm: 
alg = PsiPhi('PsiPhi')
alg.addTool ( PhysDesktop() )
alg.PhysDesktop.InputLocations = ['Phys/StdTightKaons' , 'Phys/StdTightMuons']
alg.PP2MCs = [ 'Relations/Rec/ProtoP/Charged' ]

appMgr.TopAlg += [ alg ]

## configure MC Check
check = CheckMC ( "CheckMC" )
check.addTool ( PhysDesktop() )
check.PhysDesktop.InputLocations = ['Phys/PsiPhi']
check.PP2MCs = [ 'Relations/Rec/ProtoP/Charged' ]

appMgr.TopAlg += [ check ]

## histograms:
HistogramPersistencySvc ( OutputFile = "CheckMC_Histos.root" )

## input data:
from LoKiExample.Bs2Jpsiphi_mm_data import Inputs
EventSelector ( Input     = Inputs ,
                PrintFreq = 20     ) 

# =============================================================================
# The END
# =============================================================================
