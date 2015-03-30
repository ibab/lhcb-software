"""
Stripping lines for B/Bs decays to chi_c0(1P) X or eta_c(1S) X.
All decay modes contain 6 charged hadrons in the final states:
Bs->(eta_c->KKKK)     (phi->KK)
Bs->(eta_c->KKpipi)   (phi->KK)
Bs->(eta_c->pipipipi) (phi->KK)
Bs->(chi_c->KKKK)     (phi->KK)
Bs->(chi_c->KKpipi)   (phi->KK)
Bs->(chi_c->pipipipi) (phi->KK)
Bd->(eta_c->KKKK)     (K*->Kpi)
Bd->(eta_c->KKpipi)   (K*->Kpi)
Bd->(eta_c->pipipipi) (K*->Kpi)
Bd->(chi_c->KKKK)     (K*->Kpi)
Bd->(chi_c->KKpipi)   (K*->Kpi)
Bd->(chi_c->pipipipi) (K*->Kpi)
"""
__author__ = ['Adam Morris']
__date__ = '26/08/2014'
__version__ = '$Revision: 2.0$'
"""
Adapted from previous line of same name by Katarzyna Senderowska 27/02/2011
"""
################################################################################
from Gaudi.Configuration import *
from GaudiConfUtils.ConfigurableGenerators import FilterDesktop, CombineParticles
from PhysSelPython.Wrappers import Selection, DataOnDemand
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder
from StandardParticles import StdLooseANNPions, StdLooseANNKaons
################################################################################
__all__ = (
  'StrippingB2CharmoniumX_6HConf',
  'makeBs2EtacPhi_Etac2KKKK',
  'makeBs2EtacPhi_Etac2PiPiPiPi',
  'makeBs2ChicPhi_Chic2KKKK',
  'makeBs2ChicPhi_Chic2KKPiPi',
  'makeBs2ChicPhi_Chic2PiPiPiPi',
  'makeB2EtacKst_Etac2KKKK',
  'makeB2EtacKst_Etac2KKPiPi',
  'makeB2EtacKst_Etac2PiPiPiPi',
  'makeB2ChicKst_Chic2KKKK',
  'makeB2ChicKst_Chic2KKPiPi',
  'makeB2ChicKst_Chic2PiPiPiPi',
  'default_config'
  )
################################################################################
config_params = {
  'Prescale'               : 1.,
  'Postscale'              : 1.,
  'TRCHI2'                 : 3.,
  'DOCA'                   : 0.1,
  #K parameters
  'K_PT'                   : 250.,
  'K_IPCHI2'               : 4.,
  #pi parameters
  'pi_PT'                  : 250.,
  'pi_IPCHI2'              : 4.,
  #'max' daughters
  'PT_maxetacdaughter'     : 1400.,
  'PT_maxchicdaughter'     : 1300.,
  'IPCHI2_maxdaughter'     : 9.,
  #phi parameters
  'phi_DIRA'               : 0.95,
  'phi_VDZ'                : 0.,
  'phi_PT'                 : 1000.,
  'phi_IPCHI2'             : 6.,
  'phi_VCHI2'              : 7.,
  'phi_window'             : 30.,
  #Kst parameters
  'Kst_DIRA'               : 0.93,
  'Kst_VDZ'                : 0.,
  'Kst_PT'                 : 1000.,
  'Kst_IPCHI2'             : 4.,
  'Kst_VCHI2'              : 12.,
  'Kst_window'             : 100.,
  #etac parameters
  'etac_DIRA'              : 0.9,
  'etac_VDZ'               : 0.,
  'etac_IPCHI2'            : 4.,
  'etac_VCHI2'             : 9.,
  'etac_sumPT'             : 4000.,
  'etac_window'            : 90.,
  #chic/etac parameters
  'chic_DIRA'              : 0.9,
  'chic_VDZ'               : 0.,
  'chic_IPCHI2'            : 4.,
  'chic_VCHI2'             : 9.,
  'chic_sumPT'             : 3600.,
  'chic_window'            : 60.,
  #Bs parameters
  'Bs_VDZ'                 : 0.,
  'Bs_DIRA'                : 0.9,
  'Bs_VCHI2'               : 25.,
  'Bs_window'              : 500.
  }
default_config = {
  'NAME'        : 'B2CharmoniumX_6H',
  'WGs'         : ['B2CC'],
  'BUILDERTYPE' : 'StrippingB2CharmoniumX_6HConf',
  'CONFIG'  : config_params,
  'STREAMS'     : ['Bhadron'] 
  }
################################################################################
class StrippingB2CharmoniumX_6HConf(LineBuilder):
  __configuration_keys__ = (
  'Prescale',
  'Postscale',
  'TRCHI2',
  'DOCA',
  #K parameters
  'K_PT',
  'K_IPCHI2',
  #pi parameters
  'pi_PT',
  'pi_IPCHI2',
  #'max' daughters
  'PT_maxetacdaughter',
  'PT_maxchicdaughter',
  'IPCHI2_maxdaughter',
  #phi parameters
  'phi_DIRA',
  'phi_VDZ',
  'phi_PT',
  'phi_IPCHI2',
  'phi_VCHI2',
  'phi_window',
  #Kst parameters
  'Kst_DIRA',
  'Kst_VDZ',
  'Kst_PT',
  'Kst_IPCHI2',
  'Kst_VCHI2',
  'Kst_window',
  #etac parameters
  'etac_DIRA',
  'etac_VDZ',
  'etac_IPCHI2',
  'etac_VCHI2',
  'etac_sumPT',
  'etac_window',
  #chic/etac parameters
  'chic_DIRA',
  'chic_VDZ',
  'chic_IPCHI2',
  'chic_VCHI2',
  'chic_sumPT',
  'chic_window',
  #Bs parameters
  'Bs_VDZ',
  'Bs_DIRA',
  'Bs_VCHI2',
  'Bs_window'
  )
