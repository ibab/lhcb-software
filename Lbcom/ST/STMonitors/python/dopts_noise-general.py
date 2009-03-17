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
# prefixed with the content of taskName and algoBase defined in the 
# generalOptions (included below)
# *) if int is true, saveHistoSetDisplayOptions() will be called
# i.e. the changes will be applied for the whole set
setDisplay.HistoNames = {
  "/Raw/raw_$tell1" : 1 , 
  "/CMS/cms_$tell1" : 1 
  } 

# Should all display options for these Histograms be unset 
# before the changes below are made?
setDisplay.UnsetOptions = 1 
    
# The actual definition of the display options. 
# unused options should just be commented out
#   This list is not updated on regular basis
#   check histdbdoc for the latest list of 
#   availbale options

 # LABEL_X: max length 50
setDisplay.stringOptions["LABEL_X"]   = "Strip"

setDisplay.floatOptions["YMIN"]       = 0
setDisplay.floatOptions["YMAX"]       = 10

#setDisplay.intOptions["LINESTYLE"]    = 1
setDisplay.intOptions["LINECOLOR"]    = 4
#setDisplay.intOptions["LINEWIDTH"]    = 0
#setDisplay.intOptions["MARKERCOLOR"]  = 3
#setDisplay.intOptions["MARKERSTYLE"]  = 2
 ## DRAWOPTS: max length 50
setDisplay.stringOptions["DRAWOPTS"]  = "hist"
 

# ****** END EDITABLE ******

# application manager and messag service settings
importOptions("inc_dispOptsNZS.py")
