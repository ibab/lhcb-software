###############################################################################
# File for running Brunel with default options (2008 real data,.mdf in,.dst out)
###############################################################################
# Syntax is:
#   gaudirun.py Brunel-Default.py <someDataFiles>.py
###############################################################################

from Configurables import Brunel

###############################################################################
# Set here any steering options.
# Available steering options and defaults are documented in
# $BRUNELROOT/python/Brunel/Configuration.py
###############################################################################

# Just instantiate the configurable...
theApp = Brunel()
Brunel.RecL0Only=True;

###############################################################################
# I/O datasets are defined in a separate file, see examples in 2008-TED-Data.py
###############################################################################
