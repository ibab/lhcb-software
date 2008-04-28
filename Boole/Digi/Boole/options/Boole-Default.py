##############################################################################
# File for running Boole with default options (DC06 geometry)
# Edit one or more of the lines below to change the defaults
##############################################################################
# Syntax is:
#   gaudirun.py Boole-Default.py <someDataFiles>.py
##############################################################################

from Boole.Configuration import *

##############################################################################
# Available steering options with defaults.
##############################################################################
# Boole().EvtMax          = -1    # Number of events to process
# Boole().skipEvents      = 0     # Number of events to skip before first event
# Boole().skipSpill       = 0     # Number of spillover events to skip
# Boole().useSpillover    = True  # set to False to disable spillover
# Boole().generateTAE     = False # set to True to simulate time alignment events
# Boole().writeRawMDF     = False # set to True to write simulated Raw data
# Boole().writeDigi       = True  # set to False to not write POOL .digi
# Boole().writeL0ETC      = False # set to True to write ETC of L0 selected events
# Boole().writeL0Only     = False # set to True to write only L0 selected events
# Boole().extendedDigi    = False # set to True to add MCHits to .digi output file
# Boole().expertHistos    = False # set to True to write out expert histos
# Boole().noWarnings      = False # set to True to suppress all MSG::WARNING or below 
# Boole().datasetName     = ""            # string used to build file names
# Boole().DDDBtag         = "DC06-latest" # geometry database tag
# Boole().condDBtag       = "DC06-latest" # conditions database tag
##############################################################################

##############################################################################
# IMPORTANT IMPORTANT IMPORTANT IMPORTANT IMPORTANT IMPORTANT IMPORTANT
# The following line must always be *after* all Boole steering options
##############################################################################
Boole().applyConf()

##############################################################################
# I/O datasets are defined in a separate file, see examples in DC06-Files.py
##############################################################################
