##############################################################################
# File for running Brunel with default options (2008 real data,.mdf in,.dst out)
##############################################################################
# Syntax is:
#   gaudirun.py Brunel-Default.py <someDataFiles>.py
##############################################################################

from Brunel.Configuration import *

##############################################################################
# Set here any steering options.
# Available steering options and defaults are documented in
# $BRUNELROOT/python/Brunel/Configuration.py
###############################################################################

##############################################################################
# IMPORTANT IMPORTANT IMPORTANT IMPORTANT IMPORTANT IMPORTANT IMPORTANT
# The following line must come *after* setting all the steering options
##############################################################################
Brunel().applyConf()

##############################################################################
# I/O datasets are defined in a separate file, see examples in DC06-Files.py
##############################################################################
