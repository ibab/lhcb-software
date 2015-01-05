#-- GAUDI jobOptions generated on Mon Oct 20 19:22:27 2014
#-- Contains event types : 
#--   12143001 - 46 files - 508396 events - 154.94 GBytes


#--  Extra information about the data processing phases:


#--  Processing Pass Step-123900 

#--  StepId : 123900 
#--  StepName : Stripping20-NoPrescalingFlagged for MC MagDown 
#--  ApplicationName : DaVinci 
#--  ApplicationVersion : v32r2p1 
#--  OptionFiles : $APPCONFIGOPTS/DaVinci/DV-Stripping20-Stripping-MC-NoPrescaling.py;$APPCONFIGOPTS/DaVinci/DataType-2012.py;$APPCONFIGOPTS/DaVinci/InputType-DST.py;$APPCONFIGOPTS/Persistency/Compression-ZLIB-1.py 
#--  DDDB : dddb-20120831 
#--  CONDDB : sim-20121025-vc-md100 
#--  ExtraPackages : AppConfig.v3r155 
#--  Visible : Y 


#--  Processing Pass Step-124199 

#--  StepId : 124199 
#--  StepName : Sim06b with Nu=2.5 - MD - JulSep 2012 
#--  ApplicationName : Gauss 
#--  ApplicationVersion : v42r3 
#--  OptionFiles : $APPCONFIGOPTS/Gauss/Beam4000GeV-md100-JulSep2012-nu2.5.py;$DECFILESROOT/options/@{eventType}.py;$LBPYTHIAROOT/options/Pythia.py;$APPCONFIGOPTS/Gauss/G4PL_LHEP_EmNoCuts.py;$APPCONFIGOPTS/Persistency/Compression-ZLIB-1.py 
#--  DDDB : dddb-20120831 
#--  CONDDB : sim-20121025-vc-md100 
#--  ExtraPackages : AppConfig.v3r156;DecFiles.v26r21 
#--  Visible : Y 


#--  Processing Pass Step-124082 

#--  StepId : 124082 
#--  StepName : Digi12 w/o spillover - MD - L0TCK 0x0042 
#--  ApplicationName : Boole 
#--  ApplicationVersion : v24r0 
#--  OptionFiles : $APPCONFIGOPTS/Boole/Default.py;$APPCONFIGOPTS/Boole/DataType-2012.py;$APPCONFIGOPTS/L0/L0TCK-0x0042.py;$APPCONFIGOPTS/Persistency/Compression-ZLIB-1.py 
#--  DDDB : dddb-20120831 
#--  CONDDB : sim-20121025-vc-md100 
#--  ExtraPackages : AppConfig.v3r155 
#--  Visible : N 


#--  Processing Pass Step-123754 

#--  StepId : 123754 
#--  StepName : Reco14 for MC - MagDown 
#--  ApplicationName : Brunel 
#--  ApplicationVersion : v43r2p2 
#--  OptionFiles : $APPCONFIGOPTS/Brunel/DataType-2012.py;$APPCONFIGOPTS/Brunel/MC-WithTruth.py;$APPCONFIGOPTS/Persistency/Compression-ZLIB-1.py 
#--  DDDB : dddb-20120831 
#--  CONDDB : sim-20121025-vc-md100 
#--  ExtraPackages : AppConfig.v3r151 
#--  Visible : Y 


#--  Processing Pass Step-124118 

#--  StepId : 124118 
#--  StepName : Trigger - TCK 0x40990042 Flagged - MD - JulAugSep 2012 (with Boole v24r0) 
#--  ApplicationName : Moore 
#--  ApplicationVersion : v14r6p3 
#--  OptionFiles : $APPCONFIGOPTS/Moore/MooreSimProduction.py;$APPCONFIGOPTS/Conditions/TCK-0x40990042.py;$APPCONFIGOPTS/Moore/DataType-2012.py;$APPCONFIGOPTS/L0/L0TCK-0x0042.py 
#--  DDDB : dddb-20120831 
#--  CONDDB : sim-20121025-vc-md100 
#--  ExtraPackages : AppConfig.v3r156 
#--  Visible : Y 