##### Final state hadrons ######################################################
  def __init__(self, name, config) :
    self.config=config
    LineBuilder.__init__(self, name, config)
##### K+
    self.selKaon = Selection('KaonFor' + name,
                             Algorithm = self._KaonFilter(),
                             RequiredSelections = [StdLooseANNKaons]
                             )
##### pi+
    self.selPion = Selection('PionFor' + name,
                             Algorithm = self._PionFilter(),
                             RequiredSelections = [StdLooseANNPions]
                             )
##### Phi and K* ###############################################################
##### phi(1020) -> K+ K-
    self.selPhi2KK = Selection('Phi2KKFor' + name,
                               Algorithm = self._Phi2KKFilter(),
                               RequiredSelections = [ DataOnDemand(Location='Phys/StdLoosePhi2KK/Particles') ]
                               )
##### K*(892)0 -> K+ pi-
    self.selKst2KPi = Selection('Kst2KPiFor' + name,
                                Algorithm = self._Kst2KPiFilter(),
                                RequiredSelections = [self.selKaon, self.selPion]
                                )
##### Charmonium ###############################################################
##### eta_c(1S) -> K+ K- K+ K-
    self.selEtac2KKKK = Selection('Etac2KKKKFor' + name,
                                    Algorithm = self._Etac2KKKKFilter(),
                                    RequiredSelections = [self.selKaon]
                                    )
##### eta_c(1S) -> K+ K- pi+ pi-
    self.selEtac2KKPiPi = Selection('Etac2KKPiPiFor' + name,
                                    Algorithm = self._Etac2KKPiPiFilter(),
                                    RequiredSelections = [self.selKaon, self.selPion]
                                    )
##### eta_c(1S) -> pi+ pi- pi+ pi-
    self.selEtac2PiPiPiPi = Selection('Etac2PiPiPiPiFor' + name,
                                      Algorithm = self._Etac2PiPiPiPiFilter(),
                                      RequiredSelections = [self.selPion]
                                      )
##### chi_c0(1P) -> K+ K- K+ K-
    self.selChic2KKKK = Selection('Chic2KKKKFor' + name,
                                    Algorithm = self._Chic2KKKKFilter(),
                                    RequiredSelections = [self.selKaon]
                                    )
##### chi_c0(1P) -> K+ K- pi+ pi-
    self.selChic2KKPiPi = Selection('Chic2KKPiPiFor' + name,
                                    Algorithm = self._Chic2KKPiPiFilter(),
                                    RequiredSelections = [self.selKaon, self.selPion]
                                    )
##### chi_c0(1P) -> pi+ pi- pi+ pi-
    self.selChic2PiPiPiPi = Selection('Chic2PiPiPiPiFor' + name,
                                      Algorithm = self._Chic2PiPiPiPiFilter(),
                                      RequiredSelections = [self.selPion]
                                      )
##### Bs decay modes ###########################################################
##### B_s0 -> (eta_c(1S) -> K+ K- K+ K-) (phi(1020) -> K+ K-)
    self.selBs2EtacPhi_Etac2KKKK = makeBs2EtacPhi_Etac2KKKK('Bs2EtacPhi_Etac2KKKK' + name,
                                                            DecayDescriptors = [ 'B_s0 -> eta_c(1S) phi(1020)' ],
                                                            PhiSel = self.selPhi2KK,
                                                            EtacSel = self.selEtac2KKKK,
                                                            Bs_VCHI2 = config['Bs_VCHI2'],
                                                            Bs_DIRA = config['Bs_DIRA'],
                                                            Bs_VDZ = config['Bs_VDZ']
                                                            )
##### B_s0 -> (eta_c(1S) -> K+ K- pi+ pi-) (phi(1020) -> K+ K-)
    self.selBs2EtacPhi_Etac2KKPiPi = makeBs2EtacPhi_Etac2KKPiPi('Bs2EtacPhi_Etac2KKPiPi' + name,
                                                                DecayDescriptors = [ 'B_s0 -> eta_c(1S) phi(1020)' ],
                                                                PhiSel = self.selPhi2KK,
                                                                EtacSel = self.selEtac2KKPiPi,
                                                                Bs_VCHI2 = config['Bs_VCHI2'],
                                                                Bs_DIRA = config['Bs_DIRA'],
                                                                Bs_VDZ = config['Bs_VDZ']
                                                                )
