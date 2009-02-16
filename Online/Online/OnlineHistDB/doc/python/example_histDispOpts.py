# this file defines the display options for histogram

# needed in all display option files
from Gaudi.Configuration import *
from Configurables import STSetHistDisplayOptions
setDisplay = STSetHistDisplayOptions("setDisplay")

# setting setDisplay.OptionsList 
# all Options in this list will be reset 
# before applying the changes defined below.
importOptions("example_dispOptsList.py")

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
# prefixed with the content of taskName and algoBase defined in the 
# generalOptions (included below)
# *) if int is true, saveHistoSetDisplayOptions() will be called
# i.e. the changes will be applied for the whole set
setDisplay.HistosToEdit = {"Central/IT-Cluster_Map_Live/Central/0" : 0}


# The actual definition of the display options. 
# unused options should just be commented out

 # LABEL_X: max length 50
setDisplay.stringOptions["LABEL_X"]   = "xAxis"
setDisplay.floatOptions["XMIN"]       = 0.5
setDisplay.floatOptions["XMAX"]       = 2
 # boolean: 0 = flase, 1 = true
setDisplay.intOptions["LOGX"]         = 1
 # boolean: 0 = flase, 1 = true
setDisplay.intOptions["GRIDX"]        = 1

 ## LABEL_Y: max length 50
#setDisplay.stringOptions["LABEL_Y"]   = "yAxis"
#setDisplay.floatOptions["YMIN"]       = 0.5
#setDisplay.floatOptions["YMAX"]       = 0.5
 ## boolean: 0 = flase, 1 = true
#setDisplay.intOptions["LOGY"]         = 1
 ## boolean: 0 = flase, 1 = true
#setDisplay.intOptions["GRIDY"]        = 1

 ## LABEL_Z: max length 50
#setDisplay.stringOptions["LABEL_Z"]   = "zAxis"
#setDisplay.floatOptions["ZMIN"]       = 0.5
#setDisplay.floatOptions["ZMAX"]       = 0.5
 ## boolean: 0 = flase, 1 = true
#setDisplay.intOptions["LOGZ"]         = 1


 ## type of 3D plot, one of:
 ##   HIST, IMAGE, AUTO
setDisplay.stringOptions["TIMAGE"]    = "HIST"
#setDisplay.floatOptions["THETA"]      = 0.5
#setDisplay.floatOptions["PHI"]        = 0.5

#setDisplay.intOptions["STATS"]        = 1
#setDisplay.intOptions["FILLSTYLE"]    = 1
#setDisplay.intOptions["FILLCOLOR"]    = 1
#setDisplay.intOptions["LINESTYLE"]    = 1
#setDisplay.intOptions["LINECOLOR"]    = 1
#setDisplay.intOptions["LINEWIDTH"]    = 1
 ## DRAWOPTS: max length 50
#setDisplay.stringOptions["DRAWOPTS"]  = ""
 
 ## Normalisations for overplot reference (if available), one of:
 ##   NONE, AREA, ENTR
setDisplay.stringOptions["REF"]       = "AREA"

#setDisplay.floatOptions["REFRESH"]    = 0.5
setDisplay.floatOptions["TIT_X_SIZE"] = 3.5
setDisplay.floatOptions["TIT_X_OFFS"] = 4.5
#setDisplay.floatOptions["TIT_Y_SIZE"] = 0.5
#setDisplay.floatOptions["TIT_Y_OFFS"] = 0.5
#setDisplay.floatOptions["TIT_Z_SIZE"] = 0.5
#setDisplay.floatOptions["TIT_Z_OFFS"] = 0.5
setDisplay.floatOptions["LAB_X_SIZE"] = 6.5
setDisplay.floatOptions["LAB_X_OFFS"] = 7.5
#setDisplay.floatOptions["LAB_Y_SIZE"] = 0.5
#setDisplay.floatOptions["LAB_Y_OFFS"] = 0.5
#setDisplay.floatOptions["LAB_Z_SIZE"] = 0.5
#setDisplay.floatOptions["LAB_Z_OFFS"] = 0.5
 
 ## CNTPLOT: max length 5
#setDisplay.stringOptions["CNTPLOT"]   = ""

# ****** END EDITABLE ******

# application manager and messag service settings
importOptions("example_dispGenOpts.py")