# Script to create DaVinci Job

script='TestMyWGfromSettings_Run1.py'
job_name = "Stripping21test"
dataset = "new"

DV = DaVinci()
DV.version = 'v38r0'
DV.optsfile = [File(script)]
URA="/afs/cern.ch/work/t/tiwillia/private/Stripping21p1Test/"
DV.user_release_area = URA

DATA = '/afs/cern.ch/work/t/tiwillia/private/Stripping21p1Test/DaVinci_v38r0/Phys/StrippingSelections/tests/data/Reco14_Run125113.py'
INPUTDATA = DV.readInputData(DATA)
INPUTDATA.depth=2

j = Job(
    name           = job_name,
    application    = DV,
    splitter       = SplitByFiles(filesPerJob = 2),
    inputdata      = INPUTDATA,
    outputfiles     = [DiracFile('*.root'),DiracFile('*.mdst'),DiracFile('*.dst')],
    do_auto_resubmit = True,
    backend        = Dirac()
    )
j.submit()

