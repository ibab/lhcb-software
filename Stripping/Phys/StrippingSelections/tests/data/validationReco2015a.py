#-- GAUDI jobOptions generated on Fri Jun 12 14:23:22 2015
#-- Contains event types : 
#--   90000000 - 25 files - 2713421 events - 90.71 GBytes


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
IOHelper('ROOT').inputFiles([
'root://by32-4.grid.sara.nl:1094/pnfs/grid.sara.nl/data/lhcb/buffer/lhcb/validation/Collision15em/FULL.DST/00045500/0000/00045500_00000204_1.full.dst'
#'LFN:/lhcb/validation/Collision15em/FULL.DST/00045500/0000/00045500_00000254_1.full.dst',
#'LFN:/lhcb/validation/Collision15em/FULL.DST/00045500/0000/00045500_00000273_1.full.dst',
#'LFN:/lhcb/validation/Collision15em/FULL.DST/00045500/0000/00045500_00000276_1.full.dst',
#'LFN:/lhcb/validation/Collision15em/FULL.DST/00045500/0000/00045500_00000284_1.full.dst',
#'LFN:/lhcb/validation/Collision15em/FULL.DST/00045500/0000/00045500_00000309_1.full.dst',
#'LFN:/lhcb/validation/Collision15em/FULL.DST/00045500/0000/00045500_00000312_1.full.dst',
#'LFN:/lhcb/validation/Collision15em/FULL.DST/00045500/0000/00045500_00000313_1.full.dst',
#'LFN:/lhcb/validation/Collision15em/FULL.DST/00045500/0000/00045500_00000315_1.full.dst',
#'LFN:/lhcb/validation/Collision15em/FULL.DST/00045500/0000/00045500_00000317_1.full.dst',
#'LFN:/lhcb/validation/Collision15em/FULL.DST/00045500/0000/00045500_00000318_1.full.dst',
#'LFN:/lhcb/validation/Collision15em/FULL.DST/00045500/0000/00045500_00000325_1.full.dst',
#'LFN:/lhcb/validation/Collision15em/FULL.DST/00045500/0000/00045500_00000328_1.full.dst',
#'LFN:/lhcb/validation/Collision15em/FULL.DST/00045500/0000/00045500_00000330_1.full.dst',
#'LFN:/lhcb/validation/Collision15em/FULL.DST/00045500/0000/00045500_00000345_1.full.dst',
#'LFN:/lhcb/validation/Collision15em/FULL.DST/00045500/0000/00045500_00000363_1.full.dst',
#'LFN:/lhcb/validation/Collision15em/FULL.DST/00045500/0000/00045500_00000375_1.full.dst',
#'LFN:/lhcb/validation/Collision15em/FULL.DST/00045500/0000/00045500_00000383_1.full.dst',
#'LFN:/lhcb/validation/Collision15em/FULL.DST/00045500/0000/00045500_00000386_1.full.dst',
#'LFN:/lhcb/validation/Collision15em/FULL.DST/00045500/0000/00045500_00000395_1.full.dst',
#'LFN:/lhcb/validation/Collision15em/FULL.DST/00045500/0000/00045500_00000409_1.full.dst',
#'LFN:/lhcb/validation/Collision15em/FULL.DST/00045500/0000/00045500_00000421_1.full.dst',
#'LFN:/lhcb/validation/Collision15em/FULL.DST/00045500/0000/00045500_00000423_1.full.dst',
#'LFN:/lhcb/validation/Collision15em/FULL.DST/00045500/0000/00045500_00000441_1.full.dst',
#'LFN:/lhcb/validation/Collision15em/FULL.DST/00045500/0000/00045500_00000457_1.full.dst'
], clear=True)
