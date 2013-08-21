#Stripping Lines for Low Multiplicity Processes.
#Electroweak Group (Convenor: Tara Shears)
#Adaptation of lines (to use line builders) originally designed by Dermot Moran by Will Barter

# Accepts events that passed the relevant HLT line.

from Gaudi.Configuration import *
from GaudiConfUtils.ConfigurableGenerators import FilterDesktop, CombineParticles
from PhysSelPython.Wrappers import Selection, DataOnDemand
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder

from StandardParticles import StdAllNoPIDsKaons, StdAllNoPIDsPions, StdAllNoPIDsProtons
from GaudiKernel.SystemOfUnits import MeV, mm

confdict_LowMult = {
    'LowMultPrescale'           : 1.0
    , 'LowMultZeroPrescale'     : 0.0
    , 'LowMultWSPrescale'       : 0.1
    , 'LowMultHHIncPrescale'    : 0.1
    , 'LowMultLMRPrescale'      : 0.2
    , 'LowMultPrescale_ps'      : 0.005
    , 'LowMultNoFilterPrescale' : 0.1
    , 'LowMultPostscale'        : 1.0
    # Final-state particles
    , 'H_PTmin'         : 100.0 * MeV
    , 'H_Pmin'          : 5000.0 * MeV
    , 'H_TrkChi2max'    : 3.0
    , 'K_PIDKmin'       : 0.0
    , 'P_PIDPmin'       : 0.0
    , 'KKInc_K_Pmin'    : 10000.0 * MeV
    , 'KKInc_K_PIDKmin' : 5.0
    # D0 -> KPi
    , 'D2KPi_APTmin'        : 0.0 * MeV
    , 'D2KPi_ADAMASSmax'    : 80.0 * MeV
    , 'D2KPi_ADOCAmax'      : 0.5 * mm
    , 'D2KPi_APmin'         : 10000.0 * MeV
    , 'D2KPi_VtxChi2DoFmax' : 15.0
    # D+- -> KPiPi
    , 'D2KPiPi_APTmin'        : 0.0 * MeV
    , 'D2KPiPi_ADAMASSmax'    : 80.0 * MeV
    , 'D2KPiPi_ADOCAmax'      : 0.5 * mm
    , 'D2KPiPi_APmin'         : 10000.0 * MeV
    , 'D2KPiPi_VtxChi2DoFmax' : 15.0
    # D0 -> K3Pi
    , 'D2K3Pi_APTmin'        : 0.0 * MeV
    , 'D2K3Pi_ADAMASSmax'    : 80.0 * MeV
    , 'D2K3Pi_ADOCAmax'      : 0.7 * mm
    , 'D2K3Pi_APmin'         : 10000.0 * MeV
    , 'D2K3Pi_VtxChi2DoFmax' : 15.0
    # 'ChiC' -> HH (H = K, Pi)
    , 'ChiC2HH_APTmin'        : 0.0 * MeV
    , 'ChiC2HH_APTmax'        : 5000.0 * MeV
    , 'ChiC2HH_AMmin'         : 2850.0 * MeV
    , 'ChiC2HH_AMmax'         : 4500.0 * MeV
    , 'ChiC2HH_ADOCAmax'      : 0.5 * mm
    , 'ChiC2HH_APmin'         : 10000.0 * MeV
    , 'ChiC2HH_VtxChi2DoFmax' : 15.0
    # 'ChiC' -> PP
    , 'ChiC2PP_APTmin'        : 0.0 * MeV
    , 'ChiC2PP_APTmax'        : 5000.0 * MeV
    , 'ChiC2PP_AMmin'         : 2850.0 * MeV
    , 'ChiC2PP_AMmax'         : 3650.0 * MeV
    , 'ChiC2PP_ADOCAmax'      : 0.5 * mm
    , 'ChiC2PP_APmin'         : 10000.0 * MeV
    , 'ChiC2PP_VtxChi2DoFmax' : 15.0
    # 'ChiC' -> HHHH (H = K, Pi)
    , 'ChiC2HHHH_APTmin'        : 0.0 * MeV
    , 'ChiC2HHHH_APTmax'        : 5000.0 * MeV
    , 'ChiC2HHHH_AMmin'         : 2850.0 * MeV
    , 'ChiC2HHHH_AMmax'         : 4500.0 * MeV
    , 'ChiC2HHHH_ADOCAmax'      : 0.7 * mm
    , 'ChiC2HHHH_APmin'         : 10000.0 * MeV
    , 'ChiC2HHHH_VtxChi2DoFmax' : 15.0
    # Low-mass resonance -> HH (H = K, Pi)
    , 'LMR2HH_APTmin'        : 500.0 * MeV
    , 'LMR2HH_APTmax'        : 1500.0 * MeV
    , 'LMR2HH_AMmin'         : 450.0 * MeV
    , 'LMR2HH_AMmax'         : 1500.0 * MeV
    , 'LMR2HH_ADOCAmax'      : 0.1 * mm
    , 'LMR2HH_APmin'         : 15000.0 * MeV
    , 'LMR2HH_VtxChi2DoFmax' : 3.0
    # Phi resonance -> KK
    , 'PHI2KK_APTmin'        : 0.0 * MeV
    , 'PHI2KK_APTmax'        : 1500.0 * MeV
    , 'PHI2KK_AMmin'         : 990.0 * MeV
    , 'PHI2KK_AMmax'         : 1050.0 * MeV
    , 'PHI2KK_ADOCAmax'      : 0.1 * mm
    , 'PHI2KK_APmin'         : 4000.0 * MeV
    , 'PHI2KK_VtxChi2DoFmax' : 3.0
    }

default_name = "LowMult"

