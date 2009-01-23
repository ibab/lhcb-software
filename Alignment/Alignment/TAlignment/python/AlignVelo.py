from Gaudi.Configuration import *

nEvents            = 1000
nIter              = 3
alignlevel         = 'sensors'
#alignlevel = 'halves'
#alignlevel = 'all'
maxHitsForVertexCorrelations = 9999

from OTAlElements import *

from Alignables import *

#elements = Alignables("/dd/Structure/LHCb/BeforeMagnetRegion/Velo/Velo(Right|Left)/Module..", "TxTyTz")
elements = Alignables()

if alignlevel == 'sensors':
   elements.VeloRSensors("TxTyRxRy")
   elements.VeloPhiSensors("TxTyRxRyRz")
   elements.VeloModules("Tz")
   # constraint global rotation, translation and shearing
   constraints = ["Tz","Ty","Tx","Szx","Szy","Szz","Rz","Rx","Ry"]
   constraints += ["VeloRight: .*?/VeloRight/.*? : Tz Szz"]
#   constraints += ["VeloLeft: .*?/VeloLeft/.*? : Tz Szz"]  
elif alignlevel == 'halves':
   elements.VeloLeft("TxTyTzRxRyRz")
   elements.VeloRight("TxTyTzRxRyRz")
   constraints = ["Tx","Ty","Tz","Rx","Ry","Rz"]
else :
   elements.Velo("TxTyRxRy")
#   elements.TT("Tz")
#   elements.IT("Tz")
#   elements.OT("Tz")
   constraints = []#["TxTyRxRy"]
   conddepths = [0,1]
   condname   = "Global.xml"

# load a misalignment
from Configurables import ( CondDBAccessSvc,CondDB )
MisAlCh1COND = CondDBAccessSvc("MisAlCh1COND")
MisAlCh1COND.ConnectionString = "sqlite_file:/afs/cern.ch/lhcb/group/tracking/vol7/wouter/DB/VeloSliceMisalignedTest3.db/LHCBCOND"
MisAlCh1COND.ConnectionString = "sqlite_file:/afs/cern.ch/lhcb/group/tracking/vol7/wouter/DB/B2HH_LHCBCOND_Velo-slice_1sigma.db/LHCBCOND"
#MisAlCh1COND.ConnectionString = "sqlite_file:/afs/cern.ch/lhcb/group/tracking/vol7/wouter/DB/VeloSliceAlignedHalvesNoCorr.db/LHCBCOND"
CondDB().addLayer(MisAlCh1COND)

#datatype = 'minbias'
datatype = ''
data               = [
   "PFN:rfio:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001820/DIGI/0000/00001820_00000004_4.digi",
   "PFN:rfio:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001820/DIGI/0000/00001820_00000009_4.digi",
   "PFN:rfio:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001820/DIGI/0000/00001820_00000010_4.digi",
   "PFN:rfio:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001820/DIGI/0000/00001820_00000011_4.digi",
   "PFN:rfio:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001820/DIGI/0000/00001820_00000014_4.digi",
   "PFN:rfio:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001820/DIGI/0000/00001820_00000015_4.digi",
   "PFN:rfio:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001820/DIGI/0000/00001820_00000016_4.digi",
   "PFN:rfio:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001820/DIGI/0000/00001820_00000018_4.digi",
   "PFN:rfio:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001820/DIGI/0000/00001820_00000020_4.digi",
   "PFN:rfio:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001820/DIGI/0000/00001820_00000024_4.digi" ]
if datatype=='beamgas':
   data               = [ "PFN:rfio:/castor/cern.ch/user/s/sblusk/7TeV_Boff_Beam12_C/MisAlCh1/Boole/digi/Boole_7TeV_Boff_Beam12_C_MisAlCh1_1000_0.digi",
                       "PFN:rfio:/castor/cern.ch/user/s/sblusk/7TeV_Boff_Beam12_C/MisAlCh1/Boole/digi/Boole_7TeV_Boff_Beam12_C_MisAlCh1_1000_1.digi",
                       "PFN:rfio:/castor/cern.ch/user/s/sblusk/7TeV_Boff_Beam12_C/MisAlCh1/Boole/digi/Boole_7TeV_Boff_Beam12_C_MisAlCh1_1000_2.digi",
                       "PFN:rfio:/castor/cern.ch/user/s/sblusk/7TeV_Boff_Beam12_C/MisAlCh1/Boole/digi/Boole_7TeV_Boff_Beam12_C_MisAlCh1_1000_3.digi" ]