##### B_s0 -> (eta_c(1S) -> pi+ pi- pi+ pi-) (phi(1020) -> K+ K-)
    self.selBs2EtacPhi_Etac2PiPiPiPi = makeBs2EtacPhi_Etac2PiPiPiPi('Bs2EtacPhi_Etac2PiPiPiPi' + name,
                                                                    DecayDescriptors = [ 'B_s0 -> eta_c(1S) phi(1020)' ],
                                                                    PhiSel = self.selPhi2KK,
                                                                    EtacSel = self.selEtac2PiPiPiPi,
                                                                    Bs_VCHI2 = config['Bs_VCHI2'],
                                                                    Bs_DIRA = config['Bs_DIRA'],
                                                                    Bs_VDZ = config['Bs_VDZ']
                                                                    )
##### B_s0 -> (chi_c0(1P) -> K+ K- K+ K-) (phi(1020) -> K+ K-)
    self.selBs2ChicPhi_Chic2KKKK = makeBs2ChicPhi_Chic2KKKK('Bs2ChicPhi_Chic2KKKK' + name,
                                                                DecayDescriptors = [ 'B_s0 -> chi_c0(1P) phi(1020)' ],
                                                                PhiSel = self.selPhi2KK,
                                                                ChicSel = self.selChic2KKKK,
                                                                Bs_VCHI2 = config['Bs_VCHI2'],
                                                                Bs_DIRA = config['Bs_DIRA'],
                                                                Bs_VDZ = config['Bs_VDZ']
                                                                )
##### B_s0 -> (chi_c0(1P) -> K+ K- pi+ pi-) (phi(1020) -> K+ K-)
    self.selBs2ChicPhi_Chic2KKPiPi = makeBs2ChicPhi_Chic2KKPiPi('Bs2ChicPhi_Chic2KKPiPi' + name,
                                                                DecayDescriptors = [ 'B_s0 -> chi_c0(1P) phi(1020)' ],
                                                                PhiSel = self.selPhi2KK,
                                                                ChicSel = self.selChic2KKPiPi,
                                                                Bs_VCHI2 = config['Bs_VCHI2'],
                                                                Bs_DIRA = config['Bs_DIRA'],
                                                                Bs_VDZ = config['Bs_VDZ']
                                                                )
##### B_s0 -> (chi_c0(1P) -> pi+ pi- pi+ pi-) (phi(1020) -> K+ K-)
    self.selBs2ChicPhi_Chic2PiPiPiPi = makeBs2ChicPhi_Chic2PiPiPiPi('Bs2ChicPhi_Chic2PiPiPiPi' + name,
                                                                    DecayDescriptors = [ 'B_s0 -> chi_c0(1P) phi(1020)' ],
                                                                    PhiSel = self.selPhi2KK,
                                                                    ChicSel = self.selChic2PiPiPiPi,
                                                                    Bs_VCHI2 = config['Bs_VCHI2'],
                                                                    Bs_DIRA = config['Bs_DIRA'],
                                                                    Bs_VDZ = config['Bs_VDZ']
                                                                    )
##### Bd decay modes ###########################################################
##### B0 -> (eta_c(1S) -> K+ K- K+ K-) (K*(892)0 -> K+ pi-)
    self.selB2EtacKst_Etac2KKKK = makeB2EtacKst_Etac2KKKK('B2EtacKst_Etac2KKKK' + name,
                                                          DecayDescriptors = [ 'B0 -> eta_c(1S) K*(892)0' ],
                                                          KstSel = self.selKst2KPi,
                                                          EtacSel = self.selEtac2KKKK,
                                                          Bs_VCHI2 = config['Bs_VCHI2'],
                                                          Bs_DIRA = config['Bs_DIRA'],
                                                          Bs_VDZ = config['Bs_VDZ']
                                                          )
##### B0 -> (eta_c(1S) -> K+ K- pi+ pi-) (K*(892)0 -> K+ pi-)
    self.selB2EtacKst_Etac2KKPiPi = makeB2EtacKst_Etac2KKPiPi('B2EtacKst_Etac2KKPiPi' + name,
                                                              DecayDescriptors = [ 'B0 -> eta_c(1S) K*(892)0' ],
                                                              KstSel = self.selKst2KPi,
                                                              EtacSel = self.selEtac2KKPiPi,
                                                              Bs_VCHI2 = config['Bs_VCHI2'],
                                                              Bs_DIRA = config['Bs_DIRA'],
                                                              Bs_VDZ = config['Bs_VDZ']
                                                              )
##### B0 -> (eta_c(1S) -> pi+ pi- pi+ pi-) (K*(892)0 -> K+ pi-)
    self.selB2EtacKst_Etac2PiPiPiPi = makeB2EtacKst_Etac2PiPiPiPi('B2EtacKst_Etac2PiPiPiPi' + name,
                                                                  DecayDescriptors = [ 'B0 -> eta_c(1S) K*(892)0' ],
                                                                  KstSel = self.selKst2KPi,
                                                                  EtacSel = self.selEtac2PiPiPiPi,
                                                                  Bs_VCHI2 = config['Bs_VCHI2'],
                                                                  Bs_DIRA = config['Bs_DIRA'],
                                                                  Bs_VDZ = config['Bs_VDZ']
                                                                  )
