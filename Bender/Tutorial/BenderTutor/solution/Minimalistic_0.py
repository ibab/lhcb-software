# $Id: Minimalistic_0.py,v 1.8 2008-06-12 12:43:58 ibelyaev Exp $
# =============================================================================


# =============================================================================
from Bender.Main import *


importOptions( '$DAVINCIROOT/options/DaVinci.opts')

## get or create application manager 
gaudi = appMgr() 


## define input files 
import BenderTutor.data_tutorial as data 

evtSel = gaudi.evtSel()
evtSel.open( data.FILEs ) 

## run DaVinci 
gaudi.run(100)

gaudi.exit()


# =============================================================================
# The END 
# =============================================================================