# min bias
if datatype=='minbias':
   data               = [ "PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001888/DIGI/0000/00001888_00000019_4.digi",
                       "PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001888/DIGI/0000/00001888_00000050_4.digi",
                       "PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001888/DIGI/0000/00001888_00000138_4.digi",
                       "PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001888/DIGI/0000/00001888_00000171_4.digi",
                       "PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001888/DIGI/0000/00001888_00000302_4.digi",
                       "PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001888/DIGI/0000/00001888_00000306_4.digi",
                       "PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001888/DIGI/0000/00001888_00000396_4.digi",
                       "PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001888/DIGI/0000/00001888_00000397_4.digi",
                       "PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001888/DIGI/0000/00001888_00000412_4.digi",
                       "PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001888/DIGI/0000/00001888_00000413_4.digi",
                       "PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001888/DIGI/0000/00001888_00000425_4.digi",
                       "PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001888/DIGI/0000/00001888_00000436_4.digi",
                       "PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001888/DIGI/0000/00001888_00000441_4.digi",
                       "PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001888/DIGI/0000/00001888_00000465_4.digi",
                       "PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001888/DIGI/0000/00001888_00000474_4.digi",
                       "PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001888/DIGI/0000/00001888_00000485_4.digi",
                       "PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001888/DIGI/0000/00001888_00000487_4.digi",
                       "PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001888/DIGI/0000/00001888_00000498_4.digi",
                       "PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001888/DIGI/0000/00001888_00000499_4.digi",
                       "PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001888/DIGI/0000/00001888_00000513_4.digi",
                       "PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001888/DIGI/0000/00001888_00000517_4.digi",
                       "PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001888/DIGI/0000/00001888_00000551_4.digi",
                       "PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001888/DIGI/0000/00001888_00000683_4.digi",
                       "PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001888/DIGI/0000/00001888_00000691_4.digi",
                       "PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001888/DIGI/0000/00001888_00000821_4.digi",
                       "PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001888/DIGI/0000/00001888_00000993_4.digi",
                       "PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001888/DIGI/0000/00001888_00001015_4.digi",
                       "PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001888/DIGI/0000/00001888_00001027_4.digi",
                       "PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001888/DIGI/0000/00001888_00001032_4.digi",
                       "PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001888/DIGI/0000/00001888_00001037_4.digi",
                       "PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001888/DIGI/0000/00001888_00001039_4.digi",
                       "PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001888/DIGI/0000/00001888_00001052_4.digi",
                       "PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001888/DIGI/0000/00001888_00001102_4.digi",
                       "PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001888/DIGI/0000/00001888_00001117_4.digi",
                       "PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001888/DIGI/0000/00001888_00001274_4.digi",
                       "PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001888/DIGI/0000/00001888_00001278_4.digi",
                       "PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001888/DIGI/0000/00001888_00001281_4.digi",
                       "PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001888/DIGI/0000/00001888_00001300_4.digi",
                       "PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001888/DIGI/0000/00001888_00001301_4.digi",
                       "PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001888/DIGI/0000/00001888_00001338_4.digi",
                       "PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001888/DIGI/0000/00001888_00001368_4.digi",
                       "PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001888/DIGI/0000/00001888_00001389_4.digi",
                       "PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001888/DIGI/0000/00001888_00001408_4.digi",
                       "PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001888/DIGI/0000/00001888_00001411_4.digi",
                       "PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001888/DIGI/0000/00001888_00001442_4.digi",
                       "PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001888/DIGI/0000/00001888_00001461_4.digi",
                       "PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001888/DIGI/0000/00001888_00001478_4.digi",
                       "PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001888/DIGI/0000/00001888_00001497_4.digi",
                       "PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001888/DIGI/0000/00001888_00001524_4.digi",
                       "PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001888/DIGI/0000/00001888_00001604_4.digi",
                       "PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001888/DIGI/0000/00001888_00001609_4.digi",
                       "PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001888/DIGI/0000/00001888_00001620_4.digi",
                       "PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001888/DIGI/0000/00001888_00001642_4.digi",
                       "PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001888/DIGI/0000/00001888_00001644_4.digi",
                       "PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001888/DIGI/0000/00001888_00001651_4.digi",
                       "PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001888/DIGI/0000/00001888_00001690_4.digi",
                       "PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001888/DIGI/0000/00001888_00001702_4.digi",
                       "PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001888/DIGI/0000/00001888_00001713_4.digi",
                       "PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001888/DIGI/0000/00001888_00001719_4.digi",
                       "PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001888/DIGI/0000/00001888_00001723_4.digi",
                       "PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001888/DIGI/0000/00001888_00001726_4.digi",
                       "PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001888/DIGI/0000/00001888_00001728_4.digi",
                       "PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001888/DIGI/0000/00001888_00001772_4.digi",
                       "PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001888/DIGI/0000/00001888_00001840_4.digi",
                       "PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001888/DIGI/0000/00001888_00001937_4.digi",
                       "PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001888/DIGI/0000/00001888_00001941_4.digi",
                       "PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001888/DIGI/0000/00001888_00001959_4.digi",
                       "PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001888/DIGI/0000/00001888_00001975_4.digi",
                       "PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001888/DIGI/0000/00001888_00001988_4.digi",
                       "PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001888/DIGI/0000/00001888_00002005_4.digi",
                       "PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001888/DIGI/0000/00001888_00002011_4.digi",
                       "PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001888/DIGI/0000/00001888_00002135_4.digi",
                       "PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001888/DIGI/0000/00001888_00002336_4.digi",
                       "PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001888/DIGI/0000/00001888_00002372_4.digi",
                       "PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001888/DIGI/0000/00001888_00002448_4.digi",
                       "PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001888/DIGI/0000/00001888_00002457_4.digi",
                       "PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001888/DIGI/0000/00001888_00002512_4.digi",
                       "PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001888/DIGI/0000/00001888_00002574_4.digi",
                       "PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001888/DIGI/0000/00001888_00002576_4.digi",
                       "PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001888/DIGI/0000/00001888_00002583_4.digi",
                       "PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001888/DIGI/0000/00001888_00002584_4.digi",
                       "PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001888/DIGI/0000/00001888_00002604_4.digi",
                       "PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001888/DIGI/0000/00001888_00002608_4.digi",
                       "PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001888/DIGI/0000/00001888_00002611_4.digi",
                       "PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001888/DIGI/0000/00001888_00002632_4.digi",
                       "PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001888/DIGI/0000/00001888_00002644_4.digi",
                       "PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001888/DIGI/0000/00001888_00002657_4.digi",
                       "PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001888/DIGI/0000/00001888_00002664_4.digi",
                       "PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001888/DIGI/0000/00001888_00002681_4.digi",
                       "PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001888/DIGI/0000/00001888_00002683_4.digi",
                       "PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001888/DIGI/0000/00001888_00002684_4.digi",
                       "PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001888/DIGI/0000/00001888_00002689_4.digi",
                       "PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001888/DIGI/0000/00001888_00002707_4.digi",
                       "PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001888/DIGI/0000/00001888_00002711_4.digi",
                       "PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001888/DIGI/0000/00001888_00002720_4.digi",
                       "PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001888/DIGI/0000/00001888_00002729_4.digi",
                       "PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001888/DIGI/0000/00001888_00002734_4.digi",
                       "PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001888/DIGI/0000/00001888_00002747_4.digi",
                       "PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001888/DIGI/0000/00001888_00002753_4.digi",
                       "PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001888/DIGI/0000/00001888_00002754_4.digi",
                       "PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001888/DIGI/0000/00001888_00002755_4.digi",
                       "PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001888/DIGI/0000/00001888_00002757_4.digi",
                       "PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001888/DIGI/0000/00001888_00002773_4.digi",
                       "PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001888/DIGI/0000/00001888_00002783_4.digi",
                       "PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001888/DIGI/0000/00001888_00002785_4.digi",
                       "PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001888/DIGI/0000/00001888_00002793_4.digi",
                       "PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001888/DIGI/0000/00001888_00002825_4.digi",
                       "PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001888/DIGI/0000/00001888_00002826_4.digi",
                       "PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001888/DIGI/0000/00001888_00002835_4.digi",
                       "PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001888/DIGI/0000/00001888_00002840_4.digi",
                       "PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001888/DIGI/0000/00001888_00002842_4.digi",
                       "PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001888/DIGI/0000/00001888_00002843_4.digi",
                       "PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001888/DIGI/0000/00001888_00002849_4.digi",
                       "PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001888/DIGI/0000/00001888_00002850_4.digi",
                       "PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001888/DIGI/0000/00001888_00002883_4.digi",
                       "PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001888/DIGI/0000/00001888_00002889_4.digi",
                       "PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001888/DIGI/0000/00001888_00002926_4.digi",
                       "PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001888/DIGI/0000/00001888_00002958_4.digi" ]

	
