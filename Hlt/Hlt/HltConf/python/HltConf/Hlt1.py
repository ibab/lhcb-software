#!/usr/bin/env gaudirun.py
# =============================================================================
# $Id: Hlt1.py,v 1.26 2009-08-28 11:45:06 graven Exp $
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
__version__ = "CVS Tag $Name: not supported by cvs2svn $, $Revision: 1.26 $"
# =============================================================================

from Gaudi.Configuration import * 
from LHCbKernel.Configuration import *
from Hlt1Lines.HltCommissioningLines  import HltCommissioningLinesConf
from Hlt1Lines.HltLumiLines     import HltLumiLinesConf
from Hlt1Lines.HltBeamGasLines  import HltBeamGasLinesConf
from Hlt1Lines.HltL0Lines       import HltL0LinesConf
from Hlt1Lines.HltVeloLines     import HltVeloLinesConf
from Hlt1Lines.HltExpressLines  import HltExpressLinesConf
from Hlt1Lines.HltMuonLines     import HltMuonLinesConf
from Hlt1Lines.HltHadronLines   import HltHadronLinesConf
from Hlt1Lines.HltPhotonLines   import HltPhotonLinesConf
from Hlt1Lines.HltElectronLines import HltElectronLinesConf

global _type2conf
_type2conf = { 'PA' : HltCommissioningLinesConf # PA for 'PAss-thru' (PT was considered bad)
             , 'LU' : HltLumiLinesConf
             , 'BG' : HltBeamGasLinesConf
             , 'L0' : HltL0LinesConf
             , 'VE' : HltVeloLinesConf
             , 'XP' : HltExpressLinesConf
             , 'MU' : HltMuonLinesConf
             , 'HA' : HltHadronLinesConf
             , 'PH' : HltPhotonLinesConf
             , 'EL' : HltElectronLinesConf
             }

def hlt1TypeDecoder(hlttype) :
      trans = { 'Hlt1' : 'PA+LU+L0+BG+VE+XP+MU+HA+PH+EL'  }
      for short,full in trans.iteritems() : hlttype = hlttype.replace(short,full)
      # split hlttype in known and unknown bits
      known   = [ i for i in hlttype.split('+') if i in _type2conf.keys() ]
      unknown = [ i for i in hlttype.split('+') if i not in known ]
      return ( '+'.join(known), '+'.join(unknown) )



class Hlt1Conf(LHCbConfigurableUser):
   __used_configurables__ = []
   global _type2conf
   for (k,v) in _type2conf.iteritems() : __used_configurables__.append( v )

   __slots__ = { "Hlt1Type"                     : ''  # Explicitly set by HltConf.Configuration
               , "ThresholdSettings"            : {} # dictionary decoded in HltThresholdSettings
               }

   def confType(self) :
      """
      Hlt1 configuration
      """
      hlt1type          = self.getProp("Hlt1Type")
      ThresholdSettings = self.getProp("ThresholdSettings")
      
      for i in hlt1type.split('+') :
         if i == '' : continue
         if _type2conf[i] not in self.__used_configurables__ : raise AttributeError, "configurable for '%s' not in list of used configurables"%i
         log.info( '# requested ' + i + ', importing ' + str( _type2conf[i])  )
         # FIXME: warning: the next is 'brittle': if someone outside 
         #        does eg. HltMuonLinesConf(), it will get activated
         #        regardless of whether we do it over here...
         #        So anyone configuring some part explictly will _always_ get
         #        that part of the Hlt run, even if it does not appear in HltType...
         from ThresholdUtils import setThresholds
         setThresholds(ThresholdSettings,_type2conf[i])

                  
##################################################################################
#
#      
   def __apply_configuration__(self):
      from Configurables       import GaudiSequencer as Sequence
      self.confType() 
         
      # add a few thing to our printout
      from HltLine.HltLine     import addHlt1Prop
      addHlt1Prop([ 'RoutingBits', 'Accept', 'FilterDescriptor'
                  , 'Code', 'InputLocations'
                  , 'DaughtersCuts', 'CombinationCut', 'MotherCut', 'DecayDescriptor'
                  , 'OutputSelection','Context' ])
      
      importOptions('$HLTCONFROOT/options/HltInit.py')
      
      ## finally, define the Hlt1 sequence!!
      Sequence('Hlt1',  ModeOR = True, ShortCircuit = False )

      from HltLine.HltLine     import Hlt1Line
      Hlt1Line( 'Global', HLT= "HLT_PASS_SUBSTR('Hlt1') ", priority = 255 ) 
