# this file contains general configurations for the 
# display option algorithm 

# needed in all display option files
from Gaudi.Configuration import *
from Configurables import SetHistDisplayOptions
setDisplay = SetHistDisplayOptions("setDisplay")


# setting the general options of the Application

# PasswordFile points to the file containing the password for HIST_WRITER
# the options should give the file name of the password 
# file, either relative to run directory or absolut.
# default value is 'password'
#createHistAlg.PasswordFile = "password"

# HistoBase can be used to define a fixed part for all histogram names
# (e.g. task and algorithm name)
setDisplay.HistoBase = "TestMon"

# set the output level for the message service
# use one of: DEBUG (2), INFO (3), WARNING (4), ERROR (5), FATAL (6) 
# additionally there are: NIL (0), VERBOSE (1), ALWAYS (7), NUM_LEVELS (8)
setDisplay.OutputLevel = DEBUG

# Add the algorithm to the application manager
# from Gaudi
app = ApplicationMgr()
app.TopAlg.append(setDisplay)
