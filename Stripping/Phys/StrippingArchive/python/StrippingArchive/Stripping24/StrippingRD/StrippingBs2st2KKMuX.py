
"""
Stripping lines for B -> K mu X study.

Stripping lines:
 Bs2st2KKMuLine:      B_s2*0 -> ( B+ -> K+ mu+/- ) K-
 Bs2st2KKMuWSLine:    B_s2*0 -> ( B+ -> K+ mu+/- ) K+
 Bs2st2KKJpsiLine:    B_s2*0 -> ( B+ -> K+ (J/psi -> mu+ mu-) ) K-
 Bs2st2KKJpsiWSLine:  B_s2*0 -> ( B+ -> K+ (J/psi -> mu+ mu-) ) K+
"""

__author__  = ['Jianchun Wang']
__date__    = '03/22/2015'
__version__ = '$Revision: 0.0 $'
__all__ = ( 'Bs2st2KKMuXConf',
            'default_config')

from Gaudi.Configuration import *
from GaudiConfUtils.ConfigurableGenerators import FilterDesktop, CombineParticles
from PhysSelPython.Wrappers import Selection, DataOnDemand
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder
from StandardParticles import StdLooseMuons, StdLooseKaons, StdAllLooseKaons, StdMassConstrainedJpsi2MuMu

default_config = {
    'NAME'        : 'Bs2st2KKMuX',
    'BUILDERTYPE' : 'Bs2st2KKMuXConf',
    'WGs'         : ['RD'],
    'STREAMS'     : ['Semileptonic'],
    'CONFIG'      : {
        'Bs2st2KKMuPrescale'    : 1.0,
        'Bs2st2KKMuWSPrescale'  : 1.0,
        'Bs2st2KKJpsiPrescale'  : 1.0,
        'Bs2st2KKJpsiWSPrescale': 1.0,

        'GEC_nLongTrk'     : 1000. ,# adimensional
        'HLT_FILTER'       : ''    ,

        # Kaon from B_s2*0 -> B K
        'K1PT'             : 500.  ,# MeV
        'K1PTLoose'        : 250.  ,# MeV
        'K1MinIPChi2'      : 9.    ,# adimensional
        'K1PIDK'           : 16.   ,# adimensional

        # Kaon from B -> K mu X
        'K2P'              : 3000. ,# MeV
        'K2PT'             : 1000. ,# MeV
        'K2MinIPChi2'      : 9.    ,# adimensional
        'K2PIDK'           : 0.    ,# adimensional

        # Muon from B -> K mu X
        'MuP'              : 3000. ,# MeV
        'MuPT'             : 1000. ,# MeV
        'MuMinIPChi2'      : 9.    ,# adimensional
        'MuPIDmu'          : 0.    ,# adimensional

        # J/psi from B -> K J/psi
        'JpsiMassWindow'   : 80.   ,# MeV

        # B -> K mu X
        'KMuVChi2Dof'      : 4.    ,# adimensional
        'KMuFdChi2'        : 100.  ,# adimensional
        'KMuMassMin'       : 1000. ,# MeV
        'KMuMassMax'       : 5500. ,# MeV

        # B -> K Jpsi
        'KJpsiVChi2Dof'    : 4.    ,# adimensional
        'KJpsiFdChi2'      : 25.   ,# adimensional
        'KJpsiMassMin'     : 5050. ,# MeV
        'KJpsiMassMax'     : 5500. ,# MeV

        # B_s2*0 -> B K
        'Bs2PT'            : 50.0   ,# MeV
        'DMKKMu'           : 713.677,# MeV (493.677 + 220)
        'DMKKJpsi'         : 1093.677,# MeV (493.677 + 600)
        'DZBPV'            : 1.0    ,# mm

        # Track isolation
        'RelatedInfoTools' : [
            {'Type' : 'RelInfoBs2MuMuTrackIsolations',
             'Variables' : [
                 'BSMUMUTRACKPLUSISO',
                 'BSMUMUTRACKPLUSISOTWO',
                 'ISOTWOBODYMASSISOPLUS',
                 'ISOTWOBODYCHI2ISOPLUS',
                 'ISOTWOBODYISO5PLUS',
                 'BSMUMUTRACKID'
                 ],
             'DaughterLocations' : {
                 "[B*_s20 -> (B+ -> ^K+ [mu-]cc) [K-]cc]CC": "K2ISO",
                 "[B*_s20 -> (B+ -> K+ ^[mu-]cc) [K-]cc]CC": "MuISO",
                 "[B*_s20 -> (B+ -> K+ [mu-]cc) ^[K-]cc]CC": "K1ISO"
                 },
             'tracktype'  : 3,
             'angle'      : 0.27,
             'fc'         : 0.60,
             'doca_iso'   : 0.13,
             'ips'        : 3.0,
             'svdis'      : -0.15,
             'svdis_h'    : 30.,
             'pvdis'      : 0.5,
             'pvdis_h'    : 40.,
             'makeTrackCuts' : False,
             'IsoTwoBody' : True
             }
            ]
        }
    }

