# $Id: Minimalistic_0.py,v 1.11 2009-11-16 16:38:26 ibelyaev Exp $
# =============================================================================


# =============================================================================
from Bender.Main import *

from Configurables import DaVinci

DaVinci (
    DataType   = 'DC06' , # default  
    Simulation = True   ) 

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
