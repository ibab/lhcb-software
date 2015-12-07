# options to run mdf reader for lumi reader
__version__ = "$Id: LumiReaderConf.py,v 1.4 2010-01-08 17:10:11 panmanj Exp $"
__author__  = "Jaap Panman"

from os import environ, path

from Gaudi.Configuration import *
from LHCbKernel.Configuration import *

from Configurables import GaudiSequencer as Sequence
from Configurables import ( LHCbConfigurableUser, LHCbApp )

from Configurables import LoKi__ODINFilter  as ODINFilter
from Configurables import RawEventDump
from Configurables import FilterOnLumiSummary
from Configurables import LumiFileReader

import GaudiKernel.ProcessJobOptions

def _ext(name) : return path.splitext(name)[-1].lstrip('.')

def _file(f) :
    if f.lstrip().startswith('DATAFILE'): return f
    from GaudiConf.IOHelper import IOHelper
    if _ext(f).upper() in ["RAW","MDF"]:
        
        return IOHelper(Input="MDF").dressFile(f,IO="I")
    
    return IOHelper().dressFile(f,IO="I")

def _sequenceAppender( seq ) :
    return lambda x : seq.Members.append( x )

class LumiReaderConf(LHCbConfigurableUser):
  ## Possible used Configurables
  __used_configurables__ = [ LHCbApp,
                               ]

  __slots__ = {
    "EvtMax":             -1      # Maximum number of events to process
    , "SkipEvents":        0
    , "DataType":          '2010' # Data type
    , "outputFile" :       ''     # output filename
    , "inputFiles" :       [ ]    # input
    , "userAlgorithms":    [ ]    # put here user algorithms to add
    , "OutputLevel" :      ERROR  #
    , "Debug" :            False  #
    }   


  def _createReader(self, sequence):
    '''
    create reader sequence
    '''
    debugOPL = self.getProp("OutputLevel")
    debugging = self.getProp("Debug")
    readLumiSequence = _sequenceAppender( Sequence('readLumiSeq',
                                                    ModeOR = False,
                                                    ShortCircuit = True,
                                                    IgnoreFilterPassed = False,
                                                    MeasureTime = True,
                                                    OutputLevel = debugOPL  ) )
    from DAQSys.Decoders import DecoderDB
    from DAQSys.DecoderClass import decodersForBank
    # create ODIN by hand
    for d in decodersForBank(DecoderDB,"ODIN"):
        readLumiSequence( d.setup() )
    # select only the right Trigger Type
    readLumiSequence( ODINFilter ( 'OdinTriggerTypes',
                                   Code = ' ( ODIN_TRGTYP == LHCb.ODIN.LumiTrigger ) ' ))
    
    # decode lumi
    for d in decodersForBank(DecoderDB,"HltLumiSummary"):
        readLumiSequence( d.setup() ) 
    # add filter to check if this was L0 triggered
    filterRan = FilterOnLumiSummary('LumiRandomFilter', CounterName = "Random", ValueName = "RandomMethod")
    filterLow = FilterOnLumiSummary('LumiLowFilter', CounterName = "Method", ValueName = "L0RateMethod")
    readLumiSequence( Sequence('filterLumi',
                               Members = [filterRan, filterLow],
                               ModeOR = True,
                               ShortCircuit = True,
                               IgnoreFilterPassed = False,
                               MeasureTime = True )
                      )
    # read and decode input file ---
    readLumiSequence( LumiFileReader( OutputLevel = debugOPL ) ) 
    # verbose output
    if debugging:
        readLumiSequence( RawEventDump( 'InputDump', DumpData = True, OutputLevel = debugOPL ) )

    # and activate it
    sequence.Members+=[ Sequence('readLumiSeq') ]
 
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

  def __apply_configuration__(self):

    GaudiKernel.ProcessJobOptions.PrintOff()
    # not required now in LHCb App
    # EventPersistencySvc().CnvServices.append( 'LHCb::RawDataCnvSvc' )

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
    self._createReader(mainSeq)

    # output
    self._configureOutput()

    # Switch off LoKi banner
    from Configurables import LoKiSvc
    LoKiSvc().Welcome = False
