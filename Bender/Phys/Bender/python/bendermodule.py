#!/usr/bin/env python
# =============================================================================
# $Id: bendermodule.py,v 1.13 2004-08-27 11:42:22 ibelyaev Exp $ 
# =============================================================================
# CVS tag $NAme:$ 
# =============================================================================
# $Log: not supported by cvs2svn $
# Revision 1.12  2004/08/26 19:34:36  ibelyaev
#  remove explict PyLCGDict
#
# Revision 1.11  2004/08/06 12:07:07  ibelyaev
#  minor improvements
#
# =============================================================================

# =============================================================================
# @file
#
# defintion of all LoKi/Bender functions/cuts 
#
# @date   2004-07-11
# @author Vanya BELYAEV Ivan.Belyaev@itep.ru
# =============================================================================

import gaudimodule as gaudi

gaudi.loaddict( 'EventDict'     )
gaudi.loaddict( 'PhysEventDict' )
gaudi.loaddict( 'TrEventDict'   )
gaudi.loaddict( 'RichEventDict' )
gaudi.loaddict( 'MuonEventDict' )
gaudi.loaddict( 'CaloEventDict' )
gaudi.loaddict( 'VeloEventDict' )
gaudi.loaddict( 'ITEventDict'   )
gaudi.loaddict( 'OTEventDict'   )
gaudi.loaddict( 'L0EventDict'   )
#g.loaddict( 'L1EventDict'   )
gaudi.loaddict( 'BenderDict'    )

from   bendertypes   import *
from   benderaux     import *
from   bendercuts    import *
from   benderloop    import *
from   bendertuple   import *
from   bendermatch   import *
from   benderalgo    import *

import benderstr

AppMgr = gaudi.AppMgr 
g      = AppMgr()

# =============================================================================
# The END 
# =============================================================================