class LowMultConf(LineBuilder) :

    __configuration_keys__ = ('LowMultPrescale'
                              , 'LowMultZeroPrescale'
                              , 'LowMultWSPrescale'
                              , 'LowMultHHIncPrescale'
                              , 'LowMultLMRPrescale'
                              , 'LowMultPrescale_ps'
                              , 'LowMultNoFilterPrescale'
                              , 'LowMultPostscale'
                              # Final-state particles
                              , 'H_PTmin'
                              , 'H_Pmin'
                              , 'H_TrkChi2max'
                              , 'K_PIDKmin'
                              , 'P_PIDPmin'
                              , 'KKInc_K_Pmin'
                              , 'KKInc_K_PIDKmin'
                              # D0 -> KPi
                              , 'D2KPi_APTmin'
                              , 'D2KPi_ADAMASSmax'
                              , 'D2KPi_ADOCAmax'
                              , 'D2KPi_APmin'
                              , 'D2KPi_VtxChi2DoFmax'
                              # D+- -> KPiPi
                              , 'D2KPiPi_APTmin'
                              , 'D2KPiPi_ADAMASSmax'
                              , 'D2KPiPi_ADOCAmax'
                              , 'D2KPiPi_APmin'
                              , 'D2KPiPi_VtxChi2DoFmax'
                              # D0 -> K3Pi
                              , 'D2K3Pi_APTmin'
                              , 'D2K3Pi_ADAMASSmax'
                              , 'D2K3Pi_ADOCAmax'
                              , 'D2K3Pi_APmin'
                              , 'D2K3Pi_VtxChi2DoFmax'
                              # 'ChiC' -> HH (H = K, Pi)
                              , 'ChiC2HH_APTmin'
                              , 'ChiC2HH_APTmax'
                              , 'ChiC2HH_AMmin'
                              , 'ChiC2HH_AMmax'
                              , 'ChiC2HH_ADOCAmax'
                              , 'ChiC2HH_APmin'
                              , 'ChiC2HH_VtxChi2DoFmax'
                              # 'ChiC' -> PP
                              , 'ChiC2PP_APTmin'
                              , 'ChiC2PP_APTmax'
                              , 'ChiC2PP_AMmin'
                              , 'ChiC2PP_AMmax'
                              , 'ChiC2PP_ADOCAmax'
                              , 'ChiC2PP_APmin'
                              , 'ChiC2PP_VtxChi2DoFmax'
                              # 'ChiC' -> HHHH
                              , 'ChiC2HHHH_APTmin'
                              , 'ChiC2HHHH_APTmax'
                              , 'ChiC2HHHH_AMmin'
                              , 'ChiC2HHHH_AMmax'
                              , 'ChiC2HHHH_ADOCAmax'
                              , 'ChiC2HHHH_APmin'
                              , 'ChiC2HHHH_VtxChi2DoFmax'
                              # Low-mass resonance -> HH (H = K, Pi)
                              , 'LMR2HH_APTmin'
                              , 'LMR2HH_APTmax'
                              , 'LMR2HH_AMmin'
                              , 'LMR2HH_AMmax'
                              , 'LMR2HH_ADOCAmax'
                              , 'LMR2HH_APmin'
                              , 'LMR2HH_VtxChi2DoFmax'
                              # Phi -> KK
                              , 'PHI2KK_APTmin'
                              , 'PHI2KK_APTmax'
                              , 'PHI2KK_AMmin'
                              , 'PHI2KK_AMmax'
                              , 'PHI2KK_ADOCAmax'
                              , 'PHI2KK_APmin'
                              , 'PHI2KK_VtxChi2DoFmax'

                             )

    def __init__(self, name, config) :
        LineBuilder.__init__(self, name, config)

        self._myname = name

        #MUON

        ExclusiveMuonGEC = {'Code' : "(recSummaryTrack(LHCb.RecSummary.nLongTracks, TrLONG) > 0) & " \
                            "(recSummaryTrack(LHCb.RecSummary.nBackTracks, TrBACKWARD) < 1) & " \
                            "(recSummary(LHCb.RecSummary.nTracks, 'Rec/Track/Best') < 6)",
                            'Preambulo' : ["from LoKiTracks.decorators import *"]}

        self.LowMultMuon_line = StrippingLine(self._myname+"MuonLine",
                                              prescale = config['LowMultZeroPrescale'],
                                              postscale = config['LowMultPostscale'],
                                              checkPV = False,
                                              FILTER = ExclusiveMuonGEC,
                                              HLT = "HLT_PASS('Hlt2LowMultMuonDecision')"
                                              )

        self.registerLine(self.LowMultMuon_line)

        self.LowMultMuon_lineps = StrippingLine(self._myname+"MuonLinePS",
                                                prescale = config['LowMultZeroPrescale'],
                                                postscale = config['LowMultPostscale'],
                                                checkPV = False,
                                                HLT = "HLT_PASS('Hlt2LowMultMuonDecision')"
                                                )

        self.registerLine(self.LowMultMuon_lineps)

        ExclusiveDiMuonGEC = {'Code' : "(recSummaryTrack(LHCb.RecSummary.nLongTracks, TrLONG) > 0) & " \
                              "(recSummaryTrack(LHCb.RecSummary.nBackTracks, TrBACKWARD) < 1)",
                              'Preambulo' : ["from LoKiTracks.decorators import *"]}

        self.LowMultPP2PPMuMu_line = StrippingLine(self._myname+"PP2PPMuMuLine",
                                                   prescale = config['LowMultZeroPrescale'],
                                                   postscale = config['LowMultPostscale'],
                                                   checkPV = False,
                                                   FILTER = ExclusiveDiMuonGEC,
                                                   HLT = "HLT_PASS('Hlt2diPhotonDiMuonDecision')"
                                                   )

        self.registerLine(self.LowMultPP2PPMuMu_line)

        self.LowMultPP2PPMuMu_lineps = StrippingLine(self._myname+"PP2PPMuMuLinePS",
                                                     prescale = config['LowMultZeroPrescale'],
                                                     postscale = config['LowMultPostscale'],
                                                     checkPV = False,
                                                     HLT = "HLT_PASS('Hlt2diPhotonDiMuonDecision')"
                                                     )

        self.registerLine(self.LowMultPP2PPMuMu_lineps)

        #ELECTRON

        ExclusiveElectronGEC = {'Code' : "(recSummaryTrack(LHCb.RecSummary.nBackTracks, TrBACKWARD) < 1) & " \
                                "(recSummary(LHCb.RecSummary.nTracks, 'Rec/Track/Best') < 6)",
                                'Preambulo' : ["from LoKiTracks.decorators import *"]}

        self.LowMultElectron_line = StrippingLine(self._myname+"ElectronLine",
                                                  prescale = config['LowMultZeroPrescale'],
                                                  postscale = config['LowMultPostscale'],
                                                  checkPV = False,
                                                  FILTER = ExclusiveElectronGEC,
                                                  HLT = "HLT_PASS('Hlt2LowMultElectronDecision')"
                                                  )

        self.registerLine(self.LowMultElectron_line)

        self.LowMultElectron_lineps = StrippingLine(self._myname+"ElectronLinePS",
                                                    prescale = config['LowMultZeroPrescale'],
                                                    postscale = config['LowMultPostscale'],
                                                    checkPV = False,
                                                    HLT = "HLT_PASS('Hlt2LowMultElectronDecision')"
                                                    )

        self.registerLine(self.LowMultElectron_lineps)

        self.LowMultElectron_nofilter_line = StrippingLine(self._myname+"ElectronLineNoFilter",
                                                           prescale = config['LowMultZeroPrescale'],
                                                           postscale = config['LowMultPostscale'],
                                                           checkPV = False,
                                                           HLT = "HLT_PASS('Hlt2LowMultElectron_nofilterDecision')"
                                                           )

        self.registerLine(self.LowMultElectron_nofilter_line)


        #HADRON

        ExclusiveHadronGEC = {'Code' : "(recSummaryTrack(LHCb.RecSummary.nLongTracks, TrLONG) > 1) & " \
                              "(recSummaryTrack(LHCb.RecSummary.nBackTracks, TrBACKWARD) < 1) & " \
                              "(CONTAINS('Rec/Track/Best') < 6) ",
                              'Preambulo' : ["from LoKiTracks.decorators import *"]}

        self.LowMultHadron_line = StrippingLine(self._myname+"HadronLine",
                                                prescale = config['LowMultZeroPrescale'],
                                                postscale = config['LowMultPostscale'],
                                                checkPV = False,
                                                FILTER = ExclusiveHadronGEC,
                                                HLT = "HLT_PASS('Hlt2LowMultHadronDecision')"
                                                )

        self.registerLine(self.LowMultHadron_line)


        self.LowMultHadron_lineps = StrippingLine(self._myname+"HadronLinePS",
                                                  prescale = config['LowMultZeroPrescale'],
                                                  postscale = config['LowMultPostscale'],
                                                  checkPV = False,
                                                  HLT = "HLT_PASS('Hlt2LowMultHadronDecision')"
                                                  )

        self.registerLine(self.LowMultHadron_lineps)

        self.LowMultHadron_nofilter_line = StrippingLine(self._myname+"HadronLineNoFilter",
                                                         prescale = config['LowMultZeroPrescale'],
                                                         postscale = config['LowMultPostscale'],
                                                         checkPV = False,
                                                         HLT = "HLT_PASS('Hlt2LowMultHadron_nofilterDecision')"
                                                         )

        self.registerLine(self.LowMultHadron_nofilter_line)


        #PHOTON

        self.LowMultPhoton_line = StrippingLine(self._myname+"PhotonLine",
                                                prescale = config['LowMultZeroPrescale'],
                                                postscale = config['LowMultPostscale'],
                                                checkPV = False,
                                                HLT = "HLT_PASS('Hlt2LowMultPhotonDecision')"
                                                )

        self.registerLine(self.LowMultPhoton_line)

        #########
        ## CEP ##
        #########

        #
        #=== HLT and GEC filters ===#
        #

        CEPHLTReq = "HLT_PASS_RE('Hlt2LowMult(D.*|C.*|Hadron)Decision')"

        CEPFilterTracks = {'Code' :
                           "(recSummaryTrack(LHCb.RecSummary.nLongTracks, TrLONG) > 1) & " \
                           "(recSummaryTrack(LHCb.RecSummary.nBackTracks, TrBACKWARD) < 1) & " \
                           "(CONTAINS ('Rec/Track/Best') < 12)",
                           'Preambulo' : ["from LoKiTracks.decorators import *"]}

        CEPFilterTracksChiC2HH = {'Code' :
                                  "(recSummaryTrack(LHCb.RecSummary.nLongTracks, TrLONG) > 1) & " \
                                  "(recSummaryTrack(LHCb.RecSummary.nBackTracks, TrBACKWARD) < 1) & " \
                                  "(CONTAINS ('Rec/Track/Best') < 6)",
                                  'Preambulo' : ["from LoKiTracks.decorators import *"]}

        CEPFilterTracksChiC2HHHH = {'Code' :
                                    "(recSummaryTrack(LHCb.RecSummary.nLongTracks, TrLONG) > 1) & " \
                                    "(recSummaryTrack(LHCb.RecSummary.nBackTracks, TrBACKWARD) < 1) & " \
                                    "(CONTAINS ('Rec/Track/Best') < 8)",
                                    'Preambulo' : ["from LoKiTracks.decorators import *"]}

        #
        #=== Combinatorics ===#
        #

        self.selKaons = makeKaons('KaonsFor' + name,
                                  H_PTmin = config['H_PTmin'],
                                  H_Pmin = config['H_Pmin'],
                                  H_TrkChi2max = config['H_TrkChi2max'],
                                  K_PIDKmin = config['K_PIDKmin']
                                  )

        self.selKaonsForKK = makeKaonsForKK('KaonsForKKFor' + name,
                                            H_PTmin = config['H_PTmin'],
                                            KKInc_K_Pmin = config['KKInc_K_Pmin'],
                                            H_TrkChi2max = config['H_TrkChi2max'],
                                            KKInc_K_PIDKmin = config['KKInc_K_PIDKmin']
                                            )

        self.selPions = makePions('PionsFor' + name,
                                  H_PTmin = config['H_PTmin'],
                                  H_Pmin = config['H_Pmin'],
                                  H_TrkChi2max = config['H_TrkChi2max']
                                  )

        self.selProtons = makeProtons('ProtonsFor' + name,
                                      H_PTmin = config['H_PTmin'],
                                      H_Pmin = config['H_Pmin'],
                                      H_TrkChi2max = config['H_TrkChi2max'],
                                      P_PIDPmin = config['P_PIDPmin']
                                      )

        self.selD2KPi = makeD2KPi("selD2KPi",
                                  decayDesc = [ "[D0 -> K- pi+]cc" ],
                                  kaons = self.selKaons,
                                  pions = self.selPions,
                                  D2KPi_APTmin = config['D2KPi_APTmin'],
                                  D2KPi_ADAMASSmax = config['D2KPi_ADAMASSmax'],
                                  D2KPi_ADOCAmax = config['D2KPi_ADOCAmax'],
                                  D2KPi_APmin = config['D2KPi_APmin'],
                                  D2KPi_VtxChi2DoFmax = config['D2KPi_VtxChi2DoFmax']
                                  )

        self.selD2KpPipPim = makeD2KPiPi("selD2KpPipPim",
                                         decayDesc = [ "[D+ -> K+ pi+ pi-]cc" ],
                                         kaons = self.selKaons,
                                         pions = self.selPions,
                                         D2KPiPi_APTmin = config['D2KPiPi_APTmin'],
                                         D2KPiPi_ADAMASSmax = config['D2KPiPi_ADAMASSmax'],
                                         D2KPiPi_ADOCAmax = config['D2KPiPi_ADOCAmax'],
                                         D2KPiPi_APmin = config['D2KPiPi_APmin'],
                                         D2KPiPi_VtxChi2DoFmax = config['D2KPiPi_VtxChi2DoFmax']
                                         )

        self.selD2KmPipPip = makeD2KPiPi("selD2KmPipPip",
                                         decayDesc = [ "[D+ -> K- pi+ pi+]cc" ],
                                         kaons = self.selKaons,
                                         pions = self.selPions,
                                         D2KPiPi_APTmin = config['D2KPiPi_APTmin'],
                                         D2KPiPi_ADAMASSmax = config['D2KPiPi_ADAMASSmax'],
                                         D2KPiPi_ADOCAmax = config['D2KPiPi_ADOCAmax'],
                                         D2KPiPi_APmin = config['D2KPiPi_APmin'],
                                         D2KPiPi_VtxChi2DoFmax = config['D2KPiPi_VtxChi2DoFmax']
                                         )

        self.selD2K3Pi = makeD2K3Pi("selD2K3Pi",
                                    decayDesc = [ "[D0 -> K- pi+ pi- pi+]cc" ],
                                    kaons = self.selKaons,
                                    pions = self.selPions,
                                    D2K3Pi_APTmin = config['D2K3Pi_APTmin'],
                                    D2K3Pi_ADAMASSmax = config['D2K3Pi_ADAMASSmax'],
                                    D2K3Pi_ADOCAmax = config['D2K3Pi_ADOCAmax'],
                                    D2K3Pi_APmin = config['D2K3Pi_APmin'],
                                    D2K3Pi_VtxChi2DoFmax = config['D2K3Pi_VtxChi2DoFmax']
                                    )

        self.selChiC2KK = makeChiC2KK("selChiC2KK",
                                      decayDesc = [ "chi_c1(1P) -> K+ K-" ],
                                      kaons = self.selKaons,
                                      ChiC2KK_APTmin = config['ChiC2HH_APTmin'],
                                      ChiC2KK_APTmax = config['ChiC2HH_APTmax'],
                                      ChiC2KK_AMmin = config['ChiC2HH_AMmin'],
                                      ChiC2KK_AMmax = config['ChiC2HH_AMmax'],
                                      ChiC2KK_ADOCAmax = config['ChiC2HH_ADOCAmax'],
                                      ChiC2KK_APmin = config['ChiC2HH_APmin'],
                                      ChiC2KK_VtxChi2DoFmax = config['ChiC2HH_VtxChi2DoFmax']
                                      )

        self.selChiC2PiPi = makeChiC2PiPi("selChiC2PiPi",
                                          decayDesc = [ "chi_c1(1P) -> pi+ pi-" ],
                                          pions = self.selPions,
                                          ChiC2PiPi_APTmin = config['ChiC2HH_APTmin'],
                                          ChiC2PiPi_APTmax = config['ChiC2HH_APTmax'],
                                          ChiC2PiPi_AMmin = config['ChiC2HH_AMmin'],
                                          ChiC2PiPi_AMmax = config['ChiC2HH_AMmax'],
                                          ChiC2PiPi_ADOCAmax = config['ChiC2HH_ADOCAmax'],
                                          ChiC2PiPi_APmin = config['ChiC2HH_APmin'],
                                          ChiC2PiPi_VtxChi2DoFmax = config['ChiC2HH_VtxChi2DoFmax']
                                          )

        self.selChiC2PP = makeChiC2PP("selChiC2PP",
                                      decayDesc = [ "chi_c1(1P) -> p+ p~-" ],
                                      protons = self.selProtons,
                                      ChiC2PP_APTmin = config['ChiC2PP_APTmin'],
                                      ChiC2PP_APTmax = config['ChiC2PP_APTmax'],
                                      ChiC2PP_AMmin = config['ChiC2PP_AMmin'],
                                      ChiC2PP_AMmax = config['ChiC2PP_AMmax'],
                                      ChiC2PP_ADOCAmax = config['ChiC2PP_ADOCAmax'],
                                      ChiC2PP_APmin = config['ChiC2PP_APmin'],
                                      ChiC2PP_VtxChi2DoFmax = config['ChiC2PP_VtxChi2DoFmax']
                                      )

        self.selChiC2KpKpPimPim = makeChiC2KKPiPi("selChiC2KpKpPimPim",
                                                  decayDesc = [ "[chi_c1(1P) -> K+ K+ pi- pi-]cc" ],
                                                  kaons = self.selKaons,
                                                  pions = self.selPions,
                                                  ChiC2KKPiPi_APTmin = config['ChiC2HHHH_APTmin'],
                                                  ChiC2KKPiPi_APTmax = config['ChiC2HHHH_APTmax'],
                                                  ChiC2KKPiPi_AMmin = config['ChiC2HHHH_AMmin'],
                                                  ChiC2KKPiPi_AMmax = config['ChiC2HHHH_AMmax'],
                                                  ChiC2KKPiPi_ADOCAmax = config['ChiC2HHHH_ADOCAmax'],
                                                  ChiC2KKPiPi_APmin = config['ChiC2HHHH_APmin'],
                                                  ChiC2KKPiPi_VtxChi2DoFmax = config['ChiC2HHHH_VtxChi2DoFmax']
                                                  )

        self.selChiC2KpKmPipPim = makeChiC2KKPiPi("selChiC2KpKmPipPim",
                                                  decayDesc = [ "chi_c1(1P) -> K+ K- pi+ pi-" ],
                                                  kaons = self.selKaons,
                                                  pions = self.selPions,
                                                  ChiC2KKPiPi_APTmin = config['ChiC2HHHH_APTmin'],
                                                  ChiC2KKPiPi_APTmax = config['ChiC2HHHH_APTmax'],
                                                  ChiC2KKPiPi_AMmin = config['ChiC2HHHH_AMmin'],
                                                  ChiC2KKPiPi_AMmax = config['ChiC2HHHH_AMmax'],
                                                  ChiC2KKPiPi_ADOCAmax = config['ChiC2HHHH_ADOCAmax'],
                                                  ChiC2KKPiPi_APmin = config['ChiC2HHHH_APmin'],
                                                  ChiC2KKPiPi_VtxChi2DoFmax = config['ChiC2HHHH_VtxChi2DoFmax']
                                                  )

        self.selChiC2KKKK = makeChiC2KKKK("selChiC2KKKK",
                                          decayDesc = [ "chi_c1(1P) -> K+ K+ K- K-", ],
                                          kaons = self.selKaons,
                                          ChiC2KKKK_APTmin = config['ChiC2HHHH_APTmin'],
                                          ChiC2KKKK_APTmax = config['ChiC2HHHH_APTmax'],
                                          ChiC2KKKK_AMmin = config['ChiC2HHHH_AMmin'],
                                          ChiC2KKKK_AMmax = config['ChiC2HHHH_AMmax'],
                                          ChiC2KKKK_ADOCAmax = config['ChiC2HHHH_ADOCAmax'],
                                          ChiC2KKKK_APmin = config['ChiC2HHHH_APmin'],
                                          ChiC2KKKK_VtxChi2DoFmax = config['ChiC2HHHH_VtxChi2DoFmax']
                                          )

        self.selChiC2PiPiPiPi = makeChiC2PiPiPiPi("selChiC2PiPiPiPi",
                                                  decayDesc = [ "chi_c1(1P) -> pi+ pi+ pi- pi-" ],
                                                  pions = self.selPions,
                                                  ChiC2PiPiPiPi_APTmin = config['ChiC2HHHH_APTmin'],
                                                  ChiC2PiPiPiPi_APTmax = config['ChiC2HHHH_APTmax'],
                                                  ChiC2PiPiPiPi_AMmin = config['ChiC2HHHH_AMmin'],
                                                  ChiC2PiPiPiPi_AMmax = config['ChiC2HHHH_AMmax'],
                                                  ChiC2PiPiPiPi_ADOCAmax = config['ChiC2HHHH_ADOCAmax'],
                                                  ChiC2PiPiPiPi_APmin = config['ChiC2HHHH_APmin'],
                                                  ChiC2PiPiPiPi_VtxChi2DoFmax = config['ChiC2HHHH_VtxChi2DoFmax']
                                                  )

        self.selD0D0 = makeDD("selD0D0",
                              decayDesc = [ "[psi(3770) -> D0 D0]cc" ],
                              inD2KPi = self.selD2KPi,
                              inD2KpPipPim = self.selD2KpPipPim,
                              inD2KmPipPip = self.selD2KmPipPip,
                              inD2K3Pi = self.selD2K3Pi
                              )

        self.selD0D0bar = makeDD("selD0D0bar",
                                 decayDesc = [ "psi(3770) -> D0 D~0" ],
                                 inD2KPi = self.selD2KPi,
                                 inD2KpPipPim = self.selD2KpPipPim,
                                 inD2KmPipPip = self.selD2KmPipPip,
                                 inD2K3Pi = self.selD2K3Pi
                                 )

        self.selDpDm = makeDD("selDpDm",
                              decayDesc = [ "psi(3770) -> D+ D-" ],
                              inD2KPi = self.selD2KPi,
                              inD2KpPipPim = self.selD2KpPipPim,
                              inD2KmPipPip = self.selD2KmPipPip,
                              inD2K3Pi = self.selD2K3Pi
                              )

        self.selD0Dp = makeDD("selD0Dp",
                              decayDesc = [ "[psi(3770) -> D0 D+]cc" ],
                              inD2KPi = self.selD2KPi,
                              inD2KpPipPim = self.selD2KpPipPim,
                              inD2KmPipPip = self.selD2KmPipPip,
                              inD2K3Pi = self.selD2K3Pi
                              )

        self.selDpDp = makeDD("selDpDp",
                              decayDesc = [ "[psi(3770) -> D+ D+]cc" ],
                              inD2KPi = self.selD2KPi,
                              inD2KpPipPim = self.selD2KpPipPim,
                              inD2KmPipPip = self.selD2KmPipPip,
                              inD2K3Pi = self.selD2K3Pi
                              )

        self.selKpKm = makeKK("selKpKm",
                            decayDesc = [ "D0 -> K+ K-" ],
                            kaonsForKK = self.selKaonsForKK
                            )

        self.selKpKp = makeKK("selKpKp",
                            decayDesc = [ "[D0 -> K+ K+]cc" ],
                            kaonsForKK = self.selKaonsForKK
                            )

        self.selLMR2KPi = makeLMR2KPi("selLMR2KPi",
                                      decayDesc = [ "[phi(1020) -> K+ pi-]cc" ],
                                      kaons = self.selKaons,
                                      pions = self.selPions,
                                      LMR2KPi_APTmin = config['LMR2HH_APTmin'],
                                      LMR2KPi_APTmax = config['LMR2HH_APTmax'],
                                      LMR2KPi_AMmin = config['LMR2HH_AMmin'],
                                      LMR2KPi_AMmax = config['LMR2HH_AMmax'],
                                      LMR2KPi_ADOCAmax = config['LMR2HH_ADOCAmax'],
                                      LMR2KPi_APmin = config['LMR2HH_APmin'],
                                      LMR2KPi_VtxChi2DoFmax = config['LMR2HH_VtxChi2DoFmax']
                                      )

        self.selLMR2KK = makeLMR2KK("selLMR2KK",
                                    decayDesc = [ "phi(1020)  -> K+ K-" ],
                                    kaons = self.selKaons,
                                    LMR2KK_APTmin = config['LMR2HH_APTmin'],
                                    LMR2KK_APTmax = config['LMR2HH_APTmax'],
                                    LMR2KK_AMmin = config['LMR2HH_AMmin'],
                                    LMR2KK_AMmax = config['LMR2HH_AMmax'],
                                    LMR2KK_ADOCAmax = config['LMR2HH_ADOCAmax'],
                                    LMR2KK_APmin = config['LMR2HH_APmin'],
                                    LMR2KK_VtxChi2DoFmax = config['LMR2HH_VtxChi2DoFmax']
                                    )

        self.selLMR2PiPi = makeLMR2PiPi("selLMR2PiPi",
                                        decayDesc = [ "phi(1020)  -> pi+ pi-" ],
                                        pions = self.selPions,
                                        LMR2PiPi_APTmin = config['LMR2HH_APTmin'],
                                        LMR2PiPi_APTmax = config['LMR2HH_APTmax'],
                                        LMR2PiPi_AMmin = config['LMR2HH_AMmin'],
                                        LMR2PiPi_AMmax = config['LMR2HH_AMmax'],
                                        LMR2PiPi_ADOCAmax = config['LMR2HH_ADOCAmax'],
                                        LMR2PiPi_APmin = config['LMR2HH_APmin'],
                                        LMR2PiPi_VtxChi2DoFmax = config['LMR2HH_VtxChi2DoFmax']
                                        )

        self.selPHI2KK = makePHI2KK("selPHI2KK",
                                    decayDesc = [ "phi(1020)  -> K+ K-" ],
                                    kaons = self.selKaons,
                                    PHI2KK_APTmin = config['PHI2KK_APTmin'],
                                    PHI2KK_APTmax = config['PHI2KK_APTmax'],
                                    PHI2KK_AMmin = config['PHI2KK_AMmin'],
                                    PHI2KK_AMmax = config['PHI2KK_AMmax'],
                                    PHI2KK_ADOCAmax = config['PHI2KK_ADOCAmax'],
                                    PHI2KK_APmin = config['PHI2KK_APmin'],
                                    PHI2KK_VtxChi2DoFmax = config['PHI2KK_VtxChi2DoFmax']
                                    )

        #
        #=== Wrong-sign lines ===#
        #

        self.selD2KPiWS = makeD2KPi("selD2KPiWS",
                                    decayDesc = [ "[D0 -> K+ pi+]cc" ],
                                    kaons = self.selKaons,
                                    pions = self.selPions,
                                    D2KPi_APTmin = config['D2KPi_APTmin'],
                                    D2KPi_ADAMASSmax = config['D2KPi_ADAMASSmax'],
                                    D2KPi_ADOCAmax = config['D2KPi_ADOCAmax'],
                                    D2KPi_APmin = config['D2KPi_APmin'],
                                    D2KPi_VtxChi2DoFmax = config['D2KPi_VtxChi2DoFmax']
                                    )

        self.selD2KPiPiWS = makeD2KPiPi("selD2KPiPiWS",
                                        decayDesc = [ "[D+ -> K+ pi+ pi+]cc" ],
                                        kaons = self.selKaons,
                                        pions = self.selPions,
                                        D2KPiPi_APTmin = config['D2KPiPi_APTmin'],
                                        D2KPiPi_ADAMASSmax = config['D2KPiPi_ADAMASSmax'],
                                        D2KPiPi_ADOCAmax = config['D2KPiPi_ADOCAmax'],
                                        D2KPiPi_APmin = config['D2KPiPi_APmin'],
                                        D2KPiPi_VtxChi2DoFmax = config['D2KPiPi_VtxChi2DoFmax']
                                        )

        self.selD2K3PiWS = makeD2K3Pi("selD2K3PiWS",
                                      decayDesc = [ "[D0 -> K+ pi+ pi+ pi+]cc",
                                                    "[D0 -> K+ pi+ pi+ pi-]cc",
                                                    "[D0 -> K+ pi- pi- pi-]cc" ],
                                      kaons = self.selKaons,
                                      pions = self.selPions,
                                      D2K3Pi_APTmin = config['D2K3Pi_APTmin'],
                                      D2K3Pi_ADAMASSmax = config['D2K3Pi_ADAMASSmax'],
                                      D2K3Pi_ADOCAmax = config['D2K3Pi_ADOCAmax'],
                                      D2K3Pi_APmin = config['D2K3Pi_APmin'],
                                      D2K3Pi_VtxChi2DoFmax = config['D2K3Pi_VtxChi2DoFmax']
                                      )

        self.selChiC2KKWS = makeChiC2KK("selChiC2KKWS",
                                        decayDesc = [ "[chi_c1(1P) -> K+ K+]cc" ],
                                        kaons = self.selKaons,
                                        ChiC2KK_APTmin = config['ChiC2HH_APTmin'],
                                        ChiC2KK_APTmax = config['ChiC2HH_APTmax'],
                                        ChiC2KK_AMmin = config['ChiC2HH_AMmin'],
                                        ChiC2KK_AMmax = config['ChiC2HH_AMmax'],
                                        ChiC2KK_ADOCAmax = config['ChiC2HH_ADOCAmax'],
                                        ChiC2KK_APmin = config['ChiC2HH_APmin'],
                                        ChiC2KK_VtxChi2DoFmax = config['ChiC2HH_VtxChi2DoFmax']
                                        )

        self.selChiC2PiPiWS = makeChiC2PiPi("selChiC2PiPiWS",
                                            decayDesc = [ "[chi_c1(1P) -> pi+ pi+]cc" ],
                                            pions = self.selPions,
                                            ChiC2PiPi_APTmin = config['ChiC2HH_APTmin'],
                                            ChiC2PiPi_APTmax = config['ChiC2HH_APTmax'],
                                            ChiC2PiPi_AMmin = config['ChiC2HH_AMmin'],
                                            ChiC2PiPi_AMmax = config['ChiC2HH_AMmax'],
                                            ChiC2PiPi_ADOCAmax = config['ChiC2HH_ADOCAmax'],
                                            ChiC2PiPi_APmin = config['ChiC2HH_APmin'],
                                            ChiC2PiPi_VtxChi2DoFmax = config['ChiC2HH_VtxChi2DoFmax']
                                            )

        self.selChiC2PPWS = makeChiC2PP("selChiC2PPWS",
                                        decayDesc = [ "[chi_c1(1P) -> p+ p+]cc" ],
                                        protons = self.selProtons,
                                        ChiC2PP_APTmin = config['ChiC2PP_APTmin'],
                                        ChiC2PP_APTmax = config['ChiC2PP_APTmax'],
                                        ChiC2PP_AMmin = config['ChiC2PP_AMmin'],
                                        ChiC2PP_AMmax = config['ChiC2PP_AMmax'],
                                        ChiC2PP_ADOCAmax = config['ChiC2PP_ADOCAmax'],
                                        ChiC2PP_APmin = config['ChiC2PP_APmin'],
                                        ChiC2PP_VtxChi2DoFmax = config['ChiC2PP_VtxChi2DoFmax']
                                        )

        self.selChiC2KpKpPipPipWS = makeChiC2KKPiPi("selChiC2KpKpPipPipWS",
                                                    decayDesc = [ "[chi_c1(1P) -> K+ K+ pi+ pi+]cc" ],
                                                    kaons = self.selKaons,
                                                    pions = self.selPions,
                                                    ChiC2KKPiPi_APTmin = config['ChiC2HHHH_APTmin'],
                                                    ChiC2KKPiPi_APTmax = config['ChiC2HHHH_APTmax'],
                                                    ChiC2KKPiPi_AMmin = config['ChiC2HHHH_AMmin'],
                                                    ChiC2KKPiPi_AMmax = config['ChiC2HHHH_AMmax'],
                                                    ChiC2KKPiPi_ADOCAmax = config['ChiC2HHHH_ADOCAmax'],
                                                    ChiC2KKPiPi_APmin = config['ChiC2HHHH_APmin'],
                                                    ChiC2KKPiPi_VtxChi2DoFmax = config['ChiC2HHHH_VtxChi2DoFmax']
                                                    )

        self.selChiC2KpKpPipPimWS = makeChiC2KKPiPi("selChiC2KpKpPipPimWS",
                                                    decayDesc = [ "[chi_c1(1P) -> K+ K+ pi+ pi-]cc" ],
                                                    kaons = self.selKaons,
                                                    pions = self.selPions,
                                                    ChiC2KKPiPi_APTmin = config['ChiC2HHHH_APTmin'],
                                                    ChiC2KKPiPi_APTmax = config['ChiC2HHHH_APTmax'],
                                                    ChiC2KKPiPi_AMmin = config['ChiC2HHHH_AMmin'],
                                                    ChiC2KKPiPi_AMmax = config['ChiC2HHHH_AMmax'],
                                                    ChiC2KKPiPi_ADOCAmax = config['ChiC2HHHH_ADOCAmax'],
                                                    ChiC2KKPiPi_APmin = config['ChiC2HHHH_APmin'],
                                                    ChiC2KKPiPi_VtxChi2DoFmax = config['ChiC2HHHH_VtxChi2DoFmax']
                                                    )

        self.selChiC2KpKmPipPipWS = makeChiC2KKPiPi("selChiC2KpKmPipPipWS",
                                                    decayDesc = [ "[chi_c1(1P) -> K+ K- pi+ pi+]cc" ],
                                                    kaons = self.selKaons,
                                                    pions = self.selPions,
                                                    ChiC2KKPiPi_APTmin = config['ChiC2HHHH_APTmin'],
                                                    ChiC2KKPiPi_APTmax = config['ChiC2HHHH_APTmax'],
                                                    ChiC2KKPiPi_AMmin = config['ChiC2HHHH_AMmin'],
                                                    ChiC2KKPiPi_AMmax = config['ChiC2HHHH_AMmax'],
                                                    ChiC2KKPiPi_ADOCAmax = config['ChiC2HHHH_ADOCAmax'],
                                                    ChiC2KKPiPi_APmin = config['ChiC2HHHH_APmin'],
                                                    ChiC2KKPiPi_VtxChi2DoFmax = config['ChiC2HHHH_VtxChi2DoFmax']
                                                    )

        self.selChiC2KKKKWS = makeChiC2KKKK("selChiC2KKKKWS",
                                            decayDesc = [ "[chi_c1(1P) -> K+ K+ K+ K+]cc",
                                                          "[chi_c1(1P) -> K+ K+ K+ K-]cc" ],
                                            kaons = self.selKaons,
                                            ChiC2KKKK_APTmin = config['ChiC2HHHH_APTmin'],
                                            ChiC2KKKK_APTmax = config['ChiC2HHHH_APTmax'],
                                            ChiC2KKKK_AMmin = config['ChiC2HHHH_AMmin'],
                                            ChiC2KKKK_AMmax = config['ChiC2HHHH_AMmax'],
                                            ChiC2KKKK_ADOCAmax = config['ChiC2HHHH_ADOCAmax'],
                                            ChiC2KKKK_APmin = config['ChiC2HHHH_APmin'],
                                            ChiC2KKKK_VtxChi2DoFmax = config['ChiC2HHHH_VtxChi2DoFmax']
                                            )

        self.selChiC2PiPiPiPiWS = makeChiC2PiPiPiPi("selChiC2PiPiPiPiWS",
                                                    decayDesc = [ "[chi_c1(1P) -> pi+ pi+ pi+ pi+]cc",
                                                                  "[chi_c1(1P) -> pi+ pi+ pi+ pi-]cc" ],
                                                    pions = self.selPions,
                                                    ChiC2PiPiPiPi_APTmin = config['ChiC2HHHH_APTmin'],
                                                    ChiC2PiPiPiPi_APTmax = config['ChiC2HHHH_APTmax'],
                                                    ChiC2PiPiPiPi_AMmin = config['ChiC2HHHH_AMmin'],
                                                    ChiC2PiPiPiPi_AMmax = config['ChiC2HHHH_AMmax'],
                                                    ChiC2PiPiPiPi_ADOCAmax = config['ChiC2HHHH_ADOCAmax'],
                                                    ChiC2PiPiPiPi_APmin = config['ChiC2HHHH_APmin'],
                                                    ChiC2PiPiPiPi_VtxChi2DoFmax = config['ChiC2HHHH_VtxChi2DoFmax']
                                                    )

        #
        #=== Declare lines ===#
        #

        self.LowMultCEP_D2KPi_line = StrippingLine(self._myname + "CEP_D2KPi_line",
                                                   prescale = config['LowMultZeroPrescale'],
                                                   postscale = config['LowMultPostscale'],
                                                   checkPV = False,
                                                   FILTER = CEPFilterTracks,
                                                   HLT = CEPHLTReq,
                                                   algos = [ self.selD2KPi ]
                                                   )
        self.registerLine(self.LowMultCEP_D2KPi_line)

        self.LowMultCEP_D2KpPipPim_line = StrippingLine(self._myname + "CEP_D2KpPipPim_line",
                                                        prescale = config['LowMultPrescale'],
                                                        postscale = config['LowMultPostscale'],
                                                        checkPV = False,
                                                        FILTER = CEPFilterTracks,
                                                        HLT = CEPHLTReq,
                                                        algos = [ self.selD2KpPipPim ]
                                                        )
        self.registerLine(self.LowMultCEP_D2KpPipPim_line)

        self.LowMultCEP_D2KmPipPip_line = StrippingLine(self._myname + "CEP_D2KmPipPip_line",
                                                        prescale = config['LowMultPrescale'],
                                                        postscale = config['LowMultPostscale'],
                                                        checkPV = False,
                                                        FILTER = CEPFilterTracks,
                                                        HLT = CEPHLTReq,
                                                        algos = [ self.selD2KmPipPip ]
                                                        )
        self.registerLine(self.LowMultCEP_D2KmPipPip_line)

        self.LowMultCEP_D2K3Pi_line = StrippingLine(self._myname + "CEP_D2K3Pi_line",
                                                    prescale = config['LowMultZeroPrescale'],
                                                    postscale = config['LowMultPostscale'],
                                                    checkPV = False,
                                                    FILTER = CEPFilterTracks,
                                                    HLT = CEPHLTReq,
                                                    algos = [ self.selD2K3Pi ]
                                                    )
        self.registerLine(self.LowMultCEP_D2K3Pi_line)

        self.LowMultCEP_ChiC2KK_line = StrippingLine(self._myname + "CEP_ChiC2KK_line",
                                                     prescale = config['LowMultPrescale'],
                                                     postscale = config['LowMultPostscale'],
                                                     checkPV = False,
                                                     FILTER = CEPFilterTracksChiC2HH,
                                                     HLT = CEPHLTReq,
                                                     algos = [ self.selChiC2KK ]
                                                     )
        self.registerLine(self.LowMultCEP_ChiC2KK_line)

        self.LowMultCEP_ChiC2PiPi_line = StrippingLine(self._myname + "CEP_ChiC2PiPi_line",
                                                       prescale = config['LowMultPrescale'],
                                                       postscale = config['LowMultPostscale'],
                                                       checkPV = False,
                                                       FILTER = CEPFilterTracksChiC2HH,
                                                       HLT = CEPHLTReq,
                                                       algos = [ self.selChiC2PiPi ]
                                                       )
        self.registerLine(self.LowMultCEP_ChiC2PiPi_line)

        self.LowMultCEP_ChiC2PP_line = StrippingLine(self._myname + "CEP_ChiC2PP_line",
                                                     prescale = config['LowMultZeroPrescale'],
                                                     postscale = config['LowMultPostscale'],
                                                     checkPV = False,
                                                     FILTER = CEPFilterTracksChiC2HH,
                                                     HLT = CEPHLTReq,
                                                     algos = [ self.selChiC2PP ]
                                                     )
        self.registerLine(self.LowMultCEP_ChiC2PP_line)

        self.LowMultCEP_ChiC2KpKpPimPim_line = StrippingLine(self._myname + "CEP_ChiC2KpKpPimPim_line",
                                                             prescale = config['LowMultPrescale'],
                                                             postscale = config['LowMultPostscale'],
                                                             checkPV = False,
                                                             FILTER = CEPFilterTracksChiC2HHHH,
                                                             HLT = CEPHLTReq,
                                                             algos = [ self.selChiC2KpKpPimPim ]
                                                             )
        self.registerLine(self.LowMultCEP_ChiC2KpKpPimPim_line)

        self.LowMultCEP_ChiC2KpKmPipPim_line = StrippingLine(self._myname + "CEP_ChiC2KpKmPipPim_line",
                                                             prescale = config['LowMultPrescale'],
                                                             postscale = config['LowMultPostscale'],
                                                             checkPV = False,
                                                             FILTER = CEPFilterTracksChiC2HHHH,
                                                             HLT = CEPHLTReq,
                                                             algos = [ self.selChiC2KpKmPipPim ]
                                                             )
        self.registerLine(self.LowMultCEP_ChiC2KpKmPipPim_line)

        self.LowMultCEP_ChiC2KKKK_line = StrippingLine(self._myname + "CEP_ChiC2KKKK_line",
                                                       prescale = config['LowMultPrescale'],
                                                       postscale = config['LowMultPostscale'],
                                                       checkPV = False,
                                                       FILTER = CEPFilterTracksChiC2HHHH,
                                                       HLT = CEPHLTReq,
                                                       algos = [ self.selChiC2KKKK ]
                                                       )
        self.registerLine(self.LowMultCEP_ChiC2KKKK_line)

        self.LowMultCEP_ChiC2PiPiPiPi_line = StrippingLine(self._myname + "CEP_ChiC2PiPiPiPi_line",
                                                           prescale = config['LowMultPrescale'],
                                                           postscale = config['LowMultPostscale'],
                                                           checkPV = False,
                                                           FILTER = CEPFilterTracksChiC2HHHH,
                                                           HLT = CEPHLTReq,
                                                           algos = [ self.selChiC2PiPiPiPi ]
                                                           )
        self.registerLine(self.LowMultCEP_ChiC2PiPiPiPi_line)

        self.LowMultCEP_D0D0_line = StrippingLine(self._myname + "CEP_D0D0_line",
                                                  prescale = config['LowMultPrescale'],
                                                  postscale = config['LowMultPostscale'],
                                                  checkPV = False,
                                                  FILTER = CEPFilterTracks,
                                                  HLT = CEPHLTReq,
                                                  algos = [ self.selD0D0 ]
                                                  )
        self.registerLine(self.LowMultCEP_D0D0_line)

        self.LowMultCEP_D0D0bar_line = StrippingLine(self._myname + "CEP_D0D0bar_line",
                                                     prescale = config['LowMultPrescale'],
                                                     postscale = config['LowMultPostscale'],
                                                     checkPV = False,
                                                     FILTER = CEPFilterTracks,
                                                     HLT = CEPHLTReq,
                                                     algos = [ self.selD0D0bar ]
                                                     )
        self.registerLine(self.LowMultCEP_D0D0bar_line)

        self.LowMultCEP_DpDm_line = StrippingLine(self._myname + "CEP_DpDm_line",
                                                  prescale = config['LowMultPrescale'],
                                                  postscale = config['LowMultPostscale'],
                                                  checkPV = False,
                                                  FILTER = CEPFilterTracks,
                                                  HLT = CEPHLTReq,
                                                  algos = [ self.selDpDm ]
                                                  )
        self.registerLine(self.LowMultCEP_DpDm_line)

        self.LowMultCEP_D0Dp_line = StrippingLine(self._myname + "CEP_D0Dp_line",
                                                  prescale = config['LowMultPrescale'],
                                                  postscale = config['LowMultPostscale'],
                                                  checkPV = False,
                                                  FILTER = CEPFilterTracks,
                                                  HLT = CEPHLTReq,
                                                  algos = [ self.selD0Dp ]
                                                  )
        self.registerLine(self.LowMultCEP_D0Dp_line)

        self.LowMultCEP_DpDp_line = StrippingLine(self._myname + "CEP_DpDp_line",
                                                  prescale = config['LowMultPrescale'],
                                                  postscale = config['LowMultPostscale'],
                                                  checkPV = False,
                                                  FILTER = CEPFilterTracks,
                                                  HLT = CEPHLTReq,
                                                  algos = [ self.selDpDp ]
                                                  )
        self.registerLine(self.LowMultCEP_DpDp_line)

        self.LowMultCEP_KpKm_line = StrippingLine(self._myname + "CEP_KpKm_line",
                                                  prescale = config['LowMultHHIncPrescale'],
                                                  postscale = config['LowMultPostscale'],
                                                  checkPV = False,
                                                  FILTER = CEPFilterTracks,
                                                  HLT = CEPHLTReq,
                                                  algos = [ self.selKpKm ]
                                                  )
        self.registerLine(self.LowMultCEP_KpKm_line)

        self.LowMultCEP_KpKp_line = StrippingLine(self._myname + "CEP_KpKp_line",
                                                  prescale = config['LowMultHHIncPrescale'],
                                                  postscale = config['LowMultPostscale'],
                                                  checkPV = False,
                                                  FILTER = CEPFilterTracks,
                                                  HLT = CEPHLTReq,
                                                  algos = [ self.selKpKp ]
                                                  )
        self.registerLine(self.LowMultCEP_KpKp_line)

        self.LowMultCEP_LMR2KPi_line = StrippingLine(self._myname + "CEP_LMR2KPi_line",
                                                     prescale = config['LowMultLMRPrescale'],
                                                     postscale = config['LowMultPostscale'],
                                                     checkPV = False,
                                                     FILTER = CEPFilterTracksChiC2HHHH,
                                                     HLT = CEPHLTReq,
                                                     algos = [ self.selLMR2KPi ]
                                                     )
        self.registerLine(self.LowMultCEP_LMR2KPi_line)

        self.LowMultCEP_LMR2KK_line = StrippingLine(self._myname + "CEP_LMR2KK_line",
                                                    prescale = config['LowMultLMRPrescale'],
                                                    postscale = config['LowMultPostscale'],
                                                    checkPV = False,
                                                    FILTER = CEPFilterTracksChiC2HHHH,
                                                    HLT = CEPHLTReq,
                                                    algos = [ self.selLMR2KK ]
                                                    )
        self.registerLine(self.LowMultCEP_LMR2KK_line)

        self.LowMultCEP_LMR2PiPi_line = StrippingLine(self._myname + "CEP_LMR2PiPi_line",
                                                      prescale = config['LowMultLMRPrescale'],
                                                      postscale = config['LowMultPostscale'],
                                                      checkPV = False,
                                                      FILTER = CEPFilterTracksChiC2HHHH,
                                                      HLT = CEPHLTReq,
                                                      algos = [ self.selLMR2PiPi ]
                                                      )
        self.registerLine(self.LowMultCEP_LMR2PiPi_line)

        self.LowMultCEP_PHI2KK_line = StrippingLine(self._myname + "CEP_PHI2KK_line",
                                                    prescale = config['LowMultZeroPrescale'],
                                                    postscale = config['LowMultPostscale'],
                                                    checkPV = False,
                                                    FILTER = CEPFilterTracksChiC2HHHH,
                                                    HLT = CEPHLTReq,
                                                    algos = [ self.selPHI2KK ]
                                                    )
        self.registerLine(self.LowMultCEP_PHI2KK_line)

        self.LowMultCEP_D2KPiWS_line = StrippingLine(self._myname + "CEP_D2KPiWS_line",
                                                     prescale = config['LowMultZeroPrescale'],
                                                     postscale = config['LowMultPostscale'],
                                                     checkPV = False,
                                                     FILTER = CEPFilterTracks,
                                                     HLT = CEPHLTReq,
                                                     algos = [ self.selD2KPiWS ]
                                                     )
        self.registerLine(self.LowMultCEP_D2KPiWS_line)

        self.LowMultCEP_D2KPiPiWS_line = StrippingLine(self._myname + "CEP_D2KPiPiWS_line",
                                                       prescale = config['LowMultZeroPrescale'],
                                                       postscale = config['LowMultPostscale'],
                                                       checkPV = False,
                                                       FILTER = CEPFilterTracks,
                                                       HLT = CEPHLTReq,
                                                       algos = [ self.selD2KPiPiWS ]
                                                       )
        self.registerLine(self.LowMultCEP_D2KPiPiWS_line)

        self.LowMultCEP_D2K3PiWS_line = StrippingLine(self._myname + "CEP_D2K3PiWS_line",
                                                      prescale = config['LowMultZeroPrescale'],
                                                      postscale = config['LowMultPostscale'],
                                                      checkPV = False,
                                                      FILTER = CEPFilterTracks,
                                                      HLT = CEPHLTReq,
                                                      algos = [ self.selD2K3PiWS ]
                                                      )
        self.registerLine(self.LowMultCEP_D2K3PiWS_line)

        self.LowMultCEP_ChiC2KKWS_line = StrippingLine(self._myname + "CEP_ChiC2KKWS_line",
                                                       prescale = config['LowMultWSPrescale'],
                                                       postscale = config['LowMultPostscale'],
                                                       checkPV = False,
                                                       FILTER = CEPFilterTracksChiC2HH,
                                                       HLT = CEPHLTReq,
                                                       algos = [ self.selChiC2KKWS ]
                                                       )
        self.registerLine(self.LowMultCEP_ChiC2KKWS_line)

        self.LowMultCEP_ChiC2PiPiWS_line = StrippingLine(self._myname + "CEP_ChiC2PiPiWS_line",
                                                         prescale = config['LowMultWSPrescale'],
                                                         postscale = config['LowMultPostscale'],
                                                         checkPV = False,
                                                         FILTER = CEPFilterTracksChiC2HH,
                                                         HLT = CEPHLTReq,
                                                         algos = [ self.selChiC2PiPiWS ]
                                                         )
        self.registerLine(self.LowMultCEP_ChiC2PiPiWS_line)

        self.LowMultCEP_ChiC2PPWS_line = StrippingLine(self._myname + "CEP_ChiC2PPWS_line",
                                                       prescale = config['LowMultZeroPrescale'],
                                                       postscale = config['LowMultPostscale'],
                                                       checkPV = False,
                                                       FILTER = CEPFilterTracksChiC2HH,
                                                       HLT = CEPHLTReq,
                                                       algos = [ self.selChiC2PPWS ]
                                                       )
        self.registerLine(self.LowMultCEP_ChiC2PPWS_line)

        self.LowMultCEP_ChiC2KpKpPipPipWS_line = StrippingLine(self._myname + "CEP_ChiC2KpKpPipPipWS_line",
                                                               prescale = config['LowMultWSPrescale'],
                                                               postscale = config['LowMultPostscale'],
                                                               checkPV = False,
                                                               FILTER = CEPFilterTracksChiC2HHHH,
                                                               HLT = CEPHLTReq,
                                                               algos = [ self.selChiC2KpKpPipPipWS ]
                                                               )
        self.registerLine(self.LowMultCEP_ChiC2KpKpPipPipWS_line)

        self.LowMultCEP_ChiC2KpKpPipPimWS_line = StrippingLine(self._myname + "CEP_ChiC2KpKpPipPimWS_line",
                                                               prescale = config['LowMultWSPrescale'],
                                                               postscale = config['LowMultPostscale'],
                                                               checkPV = False,
                                                               FILTER = CEPFilterTracksChiC2HHHH,
                                                               HLT = CEPHLTReq,
                                                               algos = [ self.selChiC2KpKpPipPimWS ]
                                                               )
        self.registerLine(self.LowMultCEP_ChiC2KpKpPipPimWS_line)

        self.LowMultCEP_ChiC2KpKmPipPipWS_line = StrippingLine(self._myname + "CEP_ChiC2KpKmPipPipWS_line",
                                                               prescale = config['LowMultWSPrescale'],
                                                               postscale = config['LowMultPostscale'],
                                                               checkPV = False,
                                                               FILTER = CEPFilterTracksChiC2HHHH,
                                                               HLT = CEPHLTReq,
                                                               algos = [ self.selChiC2KpKmPipPipWS ]
                                                               )
        self.registerLine(self.LowMultCEP_ChiC2KpKmPipPipWS_line)

        self.LowMultCEP_ChiC2KKKKWS_line = StrippingLine(self._myname + "CEP_ChiC2KKKKWS_line",
                                                         prescale = config['LowMultWSPrescale'],
                                                         postscale = config['LowMultPostscale'],
                                                         checkPV = False,
                                                         FILTER = CEPFilterTracksChiC2HHHH,
                                                         HLT = CEPHLTReq,
                                                         algos = [ self.selChiC2KKKKWS ]
                                                         )
        self.registerLine(self.LowMultCEP_ChiC2KKKKWS_line)

        self.LowMultCEP_ChiC2PiPiPiPiWS_line = StrippingLine(self._myname + "CEP_ChiC2PiPiPiPiWS_line",
                                                             prescale = config['LowMultWSPrescale'],
                                                             postscale = config['LowMultPostscale'],
                                                             checkPV = False,
                                                             FILTER = CEPFilterTracksChiC2HHHH,
                                                             HLT = CEPHLTReq,
                                                             algos = [ self.selChiC2PiPiPiPiWS ]
                                                             )
        self.registerLine(self.LowMultCEP_ChiC2PiPiPiPiWS_line)

