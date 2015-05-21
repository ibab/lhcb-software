
from Gaudi.Configuration import *

RunNr = "117201"

NTupleSvc().Output = [ "FILE1 DATAFILE='data_" + RunNr + ".root' TYP='ROOT' OPT='NEW'" ]
HistogramPersistencySvc().OutputFile = "data_" + RunNr + "_hists.root"

#EventSelector().Input = [
#  'LFN:/lhcb/data/2012/RAW/EXPRESS/LHCb/COLLISION12/117201/117201_0000000009.raw',
#  'LFN:/lhcb/data/2012/RAW/EXPRESS/LHCb/COLLISION12/117202/117202_0000000009.raw',
#  'LFN:/lhcb/data/2012/RAW/EXPRESS/LHCb/COLLISION12/117204/117204_0000000008.raw',
#  'LFN:/lhcb/data/2012/RAW/EXPRESS/LHCb/COLLISION12/117205/117205_0000000009.raw',
#  'LFN:/lhcb/data/2012/RAW/EXPRESS/LHCb/COLLISION12/117206/117206_0000000009.raw',
#  'LFN:/lhcb/data/2012/RAW/EXPRESS/LHCb/COLLISION12/117207/117207_0000000009.raw',
#  'LFN:/lhcb/data/2012/RAW/EXPRESS/LHCb/COLLISION12/117208/117208_0000000009.raw',
#  'LFN:/lhcb/data/2012/RAW/EXPRESS/LHCb/COLLISION12/117209/117209_0000000009.raw'
#]

# Events from run 123928 from fill 2880, on 26th July 2012                                                                                                                                                                                                                     
from GaudiConf import IOHelper
IOHelper('MDF').inputFiles([
 'mdf:root://eoslhcb.cern.ch//eos/lhcb/grid/prod/lhcb/freezer/lhcb/data/2012/RAW/EXPRESS/LHCb/COLLISION12/117201/117201_0000000009.raw',
#  'mdf:root://eoslhcb.cern.ch//eos/lhcb/grid/prod/lhcb/freezer/lhcb/data/2012/RAW/EXPRESS/LHCb/COLLISION12/117202/117202_0000000009.raw',                                                                                                                                 
#  'mdf:root://eoslhcb.cern.ch//eos/lhcb/grid/prod/lhcb/freezer/lhcb/data/2012/RAW/EXPRESS/LHCb/COLLISION12/117204/117204_0000000008.raw',                                                                                                                                 
#  'mdf:root://eoslhcb.cern.ch//eos/lhcb/grid/prod/lhcb/freezer/lhcb/data/2012/RAW/EXPRESS/LHCb/COLLISION12/117205/117205_0000000009.raw',                                                                                                                                 
#  'mdf:root://eoslhcb.cern.ch//eos/lhcb/grid/prod/lhcb/freezer/lhcb/data/2012/RAW/EXPRESS/LHCb/COLLISION12/117206/117206_0000000009.raw',                                                                                                                                 
#  'mdf:root://eoslhcb.cern.ch//eos/lhcb/grid/prod/lhcb/freezer/lhcb/data/2012/RAW/EXPRESS/LHCb/COLLISION12/117207/117207_0000000009.raw',                                                                                                                                 
#  'mdf:root://eoslhcb.cern.ch//eos/lhcb/grid/prod/lhcb/freezer/lhcb/data/2012/RAW/EXPRESS/LHCb/COLLISION12/117208/117208_0000000009.raw',                                                                                                                                 
#  'mdf:root://eoslhcb.cern.ch//eos/lhcb/grid/prod/lhcb/freezer/lhcb/data/2012/RAW/EXPRESS/LHCb/COLLISION12/117209/117209_0000000009.raw',                                                                                                                                 
#  'mdf:root://eoslhcb.cern.ch//eos/lhcb/grid/prod/lhcb/freezer/lhcb/data/2012/RAW/EXPRESS/LHCb/COLLISION12/117210/117210_0000000009.raw',                                                                                                                                 
#  'mdf:root://eoslhcb.cern.ch//eos/lhcb/grid/prod/lhcb/freezer/lhcb/data/2012/RAW/EXPRESS/LHCb/COLLISION12/117211/117211_0000000009.raw',                                                                                                                                 
#  'mdf:root://eoslhcb.cern.ch//eos/lhcb/grid/prod/lhcb/freezer/lhcb/data/2012/RAW/EXPRESS/LHCb/COLLISION12/117212/117212_0000000008.raw',                                                                                                                                 
#  'mdf:root://eoslhcb.cern.ch//eos/lhcb/grid/prod/lhcb/freezer/lhcb/data/2012/RAW/EXPRESS/LHCb/COLLISION12/117244/117244_0000000009.raw',                                                                                                                                 
#  'mdf:root://eoslhcb.cern.ch//eos/lhcb/grid/prod/lhcb/freezer/lhcb/data/2012/RAW/EXPRESS/LHCb/COLLISION12/117245/117245_0000000009.raw',                                                                                                                                 
 ], clear=True)



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

importOptions("clbr_withMLA.py")
