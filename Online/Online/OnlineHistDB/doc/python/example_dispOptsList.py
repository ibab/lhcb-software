# this file defines the different display options for the histograms on a page
# and is used to reset all these options when running a script to define new 
# display options
# this will become obsolete with future versions of OnlineHistDB

# needed in all display options files
from Gaudi.Configuration import *
from Configurables import STSetHistDisplayOptions
setDisplay = STSetHistDisplayOptions("setDisplay")

# List with the display options
setDisplay.OptionsList = [
"LABEL_X", 
"XMIN", 
"XMAX", 
"LOGX", 
"LABEL_Y", 
"YMIN", 
"YMAX", 
"LABEL_Z",
"STATS", 
"FILLSTYLE", 
"FILLCOLOR", 
"LINESTYLE", 
"LINECOLOR", 
"LINEWIDTH", 
"DRAWOPTS", 
"ZMIN", 
"ZMAX", 
"LOGY", 
"LOGZ", 
"TIMAGE", 
"REF", 
"REFRESH", 
"TIT_X_SIZE", 
"TIT_X_OFFS", 
"TIT_Y_SIZE", 
"TIT_Y_OFFS", 
"TIT_Z_SIZE", 
"TIT_Z_OFFS", 
"LAB_X_SIZE", 
"LAB_X_OFFS", 
"LAB_Y_SIZE", 
"LAB_Y_OFFS", 
"LAB_Z_SIZE", 
"LAB_Z_OFFS", 
"GRIDX", 
"GRIDY", 
"THETA", 
"PHI", 
"CNTPLOT"
]
