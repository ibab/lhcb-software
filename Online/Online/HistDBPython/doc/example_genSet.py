from Gaudi.Configuration import *
from Configurables import SetHistDisplayOptions
setDisplay = SetHistDisplayOptions("setDisplay")

# specifying the password file
#   this sting points to the file containing the password for HIST_WRITER
#   the file can be given either relative to the run directory or with
#   an absolut path.
#   the default value is 'password'
#setDisplay.PasswordFile = "password"

# define a fixed part for all histogram names
#   (e.g. task and algorithm name)
setDisplay.HistoBase = "TestMon"

# set the output level for the message service
#   use one of: DEBUG (2), INFO (3), WARNING (4), ERROR (5), FATAL (6) 
#   additionally there are: NIL (0), VERBOSE (1), ALWAYS (7), NUM_LEVELS (8)
setDisplay.OutputLevel = DEBUG

# Add the algorithm to the application manager from Gaudi
app = ApplicationMgr()
app.TopAlg.append(setDisplay)
