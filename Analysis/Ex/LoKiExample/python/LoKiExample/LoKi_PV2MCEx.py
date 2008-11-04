#!/usr/bin/env gaudirun.py
# =============================================================================
# $Id: LoKi_PV2MCEx.py,v 1.2 2008-11-04 13:53:53 ibelyaev Exp $ 
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
__version__ = " CVS Tag $Name: not supported by cvs2svn $, version $Revision: 1.2 $ "
# =============================================================================
import os 
from  Gaudi.Configuration import * 
from  Configurables import DataOnDemandSvc 
from  Configurables import LoKi__Example__PV2MCEx as PV2MCEx

importOptions( "$DAVINCIROOT/options/DaVinciCommon.opts" )

import LoKiPhysMC.PV2MC_Configuration 

## configure our own algorithm: 
alg = PV2MCEx('PV2MCEx')
alg.PP2MCs = [ 'Relations/Rec/ProtoP/Charged']

## confgure the application itself:
appMgr = ApplicationMgr( EvtMax = 20 , Dlls = [ 'HepMCBack'] )

appMgr.TopAlg += [ alg ]
appMgr.ExtSvc += [ DataOnDemandSvc ( OutputLevel = 2 ) ] 

importOptions("$DAVINCIROOT/options/DaVinciTestData.opts") 

# suppress printout from muon&rich 
from Configurables import MuonPIDsFromProtoParticlesAlg as Muon 
from Configurables import RichPIDsFromProtoParticlesAlg as Rich 
Muon ( "MuonPIDsFromProtos" , OutputLevel = 6 ) 
Rich ( "RichPIDsFromProtos" , OutputLevel = 6 )

# =============================================================================
# The END
# =============================================================================
