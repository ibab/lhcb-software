# =============================================================================
# $Id: bendermodule.py,v 1.14 2004-11-08 17:06:52 ibelyaev Exp $ 
# =============================================================================
# CVS tag $NAme:$ 
# =============================================================================
# $Log: not supported by cvs2svn $
# =============================================================================
"""
This is a major Python Module for Bender application
Author: Vanya BELYAEV Ivan.Belyaev@itep.ru
"""

# =============================================================================
# @file
#
# The major Python module for Bender application 
#
# @date   2004-07-11
# @author Vanya BELYAEV Ivan.Belyaev@itep.ru
# =============================================================================

import gaudimodule

gaudimodule.loaddict( 'EventDict'     )
gaudimodule.loaddict( 'PhysEventDict' )
gaudimodule.loaddict( 'TrEventDict'   )
gaudimodule.loaddict( 'RichEventDict' )
gaudimodule.loaddict( 'MuonEventDict' )
gaudimodule.loaddict( 'CaloEventDict' )
gaudimodule.loaddict( 'VeloEventDict' )
gaudimodule.loaddict( 'ITEventDict'   )
gaudimodule.loaddict( 'OTEventDict'   )
gaudimodule.loaddict( 'L0EventDict'   )
gaudimodule.loaddict( 'TrgEventDict'  )
gaudimodule.loaddict( 'BenderDict'    )

from   bendertypes   import *
from   benderaux     import *
from   bendercuts    import *
from   benderloop    import *
from   bendertuple   import *
from   bendermatch   import *
from   benderalgo    import *


AppMgr = gaudimodule.AppMgr 

gaudi  = AppMgr()

appMgr = gaudi     #
g      = gaudi     # for 'backward compatibility'
theApp = gaudi     # ATLAS style 


import benderstr
import benderconfig


# =============================================================================
# The END 
# =============================================================================
