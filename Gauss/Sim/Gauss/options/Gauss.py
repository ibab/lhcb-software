##############################################################################
# File for running Gauss with default options
##############################################################################
# Syntax is:
#   gaudirun.py Gauss-Default.py <someInputForTheJob>.py
##############################################################################

from Configurables import Gauss, LHCbApp

###############################################################################
# Set here any steering options.
# Available steering options and defaults are documented in
# $GAUSSROOT/python/Gauss/Configuration.py
###############################################################################

theApp = Gauss()

# You MUST also set the tag of the conditions
#LHCbApp().DDDBtag = <yourChoice>
#LHCbApp().ConDBtag = <yourChoice>
