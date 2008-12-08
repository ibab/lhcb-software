#!/usr/bin/env gaudirun.py
# =============================================================================
# $Id: HltL0Lines.py,v 1.4 2008-12-08 12:30:03 graven Exp $
# =============================================================================
## @file
#  Configuration of Hlt Lines which are plain L0 lines
#  @author Gerhard Raven Gerhard.Raven@nikhef.nl
#  @date 2008-12-02
# =============================================================================
"""
 script to configure Hlt lines corresponding to L0 triggers
"""
# =============================================================================
__author__  = "Gerhard Raven Gerhard.Raven@nikhef.nl"
__version__ = "CVS Tag $Name: not supported by cvs2svn $, $Revision: 1.4 $"
# =============================================================================


from HltConf.HltLine import Hlt1Line   as Line
from HltConf.HltLine import Hlt1Member as Member
from Configurables import HltL0CaloCandidates, HltL0MuonCandidates, HadronSeedTool

l0calolines = [ 'Electron','Photon','Hadron' ,'LocalPi0','GlobalPi0' ]

calomaker = {  'Hadron'   : HadronSeedTool( decodeCalos = False )
            ,  'Electron' : 'EcalSeedForVeloMatch' }

for i in l0calolines: 
    Line ( 'L0' + i 
       , L0DU  = "L0_CHANNEL('"+i+"')"
       , postscale = 0.000001
       , algos = [ Member('L0Calo','Decision'
                         , InputSelection =  HltL0CaloCandidates().getDefaultProperties()['InputSelection']
                         , OutputSelection = '%Decision'
                         , L0Channel = i
                         , CaloMakerTool = ( '' if i not in calomaker else calomaker[i] )
                         ) ]
       )

l0muonlines = [ 'Muon','DiMuon','MuonNoGlob' ]

for i in l0muonlines: 
    Line ( 'L0' + i 
       , L0DU  = "L0_CHANNEL('"+i+"')"
       , postscale = 0.000001
       , algos = [ Member('L0Muon','Decision'
                                   , InputSelection =  HltL0MuonCandidates().getDefaultProperties()['InputSelection']
                                   , L0Channel = i
                                   , OutputSelection = '%Decision'
                                   ) ]
       )
