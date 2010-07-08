# options to run mdf reader for lumi reader
__version__ = "$Id: LumiIntegratorConf.py,v 1.1 2009-11-12 16:08:33 panmanj Exp $"
__author__  = "Jaap Panman"

from os import environ, path

from Gaudi.Configuration import *
from GaudiConf.Configuration import *
from LHCbKernel.Configuration import *

from Configurables import GaudiSequencer as Sequence
from Configurables import ( LHCbConfigurableUser, LHCbApp )
from Configurables import LumiIntegrateFSR, LumiReadBackFSR
from Configurables import GetIntegratedLuminosity


import GaudiKernel.ProcessJobOptions

class LumiIntegratorConf(LHCbConfigurableUser):
  ## Possible used Configurables
  __used_configurables__ = [ LHCbApp,
                               ]

  __slots__ = {
      "InputType"     : "DST"      # Data type, can be ['MDF','DST'], works only on DST types
    , "LumiSequencer" : None       # The sequencer to add the Lumi Accounting to - essential input
    }   


  def __apply_configuration__(self):
    '''
    create reader sequence
    '''
    sequence = self.getProp("LumiSequencer")
    if sequence == None : raise RuntimeError("ERROR : Lumi Sequencer not set")
    # Input data type - should not be a raw type
    if self.getProp("InputType") in ["MDF","RAW"]: return

    # normalization of BeamCrossing
    seqMembers=[]
    seqMembers.append( LumiIntegrateFSR('IntegrateBeamCrossing',
                                        PrimaryBXType = 'BeamCrossing',
                                        AddBXTypes = ['NoBeam'],
                                        SubtractBXTypes = ['Beam1','Beam2'],
                                        IntegratorToolName = 'IntegrateBeamCrossing',
                                        ))
    seqMembers.append( GetIntegratedLuminosity('GetIntegratedLuminosity',
                                               IntegratorToolName = 'IntegrateBeamCrossing',
                                               WriteCountersDetails = False
                                        ))
    #seqMembers.append( LumiReadBackFSR('IntegrateReadBackBeamCrossing',
    #                                   IntegratorToolName = 'IntegrateBeamCrossing',
    #                                   ))
    sequence.Members = seqMembers
    sequence.MeasureTime = True
    sequence.ModeOR = False
    sequence.ShortCircuit = True
    sequence.IgnoreFilterPassed = False
