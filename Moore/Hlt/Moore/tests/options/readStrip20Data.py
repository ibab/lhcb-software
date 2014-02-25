from Gaudi.Configuration import *
from Configurables import Moore

############# Moore config taken from printout at the top of the sequence ############

Moore().outputFile= 'SwimmingTest.dst';
Moore().DataType= '2012';
Moore().DDDBtag= 'head-20120413'; #from RunDB
Moore().WriterRequires= [];
Moore().EnableDataOnDemand= True;
Moore().CheckOdin= False;
Moore().InitialTCK= '0x00790038';
Moore().CondDBtag= 'head-20120420'; #from RunDB

from Configurables import L0Conf, HltConf
L0Conf().EnsureKnownTCK=False
Moore().ForceSingleL0Configuration = False
from Configurables import L0MuonAlg
L0MuonAlg( "L0Muon" ).L0DUConfigProviderType = "L0DUConfigProvider"

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

############# prepend a "resurectRawEventSeq"  ############

from Configurables import RecombineRawEvent
RecombineRawEvent()



