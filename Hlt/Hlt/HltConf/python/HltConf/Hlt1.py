#!/usr/bin/env gaudirun.py
# =============================================================================
# $Id: Hlt1.py,v 1.18 2009-07-20 17:19:52 pkoppenb Exp $
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
__version__ = "CVS Tag $Name: not supported by cvs2svn $, $Revision: 1.18 $"
# =============================================================================

from Gaudi.Configuration import * 
from LHCbKernel.Configuration import *
from HltLine.HltLine     import addHlt1Prop
from Hlt1Lines.HltCommissioningLines  import HltCommissioningLinesConf
from Hlt1Lines.HltVeloLines     import HltVeloLinesConf
from Hlt1Lines.HltL0Lines       import HltL0LinesConf
from Hlt1Lines.HltLumiLines     import HltLumiLinesConf
from Hlt1Lines.HltMuonLines     import HltMuonLinesConf
from Hlt1Lines.HltHadronLines   import HltHadronLinesConf
from Hlt1Lines.HltElectronLines import HltElectronLinesConf
from Hlt1Lines.HltPhotonLines   import HltPhotonLinesConf
from Hlt1Lines.HltExpressLines  import HltExpressLinesConf
from Hlt1Lines.HltBeamGasLines  import HltBeamGasLinesConf

class Hlt1Conf(LHCbConfigurableUser):
   __used_configurables__ = [ # Hlt1 Lines
                               HltCommissioningLinesConf
                             , HltVeloLinesConf
                             , HltLumiLinesConf
                             , HltBeamGasLinesConf
                             , HltL0LinesConf
                             , HltMuonLinesConf
                             , HltHadronLinesConf
                             , HltElectronLinesConf
                             , HltPhotonLinesConf
                             , HltExpressLinesConf ]

   __slots__ = {"HltType"                      : 'Hlt1+Hlt2'  # can be PA as well, Hlt1 part irrelevant
                , "LumiBankKillerAcceptFraction" : 0.0 # fraction of lumi-only events where raw event is stripped down; online: 0.9999
                , "ThresholdSettings"            : {} # dictionary decoded in HltThresholdSettings
                }

   def confType(self) :
      """
      Hlt1 configuration
      """
      hlttype           = self.getProp("HltType")
      ThresholdSettings = self.getProp("ThresholdSettings")
      
      trans = { 'Hlt1'   : 'LU+L0+VE+XP+MU+HA+PH+EL'  
              , 'DEFAULT': 'PA+LU+L0+VE+XP'
                }
      for short,full in trans.iteritems() : hlttype = hlttype.replace(short,full)
      type2conf = { 'PA' : HltCommissioningLinesConf # PA for 'PAss-thru' (PT was considered bad)
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
      
      for i in hlttype.split('+') :
         if i == 'NONE' : continue # no operation...
         if i == 'Hlt2' : continue # we deal with this later...
         if i not in type2conf : raise AttributeError, "unknown HltType fragment '%s'"%i
         if type2conf[i] not in self.__used_configurables__ : raise AttributeError, "configurable for '%s' not in list of used configurables"%i
         log.info( '# requested ' + i + ', importing ' + str(type2conf[i])  )
         # FIXME: warning: the next is 'brittle': if someone outside 
         #        does eg. HltMuonLinesConf(), it will get activated
         #        regardless of whether we do it over here...
         #        So anyone configuring some part explictly will _always_ get
         #        that part of the Hlt run, even if it does not appear in HltType...
         conf = type2conf[i]()
         if ThresholdSettings and i in ThresholdSettings : 
            for (k,v) in ThresholdSettings[i].iteritems() :
               # configurables have an exception for list and dict: 
               #   even if not explicitly set, if you ask for them, you get one...
               #   this is done to make foo().somelist += ... work.
               # hence we _assume_ that, even if we have an attr, but it matches the
               # default, it wasn't set explicitly, and we overrule it...
               if hasattr(conf,k) and conf.getProp(k) != conf.getDefaultProperty(k) :
                  log.warning('# WARNING: %s.%s has explictly been set, NOT using requested predefined threshold %s, but keeping explicit value: %s '%(conf.name(),k,str(v),getattr(conf,k)))
               else :
                  setattr(conf,k,v)

                  
##################################################################################
#
#      
   def __apply_configuration__(self):
      from Configurables       import GaudiSequencer as Sequence
      self.confType() 
         
      # add a few thing to our printout
      addHlt1Prop([ 'RoutingBits', 'Accept', 'FilterDescriptor'
                    , 'Code', 'InputLocations'
                    , 'DaughtersCuts', 'CombinationCut', 'MotherCut', 'DecayDescriptor'
                    , 'OutputSelection','Context' ])
      
      importOptions('$HLTCONFROOT/options/HltInit.py')
      
      ## finally, define the Hlt1 sequence!!
      Sequence('Hlt1',  ModeOR = True, ShortCircuit = False )
