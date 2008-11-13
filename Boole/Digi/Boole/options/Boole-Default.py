##############################################################################
# File for running Boole with default options (2008 geometry)
# Edit one or more of the lines below to change the defaults
##############################################################################
# Syntax is:
#   gaudirun.py Boole-Default.py <someDataFiles>.py
##############################################################################

from Boole.Configuration import *

##############################################################################
# Available steering options with defaults.
##############################################################################
# Boole().EvtMax       = -1    # Number of events to process
# Boole().SkipEvents   = 0     # Number of events to skip before first event
# Boole().SkipSpill    = 0     # Number of spillover events to skip
# Boole().useSpillover = True  # set to False to disable spillover
# Boole().generateTAE  = False # set to True to simulate time alignment events
# Boole().Outputs      = ["DIGI"] # Possible outputs: ['MDF','DIGI','L0ETC']
# Boole().writeL0Only  = False # set to True to write only L0 selected events
# Boole().extendedDigi = False # set to True to add MCHits to .digi output file
# Boole().Histograms   = "Default" # Histogram output ['None','Default','Expert']
# Boole().noWarnings   = False # set to True to suppress MSG::WARNING or below 
# Boole().DatasetName  = ""    # string used to build output file names
# Boole().DDDBtag      = "2008-default"          # geometry database tag
# Boole().condDBtag    = "2008-default"          # conditions database tag
# Boole().UseOracle    = False # if False, use SQLDDDB, if True use Oracle
# Boole().mainOptions  = "$BOOLEOPTS/Boole.opts" # top level options to import
# Boole().monitors     = []    # Possible monitors: ['SC','FPE']
##############################################################################

##############################################################################
# I/O datasets are defined in a separate file, see examples in DC06-Files.py
##############################################################################
