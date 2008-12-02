#!/usr/bin/env gaudirun.py
# =============================================================================
# $Id: HltL0Lines.py,v 1.3 2008-12-02 12:15:17 graven Exp $
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
__version__ = "CVS Tag $Name: not supported by cvs2svn $, $Revision: 1.3 $"
# =============================================================================


from HltConf.HltLine import Hlt1Line   as Line
from HltConf.HltLine import Hlt1Member as Member
from Configurables import HltL0CaloPrepare, HltL0MuonPrepare, HadronSeedTool

l0calolines = [ 'Electron','Photon','Hadron' ,'Pi0Local','Pi0Global','SpdMult']

HadronSeedTool( decodeCalos = False )
calomaker = {  'Hadron'   : 'HadronSeedTool' 
            ,  'Electron' : 'EcalSeedForVeloMatch' }

##TODO: why do we have our own thresholds on these? 
##      why not just promote everything L0 has???
##      anybody using this can apply their own cuts
##      before using this anyway...
minEt = { 'Electron' : 2600.0
        , 'Photon' : 2300.0
        , 'Hadron' : 3500.0
        , 'Pi0Local' : 4300.0
        , 'Pi0Global' : 3700.0 
        }


for i in l0calolines: 
    Line ( 'L0' + i 
       , L0DU  = "L0_CHANNEL('"+i+"')"
       , postscale = 0.000001
       , algos = [ Member('L0CaloPrepare','Decision'
                         , InputSelection =  HltL0CaloPrepare().getDefaultProperties()['InputSelection']
                         , OutputSelection = '%Decision'
                         , CaloType = i
                         , CaloMakerTool = ( '' if i not in calomaker else calomaker[i] )
                         , MinEt = ( 0 if i not in minEt else minEt[i] )
                         ) ]
       )

l0muonlines = [ 'Muon','DiMuon','MuonNoGlob' ]
##TODO: why do we have our own thresholds on these? 
##      why not just promote everything L0 has???
##      anybody using this can apply their own cuts
##      before using this anyway...
minPt = { 'Muon' : 1300.0 
        , 'MuonNoGlob' : 1500.0 }

for i in l0muonlines: 
    Line ( 'L0' + i 
       , L0DU  = "L0_CHANNEL('"+i+"')"
       , postscale = 0.000001
       , algos = [ Member('L0MuonPrepare','Decision'
                                   , InputSelection =  HltL0MuonPrepare().getDefaultProperties()['InputSelection']
                                   , OutputSelection = '%Decision'
                                   , MinPt = ( 0 if i not in minPt else minPt[i] )
                                   ) ]
       )
