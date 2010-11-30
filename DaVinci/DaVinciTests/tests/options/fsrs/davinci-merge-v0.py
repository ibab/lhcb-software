from GaudiConf.Configuration import *
importOptions('$DAVINCIROOT/options/DaVinci-Default.py')
importOptions('$APPCONFIGOPTS/Merging/DVMergeDST.py')
importOptions('$APPCONFIGOPTS/DaVinci/DataType-2010.py')
importOptions('$APPCONFIGOPTS/Merging/WriteFSR.py')
importOptions('$DAVINCITESTSROOT/tests/options/fsrs/davinci-merge.py')

from Configurables import DaVinci
DaVinci().InputType="DST"
EventSelector().Input = ["DATAFILE='PFN:MagUp.V0.dst' TYP='POOL_ROOTTREE' OPT='READ'", "DATAFILE='PFN:MagDown.V0.dst' TYP='POOL_ROOTTREE' OPT='READ'"]
IODataManager().AgeLimit=2</text>
