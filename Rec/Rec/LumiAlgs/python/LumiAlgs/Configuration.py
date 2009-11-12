'''
High level Configuration tools for Lumi Accounting Algorithms

Will configure a sequencer to run the Lumi Accounting algs.
For MDF files FSRs are created.
For DST files they are only created if ForceFullSequence is set to true.

The FSR tree should always be navigated, and the EventAccounting alg always called regardless.

'''
## @package LumiAlgs
#  High level Configuration tools for Lumi Accounting Algorithms
#  @author Jaap Panman
#  @date   13/02/2009

__version__ = ""
__author__  = "Jaap Panman"

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
       ,"InputType"     : "MDF"      # Data type, can be ['MDF','DST']. Different sequencer made.
       ,"ForceFullSequence" : False  # re-write the FSR independent of the InputType
       ,"SetFSRStatus" : ""        # overwrite the event FSR status to something ['UNRELIABLE', 'ERROR','VERIFIED']
       ,"LumiSequencer" : None       # The sequencer to add the Lumi Accounting to - essential input
       ,"BXTypes"       : [ 'NoBeam', 'BeamCrossing','Beam1','Beam2'] # bunch crossing types 
       ,"HistoProduce"  : False
       ,"OutputLevel"   : WARNING
        }

    _propertyDocDct = {
       "Context"   : """ The context within which to run """
       ,"DataType"      : "Data type, can be ['DC06','2008','MC09','2009']"
       ,"InputType"     : "Input Data type, can be ['MDF','DST']"
       ,"ForceFullSequence" : "False, re-write the FSR independent of the InputType"
       ,"SetFSRStatus" : "overwrite the event FSR status to something ['UNRELIABLE', 'ERROR','VERIFIED']"
       ,"LumiSequencer" : "The sequencer to add the Lumi Accounting to - essential input"
       ,"BXTypes"       : "bunch crossing types [ 'NoBeam', 'BeamCrossing','Beam1','Beam2'] "
       ,"HistoProduce"  : "No idea, Jaap should say something here"
       ,"OutputLevel"   : "What to print to screen, default only WARNING"
       }
    
    ## Helper functions
    def fillFSR(self):
        '''fill the original FSR, return a sequence'''
        from Configurables import ( LumiAccounting,
                                    HltLumiSummaryDecoder, GaudiSequencer )
        from Configurables import LoKi__ODINFilter  as ODINFilter
        # Create sub-sequences according to BXTypes
        crossings = self.getProp("BXTypes")
        BXMembers = []
        for i in crossings:
            #the logic here is definately wrong!!
            seqMembers=[]
            seqMembers.append( ODINFilter ( 'Filter'+i,
                                            Code = ' ( ODIN_TRGTYP == LHCb.ODIN.LumiTrigger ) & ( ODIN_BXTYP == LHCb.ODIN.'+i+' ) ' ))
            decoder = HltLumiSummaryDecoder('LumiDecode'+i)
            seqMembers.append( decoder )
            accounting = LumiAccounting('LumiCount'+i,
                                        OutputDataContainer = "/FileRecords/LumiFSR"+i)
            accounting.OutputLevel = self.getProp("OutputLevel")
            seqMembers.append( accounting )
            
            BXMembers.append( GaudiSequencer('Lumi'+i+'Seq', 
                                             Members = seqMembers,
                                             ModeOR = False,
                                             ShortCircuit = True,
                                             MeasureTime = True,
                                             OutputLevel = self.getProp("OutputLevel")
                                             ))
            if self.getProp('InputType') == 'DST':
                    decoder.OutputContainerName='LumiSummaries'
                    accounting.InputDataContainer='LumiSummaries'
        
        return BXMembers
    
    def fillEventFSR(self,status=""):
        '''fill the EventAccounting'''
        from Configurables import (EventAccounting, GaudiSequencer)
        accounting = EventAccounting('EventAccount')
        accounting.OutputLevel = self.getProp("OutputLevel")
        if status is not None and status != "":
            accounting.DefaultStatus=status
            accounting.OverrideStatus=True
        return [accounting]
    
    def touchFSR(self):
        '''make sure the FSR is touched and readable'''
        #from Configurables import (LumiFileReader)
        #readingFSR=LumiFileReader("TouchLumiFSR")
        from Configurables import (LumiIntegrateFSR)
        readingFSR=LumiIntegrateFSR("TouchLumiFSR")
        readingFSR.OutputLevel = self.getProp("OutputLevel")
        return [readingFSR]
    
    ## Apply the configuration to the given sequence
    def __apply_configuration__(self):
        ''' Apply the configuration to the given sequence '''
        
        sequence = self.getProp("LumiSequencer")
        if sequence == None : raise RuntimeError("ERROR : Lumi Sequencer not set")
        
        # Set context
        context = self.getProp("Context")
        
        # Check input data type
        inputType = self.getProp("InputType")
        if inputType not in ['MDF','DST']:
            raise RuntimeError("ERROR : Unknown input type " + str(inputType))
        
        forced=self.getProp("ForceFullSequence")
        status=self.getProp("SetFSRStatus")
        
        if inputType == 'MDF' or forced:
            #create the FSRs
            
            # Create sub-sequences according to BXTypes
            BXMembers=self.fillFSR()
            sequence.Members += BXMembers
            
            if status is None:
                #by definition, all raw files are verified
                status='VERIFIED'
        
        #create the Event FSR
        EvMembers=self.fillEventFSR(status)

        #finally configure the sequence
        sequence.Members += EvMembers
        
        #Touch all FSRs so they are always copied
        if inputType != 'MDF':
            TouchMembers=self.touchFSR()
            sequence.Members += TouchMembers
        
        sequence.MeasureTime = True
        sequence.ModeOR = True
        sequence.ShortCircuit = False
        sequence.OutputLevel = self.getProp("OutputLevel")
        sequence.IgnoreFilterPassed = True
        
