import os
#from ModelFinder import *
#gangaArea = "/afs/cern.ch/work/m/mcoombes/Ganga/ganga_workspace1/workspace/mcoombes/LocalXML"
modelTextFile = "Models.txt" # output models to this text file
#Chi2TextFile = "Chi2.txt"
max_iterations  = 5
MintDir = "/afs/cern.ch/work/m/mcoombes/cmtuser/Gen/Mint/src/"
UserArea = "Users/Matt/FourPiModelBuilder/" #AmpFit Users Area

inputLFNs = ['LFN:/lhcb/user/m/mcoombes/FourPiModelFit/BinningFile_D0.root',
             'LFN:/lhcb/user/m/mcoombes/FourPiModelFit/NocutsMC.root']

ModelBuilderExe = MintDir+"Users/Matt/ModelCreator/ModelBuilder"

SubmitModelBuilder = MintDir+'Users/Matt/ModelBuilderSubmission/ModelSubmission.py'

