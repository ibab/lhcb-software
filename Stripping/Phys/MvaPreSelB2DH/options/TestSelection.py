import os
from Gaudi.Configuration import *
from Configurables import DaVinci


importOptions("$MVAPRESELB2DHROOT/options/Bs2DHStrippingFisher.py")



#from Configurables import CondDB
#CondDB(UseOracle = True)
##############################################################
# EventClockSvc(InitialTime = 1)
###############################################################
# Trigger
#from HltConf.Configuration import *

#some steering options
#Number of events to process
NumEvents=10000
#NumEvents=10000

#NumEvents=-1

dv=DaVinci()
#########################
# apply all the required options
#
dv.EvtMax=NumEvents
dv.DataType="2010"
#dv.InputType='DST'
dv.InputType='SDST'
dv.Simulation=False
dv.PrintFreq=100
dv.RedoMCLinks=False
dv.L0=True

########################################
#SeqMvASelection  = GaudiSequencer("SeqSelBs2DsH")
#dv.UserAlgorithms = [SeqMvASelection]
dv.HistogramFile="BsDsH_RealData_Stripping_Fisher.root"
DaVinci().Input   = [
"DATAFILE='LFN:/lhcb/data/2010/SDST/00006563/0000/00006563_00002181_1.sdst' TYP='POOL_ROOTTREE' OPT='READ'",
"DATAFILE='LFN:/lhcb/data/2010/SDST/00006563/0000/00006563_00002182_1.sdst' TYP='POOL_ROOTTREE' OPT='READ'",
"DATAFILE='LFN:/lhcb/data/2010/SDST/00006563/0000/00006563_00002183_1.sdst' TYP='POOL_ROOTTREE' OPT='READ'",
"DATAFILE='LFN:/lhcb/data/2010/SDST/00006563/0000/00006563_00002199_1.sdst' TYP='POOL_ROOTTREE' OPT='READ'",
"DATAFILE='LFN:/lhcb/data/2010/SDST/00006563/0000/00006563_00002294_1.sdst' TYP='POOL_ROOTTREE' OPT='READ'",
"DATAFILE='LFN:/lhcb/data/2010/SDST/00006563/0000/00006563_00002308_1.sdst' TYP='POOL_ROOTTREE' OPT='READ'",
"DATAFILE='LFN:/lhcb/data/2010/SDST/00006563/0000/00006563_00002347_1.sdst' TYP='POOL_ROOTTREE' OPT='READ'"
]
FileCatalog().Catalogs= ["xmlcatalog_file:pool_xml_catalog.xml"]