#
#=== Final-state particles ===#
#

def makeKaons(name,
              H_PTmin,
              H_Pmin,
              H_TrkChi2max,
              K_PIDKmin
              ) :
    _code = "(PT > %(H_PTmin)s) & (P > %(H_Pmin)s) & (TRCHI2DOF < %(H_TrkChi2max)s) & (PIDK > %(K_PIDKmin)s)" % locals()
    _kaonsFilter = FilterDesktop(Code = _code)
    _stdKaons = DataOnDemand(Location = "Phys/StdAllNoPIDsKaons/Particles")

    return Selection(name,
                     Algorithm = _kaonsFilter,
                     RequiredSelections = [_stdKaons]
                     )

def makeKaonsForKK(name,
                   H_PTmin,
                   KKInc_K_Pmin,
                   H_TrkChi2max,
                   KKInc_K_PIDKmin
                   ) :
    _code = "(PT > %(H_PTmin)s) & (P > %(KKInc_K_Pmin)s) & (TRCHI2DOF < %(H_TrkChi2max)s) & (PIDK > %(KKInc_K_PIDKmin)s)" % locals()
    _kaonsFilter = FilterDesktop(Code = _code)
    _stdKaons = DataOnDemand(Location = "Phys/StdAllNoPIDsKaons/Particles")

    return Selection(name,
                     Algorithm = _kaonsFilter,
                     RequiredSelections = [_stdKaons]
                     )