##### B0 -> (chi_c0(1P) -> K+ K- K+ K-) (K*(892)0 -> K+ pi-)
    self.selB2ChicKst_Chic2KKKK = makeB2ChicKst_Chic2KKKK('B2ChicKst_Chic2KKKK' + name,
                                                          DecayDescriptors = [ 'B0 -> chi_c0(1P) K*(892)0' ],
                                                          KstSel = self.selKst2KPi,
                                                          ChicSel = self.selChic2KKKK,
                                                          Bs_VCHI2 = config['Bs_VCHI2'],
                                                          Bs_DIRA = config['Bs_DIRA'],
                                                          Bs_VDZ = config['Bs_VDZ']
                                                          )
##### B0 -> (chi_c0(1P) -> K+ K- pi+ pi-) (K*(892)0 -> K+ pi-)
    self.selB2ChicKst_Chic2KKPiPi = makeB2ChicKst_Chic2KKPiPi('B2ChicKst_Chic2KKPiPi' + name,
                                                              DecayDescriptors = [ 'B0 -> chi_c0(1P) K*(892)0' ],
                                                              KstSel = self.selKst2KPi,
                                                              ChicSel = self.selChic2KKPiPi,
                                                              Bs_VCHI2 = config['Bs_VCHI2'],
                                                              Bs_DIRA = config['Bs_DIRA'],
                                                              Bs_VDZ = config['Bs_VDZ']
                                                              )
##### B0 -> (chi_c0(1P) -> pi+ pi- pi+ pi-) (K*(892)0 -> K+ pi-)
    self.selB2ChicKst_Chic2PiPiPiPi = makeB2ChicKst_Chic2PiPiPiPi('B2ChicKst_Chic2PiPiPiPi' + name,
                                                                  DecayDescriptors = [ 'B0 -> chi_c0(1P) K*(892)0' ],
                                                                  KstSel = self.selKst2KPi,
                                                                  ChicSel = self.selChic2PiPiPiPi,
                                                                  Bs_VCHI2 = config['Bs_VCHI2'],
                                                                  Bs_DIRA = config['Bs_DIRA'],
                                                                  Bs_VDZ = config['Bs_VDZ']
                                                                  )
##### Stripping lines ##########################################################
##### Bs decay modes
    self.Bs2EtacPhi_Etac2KKKK = StrippingLine('Bs2EtacPhi_Etac2KKKK' + name + 'Line'
                                            , HLT  = "HLT_PASS_RE('Hlt2.*Topo.*Decision')"
                                            , prescale = config['Prescale']
                                            , selection = self.selBs2EtacPhi_Etac2KKKK
                                            )
    self.Bs2EtacPhi_Etac2KKPiPi = StrippingLine('Bs2EtacPhi_Etac2KKPiPi' + name + 'Line'
                                            , HLT  = "HLT_PASS_RE('Hlt2.*Topo.*Decision')"
                                            , prescale = config['Prescale']
                                            , selection = self.selBs2EtacPhi_Etac2KKPiPi
                                            )
    self.Bs2EtacPhi_Etac2PiPiPiPi = StrippingLine('Bs2EtacPhi_Etac2PiPiPiPi' + name + 'Line'
                                                  , HLT  = "HLT_PASS_RE('Hlt2.*Topo.*Decision')"
                                                  , prescale = config['Prescale']
                                                  , selection = self.selBs2EtacPhi_Etac2PiPiPiPi
                                                  )
    self.Bs2ChicPhi_Chic2KKKK = StrippingLine('Bs2ChicPhi_Chic2KKKK' + name + 'Line'
                                            , HLT  = "HLT_PASS_RE('Hlt2.*Topo.*Decision')"
                                            , prescale = config['Prescale']
                                            , selection = self.selBs2ChicPhi_Chic2KKKK
                                            )
    self.Bs2ChicPhi_Chic2KKPiPi = StrippingLine('Bs2ChicPhi_Chic2KKPiPi' + name + 'Line'
                                            , HLT  = "HLT_PASS_RE('Hlt2.*Topo.*Decision')"
                                            , prescale = config['Prescale']
                                            , selection = self.selBs2ChicPhi_Chic2KKPiPi
                                            )
    self.Bs2ChicPhi_Chic2PiPiPiPi = StrippingLine('Bs2ChicPhi_Chic2PiPiPiPi' + name + 'Line'
                                                  , HLT  = "HLT_PASS_RE('Hlt2.*Topo.*Decision')"
                                                  , prescale = config['Prescale']
                                                  , selection = self.selBs2ChicPhi_Chic2PiPiPiPi
                                                  )