class Bs2st2KKMuXConf(LineBuilder):

    __configuration_keys__ = (
        'Bs2st2KKMuPrescale'
        ,'Bs2st2KKMuWSPrescale'
        ,'Bs2st2KKJpsiPrescale'
        ,'Bs2st2KKJpsiWSPrescale'

        ,'GEC_nLongTrk'
        ,'HLT_FILTER'

        ,'K1PT'
        ,'K1PTLoose'
        ,'K1MinIPChi2'
        ,'K1PIDK'

        ,'K2P'
        ,'K2PT'
        ,'K2MinIPChi2'
        ,'K2PIDK'

        ,'MuP'
        ,'MuPT'
        ,'MuMinIPChi2'
        ,'MuPIDmu'

        ,'JpsiMassWindow'

        ,'KMuVChi2Dof'
        ,'KMuFdChi2'
        ,'KMuMassMin'
        ,'KMuMassMax'

        ,'KJpsiVChi2Dof'
        ,'KJpsiFdChi2'
        ,'KJpsiMassMin'
        ,'KJpsiMassMax'

        ,'Bs2PT'
        ,'DMKKMu'
        ,'DMKKJpsi'
        ,'DZBPV'

        ,'RelatedInfoTools'
        )

    __confdict__ = {}

    def __init__(self, name, config):
        LineBuilder.__init__(self, name, config)
        self.__confdict__ = config


        self.GECs = { "Code":"( recSummaryTrack(LHCb.RecSummary.nLongTracks, TrLONG) < %(GEC_nLongTrk)s )" % config,
                      "Preambulo": ["from LoKiTracks.decorators import *"]}

        #== selection of standard particles
        self.Kaon1Cuts = "(TRCHI2DOF < 3) & (TRGHOSTPROB < 0.5) & (PT > %(K1PT)s *MeV) &"\
                         "(MIPCHI2DV(PRIMARY) < %(K1MinIPChi2)s) & "\
                         "(PIDK-PIDpi > %(K1PIDK)s) & (PIDK-PIDp > 0) & (PIDK-PIDmu > 0)" %config

        self.Kaon1CutsLoose = "(TRCHI2DOF < 3) & (TRGHOSTPROB < 0.5) & (PT > %(K1PTLoose)s *MeV) &"\
                              "(MIPCHI2DV(PRIMARY) < %(K1MinIPChi2)s) & "\
                              "(PIDK-PIDpi > %(K1PIDK)s) & (PIDK-PIDp > 0) & (PIDK-PIDmu > 0)" %config

        self.Kaon2Cuts = "(TRCHI2DOF < 3) & (TRGHOSTPROB < 0.5) & "\
                         "(P > %(K2P)s *MeV) & (PT > %(K2PT)s *MeV) & "\
                         "(MIPCHI2DV(PRIMARY) > %(K2MinIPChi2)s) & "\
                         "(PIDK-PIDpi > %(K2PIDK)s) & (PIDK-PIDp > 0) & (PIDK-PIDmu > 0)" %config

        self.MuonCuts  = "(TRCHI2DOF < 3) & (TRGHOSTPROB < 0.5) & "\
                         "(P > %(MuP)s *MeV) & (PT > %(MuPT)s *MeV) & "\
                         "(MIPCHI2DV(PRIMARY) > %(MuMinIPChi2)s) & "\
                         "(PIDmu-PIDK > 0) & (PIDmu-PIDp > 0) & (PIDmu-PIDpi > %(MuPIDmu)s)" %config

        self.JpsiCuts  = "(PFUNA(ADAMASS('J/psi(1S)')) < %(JpsiMassWindow)s * MeV)" %config

        self.SelKaon1 = Selection("K1_for" + name,
                                  Algorithm = FilterDesktop(Code=self.Kaon1Cuts),
                                  RequiredSelections = [StdAllLooseKaons])

        self.SelKaon1Loose = Selection("K1Loose_for" + name,
                                       Algorithm = FilterDesktop(Code=self.Kaon1CutsLoose),
                                       RequiredSelections = [StdAllLooseKaons])

        self.SelKaon2 = Selection("K2_for" + name,
                                  Algorithm = FilterDesktop(Code=self.Kaon2Cuts),
                                  RequiredSelections = [StdLooseKaons])

        self.SelMuon  = Selection("Mu_for" + name,
                                  Algorithm = FilterDesktop(Code=self.MuonCuts),
                                  RequiredSelections = [StdLooseMuons])

        self.SelJpsi  = Selection("Jpsi_for" + name,
                                  Algorithm = FilterDesktop(Code=self.JpsiCuts),
                                  RequiredSelections = [StdMassConstrainedJpsi2MuMu])


        # B -> K Mu X, both opposite & same signed K mu can be signals
        self.Bu2KMu = CombineParticles(DecayDescriptors = ["[B+ -> K+ mu-]cc", "[B+ -> K+ mu+]cc"])
        self.Bu2KMu.CombinationCut = "(AM > %(KMuMassMin)s*MeV) & (AM < %(KMuMassMax)s*MeV)" % config
        self.Bu2KMu.MotherCut      = "(VFASPF(VCHI2/VDOF) < %(KMuVChi2Dof)s) & (BPVDIRA > 0.99) & "\
                                     "(BPVVDCHI2 > %(KMuFdChi2)s)" % config
        self.Bu2KMu.ReFitPVs       = True

        self.SelBu2KMu = Selection("Bu2KMu_for" + name,
                                   Algorithm          = self.Bu2KMu,
                                   RequiredSelections = [self.SelKaon2, self.SelMuon])

        # B -> K J/psi
        self.Bu2KJpsi = CombineParticles(DecayDescriptors = ["[B+ -> J/psi(1S) K+]cc"])
        self.Bu2KJpsi.CombinationCut = "(AM > %(KJpsiMassMin)s*MeV) & (AM < %(KJpsiMassMax)s*MeV)" % config
        self.Bu2KJpsi.MotherCut      = "(VFASPF(VCHI2/VDOF) < %(KJpsiVChi2Dof)s) & (BPVDIRA > 0.99) & "\
                                       "(BPVVDCHI2 > %(KJpsiFdChi2)s)" % config
        self.Bu2KJpsi.ReFitPVs       = True

        self.SelBu2KJpsi = Selection("Bu2KJpsi_for" + name,
                                     Algorithm          = self.Bu2KJpsi,
                                     RequiredSelections = [self.SelKaon2, self.SelJpsi])


        # B_s2*0 -> (B -> K Mu X) K, signal KK have opposite signed charge.
        self.Bs2KKMu = CombineParticles( DecayDescriptor = "[B*_s20 -> B+ K-]cc")
        self.Bs2KKMu.CombinationCut = "(AM-AM1 < %(DMKKMu)s*MeV) & "\
                                      "(abs(ACHILD(BPV(VZ),1)-ACHILD(BPV(VZ),2))<%(DZBPV)s*mm)" % config
        self.Bs2KKMu.MotherCut      = "(PT > %(Bs2PT)s*MeV)" % config
        self.Bs2KKMu.ReFitPVs       = False

        self.SelBs2KKMu = Selection("Bs2KKMu_for" + name,
                                    Algorithm          = self.Bs2KKMu,
                                    RequiredSelections = [self.SelBu2KMu, self.SelKaon1])

        # B_s2*0 -> (B -> K Mu X) K, same signed KK for back ground
        self.Bs2KKMuWS = CombineParticles( DecayDescriptor = "[B*_s20 -> B+ K+]cc")
        self.Bs2KKMuWS.CombinationCut = self.Bs2KKMu.CombinationCut
        self.Bs2KKMuWS.MotherCut      = self.Bs2KKMu.MotherCut
        self.Bs2KKMuWS.ReFitPVs       = self.Bs2KKMu.ReFitPVs

        self.SelBs2KKMuWS = Selection("Bs2KKMuWS_for" + name,
                                      Algorithm          = self.Bs2KKMuWS,
                                      RequiredSelections = [self.SelBu2KMu, self.SelKaon1])

        # B_s2*0 -> (B -> K Jpsi) K, signal KK have opposite signed charge.
        self.Bs2KKJpsi = CombineParticles( DecayDescriptor = "[B*_s20 -> B+ K-]cc")
        self.Bs2KKJpsi.CombinationCut = "(AM-AM1 < %(DMKKJpsi)s*MeV) & "\
                                        "(abs(ACHILD(BPV(VZ),1)-ACHILD(BPV(VZ),2))<%(DZBPV)s*mm)" % config
        self.Bs2KKJpsi.MotherCut      = "(PT > %(Bs2PT)s*MeV)" % config
        self.Bs2KKJpsi.ReFitPVs       = False

        self.SelBs2KKJpsi = Selection("Bs2KKJpsi_for" + name,
                                      Algorithm          = self.Bs2KKJpsi,
                                      RequiredSelections = [self.SelBu2KJpsi, self.SelKaon1Loose])

        # B_s2*0 -> (B -> K Jpsi) K, same signed KK for back ground
        self.Bs2KKJpsiWS = CombineParticles( DecayDescriptor = "[B*_s20 -> B+ K+]cc")
        self.Bs2KKJpsiWS.CombinationCut = self.Bs2KKJpsi.CombinationCut
        self.Bs2KKJpsiWS.MotherCut      = self.Bs2KKJpsi.MotherCut
        self.Bs2KKJpsiWS.ReFitPVs       = self.Bs2KKJpsi.ReFitPVs

        self.SelBs2KKJpsiWS = Selection("Bs2KKJpsiWS_for" + name,
                                        Algorithm          = self.Bs2KKJpsiWS,
                                        RequiredSelections = [self.SelBu2KJpsi, self.SelKaon1Loose])


        # register stripping lines
        self.Bs2st2KKMuLine = StrippingLine("Bs2st2KKMuLine",
                                            prescale  = config['Bs2st2KKMuPrescale'],
                                            FILTER    = self.GECs,
                                            RelatedInfoTools = config['RelatedInfoTools'],
                                            selection = self.SelBs2KKMu
                                            #, MDSTFlag  = True
                                            )

        self.Bs2st2KKMuWSLine = StrippingLine("Bs2st2KKMuWSLine",
                                              prescale  = config['Bs2st2KKMuWSPrescale'],
                                              FILTER    = self.GECs,
                                              RelatedInfoTools = self.Bs2st2KKMuLine.RelatedInfoTools,
                                              selection = self.SelBs2KKMuWS
                                              #, MDSTFlag  = True
                                              )

        self.Bs2st2KKJpsiLine = StrippingLine("Bs2st2KKJpsiLine",
                                              prescale  = config['Bs2st2KKMuPrescale'],
                                              FILTER    = self.GECs,
                                              selection = self.SelBs2KKJpsi
                                              #, MDSTFlag  = True
                                              )

        self.Bs2st2KKJpsiWSLine = StrippingLine("Bs2st2KKJpsiWSLine",
                                                prescale  = config['Bs2st2KKJpsiWSPrescale'],
                                                FILTER    = self.GECs,
                                                selection = self.SelBs2KKJpsiWS
                                                #, MDSTFlag  = True
                                                )

        # register stripping lines
        self.registerLine(self.Bs2st2KKMuLine)
        self.registerLine(self.Bs2st2KKMuWSLine)
        self.registerLine(self.Bs2st2KKJpsiLine)
        self.registerLine(self.Bs2st2KKJpsiWSLine)
