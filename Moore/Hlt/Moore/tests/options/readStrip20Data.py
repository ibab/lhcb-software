from Gaudi.Configuration import *
from Configurables import Moore

############# Moore config taken from printout at the top of the sequence ############

Moore().UseDBSnapshot= False;
Moore().Persistency= 'ROOT';
Moore().outputFile= 'SwimmingTest.dst';
Moore().Verbose= False;
Moore().DataType= '2011';
Moore().DDDBtag= 'head-20110722';
Moore().EnableRunChangeHandler= False;
Moore().WriterRequires= [];
Moore().WriteFSR= True;
Moore().EnableDataOnDemand= True;
Moore().L0= True;
Moore().CheckOdin= False;
Moore().InitialTCK= '0x00790038';
Moore().Simulation= False;
Moore().CondDBtag= 'head-20110901';
Moore().ReplaceL0BanksWithEmulated= False;
Moore().UseTCK= True;

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

############# prepend a "resurectRawEventSeq"  ############

from Configurables import RawEventMapCombiner, RawEventSimpleCombiner, DataOnDemandSvc

#### Try simple thingy ... something wierd happens #####
myCombiner=RawEventSimpleCombiner("resurectRawEvent")
myCombiner.InputRawEventLocations=[
    "Trigger/RawEvent",
    "Other/RawEvent",
    "Rich/RawEvent",
    "Muon/RawEvent",
    "Calo/RawEvent"#,
    #"Other/RawEvent" #can't use Other/RawEvent
    ] 
myCombiner.OutputLevel=VERBOSE

#### Try complicated thingy .. works so long as some banks are left alone #####

myMapCombiner=RawEventMapCombiner("resurectRawEventMap")

myMapCombiner.RawBanksToCopy={ 
    'ODIN': "Trigger/RawEvent",
    'L0DU': "Trigger/RawEvent",
    'L0Calo': "Trigger/RawEvent",
    'L0CaloFull': "Trigger/RawEvent",
    'L0DU': "Trigger/RawEvent",
    'L0Muon': "Trigger/RawEvent",
    'L0MuonProcCand': "Trigger/RawEvent",
    'L0PU': "Trigger/RawEvent",
    'HltSelReports': "Trigger/RawEvent",
    'HltDecReports': "Trigger/RawEvent",
    'HltRoutingBits': "Trigger/RawEvent",
    'HltVertexReports': "Trigger/RawEvent",
    'Rich': "Rich/RawEvent",
    'Muon': "Muon/RawEvent",
    'PrsE': "Calo/RawEvent",
    'EcalE': "Calo/RawEvent",
    'HcalE': "Calo/RawEvent",
    'PrsTrig': "Calo/RawEvent",
    'EcalTrig': "Calo/RawEvent",
    'HcalTrig': "Calo/RawEvent",
    'EcalPacked': "Calo/RawEvent",
    'HcalPacked': "Calo/RawEvent",
    'PrsPacked': "Calo/RawEvent",
    'EcalPackedError': "Calo/RawEvent",
    'HcalPackedError': "Calo/RawEvent",
    'PrsPackedError': "Calo/RawEvent"
##     'Velo' : "Other/RawEvent",
##     'TT' : "Other/RawEvent",
##     'IT' : "Other/RawEvent",
##     'OT' : "Other/RawEvent"
    }

myMapCombiner.OutputLevel=VERBOSE


necromancer               = GaudiSequencer("resurectRawEventSeq")

#try and use SimpleCombiner...
necromancer.Members       = [myCombiner]

#try and use MapCombiner
#necromancer.Members       = [myMapCombiner]

ApplicationMgr().ExtSvc = [ "DataOnDemandSvc" ]+ApplicationMgr().ExtSvc
DataOnDemandSvc().AlgMap["DAQ/RawEvent"]=myCombiner
DataOnDemandSvc().NodeMap[ "DAQ" ]    = "DataObject" #Do I need this?
#DataOnDemandSvc().PreInitialize=True #Do I need this?

from Configurables import ODINDecodeTool

#configure this public tool
#ODINDecodeTool().RawEventLocation="Trigger/RawEvent"
#ODINDecodeTool("ToolSvc.OdinTimeDecoder.ODINDecodeTool").RawEventLocation="Trigger/RawEvent"

class Necromancer(object):
    def __init__(self, seq):
        self._seq = seq
    def raiseFromTheDead(self):
        ApplicationMgr().TopAlg.insert(0, self._seq)


n = Necromancer(necromancer)
#appendPostConfigAction(n.raiseFromTheDead)

################ dataset #################
# Input data
from GaudiConf import IOHelper
IOHelper().inputFiles(['/castor/cern.ch/user/n/ntorr/test/00022726_00111043_1.CharmToBeSwum.dst'], clear=True)