##### Bd decay modes
    self.B2EtacKst_Etac2KKKK = StrippingLine('B2EtacKst_Etac2KKKK' + name + 'Line'
                                            , HLT  = "HLT_PASS_RE('Hlt2.*Topo.*Decision')"
                                            , prescale = config['Prescale']
                                            , selection = self.selB2EtacKst_Etac2KKKK
                                            )
    self.B2EtacKst_Etac2KKPiPi = StrippingLine('B2EtacKst_Etac2KKPiPi' + name + 'Line'
                                            , HLT  = "HLT_PASS_RE('Hlt2.*Topo.*Decision')"
                                            , prescale = config['Prescale']
                                            , selection = self.selB2EtacKst_Etac2KKPiPi
                                            )
    self.B2EtacKst_Etac2PiPiPiPi = StrippingLine('B2EtacKst_Etac2PiPiPiPi' + name + 'Line'
                                                  , HLT  = "HLT_PASS_RE('Hlt2.*Topo.*Decision')"
                                                  , prescale = config['Prescale']
                                                  , selection = self.selB2EtacKst_Etac2PiPiPiPi
                                                  )
    self.B2ChicKst_Chic2KKKK = StrippingLine('B2ChicKst_Chic2KKKK' + name + 'Line'
                                            , HLT  = "HLT_PASS_RE('Hlt2.*Topo.*Decision')"
                                            , prescale = config['Prescale']
                                            , selection = self.selB2ChicKst_Chic2KKKK
                                            )
    self.B2ChicKst_Chic2KKPiPi = StrippingLine('B2ChicKst_Chic2KKPiPi' + name + 'Line'
                                            , HLT  = "HLT_PASS_RE('Hlt2.*Topo.*Decision')"
                                            , prescale = config['Prescale']
                                            , selection = self.selB2ChicKst_Chic2KKPiPi
                                            )
    self.B2ChicKst_Chic2PiPiPiPi = StrippingLine('B2ChicKst_Chic2PiPiPiPi' + name + 'Line'
                                                  , HLT  = "HLT_PASS_RE('Hlt2.*Topo.*Decision')"
                                                  , prescale = config['Prescale']
                                                  , selection = self.selB2ChicKst_Chic2PiPiPiPi
                                                  )
################################################################################
    self.registerLine(self.Bs2EtacPhi_Etac2KKKK)
    self.registerLine(self.Bs2EtacPhi_Etac2KKPiPi)
    self.registerLine(self.Bs2EtacPhi_Etac2PiPiPiPi)
    self.registerLine(self.Bs2ChicPhi_Chic2KKKK)
    self.registerLine(self.Bs2ChicPhi_Chic2KKPiPi)
    self.registerLine(self.Bs2ChicPhi_Chic2PiPiPiPi)
    self.registerLine(self.B2EtacKst_Etac2KKKK)
    self.registerLine(self.B2EtacKst_Etac2KKPiPi)
    self.registerLine(self.B2EtacKst_Etac2PiPiPiPi)
    self.registerLine(self.B2ChicKst_Chic2KKKK)
    self.registerLine(self.B2ChicKst_Chic2KKPiPi)
    self.registerLine(self.B2ChicKst_Chic2PiPiPiPi)
################################################################################
  def _PionFilter( self ):
      _code = "(TRCHI2DOF < %(TRCHI2)s) & (PT > %(pi_PT)s *MeV)"\
              "& (MIPCHI2DV(PRIMARY) > %(pi_IPCHI2)s)" % self.config
      _pi = FilterDesktop( Code = _code )
      return _pi
  def _KaonFilter( self ):
      _code = "  (TRCHI2DOF < %(TRCHI2)s) & (PT > %(K_PT)s *MeV)"\
              "& (MIPCHI2DV(PRIMARY) > %(K_IPCHI2)s)" % self.config
      _k = FilterDesktop( Code = _code )
      return _k
################################################################################
  def _Phi2KKFilter( self ):
      _code = "(ADMASS('phi(1020)') < %(phi_window)s *MeV) & (PT> %(phi_PT)s) & (MIPCHI2DV(PRIMARY) > %(phi_IPCHI2)s) & (VFASPF(VCHI2) < %(phi_VCHI2)s) & (BPVDIRA > %(phi_DIRA)s)" % self.config
      _phi2kk = FilterDesktop(Code = _code)
      return _phi2kk
  def _Kst2KPiFilter( self ):
      _decayDescriptor = '[K*(892)0 -> K+ pi-]cc'
      _combinationCut = "(ADAMASS('K*(892)0') < %(Kst_window)s *MeV) & (APT > %(Kst_PT)s)" % self.config
      _motherCut = "(VFASPF(VZ) > %(Kst_VDZ)s *mm) & (BPVDIRA > %(Kst_DIRA)s) & (MIPCHI2DV(PRIMARY) > %(Kst_IPCHI2)s) & (VFASPF(VCHI2) < %(Kst_VCHI2)s)" % self.config
      _kst2kpi = CombineParticles( DecayDescriptor = _decayDescriptor,
                                  CombinationCut = _combinationCut,
                                  MotherCut = _motherCut)
      return _kst2kpi