# Go past this line only when you know what you are doing
############################################################################################################################

## File to setup Alignment
import GaudiKernel.ProcessJobOptions
GaudiKernel.ProcessJobOptions.PrintOff()

from AlConfigurable import *

alignment = AlConfigurable()
alignment.DataType                     = 'DC06'
alignment.Simulation                   = True
alignment.OutputLevel                  = INFO
alignment.Pat                          = True
alignment.ElementsToAlign              = list(elements)
alignment.NumIterations                = nIter
alignment.AlignInputTrackCont           = "Alignment/AlignmentTracks"
#alignment.AlignInputTackCont           = "Rec/Track/Best"
alignment.UseCorrelations              = True
alignment.Constraints                  = constraints
alignment.MinNumberOfHits              = 10
alignment.SolvTool                     = "DiagSolvTool"
alignment.WriteCondSubDetList          = [ 'Velo' ]
alignment.VertexLocation = "Rec/Vertex/Primary"
alignment.Chi2Outlier = 10000
alignment.EigenValueThreshold = 50
alignment.SimplifiedGeom = True

ApplicationMgr().ExtSvc += [ 'DataOnDemandSvc' ]
# this sets up the linker from particles to hits
#importOptions("$ASSOCIATORSROOT/options/Brunel.opts")
# this loads the hits
importOptions('$STDOPTS/DecodeRawEvent.py')
#IODataManager().AgeLimit = 2

