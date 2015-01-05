from Gaudi.Configuration import *

#RunNr = "MC_BuJpsiK_0ns_1000_C"
#RunNr = "MC_BuJpsiK_m05ns_1000_C"
#RunNr = "MC_BuJpsiK_05ns_1000_C"
#RunNr = "MC_BuJpsiK_2ns_1000_C"
#RunNr = "MC_BuJpsiK_1ns_1000_C"
#RunNr = "MC_BuJpsiK_m1ns_1000_C"
#RunNr = "MC_BuJpsiK_m2ns_1000_C"
#RunNr = "MC_BuJpsiK_m3ns_1000"

#RunNr = "MC_BuJpsiK_0nsWALK_1000"
#RunNr = "MC_BuJpsiK_random_1000"

#RunNr = "MC_BuJpsiK_2nsWALK"
#RunNr = "MC_BuJpsiK_default"
#RunNr = "To_write_XMLs"
RunNr = "TEST-forONLINE"

#RunNr = "MC_BuJpsiK_reference"
NTupleSvc().Output = [ "FILE1 DATAFILE='mc_" + RunNr + ".root' TYP='ROOT' OPT='NEW'" ]
HistogramPersistencySvc().OutputFile = "mc_" + RunNr + "_hists.root"

#EventSelector().Input = [
#  "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/data/2012/RAW/FULL/LHCb/COLLISION12/129643/129643_0000000001.raw' SVC='LHCb::MDFSelector'"
#    "DATAFILE='/castor/cern.ch/grid/lhcb/data/2012/RAW/FULL/LHCb/COLLISION12/129643/129643_0000000001.raw' SVC='LHCb::MDFSelector'"
#    "DATAFILE='root://castorlhcb.cern.ch///castor/cern.ch/grid/lhcb/data/2012/RAW/FULL/LHCb/COLLISION12/129643/129643_0000000001.raw' SVC='LHCb::MDFSelector'"
#    "DATAFILE='/tmp/lgrillo/129643_0000000001.raw' SVC='LHCb::MDFSelector'" #<-- when u copy the raw data in your folder
#  "DATAFILE='LFN:/lhcb/data/2012/RAW/FULL/LHCb/COLLISION12/129643/129643_0000000001.raw' SVC='LHCb::MDFSelector'"
#   "DATAFILE='mdf:root://castorlhcb.cern.ch//castor/cern.ch/grid/lhcb/data/2012/RAW/FULL/LHCb/COLLISION12/129643/129643_0000000001.raw?svcClass=lhcbtape' SVC='LHCb::MDFSelector'"
#    "DATAFILE='PFN:12143001_Bu_JpsiK_pfn.py'"
#    "DATAFILE='12143001_Bu_JpsiK_pfn.py'"
#  "DATAFILE='mdf:root://castorlhcb.cern.ch/castor/cern.ch/grid/lhcb/data/2012/RAW/FULL/LHCb/COLLISION12/129643/129643_0000000001.raw' SVC='LHCb::MDFSelector'"
#   "DATAFILE='root://castorlhcb.cern.ch/castor/cern.ch/grid/lhcb/data/2012/RAW/FULL/LHCb/COLLISION12/129643/129643_0000000001.raw'' SVC='LHCb::MDFSelector'"
#  "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/data/2012/RAW/FULL/LHCb/COLLISION12/114318/114318_0000000006.raw' SVC='LHCb::MDFSelector'",
#  "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/data/2012/RAW/FULL/LHCb/COLLISION12/114318/114318_0000000007.raw' SVC='LHCb::MDFSelector'",
#  "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/data/2012/RAW/FULL/LHCb/COLLISION12/114318/114318_0000000008.raw' SVC='LHCb::MDFSelector'",
#  "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/data/2012/RAW/FULL/LHCb/COLLISION12/114318/114318_0000000018.raw' SVC='LHCb::MDFSelector'"
#    "DATAFILE='/tmp/lgrillo/00022810_00000004_1.allstreams.dst' SVC='LHCb::MDFSelector'"
#    "DATAFILE='LFN:/lhcb/MC/2012/ALLSTREAMS.DST/00022810/0000/00022810_00000002_1.allstreams.dst' SVC='LHCb::MDFSelector'",
#    "DATAFILE='LFN:/lhcb/MC/2012/ALLSTREAMS.DST/00022810/0000/00022810_00000002_1.allstreams.dst' SVC='LHCb::MDFSelector'",
#    "DATAFILE='LFN:/lhcb/MC/2012/ALLSTREAMS.DST/00022810/0000/00022810_00000003_1.allstreams.dst' SVC='LHCb::MDFSelector'",
#    "DATAFILE='LFN:/lhcb/MC/2012/ALLSTREAMS.DST/00022810/0000/00022810_00000004_1.allstreams.dst' SVC='LHCb::MDFSelector'",
#    "DATAFILE='LFN:/lhcb/MC/2012/ALLSTREAMS.DST/00022810/0000/00022810_00000005_1.allstreams.dst' SVC='LHCb::MDFSelector'",
#    "DATAFILE='LFN:/lhcb/MC/2012/ALLSTREAMS.DST/00022810/0000/00022810_00000006_1.allstreams.dst' SVC='LHCb::MDFSelector'",
#    "DATAFILE='LFN:/lhcb/MC/2012/ALLSTREAMS.DST/00022810/0000/00022810_00000007_1.allstreams.dst' SVC='LHCb::MDFSelector'",
#    "DATAFILE='LFN:/lhcb/MC/2012/ALLSTREAMS.DST/00022810/0000/00022810_00000008_1.allstreams.dst' SVC='LHCb::MDFSelector'",
#    "DATAFILE='LFN:/lhcb/MC/2012/ALLSTREAMS.DST/00022810/0000/00022810_00000009_1.allstreams.dst' SVC='LHCb::MDFSelector'",
#    "DATAFILE='LFN:/lhcb/MC/2012/ALLSTREAMS.DST/00022810/0000/00022810_00000010_1.allstreams.dst' SVC='LHCb::MDFSelector'",
#    "DATAFILE='LFN:/lhcb/MC/2012/ALLSTREAMS.DST/00022810/0000/00022810_00000011_1.allstreams.dst' SVC='LHCb::MDFSelector'",
#    "DATAFILE='LFN:/lhcb/MC/2012/ALLSTREAMS.DST/00022810/0000/00022810_00000012_1.allstreams.dst' SVC='LHCb::MDFSelector'",
#    "DATAFILE='LFN:/lhcb/MC/2012/ALLSTREAMS.DST/00022810/0000/00022810_00000013_1.allstreams.dst' SVC='LHCb::MDFSelector'",
#    "DATAFILE='LFN:/lhcb/MC/2012/ALLSTREAMS.DST/00022810/0000/00022810_00000014_1.allstreams.dst' SVC='LHCb::MDFSelector'",
#    "DATAFILE='LFN:/lhcb/MC/2012/ALLSTREAMS.DST/00022810/0000/00022810_00000015_1.allstreams.dst' SVC='LHCb::MDFSelector'",
#    "DATAFILE='LFN:/lhcb/MC/2012/ALLSTREAMS.DST/00022810/0000/00022810_00000016_1.allstreams.dst' SVC='LHCb::MDFSelector'",
#    "DATAFILE='LFN:/lhcb/MC/2012/ALLSTREAMS.DST/00022810/0000/00022810_00000017_1.allstreams.dst' SVC='LHCb::MDFSelector'",
#    "DATAFILE='LFN:/lhcb/MC/2012/ALLSTREAMS.DST/00022810/0000/00022810_00000018_1.allstreams.dst' SVC='LHCb::MDFSelector'",
#    "DATAFILE='LFN:/lhcb/MC/2012/ALLSTREAMS.DST/00022810/0000/00022810_00000019_1.allstreams.dst' SVC='LHCb::MDFSelector'",
#    "DATAFILE='LFN:/lhcb/MC/2012/ALLSTREAMS.DST/00022810/0000/00022810_00000020_1.allstreams.dst' SVC='LHCb::MDFSelector'",
#    "DATAFILE='LFN:/lhcb/MC/2012/ALLSTREAMS.DST/00022810/0000/00022810_00000021_1.allstreams.dst' SVC='LHCb::MDFSelector'",
#    "DATAFILE='LFN:/lhcb/MC/2012/ALLSTREAMS.DST/00022810/0000/00022810_00000022_1.allstreams.dst' SVC='LHCb::MDFSelector'",
#    "DATAFILE='LFN:/lhcb/MC/2012/ALLSTREAMS.DST/00022810/0000/00022810_00000023_1.allstreams.dst' SVC='LHCb::MDFSelector'",
#    "DATAFILE='LFN:/lhcb/MC/2012/ALLSTREAMS.DST/00022810/0000/00022810_00000024_1.allstreams.dst' SVC='LHCb::MDFSelector'",
#    "DATAFILE='LFN:/lhcb/MC/2012/ALLSTREAMS.DST/00022810/0000/00022810_00000025_1.allstreams.dst' SVC='LHCb::MDFSelector'",
#    "DATAFILE='LFN:/lhcb/MC/2012/ALLSTREAMS.DST/00022810/0000/00022810_00000026_1.allstreams.dst' SVC='LHCb::MDFSelector'",
#    "DATAFILE='LFN:/lhcb/MC/2012/ALLSTREAMS.DST/00022810/0000/00022810_00000027_1.allstreams.dst' SVC='LHCb::MDFSelector'",
#    "DATAFILE='LFN:/lhcb/MC/2012/ALLSTREAMS.DST/00022810/0000/00022810_00000028_1.allstreams.dst' SVC='LHCb::MDFSelector'",
#    "DATAFILE='LFN:/lhcb/MC/2012/ALLSTREAMS.DST/00022810/0000/00022810_00000029_1.allstreams.dst' SVC='LHCb::MDFSelector'",
#    "DATAFILE='LFN:/lhcb/MC/2012/ALLSTREAMS.DST/00022810/0000/00022810_00000030_1.allstreams.dst' SVC='LHCb::MDFSelector'",
#    "DATAFILE='LFN:/lhcb/MC/2012/ALLSTREAMS.DST/00022810/0000/00022810_00000031_1.allstreams.dst' SVC='LHCb::MDFSelector'",
#    "DATAFILE='LFN:/lhcb/MC/2012/ALLSTREAMS.DST/00022810/0000/00022810_00000032_1.allstreams.dst' SVC='LHCb::MDFSelector'"
#]

