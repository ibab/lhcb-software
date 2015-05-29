
UseGrid = True
#path = "/afs/cern.ch/user/a/akozlins/cmtuser/Brunel/"
#path = "/afs/cern.ch/user/a/akozlins/cmtuser/Brunel_v42r3p2/Calibration/OTCalibration/scripts/"
#path = "/afs/cern.ch/user/l/lgrillo/cmtuser/Brunel_v42r3p2/Calibration/OTCalibration/scripts_120187/"
#path = "/afs/cern.ch/user/l/lgrillo/cmtuser/Brunel_v43r2p3/Calibration/OTCalibration/scripts_135673/"
#path = "/afs/cern.ch/user/l/lgrillo/cmtuser/Brunel_v47r3p1/Calibration/OTCalibration/scripts/"
path = "/afs/cern.ch/user/l/lgrillo/cmtuser/Brunel_v47r6p1/Calibration/OTCalibration/scripts/"

#brunel = Brunel(version = "v47r3p1", user_release_area = "/afs/cern.ch/user/l/lgrillo/cmtuser")
brunel = Brunel(version = "v47r6p1", user_release_area = "/afs/cern.ch/user/l/lgrillo/cmtuser")
brunel.optsfile = path + "clbr_withMLA.py"

#RunNr = 87891
#RunNr = 89350
#RunNr = 91933
#RunNr = 96641
#RunNr = 101414
#RunNr = 104034

#RunNr = 111185
#RunNr = 114281
#RunNr = 114318
#RunNr = 120187
#RunNr = 129643
RunNr = 117201

MaxFiles = 8

