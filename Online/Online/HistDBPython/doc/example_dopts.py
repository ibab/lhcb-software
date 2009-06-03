from Gaudi.Configuration import *
from Configurables import SetHistDisplayOptions
setDisplay = SetHistDisplayOptions("setDisplay")

# Setting up the dictionaries for the display options. 
#   There is one dictionary for each data type with the structure 
#   { string : <datatype> }
#   (floatOptions is internally stored in a <string, double>-map 
#    but the values are cast to float before setting)
setDisplay.intOptions = {}
setDisplay.floatOptions  = {}
setDisplay.stringOptions = {}


# ***********************************************
# defining the histograms to edit
#   HistoNames is a dictionary with strucutre { string : boolean } 
#   *) the string is the identifier of the histogram, the string will get 
#      prefixed with the content of HistoBase  
#   *) if int is true, saveHistoSetDisplayOptions() will be called
#      i.e. the changes will be applied for the whole set
setDisplay.HistoNames = {
  "/error-types_$tell1" : True
} 

# Should all display options for these Histograms be unset 
# before the changes below are made?
setDisplay.UnsetOptions = True 
    
# Filling the display option dicitonaries with entries 
setDisplay.stringOptions["LABEL_X"]   = "Optical Link"
setDisplay.intOptions["NDIVX"]        = 424

setDisplay.floatOptions["LAB_X_SIZE"] = 0.04
setDisplay.floatOptions["LAB_Y_SIZE"] = 0.04

setDisplay.stringOptions["DRAWOPTS"]  = "box"


# ***********************************************
# Define custom bin labels
#   for each axis there is a list with one 
#   string for each bin 
#setDisplay.xLabels = [];
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


# ***********************************************
# application manager and messag service settings
importOptions("example_genSet.py")
