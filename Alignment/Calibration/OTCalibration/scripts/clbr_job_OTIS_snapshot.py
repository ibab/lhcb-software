
UseGrid = True
#path = "/afs/cern.ch/user/a/akozlins/cmtuser/Brunel/"
#path = "/afs/cern.ch/user/a/akozlins/cmtuser/Brunel_v42r3p2/Calibration/OTCalibration/scripts/"
#path = "/afs/cern.ch/user/l/lgrillo/cmtuser/Brunel_v42r3p2/Calibration/OTCalibration/scripts_120187/"
#path = "/afs/cern.ch/user/l/lgrillo/cmtuser/Brunel_v43r2p3/Calibration/OTCalibration/scripts_135673/"
#path = "/afs/cern.ch/user/l/lgrillo/cmtuser/Brunel_v47r3p1/Calibration/OTCalibration/scripts/"
#path = "/afs/cern.ch/user/l/lgrillo/cmtuser/Brunel_v47r6p1/Calibration/OTCalibration/scripts/"
path = "/afs/cern.ch/user/l/lgrillo/cmtuser/Brunel_v47r7/Calibration/OTCalibration/scripts/"

#brunel = Brunel(version = "v47r3p1", user_release_area = "/afs/cern.ch/user/l/lgrillo/cmtuser")
#brunel = Brunel(version = "v47r6p1", user_release_area = "/afs/cern.ch/user/l/lgrillo/cmtuser")
brunel = Brunel(version = "v47r7", user_release_area = "/afs/cern.ch/user/l/lgrillo/cmtuser")
brunel.optsfile = path + "Brunel-Tracking-t0Calib_snapshot.py"

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
RunNr =  154182

MaxFiles = 100

brunel.extraopts="""
Brunel().EvtMax = 10000
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
#  job.backend.settings["Destination"] = "LCG.CERN.ch"

#job.outputsandbox = [ "clbr_hists.root" ]
#job.outputdata = [ "clbr.root" ]
#job.merger = RootMerger(files = [ "clbr_hists.root" ], overwrite = True, ignorefailed = True)

#job.outputsandbox = []
#job.outputdata = [ "clbr.root", "clbr_hists.root" ]
job.outputfiles = [ DiracFile('clbr.root'),DiracFile('clbr_hists.root') ]

#job.inputsandbox = ["ONLINE-2015.db"]

#pf = PhysicalFile('/afs/cern.ch/user/l/lgrillo/cmtuser/Brunel_v47r7/Calibration/OTCalibration/scripts/ONLINE-2015.db')
#lf = pf.upload('/ONLINE-2015.db', 'CERN-USER')

#d = DiracFile('ONLINE-2015.db')
#d.put()

job.backend.inputSandboxLFNs = ['LFN:/lhcb/user/l/lgrillo/dcda316b-57ff-4e73-bfc7-facabf9f9012/ONLINE-2015.db']


bkq = BKQuery(type='Run',dqflag='ALL',path='/154187/Real Data/95000000/RAW')
ds = bkq.getDataset()
bkq = BKQuery(type='Run',dqflag='ALL',path='/154182/Real Data/95000000/RAW')
ds.extend(bkq.getDataset())
bkq = BKQuery(type='Run',dqflag='ALL',path='/154189/Real Data/95000000/RAW')
ds.extend(bkq.getDataset())
bkq = BKQuery(type='Run',dqflag='ALL',path='/154192/Real Data/95000000/RAW')
ds.extend(bkq.getDataset())
bkq = BKQuery(type='Run',dqflag='ALL',path='/154161/Real Data/95000000/RAW')
ds.extend(bkq.getDataset())
bkq = BKQuery(type='Run',dqflag='ALL',path='/154157/Real Data/95000000/RAW')
ds.extend(bkq.getDataset())
bkq = BKQuery(type='Run',dqflag='ALL',path='/154084/Real Data/95000000/RAW')
ds.extend(bkq.getDataset())
bkq = BKQuery(type='Run',dqflag='ALL',path='/154052/Real Data/95000000/RAW')
ds.extend(bkq.getDataset())
bkq = BKQuery(type='Run',dqflag='ALL',path='/154034/Real Data/95000000/RAW')
ds.extend(bkq.getDataset())
bkq = BKQuery(type='Run',dqflag='ALL',path='/154030/Real Data/95000000/RAW')
ds.extend(bkq.getDataset())
bkq = BKQuery(type='Run',dqflag='ALL',path='/153551/Real Data/95000000/RAW')
ds.extend(bkq.getDataset())
bkq = BKQuery(type='Run',dqflag='ALL',path='/153545/Real Data/95000000/RAW')
ds.extend(bkq.getDataset())
bkq = BKQuery(type='Run',dqflag='ALL',path='/153546/Real Data/95000000/RAW')
ds.extend(bkq.getDataset())
bkq = BKQuery(type='Run',dqflag='ALL',path='/153603/Real Data/95000000/RAW')
ds.extend(bkq.getDataset())

job.inputdata = ds
job.inputdata.depth = 0

job.do_auto_resubmit=True

job.submit()
