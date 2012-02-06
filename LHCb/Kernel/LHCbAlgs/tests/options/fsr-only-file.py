from Gaudi.Configuration import *

from GaudiConf import IOHelper;
IOHelper().inputFiles(['castor:/castor/cern.ch/user/r/rlambert/testfiles/JustFSR.2010.EW.dst']);

from Configurables import LHCbApp;
LHCbApp().EvtMax=-1;
LHCbApp().DataType="2010"
#LHCbApp().Persistency='ROOT';

#IOHelper().outStream("tryRoot.dst","InputCopyStream")
IOHelper().postConfigDebug()
