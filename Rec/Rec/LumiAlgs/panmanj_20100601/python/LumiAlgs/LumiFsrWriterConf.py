# options to run mdf writer for lumi writer
__version__ = "$Id: LumiFsrWriterConf.py,v 1.1 2010-01-08 17:10:11 panmanj Exp $"
__author__  = "Jaap Panman"


from os import environ, path

from Gaudi.Configuration import *
from GaudiConf.Configuration import *
from LHCbKernel.Configuration import *

from Configurables import LHCb__MDFWriter as MDFWriter
from Configurables import GaudiSequencer as Sequence
from Configurables import ( LHCbConfigurableUser, LHCbApp )

from Configurables import createODIN
from Configurables import bankKiller 
from Configurables import FileIdBankWriter

from LumiAlgs.Configuration import LumiAlgsConf


import GaudiKernel.ProcessJobOptions

def _ext(name) : return path.splitext(name)[-1].lstrip('.')

def _file(f) :
    extensions = { 'RAW' : "' SVC='LHCb::MDFSelector'",
                   'MDF' : "' SVC='LHCb::MDFSelector'",
                   'DST' : "' TYP='POOL_ROOTTREE' OPT='READ'" }
    if f.lstrip().startswith('DATAFILE'): return f
    else: return "DATAFILE='PFN:"+ f + extensions[ _ext(f).upper() ] 

def _sequenceAppender( seq ) :
    return lambda x : seq.Members.append( x )

class LumiFsrWriterConf(LHCbConfigurableUser):
  ## Possible used Configurables
  __used_configurables__ = [ LHCbApp,
                             LumiAlgsConf,
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
    }   


  def _createWriter(self, sequence):
    '''
    create writer sequence
    '''
    writeLumiSequence = _sequenceAppender( Sequence('writeLumiSeq',
                                                    ModeOR = False,
                                                    ShortCircuit = True,
                                                    IgnoreFilterPassed = False,
                                                    MeasureTime = True,
                                                    OutputLevel = self.getProp("OutputLevel") ) )
        
    # create ODIN by hand
    writeLumiSequence( createODIN ('createODIN') )

    # kill non-lumi banks to make output small
    if self.getProp('KillBanks') :
      writeLumiSequence(
        bankKiller( 'KillAll', BankTypes=[ 'ODIN','HltLumiSummary','DAQ' ],  DefaultIsKill=True )
        )
    # tag input file ---
    writeLumiSequence( FileIdBankWriter( OutputLevel = self.getProp("OutputLevel") ) ) 

    # and activate it
    sequence.Members+=[ Sequence('writeLumiSeq') ]

    # create lumi sequence
    lumiFsrSeq = GaudiSequencer("LumiFsrSeq", OutputLevel = INFO )
    LumiAlgsConf().LumiSequencer = lumiFsrSeq
    sequence.Members+=[ lumiFsrSeq ]


  def _configureInput(self):
    files = self.getProp('inputFiles')
    if len(files) > 0 :
        EventSelector().Input = [ _file(f) for f in files ]

  def _configureOutput(self):
    # first empty the outstream, because it would write all the time
    ApplicationMgr().OutStream = [ ]
    # only configure the writer - use it where applicable
    outputFile = self.getProp('outputFile')
    if not outputFile : return

    # POOL Persistency
    importOptions("$GAUDIPOOLDBROOT/options/GaudiPoolDbRoot.opts")
    # event output
    from Configurables import OutputStream
    writerName = "DstWriter"
    dstWriter = OutputStream( writerName,
                              ItemList = [ "/Event#999" ],     # miniDST selection: #1
                              Output   = "DATAFILE='PFN:" + outputFile + "' TYP='POOL_ROOTTREE' OPT='REC'",
                              )
    ApplicationMgr().OutStream.append(dstWriter)

    # TES setup
    FileRecordDataSvc().ForceLeaves         = True
    FileRecordDataSvc().RootCLID            = 1
    FileRecordDataSvc().PersistencySvc      = "PersistencySvc/FileRecordPersistencySvc"

    # Persistency service setup
    ApplicationMgr().ExtSvc += [ PoolDbCnvSvc("FileRecordCnvSvc",
                                              DbType = "POOL_ROOTTREE",
                                              ShareFiles = "YES"
                                              )
                                 ]

    # FSR output stream
    from Configurables import RecordStream
    fsrWriter = RecordStream( "FsrWriter",
                              ItemList = [ "/FileRecords#999" ],
                              EvtDataSvc = "FileRecordDataSvc",
                              EvtConversionSvc = "FileRecordPersistencySvc",
                              )
    fsrWriter.Output = dstWriter.getProp("Output")
    ApplicationMgr().OutStream.append(fsrWriter)


  def __apply_configuration__(self):

    GaudiKernel.ProcessJobOptions.PrintOff()
    EventPersistencySvc().CnvServices.append( 'LHCb::RawDataCnvSvc' )

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
