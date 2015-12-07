# options to run mdf writer for lumi writer
__version__ = "$Id: LumiWriterConf.py,v 1.4 2010-01-08 17:10:11 panmanj Exp $"
__author__  = "Jaap Panman"


from os import environ, path

from Gaudi.Configuration import *
from LHCbKernel.Configuration import *

from Configurables import LHCb__MDFWriter as MDFWriter
from Configurables import GaudiSequencer as Sequence
from Configurables import ( LHCbConfigurableUser, LHCbApp )

from Configurables import LoKi__ODINFilter  as ODINFilter
from Configurables import RawEventDump
from Configurables import bankKiller 
from Configurables import FileIdBankWriter
from Configurables import FilterOnLumiSummary


import GaudiKernel.ProcessJobOptions

def _ext(name) : return path.splitext(name)[-1].lstrip('.')

def _file(f) :
    if f.lstrip().startswith('DATAFILE'): return f
    from GaudiConf.IOHelper import IOHelper
    
    if _ext(f).upper()  in ["RAW","MDF"]:
        
        return IOHelper(Input="MDF").dressFile(f,IO="I")
    
    return IOHelper().dressFile(f,IO="I")

def _sequenceAppender( seq ) :
    return lambda x : seq.Members.append( x )

class LumiWriterConf(LHCbConfigurableUser):
  ## Possible used Configurables
  __used_configurables__ = [ LHCbApp,
                               ]

  __slots__ = {
    "EvtMax":             -1      # Maximum number of events to process
    , "SkipEvents":        0
    , "DataType":          '2010' # Data type
    , "outputFile" :       ''     # output filename
    , "inputFiles" :       [ ]    # input
    , "KillBanks" :        True   # whether to kill non-lumi banks
    , "userAlgorithms":    [ ]    # put here user algorithms to add
    , "OutputLevel" :      ERROR  #
    , "Debug" :            False  #
    }   


  def _createWriter(self, sequence):
    '''
    create writer sequence
    '''
    debugOPL = self.getProp("OutputLevel")
    debugging = self.getProp("Debug")
    writeLumiSequence = _sequenceAppender( Sequence('writeLumiSeq',
                                                    ModeOR = False,
                                                    ShortCircuit = True,
                                                    IgnoreFilterPassed = False,
                                                    MeasureTime = True,
                                                    OutputLevel = debugOPL  ) )
    from DAQSys.Decoders import DecoderDB
    from DAQSys.DecoderClass import decodersForBank
    # create ODIN by hand
    for d in decodersForBank(DecoderDB,"ODIN"):
        writeLumiSequence( d.setup() )
    # verbose output
    if debugging:
      writeLumiSequence( RawEventDump( 'InputDump', DumpData = False, OutputLevel = debugOPL ) )
      pass
      
    # select only the right Trigger Type
    writeLumiSequence( ODINFilter ( 'OdinTriggerTypes',
                                    Code = ' ( ODIN_TRGTYP == LHCb.ODIN.LumiTrigger ) ' ))

    ## Lumi only triggers 
    # decode summary data
    for d in decodersForBank(DecoderDB,"HltLumiSummary"):
        writeLumiSequence( d.setup() ) 
    # add filter to check if this was L0 triggered
    filterRan = FilterOnLumiSummary('LumiRandomFilter', CounterName = "Random", ValueName = "RandomMethod")
    #filterLow = FilterOnLumiSummary('LumiLowFilter', CounterName = "Method", ValueName = "L0RateMethod")
    writeLumiSequence( Sequence('filterLumi', 
                                Members = [filterRan], #, filterLow],  
                                ModeOR = True,
                                ShortCircuit = True,
                                IgnoreFilterPassed = False,
                                MeasureTime = True )
                       )
    # kill non-lumi banks
    if self.getProp('KillBanks') :
      writeLumiSequence(
        bankKiller( 'KillAll', BankTypes=[ 'ODIN','HltLumiSummary','HltRoutingBits','DAQ' ],  DefaultIsKill=True )
        )
    # tag input file ---
    writeLumiSequence( FileIdBankWriter( OutputLevel = debugOPL ) ) 
    # verbose output
    if debugging:
        writeLumiSequence( RawEventDump( 'OutputDump', DumpData = True, OutputLevel = debugOPL ) )

    writeLumiSequence( MDFWriter( "MDFWriter" ) )  # configure somewhere else

    # and activate it
    sequence.Members+=[ Sequence('writeLumiSeq') ]

  def _configureInput(self):
    files = self.getProp('inputFiles')
    if len(files) > 0 :
        EventSelector().Input = [ _file(f) for f in files ]

  def _configureOutput(self):
    # first empty the outstream, because it would write all the time
    ApplicationMgr().OutStream = [ ]
    # only configure the writer - use it where applicable
    fname = self.getProp('outputFile')
    if not fname : return
    if _ext(fname).upper() not in [ 'MDF','RAW' ] :
      raise NameError('unsupported filetype for file "%s"'%fname)
    writer = MDFWriter( 'MDFWriter'
                        , Connection = 'file://' + fname
                        , OutputLevel = WARNING
                        )

  def __apply_configuration__(self):

    GaudiKernel.ProcessJobOptions.PrintOff()
    #done by LHCbApp
    #EventPersistencySvc().CnvServices.append( 'LHCb::RawDataCnvSvc' )

    # forward some settings...
    self.setOtherProps( LHCbApp(), ['EvtMax','SkipEvents','DataType'] )

    # instantiate the sequencer
    mainSeq = GaudiSequencer("LumiSeq")

    # activate the sequence
    appMgr=ApplicationMgr(TopAlg=[],
                          HistogramPersistency='NONE',
                          )
    ApplicationMgr().TopAlg += [ mainSeq ]
    
    # add user algorithms at the end
    if self.getProp("userAlgorithms"):
      for userAlg in self.getProp("userAlgorithms"):
        ApplicationMgr().TopAlg += [ userAlg ]
    
    # input
    self._configureInput()
    
    # configure the main sequence
    self._createWriter(mainSeq)
    
    # output
    self._configureOutput()

    # Switch off LoKi banner
    from Configurables import LoKiSvc
    LoKiSvc().Welcome = False
