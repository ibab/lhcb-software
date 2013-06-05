#Moore without data or database flags, prepare for running offline

from Gaudi.Configuration import *
from Configurables import Moore
from Configurables import L0Conf, HltConf

############# Moore config taken from printout at the top of the sequence ############

Moore().UseDBSnapshot= False;
Moore().Persistency= 'ROOT';
Moore().outputFile= 'Test.dst';
Moore().Verbose= False;
Moore().EnableRunChangeHandler= False;
Moore().WriterRequires= [];
Moore().WriteFSR= True;
Moore().EnableDataOnDemand= True;
Moore().CheckOdin= False;
Moore().InitialTCK= '0x00790038';
Moore().Simulation= False;
Moore().ThresholdSettings="Commissioning_PassThrough"

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

