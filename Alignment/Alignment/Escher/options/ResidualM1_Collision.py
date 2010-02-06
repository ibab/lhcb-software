##############################################################################
# File for running Brunel on real data with default 2008 geometry, as defined in
# DDDB/Configuration.py
# Syntax is:
#
#   gaudirun.py $ESCHEROPTS/AlignMuonOTCosmics.py
#
# or
#
# $ESCHEROPTS/gaudiiter.py -n NUMITER -e NUMEVENTS $ESCHEROPTS/AlignMuonOTCosmics.py
#
# Run list contains dst with at least one track in the OT in good OT runs
##############################################################################

from Configurables import ( Escher, TrackSys, RecSysConf )

Escher().DataType     = "2009"
Escher().Kalman = True
Escher().Millepede = False
Escher().TrackContainer = "Rec/Track/Best" # Velo, VeloR, Long, Upstream, Downstream, Ttrack, Muon or Calo
Escher().EvtMax = -1

TrackSys().ExpertTracking += [ "simplifiedGeometry" ]
TrackSys().TrackExtraInfoAlgorithms = ['']
TrackSys().TrackPatRecAlgorithms = ["Velo","TsaSeed","Match","Downstream"] 
RecSysConf().RecoSequence = ["VELO","TT","IT","OT","Tr"]

###############################################################


from GaudiKernel.SystemOfUnits import GeV, mm
from Gaudi.Configuration import *

uselocalframe       = True
chisqconstraints    = [] # un numero in corrispondenza di ogni constraint
constraints         = []
dof                 = 'TxTy'


outputlevel         = ERROR #INFO#ERROR# INFO

maskNoise           = True # to mask Noisy Channels
nameCond            = 'ResidualsM1'


Write_Ntuple       = False   # if True writes an Ntuple for debugging purposes


from TAlignment.Alignables import *
elements           = Alignables()
elements.MuonHalfStationsA(dof) ;            # configure for Muon half-Stations
elements.MuonHalfStationsC(dof) ;            # configure for Muon half-Stations

data          = []
localVetoLIST = ["1122.52", "3222.132", "3222.133", "3222.134", "3222.135", "3222.136", "3222.137",
                 "3222.138", "3222.139", "3311.4", "3311.5", "3311.6", "3311.8", "3311.36",
                 "3311.37", "3311.38", "3311.40", "3332.144", "4332.82", "4332.83", "4332.84",
                 "4332.85", "4332.86", "4332.88", "4332.89", "4332.90", "4332.92"]
tileVetoLIST  = [ 140518139, 146889741, 146889757, 146889773, 146889789, 146889805,
                  146889821, 146889837, 146889853]
Bfield        = True

#sample = 'Mcarlo'
#sample ='Beam450GeV-Boff'
#sample='Beam450GeV-VeloOpen-MagDown-Excl-VE' 
#sample='Beam450GeV-VeloOpen-MagDown-Excl-R1' 
#sample = 'Beam450GeV-VeloOpen-MagDown-Excl-R1R2'
#sample = 'Beam450GeV-VeloOpen-MagDown-Excl-IT-R1R2-TT'
sample ='Beam450GeV-VeloOpen-MagDown'
if sample=='Mcarlo' :
   data = [
      '/castor/cern.ch/grid/lhcb/MC/2009/XDST/00005780/0000/00005780_00000001_1.xdst',
      '/castor/cern.ch/grid/lhcb/MC/2009/XDST/00005780/0000/00005780_00000002_1.xdst',
      '/castor/cern.ch/grid/lhcb/MC/2009/XDST/00005780/0000/00005780_00000003_1.xdst',
      '/castor/cern.ch/grid/lhcb/MC/2009/XDST/00005780/0000/00005780_00000004_1.xdst'
      ]
   localVetoLIST = []
   tileVetoLIST = []

elif sample=='Beam450GeV-Boff' :
   Bfield = False
   Escher().SpecialData = ["fieldOff" ]
   data = [
      '/castor/cern.ch/grid/lhcb/data/2009/DST/00005738/0000/00005738_00000001_1.dst' ]

