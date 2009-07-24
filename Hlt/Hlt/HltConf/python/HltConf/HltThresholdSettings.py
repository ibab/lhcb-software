"""
Threshold settings for various trigger configurations

@author G.Raven, P.Koppenburg
@date 2009-07-21
"""

__all__ = ( 'Miriam_20090430', 'FEST', 'SettingsForDataType',
            'Hlt2_Leptonic', 'Hlt2_Hadronic', 'Hlt2_Charming', 'Hlt2_Optimised' )

def SettingsForDataType( x ) :
    _dataType2Settings = { 'DC06' : Hlt2_Optimised ,
                           'MC09' : Miriam_20090430 }  # temporary
    return _dataType2Settings[x] if x in _dataType2Settings else None

#########################################################################################
# HLT1
#########################################################################################
from Hlt1Lines.HltCommissioningLines  import HltCommissioningLinesConf
from Hlt1Lines.HltMuonLines           import HltMuonLinesConf
from Hlt1Lines.HltHadronLines         import HltHadronLinesConf
from Hlt1Lines.HltElectronLines       import HltElectronLinesConf
from Hlt1Lines.HltPhotonLines         import HltPhotonLinesConf
from Hlt1Lines.HltExpressLines        import HltExpressLinesConf
from Hlt1Lines.HltVeloLines           import HltVeloLinesConf
#
# Miriam's Hlt1 bandwidth division for 2*10^32 @ 40 MHz, done on DC06 MC
# @author miriam.gandelman@cern.ch
#
Miriam_20090430 = { HltHadronLinesConf : { 'SingleHadron_PTCut' : 4450
                                         , 'HadMain_IPCut'      : 0.09
                                         , 'HadMain_PTCut'      : 1600
                                         , 'HadCompanion_DZCut' : 0.05
                                         , 'HadCompanion_IPCut' : 0.13
                                         , 'HadCompanion_PTCut' : 950
                                         , 'HadCompanion_PointingCut' : 1.0 
                                         }
                  , HltPhotonLinesConf : { 'Pho_EtCut'          : 2800.0
                                         , 'Track_PtCut'        : 650
                                         , 'Track_IPCut3D'      : 0.15
                                         , 'Pho_IsPho'          : -0.082 
                                         }
                  , HltElectronLinesConf : { 'SingleEle_PtCut'    : 2820.0
                                           , 'Compan_PtCut'       : 2750
                                           , 'Ele_IPCut'          : 0.13 
                                           }
                  , HltMuonLinesConf : { 'DiMuon_SumPtCut'    : 1480.0
                                       , 'Muon_PtCut'         : 6200
                                       , 'MuonIP_PtCut'       : 1800
                                       , 'Muon_IPMinCut'      : 0.07
                                       , 'DiMuon_DOCACut'     : 1.6
                                       , 'DiMuon_MassCut'     : 2500
                                       , 'DiMuonIP_MassCut'   : 300
                                       , 'DiMuon_IPCut'       : 0.08
                                       , 'Prescale' : { 'Hlt1MuTrack4JPsi' : 0. }
                                       , 'MuTrackMuPt'        : 950
                                       , 'MuTrackMuIP'        : 0.07
                                       , 'MuTrackTrPt'        : 550
                                       , 'MuTrackTrIP'        : 0.1
                                       , 'MuTrackDoca'        : 0.2
                                       , 'MuTrackDZ'          : 2.0
                                       , 'MuTrackDimuMass'    : 1000
                                       , 'MuTrackPoint'       : 0.4
                                       , 'MuTrackTrChi2'      : 10 
                                       }
                  }
#
# FEST settings Gerhard.Raven@nikhef.nl
# @author Gerhard.Raven@nikhef.nl
#
FEST = {}
FEST.update(  { HltVeloLinesConf : { 'Prescale' : { '.*'            : 1 } }
              , HltExpressLinesConf : { 'Prescale' : { 'Hlt1XPress'    : 0.0025
                                                     , 'Hlt1VeloAlign' : 1 } 
                                      }
              , HltCommissioningLinesConf : { 'Prescale' : { 'Hlt1Physics'    : 1 
                                                           , 'Hlt1Random'     : 1 # @OnlineEnv.AcceptRate
                                                           , 'Hlt1Tell1Error' : 0
                                                           }
                                            }
              }
           )

#########################################################################################
# HLT2 for 2*10^32 @ 40 MHz, done on DC06 MC
#
# @author Compiled by Patrick.Koppenburg@cern.ch
#
# TOPO cuts by <p.spradlin1@physics.ox.ac.uk>
# LEPT cuts by
# PHI  cuts by <Kim.Vervink@cern.ch>
#
####
from Hlt2Lines.Hlt2TopologicalLines  import Hlt2TopologicalLinesConf
from Hlt2Lines.Hlt2InclusivePhiLines import Hlt2InclusivePhiLinesConf
####
# Phi cuts first, as one uses the same for leptonic and charm
# 200 Hz runs with default cuts
Hlt2_Phi_200Hz = { Hlt2InclusivePhiLinesConf   : { } }
# 100 Hz tightens 3 of them
Hlt2_Phi_100Hz = { Hlt2InclusivePhiLinesConf   : { 'TFPhiPT'       : 2000
                                                 , 'TFPhiVCHI2'    : 9
                                                 , 'TFKaonRichPID' : 2
                                                 , 'TFKaonIPS'     : 9
                                                 }
                 }