################################################################################
  def _Etac2KKKKFilter( self ):
      _decayDescriptors = [ 'eta_c(1S) -> K+ K- K+ K-' ]
      _combinationCut = "(ADAMASS('eta_c(1S)') < %(etac_window)s *MeV) & (ACHILD(PT,1)+ACHILD(PT,2)+ACHILD(PT,3)+ACHILD(PT,4) > %(etac_sumPT)s *MeV) & (AMAXDOCA('') < %(DOCA)s) & (AMAXCHILD(MAXTREE(((ABSID=='pi+') | (ABSID=='K+')),PT)) > %(PT_maxetacdaughter)s) & (AMAXCHILD(MAXTREE(((ABSID=='pi+') | (ABSID=='K+')),MIPCHI2DV(PRIMARY))) > %(IPCHI2_maxdaughter)s)" % self.config
      _motherCut = "(BPVDIRA> %(etac_DIRA)s) & (MIPCHI2DV(PRIMARY) > %(etac_IPCHI2)s) & (VFASPF(VCHI2/VDOF) < %(etac_VCHI2)s)"  % self.config
      _etac2kkkk = CombineParticles( DecayDescriptors = _decayDescriptors,
                                     CombinationCut = _combinationCut,
                                     MotherCut = _motherCut)
      return _etac2kkkk
  def _Etac2KKPiPiFilter( self ):
      _decayDescriptors = [ 'eta_c(1S) -> K+ K- pi+ pi-' ]
      _combinationCut = "(ADAMASS('eta_c(1S)') < %(etac_window)s *MeV) & (ACHILD(PT,1)+ACHILD(PT,2)+ACHILD(PT,3)+ACHILD(PT,4) > %(etac_sumPT)s *MeV) & (AMAXDOCA('') < %(DOCA)s) & (AMAXCHILD(MAXTREE(((ABSID=='pi+') | (ABSID=='K+')),PT)) > %(PT_maxetacdaughter)s) & (AMAXCHILD(MAXTREE(((ABSID=='pi+') | (ABSID=='K+')),MIPCHI2DV(PRIMARY))) > %(IPCHI2_maxdaughter)s)" % self.config
      _motherCut = "(BPVDIRA> %(etac_DIRA)s) & (MIPCHI2DV(PRIMARY) > %(etac_IPCHI2)s) & (VFASPF(VCHI2/VDOF) < %(etac_VCHI2)s)"  % self.config
      _etac2kkpipi = CombineParticles( DecayDescriptors = _decayDescriptors,
                                       CombinationCut = _combinationCut,
                                       MotherCut = _motherCut)
      return _etac2kkpipi
  def _Etac2PiPiPiPiFilter( self ):
      _decayDescriptors = [ 'eta_c(1S) -> pi+ pi- pi+ pi-' ]
      _combinationCut = "(ADAMASS('eta_c(1S)') < %(etac_window)s *MeV) & (ACHILD(PT,1)+ACHILD(PT,2)+ACHILD(PT,3)+ACHILD(PT,4) > %(etac_sumPT)s *MeV) & (AMAXDOCA('') < %(DOCA)s) & (AMAXCHILD(MAXTREE(((ABSID=='pi+') | (ABSID=='K+')),PT)) > %(PT_maxetacdaughter)s) & (AMAXCHILD(MAXTREE(((ABSID=='pi+') | (ABSID=='K+')),MIPCHI2DV(PRIMARY))) > %(IPCHI2_maxdaughter)s)" % self.config
      _motherCut = "(BPVDIRA> %(etac_DIRA)s) & (MIPCHI2DV(PRIMARY) > %(etac_IPCHI2)s) & (VFASPF(VCHI2/VDOF) < %(etac_VCHI2)s)"  % self.config
      _etac2pipipipi = CombineParticles( DecayDescriptors = _decayDescriptors,
                                         CombinationCut = _combinationCut,
                                         MotherCut = _motherCut)
      return _etac2pipipipi
  def _Chic2KKKKFilter( self ):
      _decayDescriptors = [ 'chi_c0(1P) -> K+ K- K+ K-' ]
      _combinationCut = "(ADAMASS('chi_c0(1P)') < %(chic_window)s *MeV) & (ACHILD(PT,1)+ACHILD(PT,2)+ACHILD(PT,3)+ACHILD(PT,4) > %(chic_sumPT)s *MeV) & (AMAXDOCA('') < %(DOCA)s) & (AMAXCHILD(MAXTREE(((ABSID=='pi+') | (ABSID=='K+')),PT)) > %(PT_maxchicdaughter)s) & (AMAXCHILD(MAXTREE(((ABSID=='pi+') | (ABSID=='K+')),MIPCHI2DV(PRIMARY))) > %(IPCHI2_maxdaughter)s)" % self.config
      _motherCut = "(BPVDIRA> %(chic_DIRA)s) & (MIPCHI2DV(PRIMARY) > %(chic_IPCHI2)s) & (VFASPF(VCHI2/VDOF) < %(chic_VCHI2)s)"  % self.config
      _chic2kkkk = CombineParticles( DecayDescriptors = _decayDescriptors,
                                     CombinationCut = _combinationCut,
                                     MotherCut = _motherCut)
      return _chic2kkkk
  def _Chic2KKPiPiFilter( self ):
      _decayDescriptors = [ 'chi_c0(1P) -> K+ K- pi+ pi-' ]
      _combinationCut = "(ADAMASS('chi_c0(1P)') < %(chic_window)s *MeV) & (ACHILD(PT,1)+ACHILD(PT,2)+ACHILD(PT,3)+ACHILD(PT,4) > %(chic_sumPT)s *MeV) & (AMAXDOCA('') < %(DOCA)s) & (AMAXCHILD(MAXTREE(((ABSID=='pi+') | (ABSID=='K+')),PT)) > %(PT_maxchicdaughter)s) & (AMAXCHILD(MAXTREE(((ABSID=='pi+') | (ABSID=='K+')),MIPCHI2DV(PRIMARY))) > %(IPCHI2_maxdaughter)s)" % self.config
      _motherCut = "(BPVDIRA> %(chic_DIRA)s) & (MIPCHI2DV(PRIMARY) > %(chic_IPCHI2)s) & (VFASPF(VCHI2/VDOF) < %(chic_VCHI2)s)"  % self.config
      _chic2kkpipi = CombineParticles( DecayDescriptors = _decayDescriptors,
                                       CombinationCut = _combinationCut,
                                       MotherCut = _motherCut)
      return _chic2kkpipi
  def _Chic2PiPiPiPiFilter( self ):
      _decayDescriptors = [ 'chi_c0(1P) -> pi+ pi- pi+ pi-' ]
      _combinationCut = "(ADAMASS('chi_c0(1P)') < %(chic_window)s *MeV) & (ACHILD(PT,1)+ACHILD(PT,2)+ACHILD(PT,3)+ACHILD(PT,4) > %(chic_sumPT)s *MeV) & (AMAXDOCA('') < %(DOCA)s) & (AMAXCHILD(MAXTREE(((ABSID=='pi+') | (ABSID=='K+')),PT)) > %(PT_maxchicdaughter)s) & (AMAXCHILD(MAXTREE(((ABSID=='pi+') | (ABSID=='K+')),MIPCHI2DV(PRIMARY))) > %(IPCHI2_maxdaughter)s)" % self.config
      _motherCut = "(BPVDIRA> %(chic_DIRA)s) & (MIPCHI2DV(PRIMARY) > %(chic_IPCHI2)s) & (VFASPF(VCHI2/VDOF) < %(chic_VCHI2)s)"  % self.config
      _chic2pipipipi = CombineParticles( DecayDescriptors = _decayDescriptors,
                                         CombinationCut = _combinationCut,
                                         MotherCut = _motherCut)
      return _chic2pipipipi
