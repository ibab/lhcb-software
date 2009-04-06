from Gaudi.Configuration import *
from Configurables import RemovePages
rmAlg = RemovePages("rmAlg")


# StartFolders is the list of folders 
# that should be deleted.
#   !! All pages and subfolders will be 
#   !! delete as well.
rmAlg.StartFolders = [ "/TestFolder/subfolder1", "/TestFolder/subfolder2" ]

# If DryRun = True no changes will be 
# written to the OnlineHistDB.
# This should be used to check which 
# pages really get deleted. 
rmAlg.DryRun       = True


# PasswordFile points to the file containing the password for HIST_WRITER
# the options should give the file name of the password 
# file, either relative to run directory or absolut.
# default value is 'password'
#rmAlg.PasswordFile = "password"

# ----------------------------------------
# !! do not change unless you know what !!
# !! you are doing!                     !!
# ----------------------------------------
# This protects top level directories  
#   (e.g. "/TT" or "/VELO")
# from being deleted 
# default value is True
#rmAlg.ProtectTopLevel = True

# set output level 
rmAlg.OutputLevel  = INFO

app = ApplicationMgr()
app.TopAlg.append(rmAlg)