####
# Leptonic scenario
#
Hlt2_Leptonic = { Hlt2TopologicalLinesConf : { 'ComRobAllTrkPtLL' : 800.0
                                             , 'ComRobAllTrkPVIPLL' : 0.025
                                             , 'ComRobPairMinDocaUL' : 0.1
                                             , 'ComRobTrkMaxPtLL' : 1500.0
                                             , 'ComTFVtxPVDispChi2LL' : 169.0
                                             , 'RobustPointingUL' : 0.12
                                             , 'TFPointUL' : 0.2
                                             , 'CharmRobustPointUL' : 0.04
                                             , 'CharmTFPointUL' : 0.04
                                             , 'Prescale' : { 'Hlt2TopoTF4BodyCharmSignal' : 0.0
                                                            , 'Hlt2TopoTF4BodyReq4Yes' : 0.2
                                                            , 'Hlt2TopoTF4BodyReq3Yes' : 0.2
                                                            , 'Hlt2TopoTF4BodyReq2Yes' : 0.2 } 
                                             }
                  }
Hlt2_Leptonic.update(Hlt2_Phi_100Hz)    # append 100 Hz of Phis
Hlt2_Leptonic.update(Miriam_20090430)   # append Hlt1 cuts
####
# hadronic scenario
#
Hlt2_Hadronic = { Hlt2TopologicalLinesConf   : { 'ComRobAllTrkPtLL' : 400.0
                                               , 'ComRobAllTrkPVIPLL' : 0.025
                                               , 'ComRobPairMinDocaUL' : 0.15
                                               , 'ComRobTrkMaxPtLL' : 1500.0
                                               , 'ComTFVtxPVDispChi2LL' : 196.0
                                               , 'RobustPointingUL' : 0.28
                                               , 'TFPointUL' : 0.16
                                               , 'CharmRobustPointUL' : 0.04
                                               , 'CharmTFPointUL' : 0.04
                                               , 'Prescale' : { 'Hlt2TopoTF4BodyCharmSignal' : 0.2
                                                              , 'Hlt2TopoTF4BodyReq4Yes' : 0.2
                                                              , 'Hlt2TopoTF4BodyReq3Yes' : 0.2
                                                              , 'Hlt2TopoTF4BodyReq2Yes' : 0.2 } 
                                               }
                  }
Hlt2_Hadronic.update(Hlt2_Phi_200Hz)   # append 200 Hz of Phis   
Hlt2_Hadronic.update(Miriam_20090430)  # append Hlt1 cuts
####
# charming scenario
#
Hlt2_Charming = {  Hlt2TopologicalLinesConf  : { 'ComRobAllTrkPtLL' : 400.0
                                               , 'ComRobAllTrkPVIPLL' : 0.025
                                               , 'ComRobPairMinDocaUL' : 0.15
                                               , 'ComRobTrkMaxPtLL' : 500.0
                                               , 'ComTFVtxPVDispChi2LL' : 196.0
                                               , 'RobustPointingUL' : 0.24
                                               , 'TFPointUL' : 0.08
                                               , 'CharmRobustPointUL' : 0.12
                                               , 'CharmTFPointUL' : 0.12
                                               , 'Prescale' : { 'Hlt2TopoTF4BodyCharmSignal' : 0.4
                                                              , 'Hlt2TopoTF4BodyReq4Yes' : 0.0
                                                              , 'Hlt2TopoTF4BodyReq3Yes' : 0.0
                                                              , 'Hlt2TopoTF4BodyReq2Yes' : 0.0 } 
                                                 }
                  }
Hlt2_Charming.update(Hlt2_Phi_100Hz)    # append 100 Hz of Phis
Hlt2_Charming.update(Miriam_20090430)   # append Hlt1 cuts
####
# Optimised scenario
# @author miriam.gandelman@cern.ch
#
Hlt2_Optimised = { Hlt2TopologicalLinesConf  : {}
                 , Hlt2InclusivePhiLinesConf : {} }
Hlt2_Optimised.update(Miriam_20090430)  # append Hlt1 cuts
#########################################################################################
# Utilisty function for setting thresholds both in Hlt1 and 2
#
# @author Moved by Patrick.Koppenburg@cern.ch
#
####
def SetThresholds(ThresholdSettings,confs):
    """
    Look in ThresholdSettings fr configurable confs
    and set the appropriate settings

    @author G. Raven, P. Koppenburg
    @date 23/7/2009 (moved)
    """
    conf = confs()  # The configurable _must_ be called even if not configured. Or it will be ignored
    if confs in ThresholdSettings : 
        #print '# Found', conf.name()
        for (k,v) in ThresholdSettings[confs].iteritems() :
            # configurables have an exception for list and dict: 
            #   even if not explicitly set, if you ask for them, you get one...
            #   this is done to make foo().somelist += ... work.
            # hence we _assume_ that, even if we have an attr, but it matches the
            # default, it wasn't set explicitly, and we overrule it...
            if hasattr(conf,k) and conf.getProp(k) != conf.getDefaultProperty(k) :
                log.warning('# WARNING: %s.%s has explictly been set, NOT using requested predefined threshold %s, but keeping explicit value: %s '%(conf.name(),k,str(v),getattr(conf,k)))
            else :
                if ( type(v) == type({})): # special case for dictionaries (needed in topo)
                    val = conf.getProp(k)
                    val.update(v)                                
#                    print '# SETTING dictionary', conf.name(), val
                    setattr(conf,k,val)
                else :
#                    print '# SETTING           ', conf.name(), v
                    setattr(conf,k,v)

    
    
