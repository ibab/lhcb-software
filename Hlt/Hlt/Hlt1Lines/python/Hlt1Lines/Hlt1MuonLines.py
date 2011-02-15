#!/usr/bin/env gaudirun.py
# =============================================================================
# $Id: Hlt1MuonLines.py,v 1.34 2010-09-08 14:55:24 graven Exp $
# =============================================================================
## @file
#  Configuration of Muon Lines
#  Include New Line for Dimuon without PV: Leandro de Paula
#  2008-12-17 
#  @Translator from the original options: Gerhard Raven Gerhard.Raven@nikhef.nl
#  @date 2008-08-25
# =============================================================================
"""
 script to configure Muon trigger lines
"""
# =============================================================================
__author__  = "Gerhard Raven Gerhard.Raven@nikhef.nl"
__version__ = "CVS Tag $Name: not supported by cvs2svn $, $Revision: 1.34 $"
# =============================================================================


from Gaudi.Configuration import * 
from HltLine.HltLinesConfigurableUser import *


class Hlt1MuonLinesConf(HltLinesConfigurableUser) :
    # steering variables
    __slots__ = { 
        #  Muon Lines
        'Prescale'                  : {  'Hlt1SingleMuonNoVeloL0'      :  0.00001
                                      ,  'Hlt1DiMuonNoVeloL0'          :  0.00001
                                      }
        ,'L0SingleMuon'             :"Muon"
        ,'L0SingleMuonGEC'          :"Muon"
        ,'L0SingleMuonNoPV'         :"Muon,lowMult"
        ,'L0SingleMuon4BsMuMu'      :"MuonHigh"
        ,'L0SingleMuonHigh'         :"MuonHigh"
        ,'L0DiMuonNoPV'             :"DiMuon,lowMult"
        ,'L0DiMuon'                 :"DiMuon"
        ,'L0MuonNoVelo'             :"MUON,minbias"
        ,'DiMuon_SumPtCut'          : 1000.
        ,'L0AllMuon_PtCut'          :   80.
        ,'L0MuonNoPV_PtCut'         :  800.
        ,'Muon_DeltaPCut'           :    0.
        ,'Muon_3DVeloTMatchCut'     :    6.
        ,'Muon_VeloTMatchCut'       :  200.
        ,'Muon_ShareCut'            :    0.5
        ,'Muon_FitChiCut'           :   50.
        ,'Muon_FitMuChi2Cut'        :   75.
        ,'Muon_PtCut'               : 6000.
        ,'Muon_highPtCut'           : 5000.
        ,'MuonTS_PtCut'             :  600.
        ,'MuonIP_PtCut'             : 1300. 
        ,'Muon_IPMinCut'            :    0.08
        ,'DiMuon_DOCACut'           :    0.5
        ,'DiMuonTS_DOCACut'         : 1000.0
        ,'DiMuon_IPCut'             :    1.9
        ,'DiMuon_MassCut'           : 2500.
        ,'DiMuonNoPV_SumPtCut'      :  200.
        ,'DiMuonNoPV_MassCut'       : 1000.
        ,'DiMuonIP_MassCut'         :  500.
        ,'DiMuon_IPCut'             :    0.15
        
        # For the Muon+Track lines
        ,'MuTrackMuPt'    : 1000.
        ,'MuTrackMuIP'    : 0.025
        ,'MuTrackTrPt'    : 800.
        ,'MuTrackTrIP'    : 0.050
        ,'MuTrackDoca'    : 0.4
        ,'MuTrackDZ'      : 1.5
        #,'MuTrackAngle'   : 0.
        ,'MuTrackDimuMass': 1000.
        ,'MuTrackPoint'   : 0.4
        ,'MuTrackMuChi2'  : 10.
        ,'MuTrackTrChi2'  : 10.
        
        ,'MuTrackMuPt4JPsi'         : 1600.
        ,'MuTrackTrPt4JPsi'         : 400.
        ,'MuTrackDoca4JPsi'         : 0.1
        ,'MuTrackAngle4JPsiLow'     : 0.02
        ,'MuTrackAngle4JPsiHigh'    : 0.30
        ,'MuTrackDimuMass4JPsiLow'  : 2900.
        ,'MuTrackDimuMass4JPsiHigh' : 0.
        ,'MuTrackMuChi24JPsi'       : 4.
        ,'MuTrackTrChi24JPsi'       : 8.
        ,'bmm_IP'                   : 0.05
        ,'bmm_MuChi2'               : 16
        ,'bmm_fitChi2'              : 10
        ,'bmm_IPS'                  : 49
        ,'bmm_pt'                   : 1500
        ,'bmm_dimuonMass'           : 4700
        }
    

    def __apply_configuration__(self) : 
        pass

