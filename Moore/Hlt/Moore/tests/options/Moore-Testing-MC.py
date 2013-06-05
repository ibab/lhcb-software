
from Gaudi.Configuration import *
from Configurables import Moore
Moore().WriteFSR= False;
Moore().Simulation= True;
from Configurables import LHCbApp;
LHCbApp().DDDBtag="dddb-20120831";
LHCbApp().CondDBtag="sim-20121025-vc-md100";

############# "input data" ############
from PRConfig import TestFileDB
TestFileDB.test_file_db["Sim08_2012_ForMoore"].run(withDB=False,configurable=Moore())
