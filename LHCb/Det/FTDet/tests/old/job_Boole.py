from GaudiKernel.ProcessJobOptions import importOptions
#importOptions("$APPCONFIGROOT/options/Boole/Upgrade-WithTruth.py")

from Gaudi.Configuration import *
from Configurables import Boole, DDDBConf

Boole().DataType  = "2011"
Boole().DDDBtag   = "MC11-20111102"
Boole().CondDBtag = "sim-20111111-vc-md100"

#DDDBConf().DbRoot = "/afs/cern.ch/user/o/ogruenbe/public/FT_upgrade/myDDDB-LHCb-Feb2012/lhcb.xml"
DDDBConf().DbRoot = "/afs/cern.ch/user/p/phopchev/public/FT/DDDBSlice_FT_v3/lhcb.xml"

#EventSelector().Input = ["DATAFILE='PFN:root://castorlhcb.cern.ch//castor/cern.ch/user/o/ogruenbe/Bs_mumu.sim?svcClass=default' TYP='POOL_ROOTTREE' OPT='READ'"]
EventSelector().Input = ["DATAFILE='/castor/cern.ch/user/o/ogruenbe/Bs_mumu.sim' TYP='POOL_ROOTTREE'"]

from Configurables import DeFTTestAlg
testAlg = DeFTTestAlg()
#testAlg.OutputLevel =  DEBUG #INFO
Boole().MainSequence = [ testAlg ]
Boole().OutputLevel = DEBUG #necessary for getting the full debug output from all FT DetElement classes
Boole().Outputs = [] #no writing sequence

Boole().EvtMax = 1

'''
### Remove irrelevant algorithms (not necessary when we set the 'MainSequence' of Boole) 
Boole().DatasetName = "FTTest"
Boole().DetectorLink = {'VELO':[], 'MUON':[], 'RICH':[], 'TT':[], 'IT':[], 'L0':[], 'CALO':[], 'OT':[], 'TR':[]}
Boole().DetectorDigi = {'VELO':[], 'MUON':[], 'RICH':[], 'TT':[], 'IT':[], 'L0':[], 'CALO':[], 'OT':[]}
Boole().DetectorMoni = {'VELO':[], 'MUON':[], 'RICH':[], 'TT':[], 'IT':[], 'L0':[], 'CALO':[], 'OT':[],'MC':[]}
'''

