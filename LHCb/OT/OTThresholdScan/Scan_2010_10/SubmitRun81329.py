b = Bender()
br = Brunel()

b.version = 'v15r1p1'
b.user_release_area = '/afs/cern.ch/user/d/dvaneijk/cmtuser'
b.module = '/stage/Daan/OTEfficiency/ThresholdScans/Scan_2010_10/ProcessRun81329.py'
b.events = 100

data1 = '/stage/Daan/OTEfficiency/ThresholdScans/Scan_2010_10/RUN_81329_RealData_90000001_RAW_0_99.py'
data2 = '/stage/Daan/OTEfficiency/ThresholdScans/Scan_2010_10/RUN_81329_RealData_90000001_RAW_100_198.py'
data3 = '/stage/Daan/OTEfficiency/ThresholdScans/Scan_2010_10/RUN_81329_RealData_90000001_RAW_199_220.py'

J1 = Job(name = 'Profiles_Run81329_Part1',application = b, backend=Dirac())
#J1 = Job(name = 'Profiles_Run81329_Interactive',application = b, backend=Interactive()) 
J1.inputdata = br.readInputData( data1 )
#J1.splitter = DiracSplitter( filesPerJob = 5, maxFiles = 600)
J1.outputdata = ['histos.root']
#J1.outputdata = ['tuples.root']
J1.submit()

## J2 = Job(name = 'Profiles_Run81329_Part2',application = b, backend=Dirac())
## #J2 = Job(name = 'Profiles_Run81329_Interactive',application = b, backend=Interactive()) 
## J2.inputdata = br.readInputData( data2 )
## #J2.splitter = DiracSplitter( filesPerJob = 5, maxFiles = 600)
## J2.outputdata = ['histos.root']
## #J2.outputdata = ['tuples.root']
## J2.submit()

## J3 = Job(name = 'Profiles_Run81329_Part3',application = b, backend=Dirac())
## #J3 = Job(name = 'Profiles_Run81329_Interactive',application = b, backend=Interactive()) 
## J3.inputdata = br.readInputData( data3 )
## #J3.splitter = DiracSplitter( filesPerJob = 5, maxFiles = 600)
## J3.outputdata = ['histos.root']
## #J3.outputdata = ['tuples.root']
## J3.submit()