elif sample=='Beam450GeV-VeloOpen-MagDown-Excl-VE' :

   data = [
      '/castor/cern.ch/grid/lhcb/data/2009/DST/00005732/0000/00005732_00000001_1.dst'
      ]
   
elif sample=='Beam450GeV-VeloOpen-MagDown-Excl-R1' :
   data = [
      '/castor/cern.ch/grid/lhcb/data/2009/DST/00005730/0000/00005730_00000001_1.dst',
      '/castor/cern.ch/grid/lhcb/data/2009/DST/00005730/0000/00005730_00000002_1.dst' 
      ]

elif sample == 'Beam450GeV-VeloOpen-MagDown-Excl-R1R2' :
   data = [
      '/castor/cern.ch/grid/lhcb/data/2009/DST/00005731/0000/00005731_00000001_1.dst',
      '/castor/cern.ch/grid/lhcb/data/2009/DST/00005731/0000/00005731_00000002_1.dst',
      '/castor/cern.ch/grid/lhcb/data/2009/DST/00005731/0000/00005731_00000003_1.dst',
      '/castor/cern.ch/grid/lhcb/data/2009/DST/00005731/0000/00005731_00000004_1.dst',
      '/castor/cern.ch/grid/lhcb/data/2009/DST/00005731/0000/00005731_00000005_1.dst',
      '/castor/cern.ch/grid/lhcb/data/2009/DST/00005731/0000/00005731_00000006_1.dst',
      '/castor/cern.ch/grid/lhcb/data/2009/DST/00005731/0000/00005731_00000007_1.dst',
      '/castor/cern.ch/grid/lhcb/data/2009/DST/00005731/0000/00005731_00000008_1.dst',
      '/castor/cern.ch/grid/lhcb/data/2009/DST/00005731/0000/00005731_00000009_1.dst',
      '/castor/cern.ch/grid/lhcb/data/2009/DST/00005731/0000/00005731_00000010_1.dst',
      '/castor/cern.ch/grid/lhcb/data/2009/DST/00005731/0000/00005731_00000011_1.dst',
      '/castor/cern.ch/grid/lhcb/data/2009/DST/00005731/0000/00005731_00000012_1.dst',
      '/castor/cern.ch/grid/lhcb/data/2009/DST/00005731/0000/00005731_00000013_1.dst',
      '/castor/cern.ch/grid/lhcb/data/2009/DST/00005731/0000/00005731_00000014_1.dst',
      '/castor/cern.ch/grid/lhcb/data/2009/DST/00005731/0000/00005731_00000015_1.dst',
      '/castor/cern.ch/grid/lhcb/data/2009/DST/00005731/0000/00005731_00000016_1.dst',
      '/castor/cern.ch/grid/lhcb/data/2009/DST/00005731/0000/00005731_00000017_1.dst',
      '/castor/cern.ch/grid/lhcb/data/2009/DST/00005731/0000/00005731_00000018_1.dst',
      '/castor/cern.ch/grid/lhcb/data/2009/DST/00005731/0000/00005731_00000019_1.dst',
      '/castor/cern.ch/grid/lhcb/data/2009/DST/00005731/0000/00005731_00000020_1.dst',
      '/castor/cern.ch/grid/lhcb/data/2009/DST/00005731/0000/00005731_00000021_1.dst',
      '/castor/cern.ch/grid/lhcb/data/2009/DST/00005731/0000/00005731_00000022_1.dst',
      '/castor/cern.ch/grid/lhcb/data/2009/DST/00005731/0000/00005731_00000023_1.dst',
      '/castor/cern.ch/grid/lhcb/data/2009/DST/00005731/0000/00005731_00000024_1.dst',
      '/castor/cern.ch/grid/lhcb/data/2009/DST/00005731/0000/00005731_00000025_1.dst',
      '/castor/cern.ch/grid/lhcb/data/2009/DST/00005731/0000/00005731_00000026_1.dst',
      '/castor/cern.ch/grid/lhcb/data/2009/DST/00005731/0000/00005731_00000027_1.dst',
      '/castor/cern.ch/grid/lhcb/data/2009/DST/00005731/0000/00005731_00000028_1.dst',
      '/castor/cern.ch/grid/lhcb/data/2009/DST/00005731/0000/00005731_00000029_1.dst',
      '/castor/cern.ch/grid/lhcb/data/2009/DST/00005731/0000/00005731_00000030_1.dst',
      '/castor/cern.ch/grid/lhcb/data/2009/DST/00005731/0000/00005731_00000031_1.dst',
      '/castor/cern.ch/grid/lhcb/data/2009/DST/00005731/0000/00005731_00000032_1.dst',
      '/castor/cern.ch/grid/lhcb/data/2009/DST/00005731/0000/00005731_00000033_1.dst',
      '/castor/cern.ch/grid/lhcb/data/2009/DST/00005731/0000/00005731_00000034_1.dst',
      '/castor/cern.ch/grid/lhcb/data/2009/DST/00005731/0000/00005731_00000035_1.dst',
      '/castor/cern.ch/grid/lhcb/data/2009/DST/00005731/0000/00005731_00000036_1.dst',
      '/castor/cern.ch/grid/lhcb/data/2009/DST/00005731/0000/00005731_00000037_1.dst',
      '/castor/cern.ch/grid/lhcb/data/2009/DST/00005731/0000/00005731_00000038_1.dst',
      '/castor/cern.ch/grid/lhcb/data/2009/DST/00005731/0000/00005731_00000039_1.dst',
      '/castor/cern.ch/grid/lhcb/data/2009/DST/00005731/0000/00005731_00000040_1.dst',
      '/castor/cern.ch/grid/lhcb/data/2009/DST/00005731/0000/00005731_00000041_1.dst',
      '/castor/cern.ch/grid/lhcb/data/2009/DST/00005731/0000/00005731_00000042_1.dst',
      '/castor/cern.ch/grid/lhcb/data/2009/DST/00005731/0000/00005731_00000043_1.dst',
      '/castor/cern.ch/grid/lhcb/data/2009/DST/00005731/0000/00005731_00000044_1.dst',
      '/castor/cern.ch/grid/lhcb/data/2009/DST/00005731/0000/00005731_00000045_1.dst',
      '/castor/cern.ch/grid/lhcb/data/2009/DST/00005731/0000/00005731_00000046_1.dst',
      '/castor/cern.ch/grid/lhcb/data/2009/DST/00005731/0000/00005731_00000047_1.dst',
      '/castor/cern.ch/grid/lhcb/data/2009/DST/00005731/0000/00005731_00000048_1.dst',
      '/castor/cern.ch/grid/lhcb/data/2009/DST/00005731/0000/00005731_00000049_1.dst',
      '/castor/cern.ch/grid/lhcb/data/2009/DST/00005731/0000/00005731_00000050_1.dst',
      '/castor/cern.ch/grid/lhcb/data/2009/DST/00005731/0000/00005731_00000051_1.dst',
      '/castor/cern.ch/grid/lhcb/data/2009/DST/00005731/0000/00005731_00000052_1.dst',
      '/castor/cern.ch/grid/lhcb/data/2009/DST/00005731/0000/00005731_00000053_1.dst',
      '/castor/cern.ch/grid/lhcb/data/2009/DST/00005731/0000/00005731_00000054_1.dst',
      '/castor/cern.ch/grid/lhcb/data/2009/DST/00005731/0000/00005731_00000055_1.dst',
      '/castor/cern.ch/grid/lhcb/data/2009/DST/00005731/0000/00005731_00000056_1.dst'
      ]
