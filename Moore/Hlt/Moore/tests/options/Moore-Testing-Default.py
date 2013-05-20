
from Gaudi.Configuration import *
from Configurables import Moore
#from LHCbKernel.Configuration import *
#from GaudiConf.Configuration import *
from Configurables import L0Conf, HltConf
from Gaudi.Configuration import *
from Configurables import Moore

############# Moore config taken from printout at the top of the sequence ############

Moore().UseDBSnapshot= False;
Moore().Persistency= 'ROOT';
Moore().outputFile= 'Test.dst';
Moore().Verbose= False;
Moore().DataType= '2012';
Moore().DDDBtag= 'head-20120413'; #from RunDB
Moore().EnableRunChangeHandler= False;
Moore().WriterRequires= [];
Moore().WriteFSR= True;
Moore().EnableDataOnDemand= True;
Moore().CheckOdin= False;
Moore().InitialTCK= '0x00790038';
Moore().Simulation= False;
Moore().CondDBtag= 'head-20120420'; #from RunDB
Moore().ThresholdSettings="Physics_September2012"

########### L0-related things #######################

Moore().ForceSingleL0Configuration = False

from Configurables import L0Conf
L0Conf().EnsureKnownTCK=False

from Configurables import L0MuonAlg; L0MuonAlg( "L0Muon" ).L0DUConfigProviderType = "L0DUConfigProvider"

############# prepend a "killHltSeq" ############

#copied from Swimming/Configuration.py line 214
from Configurables import Swimming, GaudiSequencer, EventNodeKiller
nodes=Swimming.__slots__["HltNodesToKill"]
mykiller    = EventNodeKiller("killHlt")
mykiller.Nodes          = nodes
deathstar               = GaudiSequencer("killHltSeq")
deathstar.Members       = [mykiller]

class Deathstar(object):
    def __init__(self, seq):
        self._seq = seq
    def witnessTheFirepowerOfThisFullyArmedAndOperationalBattleStation(self):
        ApplicationMgr().TopAlg.insert(0, self._seq)

d = Deathstar(deathstar)
appendPostConfigAction(d.witnessTheFirepowerOfThisFullyArmedAndOperationalBattleStation)


############# prepend a "input data" ############

from GaudiConf import IOHelper
IOHelper('MDF').inputFiles(['castor:/castor/cern.ch/user/e/evh/131883/131883_0x0046_NB_L0Phys_00.raw','castor:/castor/cern.ch/user/e/evh/131884/131884_0x0046_NB_L0Phys_00.raw','castor:/castor/cern.ch/user/e/evh/131885/131885_0x0046_NB_L0Phys_00.raw']) #2012 data instead
