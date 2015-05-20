from GaudiKernel.SystemOfUnits import GeV, mm, MeV
from GaudiKernel.SystemOfUnits import picosecond as ps

class PID_EM2015 :
    """
    Threshold settings for Hlt2 PID lines: EM 2015

    WARNING :: DO NOT EDIT WITHOUT PERMISSION OF THE AUTHORS

    @author C. Linn
    @date 2015-05-19
    """

    __all__ = ( 'ActiveHlt2Lines' )


    def ActiveHlt2Lines(self) :
        """
        Returns a list of active lines
        """

        lines = [
            'Hlt2PIDDetJPsiMuMuPosTaggedTurbo',
            'Hlt2PIDDetJPsiMuMuNegTaggedTurbo',
            'Hlt2PIDB2KJPsiEESSTaggedTurbo',
            'Hlt2PIDB2KJPsiEEOSTaggedTurbo',
            'Hlt2PIDLambda2PPiLLTurbo',
            'Hlt2PIDLambda2PPiLLhighPTurbo',
            'Hlt2PIDLambda2PPiLLveryhighPTurbo',
            'Hlt2PIDLambda2PPiLLisMuonTurbo',
            'Hlt2PIDD02KPiTagTurbo',
            'Hlt2PIDLb2LcMuNuTurbo',
            'Hlt2PIDLb2LcPiTurbo',
       ]


        return lines

    def Thresholds(self) :
        l0_muons = ['Muon','DiMuon']
        l0_electrons = ['Photon', 'Electron', 'Hadron', 'Muon', 'DiMuon']
        hlt1_muons = 'Hlt1(TrackAllL0|TrackMuon|SingleMuon|DiMuon|TrackMVA|TwoTrackMVA).*Decision'
        hlt1_electrons = 'Hlt1.*Decision'

        d = {}
        from Hlt2Lines.PID.Lines                import PIDLines
        d.update ({PIDLines : { 
              'Prescale' :  {# Set all the non-Turbo versions to prescale zero because at least to
                                # start with in Run2 then Brunel will be run on the Turbo stream and
                                # we can get the full calibration sample that way.
                                'Hlt2PIDLambda2PPiLLTurbo'          : 0.003, # First tuning from Lucio (ish)
                                'Hlt2PIDLambda2PPiLLhighPTurbo'     : 0.1,
                                'Hlt2PIDLambda2PPiLLveryhighPTurbo' : 1.0,
                                'Hlt2PIDLambda2PPiLLisMuonTurbo'    : 0.05, 
                                'Hlt2PIDLambda2PPiDDTurbo'          : 0.0,
                                'Hlt2PIDLambda2PPiDDhighPTurbo'     : 0.0,
                                'Hlt2PIDLambda2PPiDDveryhighPTurbo' : 0.0,
                                'Hlt2PIDLambda2PPiDDisMuonTurbo'    : 0.0,
                                'Hlt2PIDKs2PiPiLLTurbo'             : 0.0005,
                                'Hlt2PIDKs2PiPiDDTurbo'             : 0.0,
                                'Hlt2PIDDetJPsiEEL0PosTaggedTurbo'  : 0.0, # verdict seems to be that we
                                'Hlt2PIDDetJPsiEEL0NegTaggedTurbo'  : 0.0, # will use the full calo and
                                'Hlt2PIDB2KJPsiEEL0SSTaggedTurbo'   : 0.0, # have no need for this suite
                                'Hlt2PIDB2KJPsiEEL0OSTaggedTurbo'   : 0.0, # of lines
                                'Hlt2PIDDetJPsiMuMuPosTaggedTurbo'  : 1.0,
                                'Hlt2PIDDetJPsiMuMuNegTaggedTurbo'  : 1.0,
                                'Hlt2PIDB2KJPsiMuMuSSTaggedTurbo'   : 1.0,
                                'Hlt2PIDB2KJPsiMuMuOSTaggedTurbo'   : 1.0,
                                'Hlt2PIDDetPhiMuMuPosTaggedTurbo'   : 1.0,
                                'Hlt2PIDDetPhiMuMuNegTaggedTurbo'   : 1.0,
                                'Hlt2PIDDs2PiPhiMuMuSSTaggedTurbo'  : 1.0,
                                'Hlt2PIDDs2PiPhiMuMuOSTaggedTurbo'  : 1.0,
                                'Hlt2PIDDetJPsiEEPosTaggedTurbo'    : 1.0,
                                'Hlt2PIDDetJPsiEENegTaggedTurbo'    : 1.0,
                                'Hlt2PIDB2KJPsiEESSTaggedTurbo'     : 1.0,
                                'Hlt2PIDB2KJPsiEEOSTaggedTurbo'     : 1.0,
                                'Hlt2PIDSc02LcPiTurbo'              : 1.0,
                                'Hlt2PIDScpp2LcPiTurbo'             : 1.0,
                                'Hlt2PIDLb2LcPiTurbo'               : 1.0,
                                'Hlt2PIDLb2LcMuNuTurbo'             : 1.0,
                                'Hlt2PIDD02KPiPiPiTurbo'            : 0.0,
                                'Hlt2PIDD02KPiPiPiTagTurbo'         : 0.1,
                                'Hlt2PIDD02KPiTagTurbo'             : 1.0
                                },
                  'Common'   : {'TagTrChi2'     : 3.0,
                                'TagP'          : 3 * GeV, # 6GeV in old stripping
                                #'TagPt'         : 500 * MeV, # 1.5GeV in old stripping
                                'ProbeTrChi2'   : 5, # no cut for ee in old stripping, 3 for detached mumu
                                'ProbeP'        : 3 * GeV, # 3 GeV for all but B -> (J/psi->mumu)K where it's zero in old stripping
                                'BachPt'        : 0, # no cut in old stripping
                                'BachP'         : 0, # no cut in J/psi -> mumu in old stripping, 1 GeV for old J/psi->ee
                                'BachIPChi2'    : 9.0, # old J/psi -> mumu cut of 25, 9 for J/psi -> ee
                                'LambdaMWindow' : 25 * MeV,
                                'LambdaKsVeto'  : 20 * MeV,
                                'DetLLMinIPChi2': 5.0,
                                'LLhSumPTMin'   : 2.0 * GeV,
                                'CombTisTosSpec': [] # Used in BCombiner, so B2JPsiK, Ds2PhiPi, Lb2LcMuNu, Lb2LcPi etc
                                },
                  'PIDSharedBachelor_K' : {
                                'Trk_ALL_TRCHI2DOF_MAX' : 5.0,
                                'Trk_ALL_PT_MIN'        : 300.0 * MeV,
                                'Trk_ALL_P_MIN'         : 3.0 * GeV,
                                'Trk_ALL_MIPCHI2DV_MIN' : 9.0,
                                'PID_LIM'               : 5.0
                                },
                  'PIDSharedBachelor_pi' : { # Used for Lb->LcPi and Ds->PhiPi
                                'Trk_ALL_TRCHI2DOF_MAX' : 5.0,
                                'Trk_ALL_PT_MIN'        : 300.0 * MeV,
                                'Trk_ALL_P_MIN'         : 3.0 * GeV,
                                'Trk_ALL_MIPCHI2DV_MIN' : 9.0,
                                'PID_LIM'               : -5.0
                                },
                  'PIDSharedBachelor_mu' : { # Used for Lb->LcMu
                                'Trk_ALL_TRCHI2DOF_MAX' : 5.0,
                                'Trk_ALL_PT_MIN'        : 500 * MeV,
                                'Trk_ALL_P_MIN'         : 3.0 * GeV,
                                'Trk_ALL_MIPCHI2DV_MIN' : 16.0,
                                'PID_LIM'               : 0.0
                                },
                 'Lambda2PPiLL' : {
                                'LambdaProtonP' : 2.0 * GeV,
                                'LambdaProtonPT': 0.0 * GeV},
                 'Lambda2PPiDD' : {
                                'LambdaProtonP' : 2.0 * GeV,
                                'LambdaProtonPT': 0.0 * GeV},
                 'Lambda2PPiLLhighP' : {
                                'LambdaProtonP' : 2.0 * GeV,
                                'LambdaProtonPT': 3.5 * GeV},
                 'Lambda2PPiDDhighP' : {
                                'LambdaProtonP' : 2.0 * GeV,
                                'LambdaProtonPT': 3.5 * GeV},
                 'Lambda2PPiLLveryhighP' : {
                                'LambdaProtonP' : 2.0 * GeV,
                                'LambdaProtonPT': 10.0 * GeV},
                 'Lambda2PPiDDveryhighP' : {
                                'LambdaProtonP' : 2.0 * GeV,
                                'LambdaProtonPT': 10.0 * GeV},
                 'Lambda2PPiLLisMuon' : {
                                'LambdaProtonP' : 2.0 * GeV,
                                'LambdaProtonPT': 0.0 * GeV},
                 'Lambda2PPiDDisMuon' : {
                                'LambdaProtonP' : 2.0 * GeV,
                                'LambdaProtonPT': 0.0 * GeV},
                 'JPsiMuMu' :  {'ProbePt'       : 500 * MeV,
                                'ProbeMinIPChi2': 16,
                                'ProbeTisTos'   : [
                                  "L0(" + "|".join(l0_muons) + ")Decision%TIS",
                                  hlt1_muons + "%TIS"
                                  ],
                                'TagPt'         : 1.2 * GeV,
                                'TagMinIPChi2'  : 9,
                                'LLCombAMLow'   : (3096-210) * MeV,
                                'LLCombAMHigh'  : (3096+210) * MeV,
                                'LLCombMLow'    : (3096-200) * MeV,
                                'LLCombMHigh'   : (3096+200) * MeV,
                                'LLCombMaxDocaChi2' : 10,
                                'LLhCombAMLow'  : (5279-520) * MeV,
                                'LLhCombAMHigh' : (5279+520) * MeV,
                                'LLhCombMLow'   : (5279-500) * MeV,
                                'LLhCombMHigh'  : (5279+500) * MeV,
                                'LLhVChi2'      : 25,
                                'LLhVDChi2'     : 25,
                                'LLhMaxIPChi2'  : 25,
                                'DetLLVChi2'    : 15.0,
                                'DetLLPt'       : 1.0 * GeV,
                                'DetLLVDChi2'   : 50.0},
                 'JPsiEE'   :  {'ProbePt'       : 500 * MeV,
                                'ProbeMinIPChi2': 25,
                                'ProbeTisTos'   : [
                                  "L0(" + "|".join(l0_electrons) + ")Decision%TIS",
                                  hlt1_electrons + "%TIS"
                                  ],
                                'TagMinIPChi2'  : 9,
                                'TagPIDe'       : 5.0,
                                'TagP'          : 6.0 * GeV,
                                'TagPt'         : 1.5 * GeV,
                                'BachPt'        : 1.0 * GeV,
                                'LLCombAMLow'   : (3096-910) * MeV,
                                'LLCombAMHigh'  : (3096+510) * MeV,
                                'LLCombMLow'    : (3096-900) * MeV,
                                'LLCombMHigh'   : (3096+500) * MeV,
                                'LLCombMaxDocaChi2': 18.0,
                                'LLhCombAMLow'  : (5279-1110) * MeV,
                                'LLhCombAMHigh' : (5279+1110) * MeV,
                                'LLhCombMLow'   : (5279-1000) * MeV,
                                'LLhCombMHigh'  : (5279+1000) * MeV,
                                'LLhVChi2'      : 25,
                                'LLhVDChi2'     : 50.0,
                                'LLhMaxIPChi2'  : 100,
                                'DetLLVChi2'    : 9.0,
                                'DetLLPt'       : 2.0 * GeV,
                                'DetLLVDChi2'   : 100.0,
                                'DetLLMinIPChi2': 15.0},
                 'PhiMuMu'  :  {'LLCombAMLow'   : (1020-30) * MeV,
                                'LLCombAMHigh'  : (1020+30) * MeV,
                                'LLCombMLow'    : (1020-25) * MeV,
                                'LLCombMHigh'   : (1020+25) * MeV,
                                'LLCombMaxDocaChi2': 9.0,
                                'TagMinIPChi2'  : 9.0,
                                'ProbeMinIPChi2': 25.0,
                                'ProbePt'       : 500 * MeV,
                                'TagPt'         : 800 * MeV,
                                'ProbeTisTos'   : [
                                  "L0(" + "|".join(l0_muons) + ")Decision%TIS",
                                  hlt1_muons + '%TIS'
                                  ],
                                'LLhCombAMLow'  : (1968 - 80) * MeV,
                                'LLhCombAMHigh' : (1968 + 80) * MeV,
                                'LLhCombMLow'   : (1968 - 70) * MeV, # Ds+ -> phi pi+
                                'LLhCombMHigh'  : (1968 + 70) * MeV,
                                'LLhVChi2'      : 25.0,
                                'LLhVDChi2'     : 50.0,
                                'LLhMaxIPChi2'  : 20.0,
                                'DetLLVChi2'    : 15.0,
                                'DetLLPt'       : 2.0 * GeV,
                                'DetLLVDChi2'   : 75.0,
                                'DetLLMinIPChi2': 15.0},
                 'Lb2LcMuNu' : {'BachPt'        : 500.0 * MeV, # muon
                                'BachIPChi2'    : 16.0,
                                'LLhCombAMLow'  : 0 * MeV,
                                'LLhCombAMHigh' : (5620 + 400) * MeV,
                                'LLhCombBPVCORRMHigh' : (5620 + 400) * MeV,
                                'LLhCombBPVCORRMLow'  : (5620 - 1100) * MeV,
                                'LLhVChi2'      : 25,
                                'LLhVDChi2'     : 100.0,
                                'LLhMaxIPChi2'  : 200.0, # partially reconstructed, don't cut too hard
                                'LLhSumPTMin'   : 3.0 * GeV}, # Lc_PT + pi_PT
                 'Lb2LcPi'   : {'BachPt'        : 500.0 * MeV, # pion
                                'BachIPChi2'    : 25.0,
                                'LLhCombAMLow'  : (5620 - 350) * MeV,
                                'LLhCombAMHigh' : (5620 + 350) * MeV,
                                'LLhCombMLow'   : (5620 - 300) * MeV,
                                'LLhCombMHigh'  : (5620 + 300) * MeV,
                                'LLhVChi2'      : 25,
                                'LLhVDChi2'     : 100.0,
                                'LLhMaxIPChi2'  : 10.0,
                                'LLhSumPTMin'   : 3.0 * GeV}, # Lc_PT + pi_PT
                 'Ks2PiPiLL' : {'KsMassWindow'  : 30 * MeV,
                                'KsVChi2'       : 16,
                                'KsDecayMaxZ'   : 2200 * mm,
                                'KsVDChi2'      : 25,
                                'KsLambdaVeto' : 9 * MeV},
                 'Ks2PiPiDD' : {'KsMassWindow'  : 30 * MeV,
                                'KsVChi2'       : 16,
                                'KsDecayMaxZ'   : 2200 * mm,
                                'KsVDChi2'      : 25,
                                'KsLambdaVeto' : 18 * MeV},
                 'L0Req'    : {
                     'Muon'           : '',
                     'Electron'       : '',
                     'ElectronFromL0' : "L0_CHANNEL('Electron')"
                     },
                 'Hlt1Req'  : {
                     'Muon'           : '',
                     'Electron'       : '',
                     'ElectronFromL0' : "HLT_PASS_RE('" + hlt1_electrons + "')"
                     },
                 'TrackGEC' : {'NTRACK_MAX'           : 10000},
                 "LcTightChild_pi"    : {
                                'PID_LIM'                 : -5.0,
                                'Trk_GhostProb_MAX'       : 1e9,#0.3,
                                'Trk_PT_MIN'              : 400.0 * MeV,
                                'Trk_MinIPChi2'           : 9.0},
                 'LcTightChild_K'     : {
                                'PID_LIM'                 : 5.0,
                                'Trk_GhostProb_MAX'       : 1e9,#0.3,
                                'Trk_PT_MIN'              : 400.0 * MeV,
                                'Trk_MinIPChi2'           : 9.0},
                 'LcTightChild_p'     : {
                                'Trk_GhostProb_MAX'       : 1e9,#0.3,
                                'Trk_MinIPChi2'           : 3.0,
                                'Trk_PT_MIN'              : 1000.0 * MeV}, # Point of the Lc+ sample is
                                                                           # to enhace high pT coverage..
                                                                           # we get soft protons from L0
                 # Now the combiner for the PID calib lines
                 # The pointing cut is tight to ensure a purer signal even without
                 # a PID cut on the proton
                 'Lc2KPPi' : {  'Trk_ALL_MIPCHI2DV_MIN'   :  3.0,
                                'Trk_2OF3_MIPCHI2DV_MIN'  :  12.0,
                                'Trk_1OF3_MIPCHI2DV_MIN'  :  16.0,
                                'Trk_ALL_PT_MIN'          :  100.0 * MeV,
                                'Trk_2OF3_PT_MIN'         :  400.0 * MeV,
                                'Trk_1OF3_PT_MIN'         :  400.0 * MeV,
                                'BPVVDCHI2_MIN'           :  50.0,
                                'BPVLTIME_MIN'            :  0.1 * ps, # This is the cut applied to all Lc+, even if they're then associated with Lb
                                'BPVDIRA_MIN'             :  0.0,
                                'ASUMPT_MIN'              :  2000 * MeV,
                                'MVETO_Kpipi_GT'          :  (1870.0 + 15.0) * MeV,
                                'MVETO_Kpipi_LT'          :  (1870.0 - 15.0) * MeV,
                                'MVETO_KKpi_GT'           :  (1968.0 + 15.0) * MeV,
                                'MVETO_KKpi_LT'           :  (1968.0 - 15.0) * MeV,
                                'M_Kpi_LOW'               :  (896.0 - 100.0) * MeV,
                                'M_Kpi_HIGH'              :  (896.0 + 100.0) * MeV,
                                'AM_MIN'                  :  (2286.5 - 85.0) * MeV,
                                'AM_MAX'                  :  (2286.5 + 85.0) * MeV,
                                'Mass_M_MIN'              :  (2286.5 - 75.0) * MeV,
                                'Mass_M_MAX'              :  (2286.5 + 75.0) * MeV,
                                'VCHI2PDOF_MAX'           :  10.0, # took this one from CharmHad Common
                                'TisTosSpec'              :  []},#"Hlt1(Two)?TrackMVADecision%TOS"}, # changed this
                 'Lc2KPPiPrompt' : { # Extra cuts for those Lc+ tagged with Sc0/Sc++
                                'BPVDIRA_MIN'             : 0.99995,
                                # For Lc+ which are Sc-tagged. Don't want to use the DetachedHHHCombiner
                                # TisTosSpec CharmHad gives us because we share Lc+ between the Sc
                                # and Lb tags.
                                'PromptTisTosSpec'        : "Hlt1(Two)?TrackMVADecision%TOS",
                                'BPVLTIME_MIN'            : 0.15 * ps},
                 # Note the pointing cut is now in the tagger so that we can re-use
                 # the Lc candidates in the PIDCALIB lines for Lb -> Lc pi and Lb -> Lc mu nu
                 'Sc2LcPi' : {
                                'DeltaM_AM_MIN'            :  150.0 * MeV,
                                'DeltaM_MIN'               :  155.0 * MeV,
                                'DeltaM_AM_MAX'            :  275.0 * MeV,
                                'DeltaM_MAX'               :  270.0 * MeV,
                                'TagVCHI2PDOF_MAX'         :  50.0},
                 'D02KPiPiPi' : { # Tuning from Maurizio
                                'Trk_ALL_PT_MIN'           :  250 * MeV,
                                'Trk_ALL_MIPCHI2DV_MIN'    :  3,
                                'AM_34'                    : (139.5 + 139.5) * MeV,
                                'AM_4'                     : (139.5) * MeV,
                                'AM_MIN'                   :  1780 * MeV,
                                'AM_MAX'                   :  1950 * MeV,
                                'ASUMPT_MIN'               :  2500.0 * MeV,
                                'ADOCA_MAX'                :  100.0 * mm,
                                'ACHI2DOCA_MAX'            :  10.0,
                                'VCHI2PDOF_MAX'            :  12.0,
                                'BPVDIRA_MIN'              :  0.9999,
                                'BPVLTIME_MIN'             :  0.1 * ps,
                                'Mass_M_MIN'               :  1790 * MeV,
                                'Mass_M_MAX'               :  1940 * MeV,
                                'TisTosSpec'               :  "Hlt1.*Track.*Decision%TOS"},
                 'D02KPiPiPiTag' : {
                                'TagVCHI2PDOF_MAX'         :  15.0,
                                'DeltaM_AM_MIN'            :  0.0,
                                'DeltaM_AM_MAX'            :  190.0 * MeV,
                                'DeltaM_MIN'               :  0.0,
                                'DeltaM_MAX'               :  170.0 * MeV},
                 'D02KPi' : {   'Trk_ALL_PT_MIN'           : 800.0 * MeV,   # Stripping has 250
                                'Trk_ALL_P_MIN'            : 5.0 * GeV,     # Stripping has 2.0
                                'Trk_ALL_MIPCHI2DV_MIN'    : 2.0,           # Stripping has 16
                                # We currently have an implicit cut on track chi2/dof < 3 coming
                                # from the CharmHad filtered NoPIDs inputs; stripping had < 5
                                'Pair_AMINDOCA_MAX'        : 0.10 * mm,     # Not in stripping
                                'Trk_Max_APT_MIN'          : 1500.0 * MeV,  # Not in stripping
                                'D0_BPVVDCHI2_MIN'         : 25.0,          # Stripping has 49
                                'D0_BPVDIRA_MIN'           : 0.99985,       # Stripping has 0.9999
                                'D0_VCHI2PDOF_MAX'         : 10.0,          # Stripping has 13
                                'D0_PT_MIN'                : 2.0 * GeV,     # Stripping has 1.5
                                'Mass_M_MIN'               : 1815.0 * MeV,  # This is an M cut, stripping says 1790
                                'Comb_AM_MIN'              : 1805.0 * MeV,  # This is an AM cut, stripping says 1780
                                'Mass_M_MAX'               : 1915.0 * MeV,  # This is an M cut, stripping says 1940
                                'Comb_AM_MAX'              : 1925.0 * MeV,  # This is an AM cut, stripping says 1950
                                'TisTosSpec'               : [ ]},#"Hlt1(Two)?TrackMVADecision%TOS"},
                 'D02KPiTag' : {
                                'TagVCHI2PDOF_MAX'         :  15.0,         # Stripping has 13
                                'DeltaM_AM_MIN'            :  0.0,          # Stripping has (ADAMASS('D*(2010)+')<75)
                                'DeltaM_AM_MAX'            :  165.0 * MeV,  # Stripping has 165 
                                'DeltaM_MIN'               :  0.0,          # Stripping has 130
                                'DeltaM_MAX'               :  155.0 * MeV}  # Stripping has 155
                                                                            # Stripping has D* APT > 2.2 GeV
                 }
        })

        return d
