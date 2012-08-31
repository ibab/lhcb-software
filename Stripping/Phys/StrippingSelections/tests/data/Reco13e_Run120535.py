#-- GAUDI jobOptions generated on Thu Aug 23 20:39:52 2012
#-- Contains event types : 
#--   90000000 - 95 files - 4580441 events - 457.31 GBytes


#--  Extra information about the data processing phases:


#--  Processing Pass Step-17913 

#--  StepId : 17913 
#--  StepName : DataQuality-FULL-2012-FULL.DST-cond-20120730 
#--  ApplicationName : DaVinci 
#--  ApplicationVersion : v32r1p1 
#--  OptionFiles : $APPCONFIGOPTS/DaVinci/DVMonitor-RealData.py;$APPCONFIGOPTS/DaVinci/DataType-2012.py;$APPCONFIGOPTS/DaVinci/DaVinci-InputType-SDST.py 
#--  DDDB : head-20120413 
#--  CONDDB : cond-20120730 
#--  ExtraPackages : AppConfig.v3r140;SQLDDDB.v7r5 
#--  Visible : N 


#--  Processing Pass Step-18005 

#--  StepId : 18005 
#--  StepName : FULL-Reco13e 
#--  ApplicationName : Brunel 
#--  ApplicationVersion : v43r2 
#--  OptionFiles : $APPCONFIGOPTS/Brunel/DataType-2012.py 
#--  DDDB : head-20120413 
#--  CONDDB : cond-20120730 
#--  ExtraPackages : AppConfig.v3r140;SQLDDDB.v7r6 
#--  Visible : Y 

