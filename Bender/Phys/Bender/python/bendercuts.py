#!/usr/bin/env python
# =============================================================================
# $Id: bendercuts.py,v 1.1 2004-07-11 15:47:05 ibelyaev Exp $ 
# =============================================================================
# CVS tag $NAme:$ 
# =============================================================================
# $Log: not supported by cvs2svn $
# =============================================================================


# =============================================================================
# @file
#
# defintion of all Lo?ki/Bender functions/cuts 
#
# @date   2004-07-11
# @author Vanya BELYAEV Ivan.Belyaev@itep.ru
# =============================================================================

import PyLCGDict
import gaudimodule    as gaudi

g = gaudi.AppMgr()

g.loaddict( 'EventDict'     )
g.loaddict( 'PhysEventDict' )
g.loaddict( 'BenderDict'    )

from benderparticlecuts    import *
from bendervertexcuts      import *
from bendermcparticlecuts  import *
from bendermcvertexcuts    import *

# =============================================================================
# The END 
# =============================================================================
