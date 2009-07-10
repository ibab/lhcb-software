
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
       ,"DataType"      : "2008"     # Data type, can be ['DC06','2008','MC09','2009']
       ,"InputType"     : "MDF"      # Data type, can be ['MDF','DST']
       ,"LumiSequencer" : None       # The sequencer to add the Lumi Accounting to - essential input
       ,"BXTypes"       : [ 'NoBeam', 'BeamCrossing','Beam1','Beam2'] # bunch crossing types 
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
        
        # Input data type
        inputType = self.getProp("InputType")

        # Create sub-sequences according to BXTypes
        crossings = self.getProp("BXTypes")
        BXMembers = []
        for i in crossings:
            seqMembers=[]
            seqMembers.append( OdinTypesFilter('Filter'+i,
                                               TriggerTypes=['RandomTrigger'],
                                               BXTypes=[i],
                                               OutputLevel = debugOPL ))
            decoder = HltLumiSummaryDecoder('LumiDecode'+i, OutputLevel = debugOPL )
            seqMembers.append( decoder )
            accounting = LumiAccounting('LumiCount'+i,
                                        OutputDataContainer = "/FileRecords/EOR/LumiFSR"+i,
                                        OutputLevel = debugOPL )
            seqMembers.append( accounting )
            
            BXMembers.append( GaudiSequencer('Lumi'+i+'Seq', 
                                             Members = seqMembers,
                                             ModeOR = False,
                                             ShortCircuit = True,
                                             MeasureTime = True,
                                             ))
            if inputType == 'DST':
                decoder.OutputContainerName='LumiSummaries'
                accounting.InputDataContainer='LumiSummaries'

        sequence.Members = BXMembers
        sequence.MeasureTime = True
        sequence.ModeOR = True
        sequence.ShortCircuit = False
        sequence.IgnoreFilterPassed = True