################################################################################
def makeBs2EtacPhi_Etac2KKKK(name, DecayDescriptors, PhiSel, EtacSel, Bs_VCHI2, Bs_DIRA, Bs_VDZ):
    _combinationCut = "(ADAMASS('B_s0') < 500 *MeV)"
    _motherCut = "(VFASPF(VZ) > %(Bs_VDZ)s *mm) & (BPVDIRA> %(Bs_DIRA)s) & (VFASPF(VCHI2/VDOF) < %(Bs_VCHI2)s)"  % locals()
    _B = CombineParticles(DecayDescriptors = DecayDescriptors, CombinationCut = _combinationCut, MotherCut = _motherCut)
    return Selection (name, Algorithm = _B, RequiredSelections = [PhiSel, EtacSel])
def makeBs2EtacPhi_Etac2KKPiPi(name, DecayDescriptors, PhiSel, EtacSel, Bs_VCHI2, Bs_DIRA, Bs_VDZ):
    _combinationCut = "(ADAMASS('B_s0') < 500 *MeV)"
    _motherCut = "(VFASPF(VZ) > %(Bs_VDZ)s *mm) & (BPVDIRA> %(Bs_DIRA)s) & (VFASPF(VCHI2/VDOF) < %(Bs_VCHI2)s)"  % locals()
    _B = CombineParticles(DecayDescriptors = DecayDescriptors, CombinationCut = _combinationCut, MotherCut = _motherCut)
    return Selection (name, Algorithm = _B, RequiredSelections = [PhiSel, EtacSel])
def makeBs2EtacPhi_Etac2PiPiPiPi(name, DecayDescriptors, PhiSel, EtacSel, Bs_VCHI2, Bs_DIRA, Bs_VDZ):
    _combinationCut = "(ADAMASS('B_s0') < 500 *MeV)"
    _motherCut = "(VFASPF(VZ) > %(Bs_VDZ)s *mm) & (BPVDIRA> %(Bs_DIRA)s) & (VFASPF(VCHI2/VDOF) < %(Bs_VCHI2)s)"  % locals()
    _B = CombineParticles(DecayDescriptors = DecayDescriptors, CombinationCut = _combinationCut, MotherCut = _motherCut)
    return Selection (name, Algorithm = _B, RequiredSelections = [PhiSel, EtacSel])
def makeBs2ChicPhi_Chic2KKKK(name, DecayDescriptors, PhiSel, ChicSel, Bs_VCHI2, Bs_DIRA, Bs_VDZ):
    _combinationCut = "(ADAMASS('B_s0') < 500 *MeV)"
    _motherCut = "(VFASPF(VZ) > %(Bs_VDZ)s *mm) & (BPVDIRA> %(Bs_DIRA)s) & (VFASPF(VCHI2/VDOF) < %(Bs_VCHI2)s)"  % locals()
    _B = CombineParticles(DecayDescriptors = DecayDescriptors, CombinationCut = _combinationCut, MotherCut = _motherCut)
    return Selection (name, Algorithm = _B, RequiredSelections = [PhiSel, ChicSel])
def makeBs2ChicPhi_Chic2KKPiPi(name, DecayDescriptors, PhiSel, ChicSel, Bs_VCHI2, Bs_DIRA, Bs_VDZ):
    _combinationCut = "(ADAMASS('B_s0') < 500 *MeV)"
    _motherCut = "(VFASPF(VZ) > %(Bs_VDZ)s *mm) & (BPVDIRA> %(Bs_DIRA)s) & (VFASPF(VCHI2/VDOF) < %(Bs_VCHI2)s)"  % locals()
    _B = CombineParticles(DecayDescriptors = DecayDescriptors, CombinationCut = _combinationCut, MotherCut = _motherCut)
    return Selection (name, Algorithm = _B, RequiredSelections = [PhiSel, ChicSel])
