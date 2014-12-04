from GaudiKernel.ProcessJobOptions import importOptions
#importOptions("$APPCONFIGROOT/options/Boole/Upgrade-WithTruth.py")

from Gaudi.Configuration import *
from Configurables import  Boole, DDDBConf, CondDB

#Boole().DDDBtag   = "dddb-20130408"  #Polci
#Boole().DDDBtag   = "dddb-20140120"
#Boole().DDDBtag   = "dddb-20140606"  

#Boole().CondDBtag = "simcond-20121001-vc-md100" #Polci
#Boole().CondDBtag = "sim-20140204-vc-md100"
#Boole().CondDBtag = "sim-20140204-vc-md100"

#Quaglia
CondDB().Upgrade     = True
CondDB().AllLocalTagsByDataType = ["VP_UVP+RICH_2019+UT_UUT", "FT_StereoAngle5"]

Boole().DDDBtag   = "dddb-20131025"  #Quaglia
Boole().CondDBtag = "sim-20130830-vc-md100"  #Quaglia

###Polci
#from Configurables import Gauss
#Boole().DetectorLink  = ['PuVeto', 'Velo', 'TT', 'Rich1Pmt', 'FT', 'Rich2Pmt', 'Spd', 'Prs', 'Ecal', 'Hcal', 'Muon', 'Magnet'] 
#Boole().DetectorDigi  = ['PuVeto', 'Velo', 'TT', 'Rich1Pmt', 'FT', 'Rich2Pmt', 'Spd', 'Prs', 'Ecal', 'Hcal', 'Muon', 'Magnet'] 
#Boole().DetectorMoni = ['PuVeto', 'Velo', 'TT', 'Rich1Pmt', 'FT', 'Rich2Pmt', 'Spd', 'Prs', 'Ecal', 'Hcal', 'Muon', 'Magnet'] 
Boole().DetectorLink = ['VP', 'UT', 'FT', 'Magnet' ]
Boole().DetectorDigi = ['VP', 'UT', 'FT', 'Magnet' ]
Boole().DetectorMoni =['VP', 'UT', 'FT', 'Magnet' ]
Boole().DataType = "Upgrade"
####

NTupleSvc().Output = ["FILE1 DATAFILE='/afs/cern.ch/work/d/delbuono/RootOutput/TestFT.root' TYP='ROOT' OPT='NEW'"]

#DDDBConf().DbRoot = "/afs/cern.ch/user/f/fpolci/cmtuser/Brunel_v44r5_BeforeHolidays/static_DDDB_VP+UT+FT_v20/lhcb.xml"
#DDDBConf().DbRoot = "/afs/cern.ch/user/s/seaso/public/Simulation/upgrade/Gauss-FT/Det/myDDDB-Upgrade-FTMod-July-2014/lhcb.xml"
#DDDBConf().DbRoot = "/afs/cern.ch/user/d/delbuono/cmtuser/DDDB_20140606_FT40_ST5/lhcb.xml"
#DDDBConf().DbRoot = "/afs/cern.ch/user/d/delbuono/cmtuser/DDDB_20140606_FT42_ST5/lhcb.xml"
#DDDBConf().DbRoot = "/afs/cern.ch/user/d/delbuono/cmtuser/test_DDDBupgrade_FTv4/lhcb.xml"
#DDDBConf().DbRoot = "/afs/cern.ch/user/d/delbuono/cmtuser/DDDB-20141016_SIM-20140204-vc-md100_42st5/lhcb.xml"

#Polci
#EventSelector().Input = ["DATAFILE='/afs/cern.ch/user/o/ogruenbe/public/FT_upgrade/gauss/output/MiniBias_v20_nu25.sim' TYP='POOL_ROOTTREE'"]
##EventSelector().Input = ["DATAFILE='/afs/cern.ch/work/d/dmilanes/digi/FT_upgrade/gauss/output5/MiniBias_v20_nu76.sim' TYP='POOL_ROOTTREE'"]

#datasetName = 'Bs_phiphi_nu38-5degres-24102013'
#EventSelector().Input = ["DATAFILE='/afs/cern.ch/work/d/delbuono/digi/" + datasetName + ".sim'"]
#datasetName = 'bev-Upgrade-FTMod-July2014-v2'
#EventSelector().Input = ["DATAFILE='/afs/cern.ch/work/d/delbuono/digi/Gauss_v46r7p2/" + datasetName + ".sim'"]
#datasetName = 'bev-Upgrade-FTMod-July2014-v4'
#EventSelector().Input = ["DATAFILE='/afs/cern.ch/user/d/delbuono/cmtuser/Gauss_v48r0/" + datasetName + ".sim'"]

#Yasmin november
EventSelector().Input = ["DATAFILE='LFN:/lhcb/MC/Upgrade/XDIGI/00039735/0000/00039735_00000002_1.xdigi'",
                         "DATAFILE='LFN:/lhcb/MC/Upgrade/XDIGI/00039735/0000/00039735_00000003_1.xdigi'",
                         "DATAFILE='LFN:/lhcb/MC/Upgrade/XDIGI/00039735/0000/00039735_00000004_1.xdigi'",
                         "DATAFILE='LFN:/lhcb/MC/Upgrade/XDIGI/00039735/0000/00039735_00000005_1.xdigi'",
                         "DATAFILE='LFN:/lhcb/MC/Upgrade/XDIGI/00039735/0000/00039735_00000007_1.xdigi'",
                         "DATAFILE='LFN:/lhcb/MC/Upgrade/XDIGI/00039735/0000/00039735_00000009_1.xdigi'",
                         "DATAFILE='LFN:/lhcb/MC/Upgrade/XDIGI/00039735/0000/00039735_00000011_1.xdigi'",
                         "DATAFILE='LFN:/lhcb/MC/Upgrade/XDIGI/00039735/0000/00039735_00000018_1.xdigi'",
                         "DATAFILE='LFN:/lhcb/MC/Upgrade/XDIGI/00039735/0000/00039735_00000020_1.xdigi'"
                        ]
FileCatalog().Catalogs += [ 'xmlcatalog_file:reprocessedfiles_YasminNov2014.xml' ]


from Configurables import DeFTTestAlg

testAlg = DeFTTestAlg()
testAlg.OutputLevel =  ERROR



Boole().MainSequence = [ testAlg ]
#Boole().OutputLevel = DEBUG #necessary for getting the full debug output from all FT DetElement classes
Boole().OutputLevel = ERROR #ERROR
Boole().Outputs = [] #no writing sequence

Boole().EvtMax = 100

'''
### Remove irrelevant algorithms (not necessary when we set the 'MainSequence' of Boole) 
Boole().DatasetName = "FTTest"
Boole().DetectorLink = {'VELO':[], 'MUON':[], 'RICH':[], 'TT':[], 'IT':[], 'L0':[], 'CALO':[], 'OT':[], 'TR':[]}
Boole().DetectorDigi = {'VELO':[], 'MUON':[], 'RICH':[], 'TT':[], 'IT':[], 'L0':[], 'CALO':[], 'OT':[]}
Boole().DetectorMoni = {'VELO':[], 'MUON':[], 'RICH':[], 'TT':[], 'IT':[], 'L0':[], 'CALO':[], 'OT':[],'MC':[]}
'''

