#===============================================================================
# #         j.peek()                                                         # #
# #         j.peek('stdout', 'tail -f')                                      # #
#===============================================================================

# Python modules
import os

cwd = os.getcwd()
cwd = cwd.split("_")
davinci_version = (cwd[1].split("/"))[0]

DaVinciVersion   = davinci_version
DaVinciVersion   = davinci_version

my_file          = 'PFDaVinciMC11.py'
thisdir          = os.path.abspath(os.path.curdir)
my_pyFile        = thisdir + "/" + my_file

#-------------------------------------------------------------------------------
j = Job( application = DaVinci( version = DaVinciVersion ) )
j.name = 'bbar_MC11_' + DaVinciVersion
j.application.optsfile  = [ File(my_pyFile) ]
#j.application.extraopts = '''DaVinci().EvtMax = 1000
j.application.extraopts = '''DaVinci.EvtMax = -1
DaVinci.PrintFreq  = 10000
'''

'''
from Configurables import CondDB              # <<< For submition on GANGA(DIRAC)
CondDB(UseOracle = True)                     # <<< For submition on GANGA(DIRAC)
importOptions("$APPCONFIGOPTS/DisableLFC.py") # <<< For submition on GANGA(DIRAC)
'''

#j.splitter = None

#j.splitter = SplitByFiles ( filesPerJob = 6, maxFiles = -1 )
#j.outputdata    = ["DVNtuples.root", "Bd2JpsiKS.dst"] #The ntuple will be kept in the Dirac (we need to download it).

#j.splitter = SplitByFiles ( filesPerJob = 600, maxFiles = -1 )
j.splitter = DiracSplitter ( filesPerJob = 100, maxFiles = -1 , ignoremissing = True )
#j.splitter = GenericSplitter ( filesPerJob = 200, maxFiles = -1 )
#j.splitter = DiracSplitter ( filesPerJob = 1, maxFiles = -1,ignoremissing=True )


#j.outputdata    = ["JES_minbias_histos_MC.root","JES_minbias_MC.root"]
j.outputdata    = ["JEShistosMC.root","ZJESAllMC.root"]

#j.merger = None
#j.merger = RootMerger( files = ['MuHadBinc.root'], ignorefailed = True )
#j.inputsandbox  = []
#j.outputsandbox = ["D2CharmMCTrue.root"]
#-------------------------------------------------------------------------------

#-------------------------------------------------------------------------------
#help(BKQuery)
bkq_Up = BKQuery( dqflag = "All" , path = "", type = "Path" )
bkq_Up.path  = "/MC/MC11a/Beam3500GeV-2011-MagUp-Nu2-EmNoCuts/Sim05a/Trig0x40760037Flagged/Reco12a/Stripping17NoPrescalingFlagged/10000010/ALLSTREAMS.DST"
bkq_Down = BKQuery( dqflag = "All" , path = "", type = "Path" )
bkq_Down.path  = "/MC/MC11a/Beam3500GeV-2011-MagDown-Nu2-EmNoCuts/Sim05a/Trig0x40760037Flagged/Reco12a/Stripping17NoPrescalingFlagged/10000010/ALLSTREAMS.DST"

dataset_All = LHCbDataset()
dataset_All = LHCbDataset(files = [ f for f in bkq_Up.getDataset().files ] )
dataset_All.files.extend(bkq_Down.getDataset().files)

dataset = LHCbDataset( files =[ f for f in dataset_All.files ] )

print '\n Total \n ', dataset.files
j.inputdata = dataset
#-------------------------------------------------------------------------------

#-------------------------------------------------------------------------------
#j.backend = Interactive()
#j.backend = Local()
#j.backend = LSF( queue = '8nm' ) # 8nm  = about 30 min HP735     express
#j.backend = LSF( queue = '1nh' ) # 1nh  = about  3 hours         short
#j.backend = LSF( queue = '8nh' ) # 8nh  = about  1 day           medium
#j.backend = LSF( queue = '1nd' ) # 1nd  = about  3 days          long
#j.backend = LSF( queue = '1nw' ) # 1nw  = about 10 days HP K260  verylong
j.backend = Dirac()
#-------------------------------------------------------------------------------

#-------------------------------------------------------------------------------
j.submit()
#-------------------------------------------------------------------------------