def makeBs2ChicPhi_Chic2PiPiPiPi(name, DecayDescriptors, PhiSel, ChicSel, Bs_VCHI2, Bs_DIRA, Bs_VDZ):
    _combinationCut = "(ADAMASS('B_s0') < 500 *MeV)"
    _motherCut = "(VFASPF(VZ) > %(Bs_VDZ)s *mm) & (BPVDIRA> %(Bs_DIRA)s) & (VFASPF(VCHI2/VDOF) < %(Bs_VCHI2)s)"  % locals()
    _B = CombineParticles(DecayDescriptors = DecayDescriptors, CombinationCut = _combinationCut, MotherCut = _motherCut)
    return Selection (name, Algorithm = _B, RequiredSelections = [PhiSel, ChicSel])
def makeB2EtacKst_Etac2KKKK(name, DecayDescriptors, KstSel, EtacSel, Bs_VCHI2, Bs_DIRA, Bs_VDZ):
    _combinationCut = "(ADAMASS('B0') < 500 *MeV)"
    _motherCut = "(VFASPF(VZ) > %(Bs_VDZ)s *mm) & (BPVDIRA> %(Bs_DIRA)s) & (VFASPF(VCHI2/VDOF) < %(Bs_VCHI2)s)"  % locals()
    _B = CombineParticles(DecayDescriptors = DecayDescriptors, CombinationCut = _combinationCut, MotherCut = _motherCut)
    return Selection (name, Algorithm = _B, RequiredSelections = [KstSel, EtacSel])
def makeB2EtacKst_Etac2KKPiPi(name, DecayDescriptors, KstSel, EtacSel, Bs_VCHI2, Bs_DIRA, Bs_VDZ):
    _combinationCut = "(ADAMASS('B0') < 500 *MeV)"
    _motherCut = "(VFASPF(VZ) > %(Bs_VDZ)s *mm) & (BPVDIRA> %(Bs_DIRA)s) & (VFASPF(VCHI2/VDOF) < %(Bs_VCHI2)s)"  % locals()
    _B = CombineParticles(DecayDescriptors = DecayDescriptors, CombinationCut = _combinationCut, MotherCut = _motherCut)
    return Selection (name, Algorithm = _B, RequiredSelections = [KstSel, EtacSel])
def makeB2EtacKst_Etac2PiPiPiPi(name, DecayDescriptors, KstSel, EtacSel, Bs_VCHI2, Bs_DIRA, Bs_VDZ):
    _combinationCut = "(ADAMASS('B0') < 500 *MeV)"
    _motherCut = "(VFASPF(VZ) > %(Bs_VDZ)s *mm) & (BPVDIRA> %(Bs_DIRA)s) & (VFASPF(VCHI2/VDOF) < %(Bs_VCHI2)s)"  % locals()
    _B = CombineParticles(DecayDescriptors = DecayDescriptors, CombinationCut = _combinationCut, MotherCut = _motherCut)
    return Selection (name, Algorithm = _B, RequiredSelections = [KstSel, EtacSel])
def makeB2ChicKst_Chic2KKKK(name, DecayDescriptors, KstSel, ChicSel, Bs_VCHI2, Bs_DIRA, Bs_VDZ):
    _combinationCut = "(ADAMASS('B0') < 500 *MeV)"
    _motherCut = "(VFASPF(VZ) > %(Bs_VDZ)s *mm) & (BPVDIRA> %(Bs_DIRA)s) & (VFASPF(VCHI2/VDOF) < %(Bs_VCHI2)s)"  % locals()
    _B = CombineParticles(DecayDescriptors = DecayDescriptors, CombinationCut = _combinationCut, MotherCut = _motherCut)
    return Selection (name, Algorithm = _B, RequiredSelections = [KstSel, ChicSel])
def makeB2ChicKst_Chic2KKPiPi(name, DecayDescriptors, KstSel, ChicSel, Bs_VCHI2, Bs_DIRA, Bs_VDZ):
    _combinationCut = "(ADAMASS('B0') < 500 *MeV)"
    _motherCut = "(VFASPF(VZ) > %(Bs_VDZ)s *mm) & (BPVDIRA> %(Bs_DIRA)s) & (VFASPF(VCHI2/VDOF) < %(Bs_VCHI2)s)"  % locals()
    _B = CombineParticles(DecayDescriptors = DecayDescriptors, CombinationCut = _combinationCut, MotherCut = _motherCut)
    return Selection (name, Algorithm = _B, RequiredSelections = [KstSel, ChicSel])
def makeB2ChicKst_Chic2PiPiPiPi(name, DecayDescriptors, KstSel, ChicSel, Bs_VCHI2, Bs_DIRA, Bs_VDZ):
    _combinationCut = "(ADAMASS('B0') < 500 *MeV)"
    _motherCut = "(VFASPF(VZ) > %(Bs_VDZ)s *mm) & (BPVDIRA> %(Bs_DIRA)s) & (VFASPF(VCHI2/VDOF) < %(Bs_VCHI2)s)"  % locals()
    _B = CombineParticles(DecayDescriptors = DecayDescriptors, CombinationCut = _combinationCut, MotherCut = _motherCut)
    return Selection (name, Algorithm = _B, RequiredSelections = [KstSel, ChicSel])
