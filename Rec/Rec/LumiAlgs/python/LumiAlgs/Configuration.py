
## @package LumiAlgs
#  High level Configuration tools for Lumi Accounting Algorithms
#  @author Jaap Panman
#  @date   13/02/2009

__version__ = ""
__author__  = ""

from Gaudi.Configuration  import *
import GaudiKernel.ProcessJobOptions
from Configurables import LHCbConfigurableUser 
    
# -------------------------------------------------------------------------------------------

## @class LumiAlgsConf
#  High level Configuration tools for Lumi Accounting Algorithms
#  @author Jaap Panman
#  @date   13/02/2009

class LumiAlgsConf(LHCbConfigurableUser):

    ## Steering options
    __slots__ = {
        "Context"       : "Offline"  # The context within which to run
       ,"DataType"      : "2008" # Data type, can be ['DC06','2008']
       ,"LumiSequencer" : None # The sequencer to add the Lumi Accounting to
       ,"BXTypes"       : [ 'NoBeam', 'BeamCrossing','SingleBeamRight','SingleBeamLeft'] # bunch crossing types 
       ,"HistoProduce"  : False
        }

    ## Apply the configuration to the given sequence
    def applyConf(self):
        from Configurables import ( OdinTypesFilter, LumiAccounting,
                                    HltLumiSummaryDecoder, GaudiSequencer )
        debugOPL = INFO
         
        sequence = self.getProp("LumiSequencer")
        if sequence == None : raise RuntimeError("ERROR : Lumi Sequencer not set")

        # Set context
        context = self.getProp("Context")

        # Create sub-sequences according to BXTypes

        crossings = self.getProp("BXTypes")
        BXMembers = []
        for i in crossings:
            seqMembers=[]
            seqMembers.append( OdinTypesFilter('Filter'+i,
                                               TriggerTypes=['RandomTrigger'],
                                               BXTypes=[i],
                                               OutputLevel = debugOPL ))
            seqMembers.append( HltLumiSummaryDecoder('LumiDecode'+i,
                                                  OutputLevel = debugOPL ))
            seqMembers.append( LumiAccounting('LumiCount'+i,
                                              OutputDataContainer = "/FileRecords/EOR/LumiFSR"+i,
                                              OutputLevel = debugOPL ))
            
            BXMembers.append( GaudiSequencer('Lumi'+i+'Seq', 
                                             Members = seqMembers,
                                             ModeOR = False,
                                             ShortCircuit = True,
                                             MeasureTime = True,
                                             ))

        sequence.Members = BXMembers
        sequence.MeasureTime = True
        sequence.ModeOR = True
        sequence.ShortCircuit = False
        sequence.IgnoreFilterPassed = True