elif sample =='Beam450GeV-VeloOpen-MagDown-Excl-IT-R1R2-TT':
   data = [
      '/castor/cern.ch/grid/lhcb/data/2009/DST/00005729/0000/00005729_00000001_1.dst',
      '/castor/cern.ch/grid/lhcb/data/2009/DST/00005729/0000/00005729_00000002_1.dst',
      '/castor/cern.ch/grid/lhcb/data/2009/DST/00005729/0000/00005729_00000003_1.dst',
      '/castor/cern.ch/grid/lhcb/data/2009/DST/00005729/0000/00005729_00000004_1.dst',
      '/castor/cern.ch/grid/lhcb/data/2009/DST/00005729/0000/00005729_00000005_1.dst',
      '/castor/cern.ch/grid/lhcb/data/2009/DST/00005729/0000/00005729_00000006_1.dst',
      '/castor/cern.ch/grid/lhcb/data/2009/DST/00005729/0000/00005729_00000007_1.dst',
      '/castor/cern.ch/grid/lhcb/data/2009/DST/00005729/0000/00005729_00000008_1.dst'
      ]
elif sample =='Beam450GeV-VeloOpen-MagDown' :
   data = [
      '/castor/cern.ch/grid/lhcb/data/2009/DST/00005727/0000/00005727_00000001_1.dst',
      '/castor/cern.ch/grid/lhcb/data/2009/DST/00005727/0000/00005727_00000002_1.dst',
      '/castor/cern.ch/grid/lhcb/data/2009/DST/00005727/0000/00005727_00000003_1.dst',
      '/castor/cern.ch/grid/lhcb/data/2009/DST/00005727/0000/00005727_00000004_1.dst',
      '/castor/cern.ch/grid/lhcb/data/2009/DST/00005727/0000/00005727_00000005_1.dst',
      '/castor/cern.ch/grid/lhcb/data/2009/DST/00005727/0000/00005727_00000006_1.dst',
      '/castor/cern.ch/grid/lhcb/data/2009/DST/00005727/0000/00005727_00000007_1.dst',
      '/castor/cern.ch/grid/lhcb/data/2009/DST/00005727/0000/00005727_00000008_1.dst',
      '/castor/cern.ch/grid/lhcb/data/2009/DST/00005727/0000/00005727_00000009_1.dst',
      '/castor/cern.ch/grid/lhcb/data/2009/DST/00005727/0000/00005727_00000010_1.dst',
      '/castor/cern.ch/grid/lhcb/data/2009/DST/00005727/0000/00005727_00000011_1.dst',
      '/castor/cern.ch/grid/lhcb/data/2009/DST/00005727/0000/00005727_00000012_1.dst',
      '/castor/cern.ch/grid/lhcb/data/2009/DST/00005727/0000/00005727_00000013_1.dst',
      '/castor/cern.ch/grid/lhcb/data/2009/DST/00005727/0000/00005727_00000014_1.dst',
      '/castor/cern.ch/grid/lhcb/data/2009/DST/00005727/0000/00005727_00000015_1.dst',
      '/castor/cern.ch/grid/lhcb/data/2009/DST/00005727/0000/00005727_00000016_1.dst',
      '/castor/cern.ch/grid/lhcb/data/2009/DST/00005727/0000/00005727_00000017_1.dst',
      '/castor/cern.ch/grid/lhcb/data/2009/DST/00005727/0000/00005727_00000018_1.dst',
      '/castor/cern.ch/grid/lhcb/data/2009/DST/00005727/0000/00005727_00000019_1.dst',
      '/castor/cern.ch/grid/lhcb/data/2009/DST/00005727/0000/00005727_00000020_1.dst',
      '/castor/cern.ch/grid/lhcb/data/2009/DST/00005727/0000/00005727_00000021_1.dst',
      '/castor/cern.ch/grid/lhcb/data/2009/DST/00005727/0000/00005727_00000022_1.dst',
      '/castor/cern.ch/grid/lhcb/data/2009/DST/00005727/0000/00005727_00000023_1.dst',
      '/castor/cern.ch/grid/lhcb/data/2009/DST/00005727/0000/00005727_00000024_1.dst',
      '/castor/cern.ch/grid/lhcb/data/2009/DST/00005727/0000/00005727_00000025_1.dst',
      '/castor/cern.ch/grid/lhcb/data/2009/DST/00005727/0000/00005727_00000026_1.dst',
      '/castor/cern.ch/grid/lhcb/data/2009/DST/00005727/0000/00005727_00000027_1.dst',
      '/castor/cern.ch/grid/lhcb/data/2009/DST/00005727/0000/00005727_00000028_1.dst',
      '/castor/cern.ch/grid/lhcb/data/2009/DST/00005727/0000/00005727_00000029_1.dst',
      '/castor/cern.ch/grid/lhcb/data/2009/DST/00005727/0000/00005727_00000030_1.dst',
      '/castor/cern.ch/grid/lhcb/data/2009/DST/00005727/0000/00005727_00000031_1.dst',
      '/castor/cern.ch/grid/lhcb/data/2009/DST/00005727/0000/00005727_00000032_1.dst',
      '/castor/cern.ch/grid/lhcb/data/2009/DST/00005727/0000/00005727_00000033_1.dst',
      '/castor/cern.ch/grid/lhcb/data/2009/DST/00005727/0000/00005727_00000034_1.dst',
      '/castor/cern.ch/grid/lhcb/data/2009/DST/00005727/0000/00005727_00000035_1.dst',
      '/castor/cern.ch/grid/lhcb/data/2009/DST/00005727/0000/00005727_00000036_1.dst',
      '/castor/cern.ch/grid/lhcb/data/2009/DST/00005727/0000/00005727_00000037_1.dst',
      '/castor/cern.ch/grid/lhcb/data/2009/DST/00005727/0000/00005727_00000038_1.dst',
      '/castor/cern.ch/grid/lhcb/data/2009/DST/00005727/0000/00005727_00000039_1.dst',
      '/castor/cern.ch/grid/lhcb/data/2009/DST/00005727/0000/00005727_00000040_1.dst',
      '/castor/cern.ch/grid/lhcb/data/2009/DST/00005727/0000/00005727_00000041_1.dst',
      '/castor/cern.ch/grid/lhcb/data/2009/DST/00005727/0000/00005727_00000042_1.dst',
      '/castor/cern.ch/grid/lhcb/data/2009/DST/00005727/0000/00005727_00000043_1.dst',
      '/castor/cern.ch/grid/lhcb/data/2009/DST/00005727/0000/00005727_00000044_1.dst',
      '/castor/cern.ch/grid/lhcb/data/2009/DST/00005727/0000/00005727_00000045_1.dst',
      '/castor/cern.ch/grid/lhcb/data/2009/DST/00005727/0000/00005727_00000046_1.dst',
      '/castor/cern.ch/grid/lhcb/data/2009/DST/00005727/0000/00005727_00000047_1.dst',
      '/castor/cern.ch/grid/lhcb/data/2009/DST/00005727/0000/00005727_00000048_1.dst',
      '/castor/cern.ch/grid/lhcb/data/2009/DST/00005727/0000/00005727_00000049_1.dst',
      '/castor/cern.ch/grid/lhcb/data/2009/DST/00005727/0000/00005727_00000050_1.dst',
      '/castor/cern.ch/grid/lhcb/data/2009/DST/00005727/0000/00005727_00000051_1.dst',
      '/castor/cern.ch/grid/lhcb/data/2009/DST/00005727/0000/00005727_00000052_1.dst',
      '/castor/cern.ch/grid/lhcb/data/2009/DST/00005727/0000/00005727_00000053_1.dst',
      '/castor/cern.ch/grid/lhcb/data/2009/DST/00005727/0000/00005727_00000054_1.dst',
      '/castor/cern.ch/grid/lhcb/data/2009/DST/00005727/0000/00005727_00000055_1.dst',
      '/castor/cern.ch/grid/lhcb/data/2009/DST/00005727/0000/00005727_00000056_1.dst',
      '/castor/cern.ch/grid/lhcb/data/2009/DST/00005727/0000/00005727_00000057_1.dst'
      ]