def makePions(name,
              H_PTmin,
              H_Pmin,
              H_TrkChi2max
              ) :
    _code = "(PT > %(H_PTmin)s) & (P > %(H_Pmin)s) & (TRCHI2DOF < %(H_TrkChi2max)s)" % locals()
    _pionsFilter = FilterDesktop(Code = _code)
    _stdPions = DataOnDemand(Location = "Phys/StdAllNoPIDsPions/Particles")

    return Selection(name,
                     Algorithm = _pionsFilter,
                     RequiredSelections = [_stdPions]
                     )

def makeProtons(name,
                H_PTmin,
                H_Pmin,
                H_TrkChi2max,
                P_PIDPmin
                ) :
    _code = "(PT > %(H_PTmin)s) & (P > %(H_Pmin)s) & (TRCHI2DOF < %(H_TrkChi2max)s) & (PIDp > %(P_PIDPmin)s)" % locals()
    _protonsFilter = FilterDesktop(Code = _code)
    _stdProtons = DataOnDemand(Location = "Phys/StdAllNoPIDsProtons/Particles")

    return Selection(name,
                     Algorithm = _protonsFilter,
                     RequiredSelections = [_stdProtons]
                     )


#
#=== D0 -> KPi ===#
#

def makeD2KPi(name,
              decayDesc,
              kaons,
              pions,
              D2KPi_APTmin,
              D2KPi_ADAMASSmax,
              D2KPi_ADOCAmax,
              D2KPi_APmin,
              D2KPi_VtxChi2DoFmax
              ) :
    D2KPi_Comb_cut   = "(APT > %(D2KPi_APTmin)s) & (ADAMASS('D0') < %(D2KPi_ADAMASSmax)s) & (ADOCAMAX('LoKi::DistanceCalculator') < %(D2KPi_ADOCAmax)s) & " \
                       "(AP > %(D2KPi_APmin)s)" % locals()
    D2KPi_Mother_cut = "(VFASPF(VCHI2PDOF) < %(D2KPi_VtxChi2DoFmax)s)" % locals()

    CombineD2KPi = CombineParticles( DecayDescriptors = decayDesc
                                     , CombinationCut = D2KPi_Comb_cut
                                     , MotherCut = D2KPi_Mother_cut)

    return Selection(name,
                     Algorithm = CombineD2KPi,
                     RequiredSelections = [kaons, pions])

