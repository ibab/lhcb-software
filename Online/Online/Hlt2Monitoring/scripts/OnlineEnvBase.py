import os
#Auto generated RunInfo options for partition:LHCb activity:EOF_CALIB  Tue Jan  6 17:26:36 2015
#
# ---------------- General partition parameters:
PartitionID              = 0
PartitionIDName          = "0000"
PartitionName            = "LHCb2"
Activity                 = "PHYSICS"
TAE                      = 0
OutputLevel              = 4
HltArchitecture          = "PassThrough"
CalibArchitecture        = "None"
# ---------------- Trigger parameters:
DeferHLT                 = 0
passThroughDelay         = 0
AcceptRate               = 1
CondDBTag                = "cond-20150617"
DDDBTag                  = "dddb-20150526"
# CondDBTag                = "cond-20150511"
# DDDBTag                  = "dddb-20150119-3"
LumiTrigger              = 1
BeamGasTrigger           = 0
LumiPars                 = [0.029999999999999999, 0.0064285699999999998, 0.0042857099999999999, 0.0021428599999999999]
OnlineVersion            = "Online"
DataflowVersion          = ""
GaudiVersion             = ""
RecoStartupMode          = 1
ConditionsMapping        = "/group/online/hlt/conditions/RunChangeHandler/Online.py"
ASDDir                   = os.environ["TEMPDIR"] + "/ASDDir"
OnlineXmlDir             = os.environ["TEMPDIR"] + "/OnlineXmlDir"
AlignXmlDir              = os.environ["TEMPDIR"]
RefFileName              = os.environ["TEMPDIR"] + "/ref_file.txt"