else :
   import sys
   print ' Check the sample name : no valid data selected'
   sys.exit()

# Go past this line only when you know what you are doing
############################################################################################################################
## File to setup Alignment

from Configurables import TAlignment,GetElementsToBeAligned
TAlignment().ElementsToAlign      = list(elements)
TAlignment().TrackLocation        = "Rec/Track/AlignTracks"
TAlignment().Constraints          = constraints
TAlignment().SurveyConstraints    = chisqconstraints
TAlignment().WriteCondSubDetList  = ['Muon','OT']
TAlignment().CondFilePrefix       = nameCond+sample+'_Xml/'
TAlignment().OutputLevel          = outputlevel #
TAlignment().UseLocalFrame        = uselocalframe 
elementtool = GetElementsToBeAligned( "GetElementsToBeAligned" ) 
elementtool.UseLocalFrame = False 

# To load CondDB Layers -------------

from Configurables import ( CondDBAccessSvc,CondDB )
# Muon
#MUONAlignmentCondition = CondDBAccessSvc("MuonAlignmentCondition")
#MUONAlignmentCondition.ConnectionString = 'sqlite_file:/afs/cern.ch/user/s/svecchi/public/LHCBCOND-LocalCollisionNNet2.db/LHCBCOND'

# OT
OTAlignmentCondition = CondDBAccessSvc("OTAlignmentCondition")
#OTAlignmentCondition.ConnectionString = 'sqlite_file:/afs/cern.ch/user/s/svecchi/public/ScenarioSurveyXYZ.db/LHCBCOND'
#OTAlignmentCondition.ConnectionString = 'sqlite_file:/afs/cern.ch/user/s/svecchi/public/ScenarioSurveyAll.db/LHCBCOND'
#OTAlignmentCondition.ConnectionString = 'sqlite_file:/afs/cern.ch/user/s/svecchi/public/OTCosmicsCFrameAlignmentTxTyTz2009.db/LHCBCOND'
#OTAlignmentCondition.ConnectionString = 'sqlite_file:/afs/cern.ch/user/s/svecchi/public/OTCosmicsModulesTxCFramesTxTyTzRz2009.db/LHCBCOND'
#OTAlignmentCondition.ConnectionString = 'sqlite_file:/afs/cern.ch/user/s/svecchi/public/VeloOTTxTyFieldOff.db/LHCBCOND'
#OTAlignmentCondition.ConnectionString = 'sqlite_file:/afs/cern.ch/user/s/svecchi/public/VeloOTTxTyModulesTxFieldOff.db/LHCBCOND'
OTAlignmentCondition.ConnectionString = 'sqlite_file:/afs/cern.ch/user/s/svecchi/public/VeloOTTxTyModulesTxITTxTyRzTTTxFieldOff_20100115.db/LHCBCOND'
if sample == 'Mcarlo' :
   print sample
