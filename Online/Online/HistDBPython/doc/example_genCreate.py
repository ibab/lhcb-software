from Gaudi.Configuration import *
from Configurables import CreateHistDBPages
createHistAlg = CreateHistDBPages("createHistAlg")

# specifying the password file
#   this sting points to the file containing the password for HIST_WRITER
#   the file can be given either relative to the run directory or with
#   an absolut path.
#   the default value is 'password'
#createHistAlg.PasswordFile = "password"

# define a fixed part for all page names
#   This will be prepended to all page names and should be used to define 
#   the correct main folder for the project
createHistAlg.PageBase = "/TestDir"

# define a fixed part for all histogram names
#   This will be prepended to all histogram names and can be used to specify 
#   task and algorithm name
createHistAlg.HistoBase = "TestMon"

# set the output level for the message service
# use one of: DEBUG (2), INFO (3), WARNING (4), ERROR (5), FATAL (6) 
# additionally there are: NIL (0), VERBOSE (1), ALWAYS (7), NUM_LEVELS (8)
createHistAlg.OutputLevel = INFO

# Add the algorithm to the application manager from Gaudi
app = ApplicationMgr()
app.TopAlg.append(createHistAlg)
