#!/usr/bin/env gaudirun.py
# =============================================================================
## @file
#  Configuration file for LoKiExmaple package
#  @author Vanya BELYAEV Ivan.BElayev@nikhef.nl
#  @date 2008-10-04
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
from Gaudi.Configuration         import * 
from LoKiExample.LoKi_Bs2PsiPhi  import alg

ApplicationMgr ( TopAlg = [ alg ] ,
                 EvtMax = 5000    ) 

# =============================================================================
# The END 
# =============================================================================