else :
   #CondDB().addLayer( MUONAlignmentCondition )
   CondDB().addLayer(OTAlignmentCondition)


# ================================================================================================
# add all 'private' reconstruction to this sequence which is automatically added to the alignment sequence.
# ================================================================================================
from Configurables import GaudiSequencer
trackFilterSeq = GaudiSequencer("TrackFilterSeq")

# ----------------------------------------------------------------------------------
# make a selection of the Best-tracks based on the chisquare
# ----------------------------------------------------------------------------------
from Configurables import (TrackSelector,TrackContainerCopy)
seedselectoralg  = TrackContainerCopy("SeedSelection")
seedselectoralg.addTool( TrackSelector("SeedSelector"), name = "Selector") 
seedselectoralg.inputLocation =   'Rec/Track/Best'
seedselectoralg.outputLocation =   'Rec/Track/SelectedBest'
seedselectoralg.Selector.MaxChi2Cut = 5 
#seedselectoralg.Selector.MinNOTHits = 12
trackFilterSeq.Members.append( seedselectoralg )

## # ----------------------------------------------------------------------------------
## from Configurables import TrackListRefiner
## # ----------------------------------------------------------------------------------

## longTrackSelector = TrackListRefiner("LongTrackSelector",
##                                      inputLocation = "Rec/Track/SelectedBest",
##                                      outputLocation = "Rec/Track/SeectedLong",
##                                      Selector = TrackSelector())
## longTrackSelector.Selector.TrackTypes = ["Long","Downstream"]
## trackFilterSeq.Members.append( longTrackSelector )

