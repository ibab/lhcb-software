##############################################################################
# File for running Brunel on 2008 real data
#
# Syntax is:
#   gaudirun.py Brunel-2008.py <someDataFiles>.py
##############################################################################

from Configurables import Brunel

Brunel().DataType = "2008"
Brunel().SpecialData = ["fieldOff"] # All 2008 real data was taken with Magnet OFF
##############################################################################
# I/O datasets are defined in a separate file, see examples in 2008-TED_Data.py
##############################################################################