#from Gaudi.Configuration import *
#from Configurables import CondDB, LHCbApp
#CondDB().LocalTags['LHCBCOND'] = [ 'align-20120410' ]

#from Configurables import ( CondDBAccessSvc, CondDB )
#CondDB().addLayer(CondDBAccessSvc("align_db",
#  ConnectionString = "sqlite_file:alignDB.early2012.2days.jpsi.db/LHCBCOND")
#)

#from Configurables import ( CondDBAccessSvc, CondDB )
#CondDB().addLayer(CondDBAccessSvc("t0_db",
#  ConnectionString = "sqlite_file:/afs/cern.ch/user/a/akozlins/public/OT/LHCBCOND/2012-04/ModuleT0s_2012_04_111185_v2.db/LHCBCOND")
#)

#from Configurables import ( CondDBAccessSvc, CondDB )
#CondDB().addLayer(CondDBAccessSvc("t0_db",
#  ConnectionString = "sqlite_file:/afs/cern.ch/user/l/lgrillo/cmtuser/Brunel_v42r3p2/Calibration/OTCalibration/scripts_129643/v1.db/LHCBCOND")
#  ConnectionString = "sqlite_file:/afs/cern.ch/work/l/lgrillo/Brunel_v42r3p2/Calibration/OTCalibration/scripts_129643/v1.db/LHCBCOND")
#  ConnectionString = "sqlite_file:/afs/cern.ch/user/l/lgrillo/cmtuser/Brunel_v47r0/Calibration/OTCalibration/fake_databases/t0s_0ns_simcond2.db/SIMCOND")
#  ConnectionString = "sqlite_file:/afs/cern.ch/user/l/lgrillo/cmtuser/Brunel_v47r0/Calibration/OTCalibration/fake_databases/t0s_0ns_simcond2.db/SIMCOND")
#  ConnectionString = "sqlite_file:/afs/cern.ch/user/l/lgrillo/cmtuser/Brunel_v47r0/Calibration/OTCalibration/fake_databases/t0s_2ns.db/LHCBCOND")
#)


importOptions("mc_i.py")