# ---------- needed to remove hot channels -------------------------
from Configurables import MuonPadRec, MuonMonHitDecode
decodingTool = MuonMonHitDecode(name="DecodingTool",OutputLevel=outputlevel)
decodingTool.LocalVeto = localVetoLIST
padRecTool = MuonPadRec(name="PadRecTool",OutputLevel=outputlevel)
padRecTool.TileVeto = tileVetoLIST
# ------------------------------------------------------------------
from Configurables import ResidualsM1, TrackMasterExtrapolator
residuals = ResidualsM1("ResidualsM1")
residuals.OutputLevel                                    = outputlevel
residuals.TracksLocation                                = 'Rec/Track/SelectedBest' # Long' # Best container + chisquare < 5
residuals.addTool( TrackMasterExtrapolator, name        = 'Extrapolator' )
residuals.Extrapolator.ApplyMultScattCorr                = True
residuals.Extrapolator.ApplyEnergyLossCorr               = True
residuals.Extrapolator.ApplyElectronEnergyLossCorr       = True
residuals.WhichStation                                   = 0 # 0 = M1 
residuals.doHistos                                       = True
residuals.doNtuple                                       = Write_Ntuple
residuals.nFOI                                           = 6. # search window for hits on M1
trackFilterSeq.Members.append( residuals)
# ------------------------------------------------------------------