from Configurables import (TrackContainerCopy, TrackSelector)
#trackselectortool = TrackSelector( MinPCut = "3000", MaxChi2Cut="20", TrackTypes = [ "Long","Upstream","Velo" ] ) 
#trackselectortool = TrackSelector( MaxChi2Cut="20", TrackTypes = [ "Long","Upstream","Velo" ] )
trackselectortool = TrackSelector( "AlignTrackSelectorTool",
                                   MaxChi2Cut="20",
                                   MinNDoF=2,
                                   MinNVeloPhiHits=5,
                                   MinNVeloRHits=5,
                                   TrackTypes = [ "Long","Velo","Upstream" ])
                                   #TrackTypes = [ "Long","Velo" ] ) 

trackselectoralg = TrackContainerCopy("AlignTrackSelector",
                                      inputLocation = "Rec/Track/Best",
                                      outputLocation = "Alignment/AlignmentTracks" )
trackselectoralg.addTool( trackselectortool, "Selector" )
alignment.filterSeq().Members.append( trackselectoralg  )

from Configurables import (Al__VertexResidualTool, Al__AlignConstraintTool)
trkselector = TrackSelector("VertexTrackSelector",TrackTypes = [ "Long" ] )
vertexresidualtool = Al__VertexResidualTool("Al::VertexResidualTool")
#vertexresidualtool.addTool(TrackSelector("TrackSelector",TrackTypes = [ "Long" ] ) )
#vertexresidualtool.addTool(trkselector,name = "MyTrackSelector")
#vertexresidualtool.MyTrackSelector.TrackTypes = [ "Long" ]
#vertexresidualtool.MyTrackSelectorHandle = trkselector 
#vertexresidualtool.ComputeCorrelations = computeVertexCorrelations
vertexresidualtool.MaxHitsPerTrackForCorrelations = maxHitsForVertexCorrelations

