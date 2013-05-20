
from Gaudi.Configuration import *
from Configurables import Moore
Moore().WriteFSR= False;
Moore().Simulation= True;
from Configurables import LHCbApp;
LHCbApp().DDDBtag="dddb-20120831";
LHCbApp().CondDBtag="sim-20121025-vc-md100";

############# "input data" ############

from GaudiConf import IOHelper
IOHelper().inputFiles(["PFN:/afs/cern.ch/lhcb/software/DEV/GAUSS/Tests/Tests-v42r4/HepMCYes-10000000-100ev-20130225.digi","PFN:/afs/cern.ch/lhcb/software/DEV/GAUSS/Tests/Tests-v42r4/HepMCNo-10000000-100ev-20130304.digi"]);