#
#=== D+ -> KPiPi ===#
#

def makeD2KPiPi(name,
                decayDesc,
                kaons,
                pions,
                D2KPiPi_APTmin,
                D2KPiPi_ADAMASSmax,
                D2KPiPi_ADOCAmax,
                D2KPiPi_APmin,
                D2KPiPi_VtxChi2DoFmax
                ) :

    D2KPiPi_Comb_cut   = "(APT > %(D2KPiPi_APTmin)s) & (ADAMASS('D+') < %(D2KPiPi_ADAMASSmax)s) & (ADOCAMAX('LoKi::DistanceCalculator') < %(D2KPiPi_ADOCAmax)s) & " \
                         "(AP > %(D2KPiPi_APmin)s)" % locals()
    D2KPiPi_Mother_cut = "(VFASPF(VCHI2PDOF) < %(D2KPiPi_VtxChi2DoFmax)s)" % locals()

    CombineD2KPiPi = CombineParticles( DecayDescriptors = decayDesc
                                       , CombinationCut = D2KPiPi_Comb_cut
                                       , MotherCut = D2KPiPi_Mother_cut
                                       )

    return Selection(name,
                     Algorithm = CombineD2KPiPi,
                     RequiredSelections = [kaons, pions])

#
#=== D0 -> K3Pi ===#
#