from Gaudi.Configuration import * 
from GaudiConf import IOHelper
IOHelper('ROOT').inputFiles(['LFN:/lhcb/MC/2012/ALLSTREAMS.DST/00022810/0000/00022810_00000001_1.allstreams.dst',
'LFN:/lhcb/MC/2012/ALLSTREAMS.DST/00022810/0000/00022810_00000002_1.allstreams.dst',
'LFN:/lhcb/MC/2012/ALLSTREAMS.DST/00022810/0000/00022810_00000003_1.allstreams.dst',
'LFN:/lhcb/MC/2012/ALLSTREAMS.DST/00022810/0000/00022810_00000004_1.allstreams.dst',
'LFN:/lhcb/MC/2012/ALLSTREAMS.DST/00022810/0000/00022810_00000005_1.allstreams.dst',
'LFN:/lhcb/MC/2012/ALLSTREAMS.DST/00022810/0000/00022810_00000006_1.allstreams.dst',
'LFN:/lhcb/MC/2012/ALLSTREAMS.DST/00022810/0000/00022810_00000007_1.allstreams.dst',
'LFN:/lhcb/MC/2012/ALLSTREAMS.DST/00022810/0000/00022810_00000008_1.allstreams.dst',
'LFN:/lhcb/MC/2012/ALLSTREAMS.DST/00022810/0000/00022810_00000009_1.allstreams.dst',
'LFN:/lhcb/MC/2012/ALLSTREAMS.DST/00022810/0000/00022810_00000010_1.allstreams.dst',
'LFN:/lhcb/MC/2012/ALLSTREAMS.DST/00022810/0000/00022810_00000011_1.allstreams.dst',
'LFN:/lhcb/MC/2012/ALLSTREAMS.DST/00022810/0000/00022810_00000012_1.allstreams.dst',
'LFN:/lhcb/MC/2012/ALLSTREAMS.DST/00022810/0000/00022810_00000013_1.allstreams.dst',
'LFN:/lhcb/MC/2012/ALLSTREAMS.DST/00022810/0000/00022810_00000014_1.allstreams.dst',
'LFN:/lhcb/MC/2012/ALLSTREAMS.DST/00022810/0000/00022810_00000015_1.allstreams.dst',
'LFN:/lhcb/MC/2012/ALLSTREAMS.DST/00022810/0000/00022810_00000016_1.allstreams.dst',
'LFN:/lhcb/MC/2012/ALLSTREAMS.DST/00022810/0000/00022810_00000017_1.allstreams.dst',
'LFN:/lhcb/MC/2012/ALLSTREAMS.DST/00022810/0000/00022810_00000018_1.allstreams.dst',
'LFN:/lhcb/MC/2012/ALLSTREAMS.DST/00022810/0000/00022810_00000019_1.allstreams.dst',
'LFN:/lhcb/MC/2012/ALLSTREAMS.DST/00022810/0000/00022810_00000020_1.allstreams.dst',
'LFN:/lhcb/MC/2012/ALLSTREAMS.DST/00022810/0000/00022810_00000021_1.allstreams.dst',
'LFN:/lhcb/MC/2012/ALLSTREAMS.DST/00022810/0000/00022810_00000022_1.allstreams.dst',
'LFN:/lhcb/MC/2012/ALLSTREAMS.DST/00022810/0000/00022810_00000023_1.allstreams.dst',
'LFN:/lhcb/MC/2012/ALLSTREAMS.DST/00022810/0000/00022810_00000024_1.allstreams.dst',
'LFN:/lhcb/MC/2012/ALLSTREAMS.DST/00022810/0000/00022810_00000025_1.allstreams.dst',
'LFN:/lhcb/MC/2012/ALLSTREAMS.DST/00022810/0000/00022810_00000026_1.allstreams.dst',
'LFN:/lhcb/MC/2012/ALLSTREAMS.DST/00022810/0000/00022810_00000027_1.allstreams.dst',
'LFN:/lhcb/MC/2012/ALLSTREAMS.DST/00022810/0000/00022810_00000028_1.allstreams.dst',
'LFN:/lhcb/MC/2012/ALLSTREAMS.DST/00022810/0000/00022810_00000029_1.allstreams.dst',
'LFN:/lhcb/MC/2012/ALLSTREAMS.DST/00022810/0000/00022810_00000030_1.allstreams.dst',
'LFN:/lhcb/MC/2012/ALLSTREAMS.DST/00022810/0000/00022810_00000031_1.allstreams.dst',
'LFN:/lhcb/MC/2012/ALLSTREAMS.DST/00022810/0000/00022810_00000032_1.allstreams.dst',
'LFN:/lhcb/MC/2012/ALLSTREAMS.DST/00022810/0000/00022810_00000033_1.allstreams.dst',
'LFN:/lhcb/MC/2012/ALLSTREAMS.DST/00022810/0000/00022810_00000034_1.allstreams.dst',
'LFN:/lhcb/MC/2012/ALLSTREAMS.DST/00022810/0000/00022810_00000035_1.allstreams.dst',
'LFN:/lhcb/MC/2012/ALLSTREAMS.DST/00022810/0000/00022810_00000036_1.allstreams.dst',
'LFN:/lhcb/MC/2012/ALLSTREAMS.DST/00022810/0000/00022810_00000037_1.allstreams.dst',
'LFN:/lhcb/MC/2012/ALLSTREAMS.DST/00022810/0000/00022810_00000038_1.allstreams.dst',
'LFN:/lhcb/MC/2012/ALLSTREAMS.DST/00022810/0000/00022810_00000039_1.allstreams.dst',
'LFN:/lhcb/MC/2012/ALLSTREAMS.DST/00022810/0000/00022810_00000040_1.allstreams.dst',
'LFN:/lhcb/MC/2012/ALLSTREAMS.DST/00022810/0000/00022810_00000041_1.allstreams.dst',
'LFN:/lhcb/MC/2012/ALLSTREAMS.DST/00022810/0000/00022810_00000042_1.allstreams.dst',
'LFN:/lhcb/MC/2012/ALLSTREAMS.DST/00022810/0000/00022810_00000044_1.allstreams.dst',
'LFN:/lhcb/MC/2012/ALLSTREAMS.DST/00022810/0000/00022810_00000045_1.allstreams.dst',
'LFN:/lhcb/MC/2012/ALLSTREAMS.DST/00022810/0000/00022810_00000046_1.allstreams.dst',
'LFN:/lhcb/MC/2012/ALLSTREAMS.DST/00022810/0000/00022810_00000047_1.allstreams.dst'
], clear=True)
FileCatalog().Catalogs += [ 'xmlcatalog_file:12143001_Bu_JpsiK_pfn.xml' ]
