# $Id: Minimalistic_0.py,v 1.4 2006-11-17 11:59:47 ibelyaev Exp $
# =============================================================================
# CVS tag $Name: not supported by cvs2svn $ , verison $Revision: 1.4 $
# =============================================================================

from bendermodule import *

gaudi.config( files = ['$DAVINCIROOT/options/DaVinci.opts'] )

## define input files 
evtSel = gaudi.evtSel()

import data_tutorial as data 
evtSel.open( data.FILES ) 

## run DaVinci 
gaudi.run(100)

gaudi.exit()


# =============================================================================
# $Log: not supported by cvs2svn $
#
# =============================================================================
# The END 
# =============================================================================