def makeD2K3Pi(name,
               decayDesc,
               kaons,
               pions,
               D2K3Pi_APTmin,
               D2K3Pi_ADAMASSmax,
               D2K3Pi_ADOCAmax,
               D2K3Pi_APmin,
               D2K3Pi_VtxChi2DoFmax
               ) :

    D2K3Pi_Comb_cut   = "(APT > %(D2K3Pi_APTmin)s) & (ADAMASS('D0') < %(D2K3Pi_ADAMASSmax)s) & (ADOCAMAX('LoKi::DistanceCalculator') < %(D2K3Pi_ADOCAmax)s) & " \
                        "(AP > %(D2K3Pi_APmin)s)" % locals()
    D2K3Pi_Mother_cut = "(VFASPF(VCHI2PDOF) < %(D2K3Pi_VtxChi2DoFmax)s)" % locals()

    CombineD2K3Pi = CombineParticles( DecayDescriptors = decayDesc
                                      , CombinationCut = D2K3Pi_Comb_cut
                                      , MotherCut = D2K3Pi_Mother_cut
                                      )

    return Selection(name,
                     Algorithm = CombineD2K3Pi,
                     RequiredSelections = [kaons, pions])

#
#=== 'chi_c' -> KK ===#
#

def makeChiC2KK(name,
                decayDesc,
                kaons,
                ChiC2KK_APTmin,
                ChiC2KK_APTmax,
                ChiC2KK_AMmin,
                ChiC2KK_AMmax,
                ChiC2KK_ADOCAmax,
                ChiC2KK_APmin,
                ChiC2KK_VtxChi2DoFmax
                ) :

    ChiC2KK_Comb_cut   = "(APT > %(ChiC2KK_APTmin)s) & (APT < %(ChiC2KK_APTmax)s) & (AM > %(ChiC2KK_AMmin)s) & (AM < %(ChiC2KK_AMmax)s) & " \
                         "(ADOCAMAX('LoKi::DistanceCalculator') < %(ChiC2KK_ADOCAmax)s) & (AP > %(ChiC2KK_APmin)s)" % locals()
    ChiC2KK_Mother_cut = "(VFASPF(VCHI2PDOF) < %(ChiC2KK_VtxChi2DoFmax)s)" % locals()

    CombineChiC2KK = CombineParticles( DecayDescriptors = decayDesc
                                       , CombinationCut = ChiC2KK_Comb_cut
                                       , MotherCut = ChiC2KK_Mother_cut
                                       )

    return Selection(name,
                     Algorithm = CombineChiC2KK,
                     RequiredSelections = [kaons])

