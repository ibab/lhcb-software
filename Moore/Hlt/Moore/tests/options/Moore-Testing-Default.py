
from Gaudi.Configuration import *
from Configurables import Moore
#from LHCbKernel.Configuration import *
#from GaudiConf.Configuration import *
from Configurables import L0Conf, HltConf
from Gaudi.Configuration import *
from Configurables import Moore

############# Moore config taken from printout at the top of the sequence ############

Moore().outputFile= 'Test.dst';
Moore().DataType= '2012';
Moore().DDDBtag= 'dddb-20120831'; #from RunDB
Moore().WriterRequires= [];
Moore().EnableDataOnDemand= True;
Moore().CheckOdin= False;
Moore().InitialTCK= '0x00790038';
Moore().CondDBtag= 'cond-20120831'; #from RunDB
Moore().EnableTimer=True

from Configurables import TimingAuditor, SequencerTimerTool
TimingAuditor('TIMER').addTool(SequencerTimerTool, name="TIMER")
TimingAuditor('TIMER').TIMER.NameSize=150

########### L0-related things #######################

from Configurables import L0Conf,HltConf
L0Conf().EnsureKnownTCK=False
HltConf().ForceSingleL0Configuration=False

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
