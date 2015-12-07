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
from Configurables import LumiMergeFSR
from Configurables import FSRCleaner
    
# -------------------------------------------------------------------------------------------

## @class LumiAlgsConf
#  High level Configuration tools for Lumi Accounting Algorithms
#  @author Jaap Panman
#  @date   13/02/2009

class LumiAlgsConf(LHCbConfigurableUser):

    ## Steering options
    __slots__ = {
        "Context"       : "Offline"  # The context within which to run
       ,"DataType"      : "2012"     # Data type, can be ['DC06','2008','MC09','2009','2010', '2011', ... , '2015']
       ,"InputType"     : "MDF"      # Data type, can be ['MDF','*DST','ETC','DIGI']. Different sequencer made.
       ,"ForceFullSequence" : False  # re-write the FSR independent of the InputType
       ,"MergeFSR"      : False      # merge FSRs into one container (incompatible with Integrate)
       ,"SetFSRStatus"  : ""         # overwrite the event FSR status to something ['UNRELIABLE', 'ERROR','VERIFIED']
       ,"LumiSequencer" : None       # The sequencer to add the Lumi Accounting to - essential input
       ,"BXTypes"       : [ 'NoBeam', 'BeamCrossing','Beam1','Beam2'] # bunch crossing types
       ,"UseLumiLow"    : False      # flag to use the LumiLow event for counting
       ,"OutputLevel"   : INFO
       ,"Simulation"    : False      # flag to indicate simulated data
       ,"IgnoreDQFlags" : False      # flag to ignore data quality
        }

    _propertyDocDct = {
        "Context"       : """ The context within which to run """
        ,"DataType"      : "Data type, can be ['DC06','2008','MC09','2009','2010', ..., '2015']"
       ,"InputType"     : "Input Data type, can be ['MDF','*DST','ETC','DIGI']"
       ,"ForceFullSequence" : "False, re-write the FSR independent of the InputType"
       ,"MergeFSR"      : "False, merge the FSRs into one container"
       ,"SetFSRStatus"  : "overwrite the event FSR status to something ['UNRELIABLE', 'ERROR','VERIFIED']"
       ,"LumiSequencer" : "The sequencer to add the Lumi Accounting to - essential input"
       ,"BXTypes"       : "bunch crossing types [ 'NoBeam', 'BeamCrossing','Beam1','Beam2'] "
       ,"UseLumiLow"    : "flag to use the LumiLow event for counting [False]"
       ,"OutputLevel"   : "printed output"
       ,"Simulation"    : "flag to indicate simulated data [False]"
       ,"IgnoreDQFlags" : "flag to ignore DataQuality flags [False]"
       }
    
    ## Helper functions
    def fillFSR(self):
        '''
        fill the original FSR, return a sequence
        requires LumiTrigger
        and requires Method counter == RandomMethod
        - WARNING -
        - the Method selection is valid for Moore as of v9 -
        - to keep code clean it is better to use for earlier versions of Moore only the other FSR -
        '''
        from Configurables import ( LumiAccounting,
                                    FilterOnLumiSummary, FilterFillingScheme,
                                    GaudiSequencer )
        from Configurables import LoKi__ODINFilter  as ODINFilter
        # Create sub-sequences according to BXTypes
        crossings = self.getProp("BXTypes")
        dsttype = ('---' + self.getProp('InputType'))[-3:] == 'DST'
        BXMembers = []
        from DAQSys.Decoders import DecoderDB
        HltLumiSummaryDecoder=DecoderDB["HltLumiSummaryDecoder"]
        for i in crossings:
            seqMembers=[]
            seqMembers.append( ODINFilter ( 'Filter'+i,
                                            Code = ' ( ODIN_TRGTYP == LHCb.ODIN.LumiTrigger ) & ( ODIN_BXTYP == LHCb.ODIN.'+i+' ) ' ))
            ddb=HltLumiSummaryDecoder.clone('HltLumiSummaryDecoder/LumiDecode'+i)
            #ddb.Active=True
            #don't think this will work, all decoders would be writing to the same location? What if a bunch is of two types?
            # this is OK, the bunchtypes are exclusive
            if dsttype:
                    ddb.overrideOutputs('LumiSummaries')
            decoder=ddb.setup()
            seqMembers.append( decoder )
            methodfilter = FilterOnLumiSummary('LumiFilter'+i,
                                               CounterName = "Random",
                                               ValueName = "RandomMethod",
                                               OutputLevel = self.getProp("OutputLevel") )
            seqMembers.append( methodfilter )

            if not self.getProp("Simulation"):
                # add fillingscheme filter
                OppositeBeam = '0' # default is no action
                if i == 'Beam1': OppositeBeam = '2'
                if i == 'Beam2': OppositeBeam = '1'
                
                if OppositeBeam != '0':

                    if self.getProp("DataType") == "2008":
                        pass
                    elif self.getProp("DataType") == "2009":
                        pass
                    elif self.getProp("DataType") == "2010":
                        pass
                    elif self.getProp("DataType") == "2011":
                        fillingfilter = FilterFillingScheme('Filling'+i, 
                                                            Beam = OppositeBeam,                       # check the opposite beam
                                                            MagnetState = 'UP',                        # only for magnet UP
                                                            BXOffset = -1,                             # check earlier bunch
                                                            OutputLevel = self.getProp("OutputLevel"), # self.getProp("OutputLevel")
                                                            )
                        seqMembers.append( fillingfilter )
                    
                    elif self.getProp("DataType") == "2012" or self.getProp("DataType") == "2013":     # 2013 is not a separate type
                        fillingfilter = FilterFillingScheme('Filling'+i, 
                                                            Beam = OppositeBeam,                       # check the opposite beam
                                                            MagnetState = 'ALL',                       # all magnet states
                                                            BXOffset = -1,                             # check earlier bunch
                                                            OutputLevel = self.getProp("OutputLevel"), # self.getProp("OutputLevel")
                                                            )
                        seqMembers.append( fillingfilter )
                                                    
                    elif self.getProp("DataType") == "2015":                                           # for 25 ns no filter possible
                        pass

                    elif self.getProp("DataType") == "2016":
                        pass

                    else:
                        assert False , "*** The luminosity logic is not yet set up for this datatype ***"
                
                accounting = LumiAccounting('LumiCount'+i,
                                            OutputDataContainer = "/FileRecords/LumiFSR"+i,
                                            OutputLevel = self.getProp("OutputLevel") )
                seqMembers.append( accounting )
            
            BXMembers.append( GaudiSequencer('Lumi'+i+'Seq', 
                                             Members = seqMembers,
                                             ModeOR = False,
                                             ShortCircuit = True
                                             ))
            if dsttype:
                    accounting.InputDataContainer='LumiSummaries'
        
        return BXMembers
    
    def fillLowLumiFSR(self):
        '''
        fill the low lumi FSR, return a sequence
        similar to the fillFSR method, but accepts more trigger types and
        applies the method filter Method:L0RateMethod
        this is valid for all versions om Moore 
        '''
        from Configurables import ( LumiAccounting,
                                    HltLumiSummaryDecoder, FilterOnLumiSummary, GaudiSequencer )
        from Configurables import LoKi__ODINFilter  as ODINFilter
        # Create sub-sequences according to BXTypes
        crossings = self.getProp("BXTypes")
        dsttype = ('---' + self.getProp('InputType'))[-3:] == 'DST'
        BXMembers = []
        from DAQSys.Decoders import DecoderDB
        HltLumiSummaryDecoder=DecoderDB["HltLumiSummaryDecoder"]
        for i in crossings:
            seqMembers=[]
            seqMembers.append( ODINFilter ( 'FilterLow'+i,
                                            Code = ' ( ODIN_TRGTYP <= LHCb.ODIN.LumiTrigger ) & ( ODIN_BXTYP == LHCb.ODIN.'+i+' ) ' ))
            ddb=HltLumiSummaryDecoder.clone('HltLumiSummaryDecoder/LumiDecode'+i)
            #ddb.Active=True
            #don't think this will work, all decoders would be writing to the same location? What if a bunch is of two types?
            # this is OK, the bunchtypes are exclusive
            if dsttype:
                    ddb.overrideOutputs('LumiSummaries')
            decoder=ddb.setup()
            seqMembers.append( decoder )
            methodfilter = FilterOnLumiSummary('LumiLowFilter'+i,
                                               CounterName = "Method",
                                               ValueName = "L0RateMethod",
                                               OutputLevel = self.getProp("OutputLevel") )
            seqMembers.append( methodfilter )
            
            if not self.getProp("Simulation"):
                accounting = LumiAccounting('LumiLowCount'+i,
                                            OutputDataContainer = "/FileRecords/LumiLowFSR"+i,
                                            OutputLevel = self.getProp("OutputLevel") )
                seqMembers.append( accounting )
            
            BXMembers.append( GaudiSequencer('LumiLow'+i+'Seq', 
                                             Members = seqMembers,
                                             ModeOR = False,
                                             ShortCircuit = True
                                             ))
            if dsttype:
                    accounting.InputDataContainer='LumiSummaries'
                    methodfilter.InputDataContainer='LumiSummaries'
        
        return BXMembers
    
    def fillTimeSpanFSR(self):
        '''
        fill the time span FSR, return a sequence
        it looks at the time stamp of events with the LumiTrigger
        '''
        from Configurables import ( TimeAccounting,
                                    GaudiSequencer )
        from Configurables import LoKi__ODINFilter  as ODINFilter
        # Create sub-sequence
        seqMembers=[]
        seqMembers.append( ODINFilter ( 'TimeSpanFilter',
                                        Code = ' ( ODIN_TRGTYP == LHCb.ODIN.LumiTrigger ) ' ))
        accounting = TimeAccounting('TimeSpanAccounting', OutputLevel = self.getProp("OutputLevel") )
        seqMembers.append( accounting )
        seq =  GaudiSequencer('TimeSpanSeq', 
                              Members = seqMembers,
                              ModeOR = False,
                              ShortCircuit = True
                              )
        
        return [seq]
    
    def fillEventFSR(self,status=""):
        '''fill the EventAccounting'''
        from Configurables import (EventAccounting, GaudiSequencer)
        accounting = EventAccounting('EventAccount', OutputLevel = self.getProp("OutputLevel") )
        if status is not None and status != "":
            accounting.DefaultStatus=status
            accounting.OverrideStatus=True
        return [accounting]
    
    def touchFSR(self):
        '''make sure the FSR is touched and readable'''
        from Configurables import createODIN
        odin=createODIN()
        from Configurables import (LumiIntegrateFSR)
        readingFSR=LumiIntegrateFSR("TouchLumiFSR", OutputLevel = self.getProp("OutputLevel") )
        # this also touches the LumiLow FSRs
        readingLumiLowFSR=LumiIntegrateFSR("TouchLumiLowFSR",
                                           FSRName = "/LumiLowFSR",
                                           IntegratorToolName = "LumiLowIntegrator", 
                                           OutputLevel = self.getProp("OutputLevel") )
        return [ odin, readingFSR, readingLumiLowFSR ]

    def mergeFSR(self):
        ''' create merge sequence '''
        # Input data type - should not be a raw type
        seqMembers=[]
        seqMembers.append( LumiMergeFSR('MergeFSR'))
        #remove empty FSR directories, save the 7GB memory usage!
        seqMembers.append( FSRCleaner())

        return seqMembers
    
    ## Apply the configuration to the given sequence
    def __apply_configuration__(self):
        ''' Apply the configuration to the given sequence '''
        
        sequence = self.getProp("LumiSequencer")
        if sequence == None : raise RuntimeError("ERROR : Lumi Sequencer not set")
        
        # Set context
        context = self.getProp("Context")
        
        # Check input data type
        dsttype = ('---' + self.getProp('InputType'))[-3:] == 'DST'
        inputType = self.getProp("InputType")
        if (not dsttype) and (inputType not in [ "MDF", "DIGI", "ETC"]):
            raise RuntimeError("ERROR : Unknown input type " + str(inputType))
        
        forced=self.getProp("ForceFullSequence")
        status=self.getProp("SetFSRStatus")
        merge=self.getProp("MergeFSR")
        
        if inputType == 'MDF' or forced:
            # create the FSRs
            
            # Create sub-sequences according to BXTypes
            sequence.Members += self.fillFSR()
            
            # add other method LumiSummaries - deprecated 
            if self.getProp('UseLumiLow'): 
                sequence.Members += self.fillLowLumiFSR()
            
            if status is None or status=='':
                #by definition, all raw files are verified
                status='VERIFIED'
        
            # create the TimeSpan FSR
            sequence.Members += self.fillTimeSpanFSR()
        
        # create the Event FSR
        EvMembers=self.fillEventFSR(status)
        sequence.Members += EvMembers
        
        # actions on DSTs only
        if inputType != 'MDF':
	    # touching is no longer needed
            # Touch all FSRs so they are always copied
            #TouchMembers=self.touchFSR()
            #sequence.Members += TouchMembers
            if merge:
                # merge FSRs (typically done in MERGING and MDST production)
                # must be done after Event accounting
                sequence.Members += self.mergeFSR()
        
        #finally configure the sequence
        sequence.ModeOR = True
        sequence.ShortCircuit = False
        sequence.IgnoreFilterPassed = True
        
        # Switch off LoKi banner
        from Configurables import LoKiSvc
        LoKiSvc().Welcome = False
