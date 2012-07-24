import os
#from ModelFinder import *
#gangaArea = "/afs/cern.ch/work/m/mcoombes/Ganga/ganga_workspace1/workspace/mcoombes/LocalXML"
modelTextFile = "Models.txt" # output models to this text file
#Chi2TextFile = "Chi2.txt"
max_iterations  = 5


MintDir = "/afs/cern.ch/work/m/mcoombes/cmtuser/Gen/Mint/src/"

#Users area directory where ampFit is.
UserArea = "Users/Matt/FourPiModelBuilder/" #AmpFit Users Area

inputLFNs = ['LFN:/lhcb/user/m/mcoombes/FourPiModelFit/BinningFile_D0.root',
             'LFN:/lhcb/user/m/mcoombes/FourPiModelFit/NocutsMC.root']

#Where is the executable for the ModelBuilder.
ModelBuilderExe = MintDir+"Users/Matt/ModelCreator/ModelBuilder"

#Keep as this directory. Points to where the submission script is
SubmitModelBuilder = MintDir+'Users/Matt/ModelBuilderGridSubmission/ModelSubmission.py'


print "Before submitting make sure you have compiled ampFit with DaVinci v29r2 in order to get the correct ROOT version"
print "\n"

if ( not( os.path.exists("BG4piModel.txt") ) ) :
    print "ERROR: file ", MintDir+UserArea+"BG4piModel.txt does not exist"
    print "You must have the background model file BG4piModel.txt in userarea\n\n"
    
print "To run type in the Ganga prompt execfile(SubmitModelBuilder)"
