from GaudiKernel.ProcessJobOptions import importOptions
#importOptions("$APPCONFIGROOT/options/Boole/Upgrade-WithTruth.py")

from Gaudi.Configuration import *
from Configurables import  Boole, DDDBConf

#Boole().DataType  = "2012"  POLCI
Boole().DDDBtag   = "dddb-20130408"  #POLCI
Boole().CondDBtag = "simcond-20121001-vc-md100" #POLCI

###POLCI
#from Configurables import Gauss
Boole().DetectorLink  = ['PuVeto', 'Velo', 'TT', 'Rich1Pmt', 'FT', 'Rich2Pmt', 'Spd', 'Prs', 'Ecal', 'Hcal', 'Muon', 'Magnet'] 
Boole().DetectorDigi  = ['PuVeto', 'Velo', 'TT', 'Rich1Pmt', 'FT', 'Rich2Pmt', 'Spd', 'Prs', 'Ecal', 'Hcal', 'Muon', 'Magnet'] 
Boole().DetectorMoni = ['PuVeto', 'Velo', 'TT', 'Rich1Pmt', 'FT', 'Rich2Pmt', 'Spd', 'Prs', 'Ecal', 'Hcal', 'Muon', 'Magnet'] 
Boole().DataType = "Upgrade"
####

NTupleSvc().Output = ["FILE1 DATAFILE='TestFT.root' TYP='ROOT' OPT='NEW'"]

#DDDBConf().DbRoot = "/afs/cern.ch/user/o/ogruenbe/public/FT_upgrade/myDDDB-LHCb-Feb2012/lhcb.xml"
#DDDBConf().DbRoot = "/afs/cern.ch/user/p/phopchev/public/FT/DDDBSlice_FT_v3/lhcb.xml"
#POLCI
#DDDBConf().DbRoot = "$HOME/panoramix/DDDB/DDDBSlice_FT_v20_test/lhcb.xml"
DDDBConf().DbRoot = "/afs/cern.ch/user/f/fpolci/cmtuser/Brunel_v44r5/static_DDDB_VP+UT+FT_v20/lhcb.xml"
#EventSelector().Input = ["DATAFILE='PFN:root://castorlhcb.cern.ch//castor/cern.ch/user/o/ogruenbe/Bs_mumu.sim?svcClass=default' TYP='POOL_ROOTTREE' OPT='READ'"]
#EventSelector().Input = ["DATAFILE='/castor/cern.ch/user/o/ogruenbe/Bs_mumu_v3.sim' TYP='POOL_ROOTTREE'"]
#POLCI
#EventSelector().Input = ["DATAFILE='/afs/cern.ch/user/o/ogruenbe/public/FT_upgrade/gauss/output/MiniBias_v20_nu25.sim' TYP='POOL_ROOTTREE'"]

EventSelector().Input = ["DATAFILE='/afs/cern.ch/user/o/ogruenbe/public/FT_upgrade/gauss/output5/MiniBias_v20_nu76.sim' TYP='POOL_ROOTTREE'"]

from Configurables import DeFTTestAlg

testAlg = DeFTTestAlg()
testAlg.OutputLevel =  INFO



Boole().MainSequence = [ testAlg ]
#Boole().OutputLevel = DEBUG #necessary for getting the full debug output from all FT DetElement classes
Boole().OutputLevel = ERROR #DEBUG
Boole().Outputs = [] #no writing sequence

Boole().EvtMax = 10000

'''
### Remove irrelevant algorithms (not necessary when we set the 'MainSequence' of Boole) 
Boole().DatasetName = "FTTest"
Boole().DetectorLink = {'VELO':[], 'MUON':[], 'RICH':[], 'TT':[], 'IT':[], 'L0':[], 'CALO':[], 'OT':[], 'TR':[]}
Boole().DetectorDigi = {'VELO':[], 'MUON':[], 'RICH':[], 'TT':[], 'IT':[], 'L0':[], 'CALO':[], 'OT':[]}
Boole().DetectorMoni = {'VELO':[], 'MUON':[], 'RICH':[], 'TT':[], 'IT':[], 'L0':[], 'CALO':[], 'OT':[],'MC':[]}
'''

