from Gaudi.Configuration import *
from Configurables import RemovePages
rmAlg = RemovePages("rmAlg")

# defining the root of the branches to delete
#   StartFolders is a list of stings
#   !! All pages and subfolders will be delete
rmAlg.StartFolders = [ "/TestFolder/subfolder1", "/TestFolder/subfolder2" ]

# simulate deletion?
#   If DryRun = True all pages affected by this configuration will be printed
#   but no changes will be written to the OnlineHistDB.
# This should be used to check which pages really get deleted. 
rmAlg.DryRun       = True

# specifying the password file
#   this sting points to the file containing the password for HIST_WRITER
#   the file can be given either relative to the run directory or with
#   an absolut path.
#   the default value is 'password'
#rmAlg.PasswordFile = "password"

# Protect top level directories from deletion
# ----------------------------------------
# !! do not change unless you know what !!
# !! you are doing!                     !!
# ----------------------------------------
#   if ProtectTopLevel is True, top level directories (e.g. "/TT" or "/VELO")
#   in StartFolders will get ignored
#rmAlg.ProtectTopLevel = True

# set output level 
rmAlg.OutputLevel  = INFO

app = ApplicationMgr()
app.TopAlg.append(rmAlg)
