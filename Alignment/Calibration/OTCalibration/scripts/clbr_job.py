
UseGrid = True
#path = "/afs/cern.ch/user/a/akozlins/cmtuser/Brunel/"
path = "/afs/cern.ch/user/a/akozlins/cmtuser/Brunel_v42r3p2/Calibration/OTCalibration/scripts/"

brunel = Brunel(version = "v42r3p2", user_release_area = "/afs/cern.ch/user/a/akozlins/cmtuser")
brunel.optsfile = path + "clbr.py"

#RunNr = 87891
#RunNr = 89350
#RunNr = 91933
#RunNr = 96641
#RunNr = 101414
#RunNr = 104034

#RunNr = 111185
RunNr = 114281

MaxFiles = 100

brunel.extraopts="""
Brunel().EvtMax = 10000
Brunel().PrintFreq = 50
Brunel().SkipEvents = 0
"""

job = Job(application = brunel, name = "OT_clbr_" + str(RunNr))

if UseGrid == False :
  brunel.extraopts += "\nCondDB(UseOracle = True)\n"
  brunel.setupProjectOptions = "--use-grid"
  job.backend = LSF(queue = "1nh")
  job.splitter = SplitByFiles(filesPerJob = 1, maxFiles = MaxFiles)
if UseGrid == True :
#  config.LHCb.LocalSite="LCG.CERN.ch"
  job.backend = Dirac()
  job.splitter = DiracSplitter(filesPerJob = 1, maxFiles = MaxFiles)
  job.backend.settings["BannedSites"] = [ "LCG.JINR.ru", "LCG.SINP.ru" ]
  job.backend.settings["Destination"] = "LCG.CERN.ch"

#job.outputsandbox = [ "clbr_hists.root" ]
#job.outputdata = [ "clbr.root" ]
#job.merger = RootMerger(files = [ "clbr_hists.root" ], overwrite = True, ignorefailed = True)

#job.outputsandbox = []
job.outputdata = [ "clbr.root", "clbr_hists.root" ]

job.inputsandbox = [
#  path + "alignDB.early2012.2days.jpsi.db",
  path + "T1UQ0@CalibrationModules.xml",
  path + "T1UQ1@CalibrationModules.xml",
  path + "T1UQ2@CalibrationModules.xml",
  path + "T1UQ3@CalibrationModules.xml",
  path + "T1VQ0@CalibrationModules.xml",
  path + "T1VQ1@CalibrationModules.xml",
  path + "T1VQ2@CalibrationModules.xml",
  path + "T1VQ3@CalibrationModules.xml",
  path + "T1X1Q0@CalibrationModules.xml",
  path + "T1X1Q1@CalibrationModules.xml",
  path + "T1X1Q2@CalibrationModules.xml",
  path + "T1X1Q3@CalibrationModules.xml",
  path + "T1X2Q0@CalibrationModules.xml",
  path + "T1X2Q1@CalibrationModules.xml",
  path + "T1X2Q2@CalibrationModules.xml",
  path + "T1X2Q3@CalibrationModules.xml",
  path + "T2UQ0@CalibrationModules.xml",
  path + "T2UQ1@CalibrationModules.xml",
  path + "T2UQ2@CalibrationModules.xml",
  path + "T2UQ3@CalibrationModules.xml",
  path + "T2VQ0@CalibrationModules.xml",
  path + "T2VQ1@CalibrationModules.xml",
  path + "T2VQ2@CalibrationModules.xml",
  path + "T2VQ3@CalibrationModules.xml",
  path + "T2X1Q0@CalibrationModules.xml",
  path + "T2X1Q1@CalibrationModules.xml",
  path + "T2X1Q2@CalibrationModules.xml",
  path + "T2X1Q3@CalibrationModules.xml",
  path + "T2X2Q0@CalibrationModules.xml",
  path + "T2X2Q1@CalibrationModules.xml",
  path + "T2X2Q2@CalibrationModules.xml",
  path + "T2X2Q3@CalibrationModules.xml",
  path + "T3UQ0@CalibrationModules.xml",
  path + "T3UQ1@CalibrationModules.xml",
  path + "T3UQ2@CalibrationModules.xml",
  path + "T3UQ3@CalibrationModules.xml",
  path + "T3VQ0@CalibrationModules.xml",
  path + "T3VQ1@CalibrationModules.xml",
  path + "T3VQ2@CalibrationModules.xml",
  path + "T3VQ3@CalibrationModules.xml",
  path + "T3X1Q0@CalibrationModules.xml",
  path + "T3X1Q1@CalibrationModules.xml",
  path + "T3X1Q2@CalibrationModules.xml",
  path + "T3X1Q3@CalibrationModules.xml",
  path + "T3X2Q0@CalibrationModules.xml",
  path + "T3X2Q1@CalibrationModules.xml",
  path + "T3X2Q2@CalibrationModules.xml",
  path + "T3X2Q3@CalibrationModules.xml"
]

inputData = []
import os
#inputDataPath = "/lhcb/data/2011/RAW/FULL/LHCb/COLLISION11/" + str(RunNr) + "/"
inputDataPath = "/lhcb/data/2012/RAW/FULL/LHCb/COLLISION12/" + str(RunNr) + "/"
put, get = os.popen4("nsls /castor/cern.ch/grid" + inputDataPath)
for line in get.readlines() :
  split = line.split(' ')
  inputFile = inputDataPath + split[0].strip()
  if UseGrid == False : inputData += [ "PFN:castor:/castor/cern.ch/grid" + inputFile ]
  if UseGrid == True  : inputData += [ "LFN:"                            + inputFile ]
print inputData
job.inputdata = inputData
job.inputdata.depth = 0

job.submit()
