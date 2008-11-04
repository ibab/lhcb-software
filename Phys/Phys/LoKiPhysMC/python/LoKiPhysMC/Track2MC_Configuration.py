# $Id: Track2MC_Configuration.py,v 1.1 2008-11-04 13:52:50 ibelyaev Exp $
# =============================================================================
## @file
#  The configuration file to run Track --> MC 'on-demand'
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
#  @see Kernel/Track2MC.h
#  @see LoKi::Track2MC 
#  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
#  @date   2008-11-04
# =============================================================================
"""
#  The configuration file to run Track --> MC 'on-demand'

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
from  Configurables       import LoKi__Track2MC 

alg = LoKi__Track2MC  ()

dod = DataOnDemandSvc ()
dod.AlgMap ['Relations/Rec/Track/Default' ] = alg.getFullName()


# =============================================================================
# The END
# =============================================================================
