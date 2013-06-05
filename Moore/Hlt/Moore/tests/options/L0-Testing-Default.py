
from Gaudi.Configuration import *
from Configurables import L0App

############# Moore config taken from printout at the top of the sequence ############

L0App().Persistency= 'ROOT';
L0App().Verbose= False;
L0App().DataType= '2012';
L0App().DDDBtag= 'head-20120413'; #from RunDB
L0App().Simulation= False;
L0App().CondDBtag= 'head-20120420'; #from RunDB

########### L0-related things #######################

L0App().ForceSingleL0Configuration = False
L0App().EnsureKnownTCK=False

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


from PRConfig import TestFileDB
TestFileDB.test_file_db["2012_raw_default"].run(withDB=False,configurable=Moore())
