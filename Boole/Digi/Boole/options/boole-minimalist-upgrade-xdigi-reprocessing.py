####################################################################################
# Date      : 01-07-2014
# Purpose   : Run a cleanup job using Boole
#             to make a digi file out of an xdigi.
# Author    : SciFi usual suspects
# Version   : this file was written for Boole v28r1

# Questions : Please contact lhcb-upgrade-ft-software@cern.ch

###################################################################################

from Gaudi.Configuration import *
from Configurables import Boole, LHCbApp,DDDBConf,CondDB


from Boole.Configuration import *
	
from Configurables import L0Conf

L0Conf().TCK = '0x0037'
CondDB().Upgrade     = True
LHCbApp().DDDBtag     = "dddb-20131025"
LHCbApp().CondDBtag   = "sim-20130830-vc-md100"
LHCbApp().Simulation = True
CondDB().AllLocalTagsByDataType = ["VP_UVP+RICH_2019+UT_UUT", "FT_StereoAngle2"]
importOptions("$APPCONFIGOPTS/Boole/Default.py")
importOptions("$APPCONFIGOPTS/Boole/xdigi.py")



from Configurables import Boole
Boole().DetectorLink  = ['VP', 'UT', 'FT', 'Tr','Magnet'] 
Boole().DetectorDigi  = ['VP', 'UT', 'FT', 'Magnet'] 
Boole().DetectorMoni = ['VP', 'UT', 'FT',  'Magnet']
Boole().DataType = "Upgrade"
Boole().Outputs = ["DIGI"]
Boole().DigiType = 'Extended'
Boole().EvtMax = 10
Boole().DatasetName = 'reprocess'
Boole().InputDataType    = 'XDIGI'## add 



