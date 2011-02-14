#!/usr/bin/env gaudirun.py
# =============================================================================
## @file
#  Configuration of Photon Lines
#  @author Gerhard Raven Gerhard.Raven@nikhef.nl
#  @date 2008-08-25
# =============================================================================
'''
 script to configure Photon trigger lines
'''
# =============================================================================
__author__  = 'Gerhard Raven Gerhard.Raven@nikhef.nl'
__version__ = 'CVS Tag $Name: not supported by cvs2svn $, $Revision: 1.12 $'
# =============================================================================


from HltLine.HltLinesConfigurableUser import *

#//---------------------------
#// HLT Photon Alley
#//--------------------------


class Hlt1PhotonLinesConf(HltLinesConfigurableUser):
   __slots__ = { 'Pho_IsPho'         : -0.1   
               , 'Pho_EtCut'         : 2800.
               , 'Track_PtCut'       : 1250.
               , 'Track_IPCut'       : 0.15
               , 'DiTrack_PtCut'     : 650.  
               }

   def __apply_configuration__(self):
        pass
