# $Id: Minimalistic_0.py,v 1.10 2009-03-04 12:56:49 ibelyaev Exp $
# =============================================================================


# =============================================================================
from Bender.Main import *

from Configurables import DaVinci

DaVinci (
    DataType   = 'DC06'     , # default  
    Simulation = True       ,
    HltType    = '' ) 

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