from Configurables import (TrackSelector,TrackContainerCopy)
tmuonselectoralg  = TrackContainerCopy( "TmuonSelection" )
tmuonselectoralg.addTool( TrackSelector, name = "Selector")
tmuonselectoralg.Selector.MaxChi2Cut = 10
tmuonselectoralg.inputLocation =   'Rec/Track/SelectedBest'
tmuonselectoralg.outputLocation =   'Rec/Track/AlignTracks'
tmuonselectoralg.OutputLevel = outputlevel 

trackFilterSeq.Members.append( tmuonselectoralg )

##==============================================================

#-- Use latest database tags for real data
from Configurables import LHCbApp
if sample=='Mcarlo' :
   Escher().DataType     = "MC09"
   LHCbApp().DDDBtag   = "head-20091120"
   LHCbApp().CondDBtag =  "sim-20091211-vc15mm-md100"
   LHCbApp().Simulation = True
   Escher().Simulation = True
else : 
   LHCbApp().DDDBtag   = "head-20091120"
   LHCbApp().CondDBtag = "head-20091211"

Escher().DatasetName = 'Collision'   
#Escher().UseOracle = True
EventSelector().PrintFreq = 5000
Escher().PrintFreq = 5000
Escher().MoniSequence = ["Tr", "OT","ST"] # excluded VELO since it gave too many error messages (??)


ApplicationMgr().HistogramPersistency = 'ROOT'  
HistogramPersistencySvc().OutputFile = nameCond+sample+'-Histos.root' 

if Write_Ntuple :
   from Gaudi.Configuration import NTupleSvc 
   NTupleSvc().Output = [ "FILE1 DATAFILE='$HOME/"+nameCond+sample+"-Ntuple.root' TYPE='ROOT' OPT='NEW' " ]

## Need this to read raw data
#importOptions('$STDOPTS/DecodeRawEvent.py') 

Escher().InputType = 'DST'
EventSelector().Input = []

for d in data:
    name = "DATAFILE='PFN:castor:" + d + "'  TYP='POOL_ROOTTREE' OPT='READ'"
    EventSelector().Input.append(name) 


# do not do the TES check because these data do not have the required lists
# use this when reading a DST without tracks
GaudiSequencer("InitReprocSeq").Enable = False



