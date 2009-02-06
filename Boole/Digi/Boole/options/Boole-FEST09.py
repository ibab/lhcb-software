##############################################################################
# File for Boole production of MDF files for 2009 Full Experiment System Test
#
# Syntax is:
#   gaudirun.py Boole-FEST09.py <someDataFiles>.py
##############################################################################

from Configurables import Boole

Boole().Outputs    = ["MDF"]
Boole().NoWarnings = True

# Write out only events passing L0, or with simulated random trigger
Boole().FilterSequence = ["L0","ODIN"]
OdinTypesFilter().TriggerTypes = ["RandomTrigger"]

##############################################################################
# I/O datasets and database tags are defined in a separate file,
# see examples in 2008-Files.py
##############################################################################