from Gaudi.Configuration import * 
from GaudiConf import IOHelper
IOHelper('ROOT').inputFiles(['LFN:/lhcb/validation/Collision12/FULL.DST/00019821/0000/00019821_00000001_1.full.dst',
'LFN:/lhcb/validation/Collision12/FULL.DST/00019821/0000/00019821_00000002_1.full.dst',
'LFN:/lhcb/validation/Collision12/FULL.DST/00019821/0000/00019821_00000003_1.full.dst',
'LFN:/lhcb/validation/Collision12/FULL.DST/00019821/0000/00019821_00000004_1.full.dst',
'LFN:/lhcb/validation/Collision12/FULL.DST/00019821/0000/00019821_00000005_1.full.dst',
'LFN:/lhcb/validation/Collision12/FULL.DST/00019821/0000/00019821_00000006_1.full.dst',
'LFN:/lhcb/validation/Collision12/FULL.DST/00019821/0000/00019821_00000007_1.full.dst',
'LFN:/lhcb/validation/Collision12/FULL.DST/00019821/0000/00019821_00000008_1.full.dst',
'LFN:/lhcb/validation/Collision12/FULL.DST/00019821/0000/00019821_00000009_1.full.dst',
'LFN:/lhcb/validation/Collision12/FULL.DST/00019821/0000/00019821_00000011_1.full.dst',
'LFN:/lhcb/validation/Collision12/FULL.DST/00019821/0000/00019821_00000012_1.full.dst',
'LFN:/lhcb/validation/Collision12/FULL.DST/00019821/0000/00019821_00000013_1.full.dst',
'LFN:/lhcb/validation/Collision12/FULL.DST/00019821/0000/00019821_00000014_1.full.dst',
'LFN:/lhcb/validation/Collision12/FULL.DST/00019821/0000/00019821_00000015_1.full.dst',
'LFN:/lhcb/validation/Collision12/FULL.DST/00019821/0000/00019821_00000016_1.full.dst',
'LFN:/lhcb/validation/Collision12/FULL.DST/00019821/0000/00019821_00000017_1.full.dst',
'LFN:/lhcb/validation/Collision12/FULL.DST/00019821/0000/00019821_00000018_1.full.dst',
'LFN:/lhcb/validation/Collision12/FULL.DST/00019821/0000/00019821_00000019_1.full.dst',
'LFN:/lhcb/validation/Collision12/FULL.DST/00019821/0000/00019821_00000020_1.full.dst',
'LFN:/lhcb/validation/Collision12/FULL.DST/00019821/0000/00019821_00000021_1.full.dst',
'LFN:/lhcb/validation/Collision12/FULL.DST/00019821/0000/00019821_00000022_1.full.dst',
'LFN:/lhcb/validation/Collision12/FULL.DST/00019821/0000/00019821_00000023_1.full.dst',
'LFN:/lhcb/validation/Collision12/FULL.DST/00019821/0000/00019821_00000024_1.full.dst',
'LFN:/lhcb/validation/Collision12/FULL.DST/00019821/0000/00019821_00000025_1.full.dst',
'LFN:/lhcb/validation/Collision12/FULL.DST/00019821/0000/00019821_00000026_1.full.dst',
'LFN:/lhcb/validation/Collision12/FULL.DST/00019821/0000/00019821_00000027_1.full.dst',
'LFN:/lhcb/validation/Collision12/FULL.DST/00019821/0000/00019821_00000028_1.full.dst',
'LFN:/lhcb/validation/Collision12/FULL.DST/00019821/0000/00019821_00000029_1.full.dst',
'LFN:/lhcb/validation/Collision12/FULL.DST/00019821/0000/00019821_00000030_1.full.dst',
'LFN:/lhcb/validation/Collision12/FULL.DST/00019821/0000/00019821_00000031_1.full.dst',
'LFN:/lhcb/validation/Collision12/FULL.DST/00019821/0000/00019821_00000032_1.full.dst',
'LFN:/lhcb/validation/Collision12/FULL.DST/00019821/0000/00019821_00000033_1.full.dst',
'LFN:/lhcb/validation/Collision12/FULL.DST/00019821/0000/00019821_00000034_1.full.dst',
'LFN:/lhcb/validation/Collision12/FULL.DST/00019821/0000/00019821_00000035_1.full.dst',
'LFN:/lhcb/validation/Collision12/FULL.DST/00019821/0000/00019821_00000036_1.full.dst',
'LFN:/lhcb/validation/Collision12/FULL.DST/00019821/0000/00019821_00000037_1.full.dst',
'LFN:/lhcb/validation/Collision12/FULL.DST/00019821/0000/00019821_00000039_1.full.dst',
'LFN:/lhcb/validation/Collision12/FULL.DST/00019821/0000/00019821_00000040_1.full.dst',
'LFN:/lhcb/validation/Collision12/FULL.DST/00019821/0000/00019821_00000041_1.full.dst',
'LFN:/lhcb/validation/Collision12/FULL.DST/00019821/0000/00019821_00000042_1.full.dst',
'LFN:/lhcb/validation/Collision12/FULL.DST/00019821/0000/00019821_00000043_1.full.dst',
'LFN:/lhcb/validation/Collision12/FULL.DST/00019821/0000/00019821_00000044_1.full.dst',
'LFN:/lhcb/validation/Collision12/FULL.DST/00019821/0000/00019821_00000045_1.full.dst',
'LFN:/lhcb/validation/Collision12/FULL.DST/00019821/0000/00019821_00000046_1.full.dst',
'LFN:/lhcb/validation/Collision12/FULL.DST/00019821/0000/00019821_00000047_1.full.dst',
'LFN:/lhcb/validation/Collision12/FULL.DST/00019821/0000/00019821_00000048_1.full.dst',
'LFN:/lhcb/validation/Collision12/FULL.DST/00019821/0000/00019821_00000050_1.full.dst',
'LFN:/lhcb/validation/Collision12/FULL.DST/00019821/0000/00019821_00000051_1.full.dst',
'LFN:/lhcb/validation/Collision12/FULL.DST/00019821/0000/00019821_00000052_1.full.dst',
'LFN:/lhcb/validation/Collision12/FULL.DST/00019821/0000/00019821_00000053_1.full.dst',
'LFN:/lhcb/validation/Collision12/FULL.DST/00019821/0000/00019821_00000054_1.full.dst',
'LFN:/lhcb/validation/Collision12/FULL.DST/00019821/0000/00019821_00000055_1.full.dst',
'LFN:/lhcb/validation/Collision12/FULL.DST/00019821/0000/00019821_00000056_1.full.dst',
'LFN:/lhcb/validation/Collision12/FULL.DST/00019821/0000/00019821_00000057_1.full.dst',
'LFN:/lhcb/validation/Collision12/FULL.DST/00019821/0000/00019821_00000058_1.full.dst',
'LFN:/lhcb/validation/Collision12/FULL.DST/00019821/0000/00019821_00000059_1.full.dst',
'LFN:/lhcb/validation/Collision12/FULL.DST/00019821/0000/00019821_00000060_1.full.dst',
'LFN:/lhcb/validation/Collision12/FULL.DST/00019821/0000/00019821_00000061_1.full.dst',
'LFN:/lhcb/validation/Collision12/FULL.DST/00019821/0000/00019821_00000062_1.full.dst',
'LFN:/lhcb/validation/Collision12/FULL.DST/00019821/0000/00019821_00000063_1.full.dst',
'LFN:/lhcb/validation/Collision12/FULL.DST/00019821/0000/00019821_00000064_1.full.dst',
'LFN:/lhcb/validation/Collision12/FULL.DST/00019821/0000/00019821_00000065_1.full.dst',
'LFN:/lhcb/validation/Collision12/FULL.DST/00019821/0000/00019821_00000066_1.full.dst',
'LFN:/lhcb/validation/Collision12/FULL.DST/00019821/0000/00019821_00000067_1.full.dst',
'LFN:/lhcb/validation/Collision12/FULL.DST/00019821/0000/00019821_00000068_1.full.dst',
'LFN:/lhcb/validation/Collision12/FULL.DST/00019821/0000/00019821_00000069_1.full.dst',
'LFN:/lhcb/validation/Collision12/FULL.DST/00019821/0000/00019821_00000070_1.full.dst',
'LFN:/lhcb/validation/Collision12/FULL.DST/00019821/0000/00019821_00000071_1.full.dst',
'LFN:/lhcb/validation/Collision12/FULL.DST/00019821/0000/00019821_00000072_1.full.dst',
'LFN:/lhcb/validation/Collision12/FULL.DST/00019821/0000/00019821_00000073_1.full.dst',
'LFN:/lhcb/validation/Collision12/FULL.DST/00019821/0000/00019821_00000074_1.full.dst',
'LFN:/lhcb/validation/Collision12/FULL.DST/00019821/0000/00019821_00000075_1.full.dst',
'LFN:/lhcb/validation/Collision12/FULL.DST/00019821/0000/00019821_00000076_1.full.dst',
'LFN:/lhcb/validation/Collision12/FULL.DST/00019821/0000/00019821_00000077_1.full.dst',
'LFN:/lhcb/validation/Collision12/FULL.DST/00019821/0000/00019821_00000078_1.full.dst',
'LFN:/lhcb/validation/Collision12/FULL.DST/00019821/0000/00019821_00000079_1.full.dst',
'LFN:/lhcb/validation/Collision12/FULL.DST/00019821/0000/00019821_00000080_1.full.dst',
'LFN:/lhcb/validation/Collision12/FULL.DST/00019821/0000/00019821_00000081_1.full.dst',
'LFN:/lhcb/validation/Collision12/FULL.DST/00019821/0000/00019821_00000082_1.full.dst',
'LFN:/lhcb/validation/Collision12/FULL.DST/00019821/0000/00019821_00000083_1.full.dst',
'LFN:/lhcb/validation/Collision12/FULL.DST/00019821/0000/00019821_00000084_1.full.dst',
'LFN:/lhcb/validation/Collision12/FULL.DST/00019821/0000/00019821_00000085_1.full.dst',
'LFN:/lhcb/validation/Collision12/FULL.DST/00019821/0000/00019821_00000086_1.full.dst',
'LFN:/lhcb/validation/Collision12/FULL.DST/00019821/0000/00019821_00000087_1.full.dst',
'LFN:/lhcb/validation/Collision12/FULL.DST/00019821/0000/00019821_00000088_1.full.dst',
'LFN:/lhcb/validation/Collision12/FULL.DST/00019821/0000/00019821_00000089_1.full.dst',
'LFN:/lhcb/validation/Collision12/FULL.DST/00019821/0000/00019821_00000090_1.full.dst',
'LFN:/lhcb/validation/Collision12/FULL.DST/00019821/0000/00019821_00000091_1.full.dst',
'LFN:/lhcb/validation/Collision12/FULL.DST/00019821/0000/00019821_00000092_1.full.dst',
'LFN:/lhcb/validation/Collision12/FULL.DST/00019821/0000/00019821_00000093_1.full.dst',
'LFN:/lhcb/validation/Collision12/FULL.DST/00019821/0000/00019821_00000094_1.full.dst',
'LFN:/lhcb/validation/Collision12/FULL.DST/00019821/0000/00019821_00000095_1.full.dst',
'LFN:/lhcb/validation/Collision12/FULL.DST/00019821/0000/00019821_00000096_1.full.dst',
'LFN:/lhcb/validation/Collision12/FULL.DST/00019821/0000/00019821_00000097_1.full.dst',
'LFN:/lhcb/validation/Collision12/FULL.DST/00019821/0000/00019821_00000098_1.full.dst'
], clear=True)

FileCatalog().Catalogs = [ 'xmlcatalog_file:out.xml',
                           'xmlcatalog_file:$STRIPPINGSELECTIONSROOT/tests/data/pool_xml_catalog_Reco13e_Run120535.xml' ]

