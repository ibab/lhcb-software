# #         j.peek()                                                         # #
# #         j.peek('stdout', 'tail -f')                                      # #
#===============================================================================

# Python modules
import os

DaVinciVersion   = 'v29r1'
my_file          = 'PFDaVinci.py'
thisdir          = os.path.abspath(os.path.curdir)
my_pyFile        = thisdir + "/" + my_file

#-------------------------------------------------------------------------------
j = Job( application = DaVinci( version = DaVinciVersion ))#, platform = 'x86_64-slc5-gcc43-opt')) #'slc4_ia32_gcc34') )
j.name = "MC10Zmumu-MCJetsAnalysis"#'LoKiv2-Dirac-Splitter-Dirac' #'MCTrue10_DV' + DaVinciVersion + "LFN"
j.application.optsfile  = [ File(my_pyFile) ]
j.application.extraopts = '''DaVinci.EvtMax = -1
DaVinci.PrintFreq  = 1000
#from Configurables import CondDB              # <<< For submition on GANGA(DIRAC)
#CondDB(UseOracle = True)                     # <<< For submition on GANGA(DIRAC)
#importOptions("$APPCONFIGOPTS/DisableLFC.py") # <<< For submition on GANGA(DIRAC)
'''

#j.splitter = None

#j.splitter = SplitByFiles ( filesPerJob = 6, maxFiles = -1 )
#j.outputdata    = ["DVNtuples.root", "Bd2JpsiKS.dst"] #The ntuple will be kept in the Dirac (we need to download it).

#j.splitter = SplitByFiles ( filesPerJob = 1, maxFiles = -1 )

j.splitter = DiracSplitter ( filesPerJob = 1, maxFiles = -1, ignoremissing = True )
 
j.outputdata    = ["JEShistosMC.root","JESAllMC.root"] #The ntuple will be kept in the Dirac (we need to download it).

#j.merger = None
#j.merger = RootMerger( files = ['"MCMuHadmuqq.root'], ignorefailed = True )
j.inputsandbox  = []
#j.outputsandbox = ["D2CharmMCTrue.root"]
#-------------------------------------------------------------------------------

#-------------------------------------------------------------------------------
#help(BKQuery)
bkq = BKQuery( dqflag = "All" , path = "", type = "Path" )

##bkq.path = "/MC/MC09/Beam5TeV-VeloClosed-MagDown-Nu1/MC09-Sim04Reco02-withTruth/46000014/DST" # muqq 
#bkq.path = "/MC/MC09/Beam5TeV-VeloClosed-MagDown-Nu1/Sim04/Reco02-withTruth/46000014/DST"
#sim://MC/MC10/Beam3500GeV-Oct2010-MagUp-Nu2,5/Sim01/Trig0x002e002aFlagged/Reco08/Stripping12Flagged/42112020 ( Z_mumujet )/ALLSTREAMS.DST
bkq.path="/MC/MC10/Beam3500GeV-Oct2010-MagDown-Nu2,5/Sim01/Trig0x002e002aFlagged/Reco08/Stripping12Flagged/42112020/ALLSTREAMS.DST"

dataset = bkq.getDataset()
#print '\n ', bkq.path, '\n ', dataset.files
#-----------------------------------------------------------------------------
# Get the triggered data
#-----------------------------------------------------------------------------
#dataset = LHCbDataset()
#for sj in jobs(154).subjobs:
#     if sj.status == 'completed':  
#           dataset.extend(sj.backend.getOutputDataLFNs())
#print '\n ', dataset.files
#-----------------------------------------------------------------------------
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
j.submit()
#-------------------------------------------------------------------------------
