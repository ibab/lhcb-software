# $Id: Minimalistic_0.py,v 1.6 2007-09-29 13:46:18 ibelyaev Exp $
# =============================================================================


# =============================================================================
from Bender.Main import *

import data_tutorial as data 

gaudi.config( files = ['$DAVINCIROOT/options/DaVinci.opts'] )

## define input files 
evtSel = gaudi.evtSel()

evtSel.open( data.FILEs ) 

## run DaVinci 
gaudi.run(100)

gaudi.exit()


# =============================================================================
# The END 
# =============================================================================
