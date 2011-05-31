# options to run fsr merger
__version__ = "$Id: LumiMergeFSRConf.py,v 1.1 2010-10-07 16:08:33 panmanj Exp $"
__author__  = "Jaap Panman"

from os import environ, path

from Gaudi.Configuration import *
from GaudiConf.Configuration import *
from LHCbKernel.Configuration import *

from Configurables import GaudiSequencer as Sequence
from Configurables import LumiMergeFSR
from Configurables import EventAccounting

import GaudiKernel.ProcessJobOptions

def _hasMDF(files):
    from GaudiConf.IOHelper import IOHelper
    for f in files:
        f=IOHelper().undressFile(f)
        if _ext(f).upper()  in ["RAW","MDF"]:
            return True
    return False

def _ext(name) : return path.splitext(name)[-1].lstrip('.')

def _sequenceAppender( seq ) :
    return lambda x : seq.Members.append( x )

class LumiMergeFSRConf(LHCbConfigurableUser):
  ## Possible used Configurables
  
  __slots__ = {
      "InputType"     : "DST"      # Data type, can be ['MDF','DST'], works only on DST types
    , "LumiSequencer" : None       # The sequencer to add the Lumi Accounting to - essential input
    , "outputFile"    : ''         # output filename
    }   

  def _configureOutput(self):
    # first empty the outstream, because it would write all the time
    ApplicationMgr().OutStream = [ ]
    # only configure the writer - use it where applicable
    outputFile = self.getProp('outputFile')
    if not outputFile : return
    
    # POOL Persistency
    #no longer required, done by LHCbApp
    #importOptions("$GAUDIPOOLDBROOT/options/GaudiPoolDbRoot.opts")
    # event output
    from Configurables import OutputStream
    writerName = "DstWriter"
    dstWriter = OutputStream( writerName,
                              ItemList = [ "/Event#999" ])#,     # miniDST selection: #1
    #                          Output   = "DATAFILE='PFN:" + outputFile + "' TYP='POOL_ROOTTREE' OPT='REC'",
    #                          )
    #ApplicationMgr().OutStream.append(dstWriter)
    
    from GaudiConf.IOHelper import IOHelper
    
    IOHelper().outStream(filename=outputFile, writer=dstWriter)
    
    # TES setup
    # no longer required, done by LHCbApp
    #FileRecordDataSvc().ForceLeaves         = True
    #FileRecordDataSvc().RootCLID            = 1
    #FileRecordDataSvc().PersistencySvc      = "PersistencySvc/FileRecordPersistencySvc"
    
    # Persistency service setup
    # no longer required, done by LHCbApp
    #ApplicationMgr().ExtSvc += [ PoolDbCnvSvc("FileRecordCnvSvc",
    #                                          DbType = "POOL_ROOTTREE",
    #                                          ShareFiles = "YES"
    #                                          )
    #                             ]
    
    # FSR output stream
    #from Configurables import RecordStream
    #fsrWriter = RecordStream( "FsrWriter",
    #                          ItemList = [ "/FileRecords#999" ],           ## only toplevel!
    #                          EvtDataSvc = "FileRecordDataSvc",
    #                          EvtConversionSvc = "FileRecordPersistencySvc",
    #                          )
    #fsrWriter.Output = dstWriter.getProp("Output")
    #ApplicationMgr().OutStream.append(fsrWriter)
  
  def __apply_configuration__(self):
    '''
    create sequence
    '''
    sequence = self.getProp("LumiSequencer")
    if sequence == None : raise RuntimeError("ERROR : Lumi Sequencer not set")
    seqMembers=[]
    # Input data type - should not be a raw type
    if self.getProp("InputType") in ["MDF","RAW"]: return
    # event accounting
    seqMembers.append( EventAccounting('EventAccount', OutputLevel = INFO ) )
    # merge FSRs
    seqMembers.append( LumiMergeFSR('MergeFSR'))
    sequence.Members = seqMembers
    sequence.MeasureTime = True
    sequence.ModeOR = False
    sequence.ShortCircuit = True
    sequence.IgnoreFilterPassed = False

    # output
    self._configureOutput()
