# $Id: PV2MC_Configuration.py,v 1.1 2008-11-04 13:52:50 ibelyaev Exp $
# =============================================================================
## @file
#  The configuration file to run PV --> MC & PV --> GenCollision stuff 'on-demand'
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
#  @see Kernel/PV2MC.h
#  @see LoKi::PV2MCAlg 
#  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
#  @date   2008-11-04
# =============================================================================
"""
The configuration file to run PV --> MC & PV --> GenCollision stuff 'on-demand'

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

from  Gaudi.Configuration import * 
from  Configurables       import DataOnDemandSvc
from  Configurables       import LoKi__PV2MCAlg 

alg = LoKi__PV2MCAlg ()

dod = DataOnDemandSvc ()
dod.AlgMap [ 'Relations/Rec/Vertex/Primary'           ] = alg.getFullName() 
dod.AlgMap [ 'Relations/Rec/Vertex/Primary2Collision' ] = alg.getFullName() 

#needed for the proper work:
import LoKiPhysMC.Track2MC_Configuration


# =============================================================================
# The END
# =============================================================================
