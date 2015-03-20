# Script to create DaVinci Job
# Mika Vesterinen

script='Phys/StrippingSelections/tests/users/TestB2DstMuNuIncl.py'
job_name = "B2DMuNuX_180814"
dataset = "new"

DV = DaVinci()
DV.version = 'v36r4p1'
DV.optsfile = [File(script)]
URA="/afs/cern.ch/work/m/mvesteri/cmtuser/STRIPPING_DEVEL_2015EM_SetupProject/"
DV.user_release_area = URA

if dataset == "new":
    DATA_FILES = ['%s/DaVinci_v36r4p1/Phys/StrippingSelections/tests/data/Reco14_Run130390.py'%URA,
                  '%s/DaVinci_v36r4p1/Phys/StrippingSelections/tests/data/Reco14_Run130391.py'%URA,
                  '%s/DaVinci_v36r4p1/Phys/StrippingSelections/tests/data/Reco14_Run130392.py'%URA,
                  '%s/DaVinci_v36r4p1/Phys/StrippingSelections/tests/data/Reco14_Run130393.py'%URA,
                  '%s/DaVinci_v36r4p1/Phys/StrippingSelections/tests/data/Reco14_Run130394.py'%URA,
                  '%s/DaVinci_v36r4p1/Phys/StrippingSelections/tests/data/Reco14_Run130395.py'%URA,
                  '%s/DaVinci_v36r4p1/Phys/StrippingSelections/tests/data/Reco14_Run130396.py'%URA]
    DATALIST = []
    for INPUT in DATA_FILES:
        DATALIST += DV.readInputData(INPUT)
    print DATALIST
    INPUTDATA = LHCbDataset(DATALIST)
    INPUTDATA.depth=2
else:
    DATA = '/DaVinciDev_v36r4p1/Phys/StrippingSelections/tests/data/Reco14_Run125113.py'
    #INPUTDATA = DV.readInputData(DATA)
    #INPUTDATA.depth=2
    
j = Job(
    name           = job_name,
    application    = DV,
    splitter       = SplitByFiles(filesPerJob = 5,maxFiles = 100),
    inputdata      = INPUTDATA,
    outputfiles     = [LocalFile("StrippingTuples.root"),LocalFile("StrippingHistos.root")],
    do_auto_resubmit = True,
    backend        = Dirac()
    )
j.submit()

