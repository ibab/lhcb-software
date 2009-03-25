# this file contains general configurations for the creation of pages
# and adds the algorithm to the application manager

# needed in all option files
from Gaudi.Configuration import *
from Configurables import CreateHistDBPages
createHistAlg = CreateHistDBPages("createHistAlg")

# setting the general options of the Application

# PasswordFile points to the file containing the password for HIST_WRITER
# the options should give the file name of the password 
# file, either relative to run directory or absolut.
# default value is 'password'
#createHistAlg.PasswordFile = "password"

# PageBase is prepended all pages.
# This should be used to define the correct 
# main folder for the project
createHistAlg.PageBase = "/TestDir"

# HistoBase can be used to define a fixed part for all histogram names
# (e.g. task and algorithm name)
createHistAlg.HistoBase = "TestMon"

# set the output level for the message service
# use one of: DEBUG (2), INFO (3), WARNING (4), ERROR (5), FATAL (6) 
# additionally there are: NIL (0), VERBOSE (1), ALWAYS (7), NUM_LEVELS (8)
createHistAlg.OutputLevel = INFO

# Add the algorithm to the application manager
# from Gaudi
app = ApplicationMgr()
app.TopAlg.append(createHistAlg)
