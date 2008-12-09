########################################################################
#
# $Id: DaVinciCommon.py,v 1.1 2008-12-09 10:16:27 pkoppenb Exp $
#
# Standard Options for a typical DaVinci job
#
# @author Patrick Koppenburg
# @date 2008-08-06
#
########################################################################
from Gaudi.Configuration import *
########################################################################
#
# Standard configuration
#
# @todo : Replace by a python Configuration, like in Brunel
#
importOptions( "$DAVINCIROOT/options/DaVinciCommon.opts" )
