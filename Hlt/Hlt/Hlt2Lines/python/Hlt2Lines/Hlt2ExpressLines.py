# =============================================================================
# $Id: Hlt2ExpressLines.py,v 1.1 2009-12-03 13:52:00 albrecht Exp $
# =============================================================================
## @file
#  Configuration of Hlt2 Lines for the express stream
#  @author Johannes Albrecht albrecht@cern.ch
#  @date 2009-12-02
# =============================================================================
"""
 script to configure Hlt2 Express stream
"""
# =============================================================================
__author__  = "Johannes Albrecht albrecht@cern.ch"
__version__ = "CVS Tag $Name: not supported by cvs2svn $, $Revision: 1.1 $"
# =============================================================================

from HltLine.HltLinesConfigurableUser import *


class Hlt2ExpressLinesConf(HltLinesConfigurableUser):
   
   __slots__ = { 'Prescale' : { 'Hlt2ExpressJPsi'  : 0.02 
                                , 'Hlt2BeamHalo'      : 1.
                                }
                 ,'ExpressJPsiMassWindow'        :  120   # MeV
                 ,'ExpressJPsiPt'                : 1000   # MeV
                 ,'ExpressJPsiMuPt'              :  500   # MeV
                 
                 }
   def __apply_configuration__(self):
      from HltLine.HltLine import Hlt2Line, Hlt2Member, bindMembers
      from HltLine.HltReco import PV3D
      from Configurables import HltANNSvc
      from Hlt2SharedParticles.DiMuon import DiMuon
      from Configurables import FilterDesktop
      '''
      Comment content of express stream here
      '''
      
      #--------------------------------------------
      '''
      unbiased JPsi - prescaled to ~5 Hz
      '''
      
      filter = Hlt2Member(   FilterDesktop 
                             , "Filter"
                             , Code = "(MM>%(ExpressJPsiMassWindow)d*MeV) & (PT>%(ExpressJPsiPt)d*MeV)"\
                             " & (MINTREE('mu-'==ABSID,PT)>%(ExpressJPsiMuPt)d*MeV) " %  self.getProps() 
                             , InputLocations  = [ DiMuon ]
                             )
      
      line = Hlt2Line('ExpressJPsi'
                      , prescale = self.prescale 
                      , algos = [ DiMuon, filter ]
                      , postscale = self.postscale
                      )
      #--------------------------------------------
      '''
      selection of beam halo tracks
      '''
      from Configurables import Tf__PatVeloAlignTrackFilter as PatVeloAlignTrackFilter
      from HltLine.HltDecodeRaw import DecodeVELO

      Hlt2Line('BeamHalo'
               ,prescale = self.prescale
               , algos = [ DecodeVELO
                           , PatVeloAlignTrackFilter('Hlt2BeamHaloDecision') 
                           ]
               , postscale = self.postscale
            )

      #--------------------------------------------      


      HltANNSvc().Hlt2SelectionID.update( { "Hlt2ExpressJPsiDecision"   : 50090
                                            ,"Hlt2BeamHaloDecision"     : 50091
                                            } )

