##############################################################################
# File for running Brunel on 2008 cosmic data with default 2008 geometry
# Syntax is:
#   gaudirun.py Brunel-Cosmics.py 2008-Cosmic-Data.py
##############################################################################

from Configurables import Brunel

# Set the special data options
Brunel().SpecialData  = ["fieldOff","cosmics"]

##############################################################################
# I/O datasets are defined in a separate file, see examples 2008-Cosmic-Data.py
##############################################################################

