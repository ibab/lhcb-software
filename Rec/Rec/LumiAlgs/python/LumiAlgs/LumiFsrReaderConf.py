# options to run mdf writer for lumi writer
__version__ = "$Id: LumiFsrReaderConf.py,v 1.1 2010-01-08 17:10:11 panmanj Exp $"
__author__  = "Jaap Panman"


from os import environ, path

from Gaudi.Configuration import *
from LHCbKernel.Configuration import *

from Configurables import GaudiSequencer as Sequence
from Configurables import ( LHCbConfigurableUser, LHCbApp )

from Configurables import DumpFSR

import GaudiKernel.ProcessJobOptions


def _hasMDF(files):
    from GaudiConf import IOHelper
    for f in files:
        f=IOHelper().undressFile(f)
        if _ext(f).upper()  in ["RAW","MDF"]:
            return True
    return False

def _ext(name) : return path.splitext(name)[-1].lstrip('.')

def _sequenceAppender( seq ) :
    return lambda x : seq.Members.append( x )

class LumiFsrReaderConf(LHCbConfigurableUser):
  ## Possible used Configurables
  __used_configurables__ = [ LHCbApp,
                             ]

  __slots__ = {
    "EvtMax":             -1      # Maximum number of events to process
    , "SkipEvents":        0
    , "DataType":          '2010' # Data type
    , "outputFile" :       ''     # output filename
    , "inputFiles" :       [ ]    # input
    , "asciiFile"  :       ''     # ascii output filename for FSR dump
    , "DumpRequests"  :    'F'    # ascii output filename for FSR dump
    , "userAlgorithms":    [ ]    # put here user algorithms to add
    , "OutputLevel" :      ERROR  #
    , "Persistency" :      None   # change the default persistency for testing
    }   
  
  
  def _createReader(self, sequence):
    '''
    create reader sequence
    '''
    # dump FSR
    dump =  DumpFSR ('dumpFSR', AsciiFileName = self.getProp('asciiFile'),
                     DumpRequests = self.getProp('DumpRequests'),
                     OutputLevel = self.getProp("OutputLevel") )
    sequence.Members+=[ dump ]
  
  def _configureInput(self):
    files = self.getProp('inputFiles')
    if not len(files): return
    
    persistency=None
    if hasattr( self, "Persistency" ):
        persistency=self.getProp("Persistency")
    
    from GaudiConf import IOHelper
    
    if _hasMDF(files):
        IOHelper(Input="MDF",Output=persistency).inputFiles(files,clear=True)
    else:
        IOHelper(persistency,persistency).inputFiles(files,clear=True)
    
  def __apply_configuration__(self):
    
    GaudiKernel.ProcessJobOptions.PrintOff()
    
    # forward some settings...
    self.setOtherProps( LHCbApp(), ['EvtMax','SkipEvents','DataType'] )
    if hasattr( self, "Persistency" ):
        self.setOtherProps( LHCbApp(), ['Persistency'])
    
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