brunel.extraopts="""
Brunel().EvtMax = 12500
Brunel().PrintFreq = 500
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
  job.splitter = SplitByFiles(filesPerJob = 1, maxFiles = MaxFiles)
#  fullList= []
#  for i in range(MaxFiles):
#    this_list = [str(i),the_fitter]
#    this_list += standard_args
#    fullList += [ this_list ]
#  job.splitter = ArgSplitter(args = fullList)
  job.backend.settings["BannedSites"] = [ "LCG.JINR.ru", "LCG.SINP.ru" ]
  job.backend.settings["Destination"] = "LCG.CERN.ch"

#job.outputsandbox = [ "clbr_hists.root" ]
#job.outputdata = [ "clbr.root" ]
#job.merger = RootMerger(files = [ "clbr_hists.root" ], overwrite = True, ignorefailed = True)

#job.outputsandbox = []
#job.outputdata = [ "clbr.root", "clbr_hists.root" ]
job.outputfiles = [ DiracFile('clbr.root'),DiracFile('clbr_hists.root') ]

#job.inputsandbox = [
##  path + "alignDB.early2012.2days.jpsi.db",
#  path + "T1UQ0@CalibrationModules.xml",
#  path + "T1UQ1@CalibrationModules.xml",
#  path + "T1UQ2@CalibrationModules.xml",
#  path + "T1UQ3@CalibrationModules.xml",
#  path + "T1VQ0@CalibrationModules.xml",
#  path + "T1VQ1@CalibrationModules.xml",
#  path + "T1VQ2@CalibrationModules.xml",
#  path + "T1VQ3@CalibrationModules.xml",
#  path + "T1X1Q0@CalibrationModules.xml",
#  path + "T1X1Q1@CalibrationModules.xml",
#  path + "T1X1Q2@CalibrationModules.xml",
#  path + "T1X1Q3@CalibrationModules.xml",
#  path + "T1X2Q0@CalibrationModules.xml",
#  path + "T1X2Q1@CalibrationModules.xml",
#  path + "T1X2Q2@CalibrationModules.xml",
#  path + "T1X2Q3@CalibrationModules.xml",
#  path + "T2UQ0@CalibrationModules.xml",
#  path + "T2UQ1@CalibrationModules.xml",
#  path + "T2UQ2@CalibrationModules.xml",
#  path + "T2UQ3@CalibrationModules.xml",
#  path + "T2VQ0@CalibrationModules.xml",
#  path + "T2VQ1@CalibrationModules.xml",
#  path + "T2VQ2@CalibrationModules.xml",
#  path + "T2VQ3@CalibrationModules.xml",
#  path + "T2X1Q0@CalibrationModules.xml",
#  path + "T2X1Q1@CalibrationModules.xml",
#  path + "T2X1Q2@CalibrationModules.xml",
#  path + "T2X1Q3@CalibrationModules.xml",
#  path + "T2X2Q0@CalibrationModules.xml",
#  path + "T2X2Q1@CalibrationModules.xml",
#  path + "T2X2Q2@CalibrationModules.xml",
#  path + "T2X2Q3@CalibrationModules.xml",
#  path + "T3UQ0@CalibrationModules.xml",
#  path + "T3UQ1@CalibrationModules.xml",
#  path + "T3UQ2@CalibrationModules.xml",
#  path + "T3UQ3@CalibrationModules.xml",
#  path + "T3VQ0@CalibrationModules.xml",
#  path + "T3VQ1@CalibrationModules.xml",
#  path + "T3VQ2@CalibrationModules.xml",
#  path + "T3VQ3@CalibrationModules.xml",
#  path + "T3X1Q0@CalibrationModules.xml",
#  path + "T3X1Q1@CalibrationModules.xml",
#  path + "T3X1Q2@CalibrationModules.xml",
#  path + "T3X1Q3@CalibrationModules.xml",
#  path + "T3X2Q0@CalibrationModules.xml",
#  path + "T3X2Q1@CalibrationModules.xml",
#  path + "T3X2Q2@CalibrationModules.xml",
#  path + "T3X2Q3@CalibrationModules.xml"
#]

                                                                                                             
#from Gaudi.Configuration import FileCatalog, EventSelector
#FileCatalog().Catalogs = [ "xmlcatalog_file:MyCatalog.xml" ]
#
#from GaudiConf import IOHelper
#IOHelper('MDF').inputFiles([
#      'mdf:root://eoslhcb.cern.ch//eos/lhcb/grid/prod/lhcb/freezer/lhcb/data/2012/RAW/EXPRESS/LHCb/COLLISION12/117201/117201_0000000009.raw',
#      'mdf:root://eoslhcb.cern.ch//eos/lhcb/grid/prod/lhcb/freezer/lhcb/data/2012/RAW/EXPRESS/LHCb/COLLISION12/117202/117202_0000000009.raw',
#      'mdf:root://eoslhcb.cern.ch//eos/lhcb/grid/prod/lhcb/freezer/lhcb/data/2012/RAW/EXPRESS/LHCb/COLLISION12/117204/117204_0000000008.raw',
#      'mdf:root://eoslhcb.cern.ch//eos/lhcb/grid/prod/lhcb/freezer/lhcb/data/2012/RAW/EXPRESS/LHCb/COLLISION12/117205/117205_0000000009.raw',
#      'mdf:root://eoslhcb.cern.ch//eos/lhcb/grid/prod/lhcb/freezer/lhcb/data/2012/RAW/EXPRESS/LHCb/COLLISION12/117206/117206_0000000009.raw',
#      'mdf:root://eoslhcb.cern.ch//eos/lhcb/grid/prod/lhcb/freezer/lhcb/data/2012/RAW/EXPRESS/LHCb/COLLISION12/117207/117207_0000000009.raw',
#      'mdf:root://eoslhcb.cern.ch//eos/lhcb/grid/prod/lhcb/freezer/lhcb/data/2012/RAW/EXPRESS/LHCb/COLLISION12/117208/117208_0000000009.raw',
#      'mdf:root://eoslhcb.cern.ch//eos/lhcb/grid/prod/lhcb/freezer/lhcb/data/2012/RAW/EXPRESS/LHCb/COLLISION12/117209/117209_0000000009.raw',
#      'mdf:root://eoslhcb.cern.ch//eos/lhcb/grid/prod/lhcb/freezer/lhcb/data/2012/RAW/EXPRESS/LHCb/COLLISION12/117210/117210_0000000009.raw',
#      'mdf:root://eoslhcb.cern.ch//eos/lhcb/grid/prod/lhcb/freezer/lhcb/data/2012/RAW/EXPRESS/LHCb/COLLISION12/117211/117211_0000000009.raw',
#      'mdf:root://eoslhcb.cern.ch//eos/lhcb/grid/prod/lhcb/freezer/lhcb/data/2012/RAW/EXPRESS/LHCb/COLLISION12/117212/117212_0000000008.raw',
#      'mdf:root://eoslhcb.cern.ch//eos/lhcb/grid/prod/lhcb/freezer/lhcb/data/2012/RAW/EXPRESS/LHCb/COLLISION12/117244/117244_0000000009.raw',
#      'mdf:root://eoslhcb.cern.ch//eos/lhcb/grid/prod/lhcb/freezer/lhcb/data/2012/RAW/EXPRESS/LHCb/COLLISION12/117245/117245_0000000009.raw',
#    ], clear=True)
#
#/castor/cern.ch/grid/lhcb/data/2012/RAW/EXPRESS/LHCb/COLLISION12/117201/117201_0000000009.raw
inputData = [
  'LFN:/lhcb/data/2012/RAW/EXPRESS/LHCb/COLLISION12/117201/117201_0000000009.raw',
  'LFN:/lhcb/data/2012/RAW/EXPRESS/LHCb/COLLISION12/117202/117202_0000000009.raw',
  'LFN:/lhcb/data/2012/RAW/EXPRESS/LHCb/COLLISION12/117204/117204_0000000008.raw',
  'LFN:/lhcb/data/2012/RAW/EXPRESS/LHCb/COLLISION12/117205/117205_0000000009.raw',
  'LFN:/lhcb/data/2012/RAW/EXPRESS/LHCb/COLLISION12/117206/117206_0000000009.raw',
  'LFN:/lhcb/data/2012/RAW/EXPRESS/LHCb/COLLISION12/117207/117207_0000000009.raw',
  'LFN:/lhcb/data/2012/RAW/EXPRESS/LHCb/COLLISION12/117208/117208_0000000009.raw',
  'LFN:/lhcb/data/2012/RAW/EXPRESS/LHCb/COLLISION12/117209/117209_0000000009.raw'
]
#inputData = []
#import os
#inputDataPath = "/lhcb/data/2011/RAW/FULL/LHCb/COLLISION11/" + str(RunNr) + "/"
#inputDataPath = "/lhcb/data/2013/RAW/FULL/LHCb/PROTONION13/" + str(RunNr) + "/"
#put, get = os.popen4("nsls /castor/cern.ch/grid" + inputDataPath)
#for line in get.readlines() :
#  split = line.split(' ')
#  inputFile = inputDataPath + split[0].strip()
#  if UseGrid == False : inputData += [ "PFN:castor:/castor/cern.ch/grid" + inputFile ]
#  if UseGrid == True  : inputData += [ "LFN:"                            + inputFile ]
print inputData
job.inputdata = inputData
job.inputdata.depth = 0

job.do_auto_resubmit=True

job.submit()
