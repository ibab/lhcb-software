# $Id: Minimalistic_0.py,v 1.5 2007-05-31 11:29:46 ibelyaev Exp $
# =============================================================================
# CVS tag $Name: not supported by cvs2svn $ , verison $Revision: 1.5 $
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
