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
# Boole().UseSpillover = True  # set to False to disable spillover
# Boole().GenerateTAE  = False # set to True to simulate time alignment events
# Boole().Outputs      = ["DIGI"] # Possible outputs: ['MDF','DIGI','L0ETC']
# Boole().WriteL0Only  = False # set to True to write only L0 selected events
# Boole().ExtendedDigi = False # set to True to add MCHits to .digi output file
# Boole().Histograms   = "Default" # Histogram output ['None','Default','Expert']
# Boole().NoWarnings   = False # set to True to suppress MSG::WARNING or below 
# Boole().DatasetName  = ""    # string used to build output file names
# Boole().DataType     = "2008"# Data type, can be ['DC06','2008']
# Boole().DDDBtag      = ""    # Tag for DDDB. Default as set in DDDBConf
# Boole().CondDBtag    = ""    # Tag for CondDB. Default as set in DDDBConf
# Boole().UseOracle    = False # if False, use SQLDDDB, if True use Oracle
# Boole().MainOptions  = "$BOOLEOPTS/Boole.opts" # top level options to import
# Boole().Monitors     = []    # Possible monitors: ['SC','FPE']
##############################################################################

##############################################################################
# I/O datasets are defined in a separate file, see examples in DC06-Files.py
##############################################################################
