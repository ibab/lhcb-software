# $Id: Minimalistic_0.py,v 1.7 2008-03-15 13:35:38 ibelyaev Exp $
# =============================================================================


# =============================================================================
from Bender.Main import *

import BenderTutor.data_tutorial as data 

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
