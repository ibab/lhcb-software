# Script to create DaVinci Job

script='RunStrippingTool.py'
job_name = "Bc_Kpipi"
dataset = "new"

DV = DaVinci()
DV.version = 'v38r0'
DV.optsfile = [File(script)]
URA="/afs/cern.ch/work/t/tiwillia/private/StrippingTestDir/"
DV.user_release_area = URA

DATA = 'BhadronList.py'
INPUTDATA = DV.readInputData(DATA)
INPUTDATA.depth=2

j = Job(
    name           = job_name,
    application    = DV,
    splitter       = SplitByFiles(filesPerJob = 5,maxFiles = 100),
    inputdata      = INPUTDATA,
    outputfiles     = [DiracFile('*.root')],
    do_auto_resubmit = True,
    backend        = Dirac()
    )
j.submit()

