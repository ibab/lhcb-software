from Gaudi.Configuration import *
from Configurables import AddNewHistToDB
addHistAlg = AddNewHistToDB("addHistAlg")

# set task and algorithm name of the histograms
#   if needed task and algorithm will be added 
#   to the DB as well
addHistAlg.TaskName      = "TTSumMon"
addHistAlg.AlgorithmName = "STErrorMonitor"

# ***********************************************
# defining helper variables to generate the
# histogram names
histName  = "error-types_$tell" 
histNum   = range(1, 49) 


# ***********************************************

# setting the type for all histograms
#   only histograms of one type can be generated 
#   at the same time
# possible values are (OnlineHistDB v5r0):
#   H1D, H2D, P1D, P2D, CNT
addHistAlg.HistoType  = "H2D"

# filling HistoNames
#   this is a list with the names of all histograms
#   that should be created.
addHistAlg.HistoNames = []
for num in histNum:
        addHistAlg.HistoNames.append(histName + str(num))



# ***********************************************

# setting the Output level
addHistAlg.OutputLevel = INFO 

# PasswordFile points to the file containing the password for HIST_WRITER
# the options should give the file name of the password 
# file, either relative to run directory or absolut.
# default value is 'password'
#addHistAlg.PasswordFile = "password"

# adding the algorithm to execution list
app = ApplicationMgr()
app.TopAlg.append(addHistAlg)
