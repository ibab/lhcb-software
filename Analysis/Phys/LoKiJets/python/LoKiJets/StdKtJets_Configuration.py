# $Id: StdKtJets_Configuration.py,v 1.1 2008-11-09 14:31:03 ibelyaev Exp $
# =============================================================================
## @file
#  The configuration file to build 'Standard Kt-Jets' 'on-demand'
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
#  @see  LoKi::KtJetMaker
#  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
#  @date   2008-11-04
# =============================================================================
"""
The configuration file to build 'Standard Kt-Jets' 'on-demand'

This file is a part of LoKi project -
\"C++ ToolKit  for Smart and Friendly Physics Analysis\"

`The package has been designed with the kind help from
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
from  Configurables       import LoKi__JetMaker
from  Configurables       import PhysDesktop 


alg = LoKi__JetMaker( 'StdKtJets')
alg.addTool ( PhysDesktop (
    InputLocations =  [ 'Phys/StdNoPIDsPions'     ,
                        'Phys/StdLooseAllPhotons' ] ) ) 

dod = DataOnDemandSvc ()
dod.AlgMap[ 'Phys/StdKtJet/Particles'] = alg.getFullName()
dod.AlgMap[ 'Phys/StdKtJet/Vertices' ] = alg.getFullName()


# =============================================================================
# The END
# =============================================================================