#
#=== 'chi_c' -> PiPi ===#
#

def makeChiC2PiPi(name,
                  decayDesc,
                  pions,
                  ChiC2PiPi_APTmin,
                  ChiC2PiPi_APTmax,
                  ChiC2PiPi_AMmin,
                  ChiC2PiPi_AMmax,
                  ChiC2PiPi_ADOCAmax,
                  ChiC2PiPi_APmin,
                  ChiC2PiPi_VtxChi2DoFmax
                  ) :

    ChiC2PiPi_Comb_cut   = "(APT > %(ChiC2PiPi_APTmin)s) & (APT < %(ChiC2PiPi_APTmax)s) & (AM > %(ChiC2PiPi_AMmin)s) & (AM < %(ChiC2PiPi_AMmax)s) & " \
                           "(ADOCAMAX('LoKi::DistanceCalculator') < %(ChiC2PiPi_ADOCAmax)s) & (AP > %(ChiC2PiPi_APmin)s)" % locals()
    ChiC2PiPi_Mother_cut = "(VFASPF(VCHI2PDOF) < %(ChiC2PiPi_VtxChi2DoFmax)s)" % locals()

    CombineChiC2PiPi = CombineParticles( DecayDescriptors = decayDesc
                                       , CombinationCut = ChiC2PiPi_Comb_cut
                                       , MotherCut = ChiC2PiPi_Mother_cut
                                       )

    return Selection(name,
                     Algorithm = CombineChiC2PiPi,
                     RequiredSelections = [pions])

#
#=== 'chi_c' -> PP ===#
#

def makeChiC2PP(name,
                decayDesc,
                protons,
                ChiC2PP_APTmin,
                ChiC2PP_APTmax,
                ChiC2PP_AMmin,
                ChiC2PP_AMmax,
                ChiC2PP_ADOCAmax,
                ChiC2PP_APmin,
                ChiC2PP_VtxChi2DoFmax
                ) :

    ChiC2PP_Comb_cut   = "(APT > %(ChiC2PP_APTmin)s) & (APT < %(ChiC2PP_APTmax)s) & (AM > %(ChiC2PP_AMmin)s) & (AM < %(ChiC2PP_AMmax)s) & " \
                         "(ADOCAMAX('LoKi::DistanceCalculator') < %(ChiC2PP_ADOCAmax)s) & (AP > %(ChiC2PP_APmin)s)" % locals()
    ChiC2PP_Mother_cut = "(VFASPF(VCHI2PDOF) < %(ChiC2PP_VtxChi2DoFmax)s)" % locals()

    CombineChiC2PP = CombineParticles( DecayDescriptors = decayDesc
                                       , CombinationCut = ChiC2PP_Comb_cut
                                       , MotherCut = ChiC2PP_Mother_cut
                                       )

    return Selection(name,
                     Algorithm = CombineChiC2PP,
                     RequiredSelections = [protons])

#
#=== 'chi_c' -> 4H ===#
#

