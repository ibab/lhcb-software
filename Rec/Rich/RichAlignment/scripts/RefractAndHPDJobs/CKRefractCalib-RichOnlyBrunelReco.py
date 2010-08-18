
from Brunel.Configuration import *
from Configurables import Brunel, LHCbApp

importOptions("CKRefractCalib-FullBrunelReco.py")

Brunel().InputType = "DST"

from Configurables import DstConf
ApplicationMgr().ExtSvc += [ "DataOnDemandSvc" ]
DstConf().EnableUnpack = True

from Configurables import RecMoniConf
RecMoniConf().MoniSequence = ["RICH"]
Brunel().RecoSequence    = [ "RICH" ] 
Brunel().InitSequence    = [ "Brunel" ]
Brunel().MCCheckSequence = [ "RICH" ]
