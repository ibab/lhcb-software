from Gaudi.Configuration import *

from PRConfig import TestFileDB
TestFileDB.test_file_db["2010_justFSR_EW"].run();

from Configurables import LHCbApp;
LHCbApp().EvtMax=-1;
LHCbApp().DataType="2010"
#LHCbApp().Persistency='ROOT';

from GaudiConf import IOHelper
#IOHelper().outStream("tryRoot.dst","InputCopyStream")
IOHelper().postConfigDebug()
