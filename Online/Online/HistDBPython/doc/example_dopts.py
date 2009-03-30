# this file defines the display options for histogram

# needed in all display option files
from Gaudi.Configuration import *
from Configurables import SetHistDisplayOptions
setDisplay = SetHistDisplayOptions("setDisplay")

# Setting up the lists for the display options. There is one 
# list for each data type. 
# floatOptions is actually stored in a <string, double>-map 
# but the values are cast to float before setting
setDisplay.intOptions = {}
setDisplay.floatOptions  = {}
setDisplay.stringOptions = {}


# ****** BEGIN EDITABLE ******

# map with the identifiers for the histograms to edit
# strucutre:  { string : int } 
# *) the string is the identifier of the histogram, the string will get 
# prefixed with the content of HistoBase defined in the 
# generalOptions (included below)
# *) if int is true, saveHistoSetDisplayOptions() will be called
# i.e. the changes will be applied for the whole set
setDisplay.HistoNames = {
  "/error-types_$tell1" : 1
} 

# Should all display options for these Histograms be unset 
# before the changes below are made?
setDisplay.UnsetOptions = 1 
    
# The actual definition of the display options. 
# unused options should just be commented out
#   This list is not updated on regular basis
#   check histdbdoc for the latest list of 
#   availbale options

setDisplay.stringOptions["LABEL_X"]   = "Optical Link"
setDisplay.intOptions["NDIVX"]        = 424

setDisplay.floatOptions["LAB_X_SIZE"] = 0.04
setDisplay.floatOptions["LAB_Y_SIZE"] = 0.04

setDisplay.stringOptions["DRAWOPTS"]  = "box"

# Define custom bin labels
#   for each axis there is a list with one 
#   string for each bin  
setDisplay.yLabels = []
setDisplay.yLabels.append("None");
setDisplay.yLabels.append("CorruptedBank");
setDisplay.yLabels.append("OptLinkDisabled");
setDisplay.yLabels.append("TlkLinkLoss");
setDisplay.yLabels.append("OptLinkNoClock");
setDisplay.yLabels.append("SyncRAMFull");
setDisplay.yLabels.append("SyncEvtSize");
setDisplay.yLabels.append("OptLinkNoEvent");
setDisplay.yLabels.append("PseudoHeader");
setDisplay.yLabels.append("WrongPCN");


# ****** END EDITABLE ******

# application manager and messag service settings
importOptions("example_genSet.py")
