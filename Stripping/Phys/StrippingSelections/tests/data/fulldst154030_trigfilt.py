#-- GAUDI jobOptions generated on Thu Jun 18 15:45:21 2015
#-- Contains event types : 
#--   90000000 - 38 files - 3518518 events - 155.33 GBytes


#--  Extra information about the data processing phases:


#--  Processing Pass Step-127910 

#--  StepId : 127910 
#--  StepName : FULL-Reco15a-cond-20150602-VALIDATION 
#--  ApplicationName : Brunel 
#--  ApplicationVersion : v47r8 
#--  OptionFiles : $APPCONFIGOPTS/Brunel/DataType-2015.py;$APPCONFIGOPTS/Brunel/saveFittedVeloTracks.py 
#--  DDDB : dddb-20150526 
#--  CONDDB : cond-20150602 
#--  ExtraPackages : AppConfig.v3r216;SQLDDDB.v7r10 
#--  Visible : Y 


#--  Processing Pass Step-127911 

#--  StepId : 127911 
#--  StepName : DataQuality-FULL-2015-FirstDataType2015 
#--  ApplicationName : DaVinci 
#--  ApplicationVersion : v36r7p1 
#--  OptionFiles : $APPCONFIGOPTS/DaVinci/DVMonitor-RealData.py;$APPCONFIGOPTS/DaVinci/DataType-2015.py;$APPCONFIGOPTS/DaVinci/DaVinci-InputType-SDST.py 
#--  DDDB : fromPreviousStep 
#--  CONDDB : fromPreviousStep 
#--  ExtraPackages : AppConfig.v3r214;SQLDDDB.v7r10 
#--  Visible : N 

from Gaudi.Configuration import * 
from GaudiConf import IOHelper
IOHelper('ROOT').inputFiles(['root://eoslhcb.cern.ch//eos/lhcb/user/a/acontu/FULLDST_TrifFiltEM.dst',
#'LFN:/lhcb/validation/Collision15em/FULL.DST/00045500/0000/00045500_00000367_1.full.dst',
#'LFN:/lhcb/validation/Collision15em/FULL.DST/00045500/0000/00045500_00000368_1.full.dst',
#'LFN:/lhcb/validation/Collision15em/FULL.DST/00045500/0000/00045500_00000369_1.full.dst',
#'LFN:/lhcb/validation/Collision15em/FULL.DST/00045500/0000/00045500_00000370_1.full.dst',
#'LFN:/lhcb/validation/Collision15em/FULL.DST/00045500/0000/00045500_00000371_1.full.dst',
#'LFN:/lhcb/validation/Collision15em/FULL.DST/00045500/0000/00045500_00000372_1.full.dst',
#'LFN:/lhcb/validation/Collision15em/FULL.DST/00045500/0000/00045500_00000373_1.full.dst',
#'LFN:/lhcb/validation/Collision15em/FULL.DST/00045500/0000/00045500_00000374_1.full.dst',
#'LFN:/lhcb/validation/Collision15em/FULL.DST/00045500/0000/00045500_00000375_1.full.dst',
#'LFN:/lhcb/validation/Collision15em/FULL.DST/00045500/0000/00045500_00000376_1.full.dst',
#'LFN:/lhcb/validation/Collision15em/FULL.DST/00045500/0000/00045500_00000377_1.full.dst',
#'LFN:/lhcb/validation/Collision15em/FULL.DST/00045500/0000/00045500_00000378_1.full.dst',
#'LFN:/lhcb/validation/Collision15em/FULL.DST/00045500/0000/00045500_00000379_1.full.dst',
#'LFN:/lhcb/validation/Collision15em/FULL.DST/00045500/0000/00045500_00000380_1.full.dst',
#'LFN:/lhcb/validation/Collision15em/FULL.DST/00045500/0000/00045500_00000381_1.full.dst',
#'LFN:/lhcb/validation/Collision15em/FULL.DST/00045500/0000/00045500_00000382_1.full.dst',
#'LFN:/lhcb/validation/Collision15em/FULL.DST/00045500/0000/00045500_00000383_1.full.dst',
#'LFN:/lhcb/validation/Collision15em/FULL.DST/00045500/0000/00045500_00000384_1.full.dst',
#'LFN:/lhcb/validation/Collision15em/FULL.DST/00045500/0000/00045500_00000385_1.full.dst',
#'LFN:/lhcb/validation/Collision15em/FULL.DST/00045500/0000/00045500_00000386_1.full.dst',
#'LFN:/lhcb/validation/Collision15em/FULL.DST/00045500/0000/00045500_00000387_1.full.dst',
#'LFN:/lhcb/validation/Collision15em/FULL.DST/00045500/0000/00045500_00000388_1.full.dst',
#'LFN:/lhcb/validation/Collision15em/FULL.DST/00045500/0000/00045500_00000389_1.full.dst',
#'LFN:/lhcb/validation/Collision15em/FULL.DST/00045500/0000/00045500_00000390_1.full.dst',
#'LFN:/lhcb/validation/Collision15em/FULL.DST/00045500/0000/00045500_00000391_1.full.dst',
#'LFN:/lhcb/validation/Collision15em/FULL.DST/00045500/0000/00045500_00000392_1.full.dst',
#'LFN:/lhcb/validation/Collision15em/FULL.DST/00045500/0000/00045500_00000393_1.full.dst',
#'LFN:/lhcb/validation/Collision15em/FULL.DST/00045500/0000/00045500_00000394_1.full.dst',
#'LFN:/lhcb/validation/Collision15em/FULL.DST/00045500/0000/00045500_00000395_1.full.dst',
#'LFN:/lhcb/validation/Collision15em/FULL.DST/00045500/0000/00045500_00000396_1.full.dst',
#'LFN:/lhcb/validation/Collision15em/FULL.DST/00045500/0000/00045500_00000397_1.full.dst',
#'LFN:/lhcb/validation/Collision15em/FULL.DST/00045500/0000/00045500_00000398_1.full.dst',
], clear=True)
