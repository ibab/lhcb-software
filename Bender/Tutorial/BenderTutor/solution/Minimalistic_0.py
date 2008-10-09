# $Id: Minimalistic_0.py,v 1.9 2008-10-09 13:11:41 ibelyaev Exp $
# =============================================================================


# =============================================================================
from Bender.Main import *


importOptions( '$DAVINCIROOT/options/DaVinci.opts')

## get or create application manager 
gaudi = appMgr() 

## redefine input files 
evtSel = gaudi.evtSel()
import LoKiExample.Bs2Jpsiphi_mm_data as data 
evtSel.open( data.Files ) 

## run DaVinci 
gaudi.run(100)

gaudi.exit()


# =============================================================================
# The END 
# =============================================================================
