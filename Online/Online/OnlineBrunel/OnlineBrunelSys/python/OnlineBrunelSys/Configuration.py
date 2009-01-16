"""
      High level configuration tools for LHCb applications
"""
__version__ = "$Id: Configuration.py,v 1.2 2009-01-16 14:26:10 frankb Exp $"
__author__  = "Markus Frank <Markus.Frank@cern.ch>"

from Gaudi.Configuration import *

class Serialisation(ConfigurableUser):
    __slots__ = {
      'Writer'        : 'Writer',
      'Buffer'        : 'Output',
      'Location'      : '/Event/GaudiSerialize',
      'OutputLevel'   : 4
      }

    _propertyDocDct = { 
      'Writer'        : """ Name of OutputStream writing the DST """,
      'Buffer'        : """ Output buffer name """,
      'Location'      : """ Location of serialization banks in TES """,
      'OutputLevel'   : """ Printout level of output stream """
      }
    
    def _doWrite( self ):
      """
      Define the file content and write it out
      """
      import OnlineEnv as Online
      app = ApplicationMgr()
      nam = self.getProp("Writer")
      loc = self.getProp("Location")
      buf = self.getProp("Buffer")
      lvl = self.getProp("OutputLevel")
      # First update data of the standard DST writer
      serial = Online.serialWriter(name='DstWriter',location=loc)
      serial.RequireAlgs += ['Reco']
      serial.OutputLevel = lvl
      # Now append
      writer = Online.evtMerger(buffer=buf,name=nam,location=loc,routing=0x10)
      app.OutStream.append(writer)

    def __apply_configuration__(self):
        self._doWrite()
