# options to run fsr merger
__version__ = "$Id: LumiMergeFSRConf.py,v 1.1 2010-10-07 16:08:33 panmanj Exp $"
__author__  = "Jaap Panman"

from os import environ, path

from Gaudi.Configuration import *
from LHCbKernel.Configuration import *

from Configurables import GaudiSequencer as Sequence
from Configurables import LumiMergeFSR, LHCbApp, FSRCleaner
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
    , "Persistency" :      None   # change the default persistency for testing
    }   
  
  def _configureOutput(self):
    # first empty the outstream, because it would write all the time
    ApplicationMgr().OutStream = [ ]
    # only configure the writer - use it where applicable
    outputFile = self.getProp('outputFile')
    if not outputFile : return
    
    from Configurables import OutputStream
    writerName = "DstWriter"
    dstWriter = OutputStream( writerName,
                              ItemList = [ "/Event#999" ])#,     # miniDST selection: #1

    
    persistency=None
    if hasattr( self, "Persistency" ):
        persistency=self.getProp("Persistency")
    
    
    from GaudiConf.IOHelper import IOHelper
    
    IOHelper(persistency,persistency).outStream(filename=outputFile, writer=dstWriter)
    
  
  def __apply_configuration__(self):
    '''
    create sequence
    '''
    sequence = self.getProp("LumiSequencer")
    if hasattr( self, "Persistency" ):
        self.setOtherProps( LHCbApp(), ['Persistency'])
    if sequence == None : raise RuntimeError("ERROR : Lumi Sequencer not set")
    seqMembers=[]
    # Input data type - should not be a raw type
    if self.getProp("InputType") in ["MDF","RAW"]: return
    # event accounting
    seqMembers.append( EventAccounting('EventAccount', OutputLevel = INFO ) )
    # merge FSRs
    seqMembers.append( LumiMergeFSR('MergeFSR'))
    seqMembers.append( FSRCleaner()) #clean empty FSR directories
    sequence.Members = seqMembers
    sequence.MeasureTime = True
    sequence.ModeOR = False
    sequence.ShortCircuit = True
    sequence.IgnoreFilterPassed = False
    
    # output
    self._configureOutput()