def makeChiC2KKPiPi(name,
                    decayDesc,
                    kaons,
                    pions,
                    ChiC2KKPiPi_APTmin,
                    ChiC2KKPiPi_APTmax,
                    ChiC2KKPiPi_AMmin,
                    ChiC2KKPiPi_AMmax,
                    ChiC2KKPiPi_ADOCAmax,
                    ChiC2KKPiPi_APmin,
                    ChiC2KKPiPi_VtxChi2DoFmax
                    ) :

    ChiC2KKPiPi_Comb_cut   = "(APT > %(ChiC2KKPiPi_APTmin)s) & (APT < %(ChiC2KKPiPi_APTmax)s) & (AM > %(ChiC2KKPiPi_AMmin)s) & (AM < %(ChiC2KKPiPi_AMmax)s) & " \
                           "(ADOCAMAX('LoKi::DistanceCalculator') < %(ChiC2KKPiPi_ADOCAmax)s) & (AP > %(ChiC2KKPiPi_APmin)s)" % locals()
    ChiC2KKPiPi_Mother_cut = "(VFASPF(VCHI2PDOF) < %(ChiC2KKPiPi_VtxChi2DoFmax)s)" % locals()
    CombineChiC2KKPiPi = CombineParticles( DecayDescriptors = decayDesc
                                         , CombinationCut = ChiC2KKPiPi_Comb_cut
                                         , MotherCut = ChiC2KKPiPi_Mother_cut
                                         )

    return Selection(name,
                     Algorithm = CombineChiC2KKPiPi,
                     RequiredSelections = [kaons, pions])

def makeChiC2KKKK(name,
                  decayDesc,
                  kaons,
                  ChiC2KKKK_APTmin,
                  ChiC2KKKK_APTmax,
                  ChiC2KKKK_AMmin,
                  ChiC2KKKK_AMmax,
                  ChiC2KKKK_ADOCAmax,
                  ChiC2KKKK_APmin,
                  ChiC2KKKK_VtxChi2DoFmax
                  ) :

    ChiC2KKKK_Comb_cut   = "(APT > %(ChiC2KKKK_APTmin)s) & (APT < %(ChiC2KKKK_APTmax)s) & (AM > %(ChiC2KKKK_AMmin)s) & (AM < %(ChiC2KKKK_AMmax)s) & " \
                           "(ADOCAMAX('LoKi::DistanceCalculator') < %(ChiC2KKKK_ADOCAmax)s) & (AP > %(ChiC2KKKK_APmin)s)" % locals()
    ChiC2KKKK_Mother_cut = "(VFASPF(VCHI2PDOF) < %(ChiC2KKKK_VtxChi2DoFmax)s)" % locals()
    CombineChiC2KKKK = CombineParticles( DecayDescriptors = decayDesc
                                         , CombinationCut = ChiC2KKKK_Comb_cut
                                         , MotherCut = ChiC2KKKK_Mother_cut
                                         )

    return Selection(name,
                     Algorithm = CombineChiC2KKKK,
                     RequiredSelections = [kaons])

def makeChiC2PiPiPiPi(name,
                      decayDesc,
                      pions,
                      ChiC2PiPiPiPi_APTmin,
                      ChiC2PiPiPiPi_APTmax,
                      ChiC2PiPiPiPi_AMmin,
                      ChiC2PiPiPiPi_AMmax,
                      ChiC2PiPiPiPi_ADOCAmax,
                      ChiC2PiPiPiPi_APmin,
                      ChiC2PiPiPiPi_VtxChi2DoFmax
                      ) :

    ChiC2PiPiPiPi_Comb_cut   = "(APT > %(ChiC2PiPiPiPi_APTmin)s) & (APT < %(ChiC2PiPiPiPi_APTmax)s) & (AM > %(ChiC2PiPiPiPi_AMmin)s) & (AM < %(ChiC2PiPiPiPi_AMmax)s) & " \
                               "(ADOCAMAX('LoKi::DistanceCalculator') < %(ChiC2PiPiPiPi_ADOCAmax)s) & (AP > %(ChiC2PiPiPiPi_APmin)s)" % locals()
    ChiC2PiPiPiPi_Mother_cut = "(VFASPF(VCHI2PDOF) < %(ChiC2PiPiPiPi_VtxChi2DoFmax)s)" % locals()
    CombineChiC2PiPiPiPi = CombineParticles( DecayDescriptors = decayDesc
                                           , CombinationCut = ChiC2PiPiPiPi_Comb_cut
                                           , MotherCut = ChiC2PiPiPiPi_Mother_cut
                                           )

    return Selection(name,
                     Algorithm = CombineChiC2PiPiPiPi,
                     RequiredSelections = [pions])

#
#=== DD combination ===#
#

def makeDD(name,
           decayDesc,
           inD2KPi,
           inD2KpPipPim,
           inD2KmPipPip,
           inD2K3Pi
           ) :

    CombineDD = CombineParticles( DecayDescriptors = decayDesc
                                  , CombinationCut = "AALL"
                                  , MotherCut = "ALL"
                                  )
    return Selection(name,
                     Algorithm = CombineDD,
                     RequiredSelections = [inD2KPi, inD2KpPipPim, inD2KmPipPip, inD2K3Pi])

#
#=== KK inclusive ===#
#

def makeKK(name,
           decayDesc,
           kaonsForKK
           ) :

    CombineKK = CombineParticles( DecayDescriptors = decayDesc
                                  , CombinationCut = "AALL"
                                  , MotherCut = "ALL"
                                  )

    KKConf = CombineKK.configurable("Combine_" + name + "_KK")
    KKConf.ParticleCombiners.update({'':'MomentumCombiner'})

    return Selection(name,
                     Algorithm = CombineKK,
                     RequiredSelections = [kaonsForKK])

#
#=== Low-mass resonance -> HH ===#
#

def makeLMR2KPi(name,
                decayDesc,
                kaons,
                pions,
                LMR2KPi_APTmin,
                LMR2KPi_APTmax,
                LMR2KPi_AMmin,
                LMR2KPi_AMmax,
                LMR2KPi_ADOCAmax,
                LMR2KPi_APmin,
                LMR2KPi_VtxChi2DoFmax
                ) :

    LMR2KPi_Comb_cut   = "(APT > %(LMR2KPi_APTmin)s) & (APT < %(LMR2KPi_APTmax)s) & (AM > %(LMR2KPi_AMmin)s) & (AM < %(LMR2KPi_AMmax)s) & " \
                         "(ADOCAMAX('LoKi::DistanceCalculator') < %(LMR2KPi_ADOCAmax)s) & (AP > %(LMR2KPi_APmin)s)" % locals()
    LMR2KPi_Mother_cut = "(VFASPF(VCHI2PDOF) < %(LMR2KPi_VtxChi2DoFmax)s)" % locals()

    CombineLMR2KPi = CombineParticles( DecayDescriptors = decayDesc
                                      , CombinationCut = LMR2KPi_Comb_cut
                                      , MotherCut = LMR2KPi_Mother_cut
                                      )

    return Selection(name,
                     Algorithm = CombineLMR2KPi,
                     RequiredSelections = [kaons, pions])

def makeLMR2KK(name,
               decayDesc,
               kaons,
               LMR2KK_APTmin,
               LMR2KK_APTmax,
               LMR2KK_AMmin,
               LMR2KK_AMmax,
               LMR2KK_ADOCAmax,
               LMR2KK_APmin,
               LMR2KK_VtxChi2DoFmax
               ) :

    LMR2KK_Comb_cut   = "(APT > %(LMR2KK_APTmin)s) & (APT < %(LMR2KK_APTmax)s) & (AM > %(LMR2KK_AMmin)s) & (AM < %(LMR2KK_AMmax)s) & " \
                        "(ADOCAMAX('LoKi::DistanceCalculator') < %(LMR2KK_ADOCAmax)s) & (AP > %(LMR2KK_APmin)s)" % locals()
    LMR2KK_Mother_cut = "(VFASPF(VCHI2PDOF) < %(LMR2KK_VtxChi2DoFmax)s)" % locals()

    CombineLMR2KK = CombineParticles( DecayDescriptors = decayDesc
                                      , CombinationCut = LMR2KK_Comb_cut
                                      , MotherCut = LMR2KK_Mother_cut
                                      )

    return Selection(name,
                     Algorithm = CombineLMR2KK,
                     RequiredSelections = [kaons])

def makeLMR2PiPi(name,
                 decayDesc,
                 pions,
                 LMR2PiPi_APTmin,
                 LMR2PiPi_APTmax,
                 LMR2PiPi_AMmin,
                 LMR2PiPi_AMmax,
                 LMR2PiPi_ADOCAmax,
                 LMR2PiPi_APmin,
                 LMR2PiPi_VtxChi2DoFmax
                 ) :

    LMR2PiPi_Comb_cut   = "(APT > %(LMR2PiPi_APTmin)s) & (APT < %(LMR2PiPi_APTmax)s) & (AM > %(LMR2PiPi_AMmin)s) & (AM < %(LMR2PiPi_AMmax)s) & " \
                          "(ADOCAMAX('LoKi::DistanceCalculator') < %(LMR2PiPi_ADOCAmax)s) & (AP > %(LMR2PiPi_APmin)s)" % locals()
    LMR2PiPi_Mother_cut = "(VFASPF(VCHI2PDOF) < %(LMR2PiPi_VtxChi2DoFmax)s)" % locals()

    CombineLMR2PiPi = CombineParticles( DecayDescriptors = decayDesc
                                      , CombinationCut = LMR2PiPi_Comb_cut
                                      , MotherCut = LMR2PiPi_Mother_cut
                                      )

    return Selection(name,
                     Algorithm = CombineLMR2PiPi,
                     RequiredSelections = [pions])


def makePHI2KK(name,
               decayDesc,
               kaons,
               PHI2KK_APTmin,
               PHI2KK_APTmax,
               PHI2KK_AMmin,
               PHI2KK_AMmax,
               PHI2KK_ADOCAmax,
               PHI2KK_APmin,
               PHI2KK_VtxChi2DoFmax
               ) :

    PHI2KK_Comb_cut   = "(APT > %(PHI2KK_APTmin)s) & (APT < %(PHI2KK_APTmax)s) & (AM > %(PHI2KK_AMmin)s) & (AM < %(PHI2KK_AMmax)s) & " \
                        "(ADOCAMAX('LoKi::DistanceCalculator') < %(PHI2KK_ADOCAmax)s) & (AP > %(PHI2KK_APmin)s)" % locals()
    PHI2KK_Mother_cut = "(VFASPF(VCHI2PDOF) < %(PHI2KK_VtxChi2DoFmax)s)" % locals()

    CombinePHI2KK = CombineParticles( DecayDescriptors = decayDesc
                                      , CombinationCut = PHI2KK_Comb_cut
                                      , MotherCut = PHI2KK_Mother_cut
                                      )

    return Selection(name,
                     Algorithm = CombinePHI2KK,
                     RequiredSelections = [kaons])