## run some monitoring code
from Configurables import ( TrackMonitor,TrackVertexMonitor )


preMonitorSeq = GaudiSequencer('PreMonitorSeq')
postMonitorSeq = GaudiSequencer('PostMonitorSeq')
preMonitorSeq.Members.append( TrackMonitor(name = "PreAlignTrackMonitor",
                                           TracksInContainer = 'Alignment/AlignmentTracks' ) )
postMonitorSeq.Members.append( TrackMonitor(name = "PostAlignTrackMonitor",
                                            TracksInContainer = 'Alignment/AlignmentTracks' ) )
preMonitorSeq.Members.append( TrackVertexMonitor(name = "PreVertexMonitor") )
postMonitorSeq.Members.append( TrackVertexMonitor(name = "PostVertexMonitor") )
alignment.filterSeq().Members += [preMonitorSeq,postMonitorSeq]

## Now lets run it
from GaudiPython import *
from GaudiPython import gbl

# Hack to overide default EvtSel open
from GaudiPython.Bindings import iEventSelector
iEventSelector.__open_orig__ = iEventSelector.open
def _my_open_(self,stream, typ = 'POOL_ROOT', opt = 'READ', sel = None, fun = None, collection = None):
   if typ == "MDF":
       if type(stream) != list : stream = [stream]
       fixpart = "TYP=\'%s\' OPT=\'%s\' SVC='LHCb::MDFSelector'" % ( typ, opt )
       if sel        : fixpart += " SEL=\'%s\'" % sel
       if fun        : fixpart += " FUN=\'%s\'" % fun
       if collection : fixpart += " COLLECTION=\'%s\'" % collection
       cstream = ["DATAFILE=\'%s\' %s" % ( s, fixpart) for s in stream]
       self.Input = cstream
       self.reinitialize()
   elif typ == 'POOL_ROOT':
       if type(stream) != list : stream = [stream]
       fixpart = "TYP=\'%s\' OPT=\'%s\'" % ( typ, opt )
       if sel        : fixpart += " SEL=\'%s\'" % sel
       if fun        : fixpart += " FUN=\'%s\'" % fun
       if collection : fixpart += " COLLECTION=\'%s\'" % collection
       cstream = ["DATAFILE=\'%s\' %s" % ( s, fixpart) for s in stream]
       self.Input = cstream
       self.reinitialize()
   else:
      self.__open_orig__(stream,typ,opt,sel,fun,collection)
iEventSelector.open = _my_open_

def update(algorithm, appMgr) :
   # get pointer to incident service
   incSvc = appMgr.service( 'IncidentSvc', 'IIncidentSvc' )
   # incident to trigger update of constants
   updateConstants = gbl.Incident( algorithm, 'UpdateConstants' )
   incSvc.fireIncident( updateConstants )

HistogramPersistencySvc().OutputFile = "alignhistos.root"

## Instantiate application manager
appMgr = AppMgr()
mainSeq = appMgr.algorithm( 'AlignmentMainSeq' )

## Print flow of application
alignment.printFlow(appMgr)

evtSel           = appMgr.evtSel()
evtSel.printfreq = 1
##evtSel.FirstEvent = 604

## Open Files; Also initialises Application Manager
evtSel.open( data, typ = 'POOL_ROOT' )

for i in range( nIter ) :
   mainSeq.Enable = False
   evtSel.rewind()
   mainSeq.Enable = True
   appMgr.algorithm('PreMonitorSeq').Enable = ( i == 0)
   appMgr.algorithm('PostMonitorSeq').Enable = ( i == nIter-1)
   appMgr.run( nEvents )
   update( "Alignment", appMgr )

appMgr.finalize()


