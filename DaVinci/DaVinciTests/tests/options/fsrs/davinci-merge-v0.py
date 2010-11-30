from GaudiConf.Configuration import *
importOptions('$APPCONFIGOPTS/Merging/DV-Stripping12-Merging.py')
importOptions('$APPCONFIGOPTS/Merging/WriteFSR.py')
importOptions('$DAVINCITESTSROOT/tests/options/fsrs/davinci-merge.py')
EventSelector().Input = ["DATAFILE='PFN:MagUp.V0.dst' TYP='POOL_ROOTTREE' OPT='READ'", "DATAFILE='PFN:MagDown.V0.dst' TYP='POOL_ROOTTREE' OPT='READ'"]
