#!/usr/bin/env gaudirun.py
# =============================================================================
# $Id: Hlt1.py,v 1.44 2010-08-22 22:50:46 gligorov Exp $
# =============================================================================
## @file
#  Configuration of HLT1
#  @author Gerhard Raven Gerhard.Raven@nikhef.nl
#  @date 2008-08-25
# =============================================================================
"""
 script to configure HLT1 trigger lines
 Note: the actual line definitions are NOT in this file,
 this script relies on them being available through hlt1Lines()
"""
# =============================================================================
__author__  = "Gerhard Raven Gerhard.Raven@nikhef.nl"
__version__ = "CVS Tag $Name: not supported by cvs2svn $, $Revision: 1.44 $"
# =============================================================================

from Gaudi.Configuration import *
from LHCbKernel.Configuration import *
from ThresholdUtils import importLineConfigurables

#import all Hlt1 lines configurables  -- and make sure the are kept alive long enough!
import Hlt1Lines
_hlt1linesconfs = importLineConfigurables(Hlt1Lines, endsWithLines = True)
# add them in our scope so that genConfUser can find it...
globals().update( ( cfg.__name__, cfg ) for cfg in _hlt1linesconfs )


class Hlt1Conf(LHCbConfigurableUser):
   __used_configurables__ = _hlt1linesconfs

   __slots__ = { "ThresholdSettings"            : {} # dictionary decoded in HltThresholdSettings
               }

##################################################################################
#
#
   def __apply_configuration__(self):
      ## Apply ThresholdSettings to HLT1 lines configurables
      from ThresholdUtils import setThresholds
      from functools import partial
      map( partial( setThresholds, self.getProp("ThresholdSettings") ), _hlt1linesconfs )

      ## Create Hlt1Global
      from HltLine.HltLine     import Hlt1Line
      from Configurables import LoKi__HDRFilter   as HDRFilter
      from DAQSys.Decoders import DecoderDB
      decoder = DecoderDB["HltDecReportsDecoder/Hlt1DecReportsDecoder"]
      Hlt1Line( 'Global', priority = 255
                 , algos = [HDRFilter('Hlt1GlobalFilter' ,
                            Code = "HLT_PASS_SUBSTR('Hlt1') ",
                            Location = decoder.listOutputs()[0])]
              )

      # add a few thing to our printout
      def __counter(n):
         m = 3
         while m <= n:
            yield ''.join(str(i) for i in range(1, m))
            m += 1

      from HltLine.HltLine import addHlt1Prop
      addHlt1Prop([ 'RoutingBits', 'Accept', 'FilterDescriptor'
                  , 'Code', 'Preambulo', 'InputLocations', 'Input','Inputs', 'Output'
                  , 'OutputProtoParticleLocation','InputTrackLocation'
                  , 'DaughtersCuts', 'CombinationCut', 'MotherCut', 'DecayDescriptor'
                  , 'OutputSelection','Context', 'TisTosSpecs' ] +
                  [ 'Combination%sCut' % s for s in __counter(8) ])

      ## finally, define the Hlt1 sequence!!
      from Configurables import GaudiSequencer as Sequence
      Sequence('Hlt1',  ModeOR = True, ShortCircuit = False )
