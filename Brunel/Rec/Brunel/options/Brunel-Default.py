##############################################################################
# File for running Brunel with default options (DC06 db, .digi in, .dst out)
# Edit one or more of the lines below to change the defaults
##############################################################################
# Syntax is:
#   gaudirun.py Brunel-Default.py <someDataFiles>.py
##############################################################################

from Brunel.Configuration import *

##############################################################################
# Available steering options with defaults.
##############################################################################
# Brunel().EvtMax       = -1    # Number of events to process
# Brunel().skipEvents   = 0     # Number of events to skip before first event
# Brunel().withMC       = True  # set to False if real data or to ignore MC truth
# Brunel().recL0Only    = False # set to True to reconstruct only L0-yes events
# Brunel().inputType    = "DIGI"# or "MDF" or "ETC" or "RDST" or "DST"
# Brunel().outputType   = "DST" # or "RDST" or "NONE"
# Brunel().expertHistos = False # set to True to write out expert histos
# Brunel().noWarnings   = False # set to True to suppress all MSG::WARNING or below 
# Brunel().datasetName  = ""    # string used to build I/O file names
# Brunel().DDDBtag      = "DC06-latest" # geometry database tag
# Brunel().condDBtag    = "DC06-latest" # conditions database tag
# Brunel().fieldOff     = False # set to True for magnetic field off data
# Brunel().veloOpen     = False # set to True for Velo open data
##############################################################################

##############################################################################
# IMPORTANT IMPORTANT IMPORTANT IMPORTANT IMPORTANT IMPORTANT IMPORTANT
# The following line must come *after* setting all the steering options
##############################################################################
Brunel().applyConf()

##############################################################################
# I/O datasets are defined in a separate file, see examples in DC06-Files.py
##############################################################################
