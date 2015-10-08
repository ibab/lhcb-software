__author__  = 'Olli Lupton'
__date__    = '$Date: 2015-03-12$'
__version__ = '$Revision: 0.0 $'

from GaudiKernel.SystemOfUnits import GeV, MeV, mm
from GaudiKernel.SystemOfUnits import picosecond as ps
from Hlt2Lines.Utilities.Hlt2LinesConfigurableUser import Hlt2LinesConfigurableUser

turboversions = [ 'TurboCalib', '' ]
class PIDLines(Hlt2LinesConfigurableUser):
    l0_muons = ['Muon','DiMuon']
    l0_electrons = ['Photon', 'Electron', 'Hadron', 'Muon', 'DiMuon']
    hlt1_muons = 'Hlt1(TrackAllL0|TrackMuon|SingleMuon|DiMuon|TrackMVA|TwoTrackMVA).*Decision'
    hlt1_electrons = 'Hlt1.*Decision'
    __slots__ = {'Prescale' :  { 'Hlt2PID' + k + turbo : v for k, v in {
                                'Lambda2PPiLL'          : 0.003, # First tuning from Lucio (ish)
                                'Lambda2PPiLLhighPT'    : 0.1,
                                'Lambda2PPiLLveryhighPT': 1.0,
                                'Lambda2PPiLLisMuon'    : 0.05, 
                                'Lambda2PPiDD'          : 0.0,
                                'Lambda2PPiDDhighPT'    : 0.0,
                                'Lambda2PPiDDveryhighPT': 0.0,
                                'Lambda2PPiDDisMuon'    : 0.0,
                                'Ks2PiPiLL'             : 0.0005,
                                'Ks2PiPiDD'             : 0.0,
                                'DetJPsiEEL0PosTagged'  : 0.0, # verdict seems to be that we
                                'DetJPsiEEL0NegTagged'  : 0.0, # will use the full calo and
                                'B2KJPsiEEL0PosTagged'  : 0.0, # have no need for this suite
                                'B2KJPsiEEL0NegTagged'  : 0.0, # of lines
                                'DetJPsiMuMuPosTagged'  : 1.0,
                                'DetJPsiMuMuNegTagged'  : 1.0,
                                'B2KJPsiMuMuPosTagged'  : 1.0,
                                'B2KJPsiMuMuNegTagged'  : 1.0,
                                'DetPhiMuMuPosTagged'   : 0.1,
                                'DetPhiMuMuNegTagged'   : 0.1,
                                'Ds2PiPhiMuMuPosTagged' : 1.0,
                                'Ds2PiPhiMuMuNegTagged' : 1.0,
                                'DetJPsiEEPosTagged'    : 0.01,
                                'DetJPsiEENegTagged'    : 0.01,
                                'B2KJPsiEEPosTagged'    : 1.0,
                                'B2KJPsiEENegTagged'    : 1.0,
                                'DetPhiKKUnbiased'      : 0.001,
                                'DetPhiKKPosTagged'     : 0.005,
                                'DetPhiKKNegTagged'     : 0.005,
                                'DetJPsiPPPosTagged'    : 0.01,
                                'DetJPsiPPNegTagged'    : 0.01,
                                'Sc02LcPi'              : 1.0,
                                'Scpp2LcPi'             : 1.0,
                                'Lb2LcPi'               : 1.0,
                                'Lb2LcMuNu'             : 1.0,
                                'D02KPiPiPi'            : 0.0,
                                'D02KPiPiPiTag'         : 0.1,
                                'D02KPiTag'             : 1.0
                                }.iteritems() for turbo in turboversions},
                  'Common'   : {'TagTrChi2'     : 3.0,
                                'TagMaxGhostProb': 0.5,
                                'TagP'          : 3 * GeV, # 6GeV in old stripping
                                'ProbeTrChi2'   : 5.0, # no cut for ee in old stripping, 3 for detached mumu
                                'ProbeP'        : 3 * GeV, # 3 GeV for all but B -> (J/psi->mumu)K where it's zero in old stripping
                                'BachPt'        : 0, # no cut in old stripping
                                'BachP'         : 0, # no cut in J/psi -> mumu in old stripping, 1 GeV for old J/psi->ee
                                'BachIPChi2'    : 9.0, # old J/psi -> mumu cut of 25, 9 for J/psi -> ee
                                'LambdaMWindow' : 25 * MeV,
                                'LambdaKsVeto'  : 20 * MeV,
                                'LambdaIPChi2Max': 50.0, # Added for 25ns running based on EM samples
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
                 'Lambda2PPiLLhighPT' : {
                                'LambdaProtonP' : 2.0 * GeV,
                                'LambdaProtonPT': 3.0 * GeV},
                 'Lambda2PPiDDhighPT' : {
                                'LambdaProtonP' : 2.0 * GeV,
                                'LambdaProtonPT': 3.0 * GeV},
                 'Lambda2PPiLLveryhighPT' : {
                                'LambdaProtonP' : 2.0 * GeV,
                                'LambdaProtonPT': 6.0 * GeV},
                 'Lambda2PPiDDveryhighPT' : {
                                'LambdaProtonP' : 2.0 * GeV,
                                'LambdaProtonPT': 6.0 * GeV},
                 'Lambda2PPiLLisMuon' : {
                                'LambdaProtonP' : 2.0 * GeV,
                                'LambdaProtonPT': 0.0 * GeV},
                 'Lambda2PPiDDisMuon' : {
                                'LambdaProtonP' : 2.0 * GeV,
                                'LambdaProtonPT': 0.0 * GeV},
                 'JPsiMuMu' :  {'ProbePt'       : 0 * MeV, 
                                'ProbeMinIPChi2': 9.0,
                                'ProbeTisTos'   : [
                                  "L0(" + "|".join(l0_muons) + ")Decision%TIS",
                                  hlt1_muons + "%TIS"
                                  ],
                                'TagPt'         : 1.2 * GeV,
                                'TagMinIPChi2'  : 9.0,
                                'LLCombAMLow'   : (3096-210) * MeV,
                                'LLCombAMHigh'  : (3096+210) * MeV,
                                'LLCombMLow'    : (3096-200) * MeV,
                                'LLCombMHigh'   : (3096+200) * MeV,
                                'LLCombMaxDocaChi2' : 10,
                                'LLhCombAMLow'  : (5279-520) * MeV,
                                'LLhCombAMHigh' : (5279+520) * MeV,
                                'LLhCombMLow'   : (5279-500) * MeV,
                                'LLhCombMHigh'  : (5279+500) * MeV,
                                'LLhMinBPVDIRA' : 0.9999,
                                'LLhVChi2'      : 25,
                                'LLhVDChi2'     : 25,
                                'LLhMaxIPChi2'  : 25,
                                'LLhSumPTMin'   : 2.0 * GeV,
                                'DetLLVChi2'    : 15.0,
                                'DetLLPt'       : 1.0 * GeV,
                                'DetLLVDChi2'   : 150.0,
                                'DetLLMinIPChi2': 5.0,
                                'DetLLMinBPVDIRA' : 0.995},
                 'JPsiPP'   :  {'ProbePt'       : 800.0 * MeV,
                                'ProbeMinIPChi2': 16.0,
                                'ProbeTisTos'   : [ ],
                                'TagPt'         : 1.5 * GeV,
                                'TagMinIPChi2'  : 25.0,
                                'TagPIDp'       : 5.0,
                                'LLCombAMLow'   : (3096-220) * MeV,
                                'LLCombAMHigh'  : (3096+220) * MeV,
                                'LLCombMLow'    : (3096-200) * MeV,
                                'LLCombMHigh'   : (3096+200) * MeV,
                                'LLCombMaxDocaChi2' : 10,
                                'LLhCombAMLow'  : (5279-520) * MeV,
                                'LLhCombAMHigh' : (5279+520) * MeV,
                                'LLhCombMLow'   : (5279-500) * MeV,
                                'LLhCombMHigh'  : (5279+500) * MeV,
                                'LLhMinBPVDIRA' : 0.9999,
                                'LLhVChi2'      : 25,
                                'LLhVDChi2'     : 25,
                                'LLhMaxIPChi2'  : 25,
                                'LLhSumPTMin'   : 2.0 * GeV,
                                'DetLLVChi2'    : 15.0,
                                'DetLLPt'       : 1.0 * GeV,
                                'DetLLVDChi2'   : 150.0,
                                'DetLLMinIPChi2': 5.0,
                                'DetLLMinBPVDIRA' : 0.995},
                 'JPsiEE'   :  {'ProbePt'       : 500 * MeV,
                                'ProbeMinIPChi2': 9.0,
                                'ProbeTisTos'   : [
                                  "L0(" + "|".join(l0_electrons) + ")Decision%TIS",
                                  hlt1_electrons + "%TIS"
                                  ],
                                'TagMinIPChi2'  : 25.0,
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
                                'LLhMinBPVDIRA' : 0.9999,
                                'LLhSumPTMin'   : 2.0 * GeV,
                                'DetLLVChi2'    : 9.0,
                                'DetLLPt'       : 2.0 * GeV,
                                'DetLLVDChi2'   : 100.0,
                                'DetLLMinIPChi2': 15.0,
                                'DetLLMinBPVDIRA' : 0.99},
                 'PhiMuMu'  :  {'LLCombAMLow'   : (1020-40) * MeV,
                                'LLCombAMHigh'  : (1020+40) * MeV,
                                'LLCombMLow'    : (1020-25) * MeV,
                                'LLCombMHigh'   : (1020+25) * MeV,
                                'LLCombMaxDocaChi2': 9.0,
                                'TagMinIPChi2'  : 25.0,
                                'ProbeMinIPChi2': 9.0,
                                'ProbePt'       : 0 * MeV,
                                'TagPt'         : 500 * MeV,
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
                                'LLhMinBPVDIRA' : 0.9999,
                                'LLhSumPTMin'   : 1.0 * GeV,
                                'DetLLVChi2'    : 15.0,
                                'DetLLPt'       : 800 * MeV,
                                'DetLLVDChi2'   : 100.0,
                                'DetLLMinIPChi2': 15.0,
                                'DetLLMinBPVDIRA' : 0.99},
                 # Tag-and-probe phi->KK
                 'PhiKK'     : {'LLCombAMLow'   : (1020-40) * MeV,
                                'LLCombAMHigh'  : (1020+40) * MeV,
                                'LLCombMLow'    : (1020-20) * MeV,
                                'LLCombMHigh'   : (1020+20) * MeV,
                                'LLCombMaxDocaChi2' : 15.0,
                                'TagPt'         : 200.0 * MeV,
                                'TagP'          : 3.0 * GeV,
                                'TagMinIPChi2'  : 16.0,
                                'TagPIDK'       : 0.0,
                                'ProbePt'       : 0.0,
                                'ProbeP'        : 3.0 * GeV,
                                'ProbeMinIPChi2': 9.0,
                                'ProbeTisTos'   : [ ],
                                'LLhCombAMLow'  : (1968 - 80) * MeV,
                                'LLhCombAMHigh' : (1968 + 80) * MeV,
                                'LLhCombMLow'   : (1968 - 70) * MeV,
                                'LLhCombMHigh'  : (1968 + 70) * MeV,
                                'LLhVChi2'      : 25.0,
                                'LLhVDChi2'     : 50.0,
                                'LLhMaxIPChi2'  : 25.0,
                                'LLhMinBPVDIRA' : 0.9999,
                                'LLhSumPTMin'   : 1.0 * GeV,
                                'DetLLVChi2'    : 15.0,
                                'DetLLPt'       : 200.0 * MeV,
                                'DetLLVDChi2'   : 150.0,
                                'DetLLMinIPChi2': 25.0,
                                'DetLLMinBPVDIRA': 0.99},
                 # phi->KK with no kaon PID cuts.
                 'PhiKKUnbiased':{'Trk_ALL_MIPCHI2DV_MIN' : 16.0, # cuts on the phi daughter kaons
                                'Trk_ALL_PT_MIN': 0.0 * MeV,
                                'Trk_1OF2_MIPCHI2DV_MIN' : 40.0,
                                'Trk_1OF2_PT_MIN': 200.0 * MeV,
                                'ASUMPT_MIN'    : 200.0 * MeV,
                                'AMWindow'      : 40.0 * MeV, # phi mass window
                                'MaxDocaChi2'   : 15.0, #
                                'MWindow'       : 20.0 * MeV, # phi mass window
                                'LLhCombAMLow'  : (1968 - 80) * MeV, # Ds->phipi
                                'LLhCombAMHigh' : (1968 + 80) * MeV,
                                'LLhCombMLow'   : (1968 - 70) * MeV,
                                'LLhCombMHigh'  : (1968 + 70) * MeV,
                                'LLhVChi2'      : 25.0, # Ds vertex
                                'LLhVDChi2'     : 50.0, # Ds vertex displacement
                                'LLhMaxIPChi2'  : 25.0, # Ds max IPChi2 to PV
                                'LLhMinBPVDIRA' : 0.9999,
                                'LLhSumPTMin'   : 1.0 * GeV,
                                'DetLLVChi2'    : 15.0, # detached phi vertex
                                'DetLLPt'       : 800.0 * MeV, # detached phi pt
                                'DetLLVDChi2'   : 150.0, # detached phi displacement
                                'DetLLMinIPChi2': 25.0, # detached phi min IPChi2 to PV
                                'DetLLMinBPVDIRA': 0.995}, # detached phi min DIRA
                 'Lb2LcMuNu' : {'BachPt'        : 500.0 * MeV, # muon
                                'BachIPChi2'    : 16.0,
                                'LLhCombAMLow'  : 0 * MeV,
                                'LLhCombAMHigh' : (5620 + 400) * MeV,
                                'LLhCombBPVCORRMHigh' : (5620 + 400) * MeV,
                                'LLhCombBPVCORRMLow'  : (5620 - 1100) * MeV,
                                'LLhVChi2'      : 25,
                                'LLhVDChi2'     : 100.0,
                                'LLhMaxIPChi2'  : 200.0, # partially reconstructed, don't cut too hard
                                'LLhMinBPVDIRA' : 0.99,
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
                                'LLhMinBPVDIRA' : 0.9999,
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
                     'Muon'           : "|".join(["L0_CHANNEL('%s')" % chan for chan in l0_muons]),
                     'Electron'       : "|".join(["L0_CHANNEL('%s')" % chan for chan in l0_electrons]),
                     'ElectronFromL0' : "L0_CHANNEL('Electron')"
                     },
                 'Hlt1Req'  : {
                     'Muon'           : "HLT_PASS_RE('" + hlt1_muons + "')",
                     'Electron'       : "HLT_PASS_RE('" + hlt1_electrons + "')",
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
                                'AMOM_MIN'                 :  25000 * MeV,
                                'DPT_MIN'                  :  2000 * MeV,
                                'DMOM_MIN'                 :  30000 * MeV,
                                'BPVVDCHI2_MIN'            :  25,
                                'TisTosSpec'               :  "Hlt1.*Track.*Decision%TOS"},
                 'D02KPiPiPiTag' : {
                                'TagVCHI2PDOF_MAX'         :  15.0,
                                'DeltaM_AM_MIN'            :  0.0,
                                'DeltaM_AM_MAX'            :  190.0 * MeV,
                                'DeltaM_MIN'               :  0.0,
                                'DeltaM_MAX'               :  170.0 * MeV},
                 'D02KPi' : {   'Trk_ALL_PT_MIN'           : 250.0 * MeV,   # Stripping has 250
                                'Trk_ALL_P_MIN'            : 2.0 * GeV,     # Stripping has 2.0
                                'Trk_ALL_MIPCHI2DV_MIN'    : 16.0,           # Stripping has 16
                                # We currently have an implicit cut on track chi2/dof < 3 coming
                                # from the CharmHad filtered NoPIDs inputs; stripping had < 5
                                'Pair_AMINDOCA_MAX'        : 0.10 * mm,     # Not in stripping
                                'Trk_Max_APT_MIN'          : 1000.0 * MeV,  # Not in stripping
                                'D0_BPVVDCHI2_MIN'         : 49.0,          # Stripping has 49
                                'D0_BPVDIRA_MIN'           : 0.9999,        # Stripping has 0.9999
                                'D0_VCHI2PDOF_MAX'         : 10.0,          # Stripping has 13
                                'D0_BPVIPCHI2_Max'         : 20.0,          # Added for 25ns running based on EM data
                                'D0_PT_MIN'                : 1.5 * GeV,     # Stripping has 1.5
                                'Mass_M_MIN'               : 1790.0 * MeV,  # This is an M cut, stripping says 1790
                                'Comb_AM_MIN'              : 1780.0 * MeV,  # This is an AM cut, stripping says 1780
                                'Mass_M_MAX'               : 1940.0 * MeV,  # This is an M cut, stripping says 1940
                                'Comb_AM_MAX'              : 1950.0 * MeV,  # This is an AM cut, stripping says 1950
                                'TisTosSpec'               : [ ]},#"Hlt1(Two)?TrackMVADecision%TOS"},
                 'D02KPiTag' : {
                                'TagVCHI2PDOF_MAX'         :  15.0,         # Stripping has 13
                                'DeltaM_AM_MIN'            :  0.0,          # Stripping has (ADAMASS('D*(2010)+')<75)
                                'DeltaM_AM_MAX'            :  165.0 * MeV,  # Stripping has 165 
                                'DeltaM_MIN'               :  0.0,          # Stripping has 130
                                'DeltaM_MAX'               :  155.0 * MeV}, # Stripping has 155
                                                                            # Stripping has D* APT > 2.2 GeV
                 }

    def __apply_configuration__(self):
        from Stages import DetachedLLFilter, BCombiner, JPsiMuMuPosTagged, JPsiMuMuNegTagged, \
            JPsiEEPosTagged, JPsiEENegTagged, JPsiEEL0PosTagged, JPsiEEL0NegTagged, \
            PhiMuMuPosTagged, PhiMuMuNegTagged, KSFilter, BachelorPions, SharedSoftPions, \
            LambdaFilter, Lc2KPPi, D02K3Pi, D02KPi, Lc2KPPi_Prompt, BachelorKaons, BachelorMuons, \
            JPsiPPPosTagged, JPsiPPNegTagged, UnbiasedPhiKK, PhiKKPosTagged, PhiKKNegTagged
        from Inputs import KsLL, KsDD, LambdaLL, LambdaDD

        from Hlt2Lines.CharmHad.Stages import TagDecay

        ds2phipi = "[D_s+ -> phi(1020) pi+]cc"
        stagemap = {
            'Muon' : {
              'DetJPsiMuMuPosTagged'  : [ DetachedLLFilter("JPsiMuMuPos", JPsiMuMuPosTagged, nickname = "JPsiMuMu") ],
              'DetJPsiMuMuNegTagged'  : [ DetachedLLFilter("JPsiMuMuNeg", JPsiMuMuNegTagged, nickname = "JPsiMuMu") ],
              'DetPhiMuMuPosTagged'   : [ DetachedLLFilter("PhiMuMuPos", PhiMuMuPosTagged, nickname = "PhiMuMu") ],
              'DetPhiMuMuNegTagged'   : [ DetachedLLFilter("PhiMuMuNeg", PhiMuMuNegTagged, nickname = "PhiMuMu") ],
              'B2KJPsiMuMuPosTagged'   : [ BCombiner("JPsiMuMuPos", [ JPsiMuMuPosTagged, BachelorKaons ], nickname = "JPsiMuMu" ) ],
              'B2KJPsiMuMuNegTagged'   : [ BCombiner("JPsiMuMuNeg", [ JPsiMuMuNegTagged, BachelorKaons ], nickname = "JPsiMuMu" ) ],
              'Ds2PiPhiMuMuPosTagged'  : [ BCombiner('PhiMuMuPos', [ PhiMuMuPosTagged, BachelorPions ], nickname = "PhiMuMu", decay = ds2phipi ) ],
              'Ds2PiPhiMuMuNegTagged'  : [ BCombiner('PhiMuMuNeg', [ PhiMuMuNegTagged, BachelorPions ], nickname = "PhiMuMu", decay = ds2phipi ) ],
              },
            'Electron' : {
              'DetJPsiEEPosTagged'    : [ DetachedLLFilter("JPsiEEPos", JPsiEEPosTagged, nickname = "JPsiEE") ],
              'DetJPsiEENegTagged'    : [ DetachedLLFilter("JPsiEENeg", JPsiEENegTagged, nickname = "JPsiEE") ],
              'B2KJPsiEEPosTagged'     : [ BCombiner('JPsiEEPos', [ JPsiEEPosTagged, BachelorKaons ], nickname = "JPsiEE" ) ],
              'B2KJPsiEENegTagged'     : [ BCombiner('JPsiEENeg', [ JPsiEENegTagged, BachelorKaons ], nickname = "JPsiEE" ) ]
              },
            'ElectronFromL0' : {
              'DetJPsiEEL0PosTagged'  : [ DetachedLLFilter("JPsiEEL0Pos", JPsiEEL0PosTagged, nickname = "JPsiEE") ],
              'DetJPsiEEL0NegTagged'  : [ DetachedLLFilter("JPsiEEL0Neg", JPsiEEL0NegTagged, nickname = "JPsiEE") ],
              'B2KJPsiEEL0PosTagged'   : [ BCombiner('JPsiEEL0Pos', [ JPsiEEL0PosTagged, BachelorKaons ], nickname = "JPsiEE" ) ],
              'B2KJPsiEEL0NegTagged'   : [ BCombiner('JPsiEEL0Neg', [ JPsiEEL0NegTagged, BachelorKaons ], nickname = "JPsiEE" ) ]
              },
            'Other' : {
              'Ks2PiPiLL'             : [ KSFilter('Ks2PiPiLL', KsLL) ],
              'Ks2PiPiDD'             : [ KSFilter('Ks2PiPiDD', KsDD) ],
              'Lambda2PPiLL'          : [ LambdaFilter('Lambda2PPiLL', LambdaLL) ],
              'Lambda2PPiDD'          : [ LambdaFilter('Lambda2PPiDD', LambdaDD) ],
              'Lambda2PPiLLhighPT'    : [ LambdaFilter('Lambda2PPiLLhighPT', LambdaLL) ],
              'Lambda2PPiDDhighPT'    : [ LambdaFilter('Lambda2PPiDDhighPT', LambdaDD) ],
              'Lambda2PPiLLveryhighPT': [ LambdaFilter('Lambda2PPiLLveryhighPT', LambdaLL) ],
              'Lambda2PPiDDveryhighPT': [ LambdaFilter('Lambda2PPiDDveryhighPT', LambdaDD) ],
              'Lambda2PPiLLisMuon'    : [ LambdaFilter('Lambda2PPiLLisMuon', LambdaLL, ismuon = True) ],
              'Lambda2PPiDDisMuon'    : [ LambdaFilter('Lambda2PPiDDisMuon', LambdaDD, ismuon = True) ],
              'DetJPsiPPPosTagged'    : [ DetachedLLFilter("JPsiPPPos", JPsiPPPosTagged, nickname = "JPsiPP") ],
              'DetJPsiPPNegTagged'    : [ DetachedLLFilter("JPsiPPNeg", JPsiPPNegTagged, nickname = "JPsiPP") ],
              'B2KJPsiPPPosTagged'    : [ BCombiner('JPsiPPPos', [ JPsiPPPosTagged, BachelorKaons ], nickname = "JPsiPP") ],
              'B2KJPsiPPNegTagged'    : [ BCombiner('JPsiPPNeg', [ JPsiPPNegTagged, BachelorKaons ], nickname = 'JPsiPP') ],
              'Lb2LcPi'               : [ BCombiner('Lb2LcPi', [ Lc2KPPi, BachelorPions ], decay = '[Lambda_b0 -> Lambda_c+ pi-]cc') ],
              'Lb2LcMuNu'             : [ BCombiner('Lb2LcMuNu', [ Lc2KPPi, BachelorMuons ], decay = '[Lambda_b0 -> Lambda_c+ mu-]cc', corrm = True) ],
              # These next two need shared=True so the Turbo versions don't re-do the combination
              'Sc02LcPi'              : [ TagDecay('PIDSc02LcPi', "[Sigma_c0 -> Lambda_c+ pi-]cc", inputs = [ Lc2KPPi_Prompt, SharedSoftPions ],
                                            nickname = 'Sc2LcPi', shared = True) ],
              'Scpp2LcPi'             : [ TagDecay('PIDScpp2LcPi', "[Sigma_c++ -> Lambda_c+ pi+]cc", inputs = [ Lc2KPPi_Prompt, SharedSoftPions ],
                                            nickname = 'Sc2LcPi', shared = True) ],
              # D0 -> K3pi noPID line
              'D02KPiPiPi'            : [ D02K3Pi ],
              'D02KPiPiPiTag'         : [ TagDecay('PIDD02KPiPiPiTag', "[D*(2010)+ -> D0 pi+]cc", inputs = [ D02K3Pi, SharedSoftPions ], shared = True,
                                            nickname = 'D02KPiPiPiTag') ],

              # D0 -> Kpi noPID line
              'D02KPiTag'             : [ TagDecay('PIDD02KPiTag', "[D*(2010)+ -> D0 pi+]cc", inputs = [ D02KPi, SharedSoftPions ], shared = True,
                                            nickname = 'D02KPiTag') ],
              'DetPhiKKUnbiased'      : [ DetachedLLFilter("PhiKKUnbiased", UnbiasedPhiKK, nickname = "PhiKKUnbiased") ],
              'Ds2PiPhiKKUnbiased'    : [ BCombiner("PhiKKUnbiased", [ UnbiasedPhiKK, BachelorPions ], nickname = 'PhiKKUnbiased', decay = ds2phipi) ],
              'DetPhiKKPosTagged'     : [ DetachedLLFilter("PhiKKPos", PhiKKPosTagged, nickname = "PhiKK") ],
              'DetPhiKKNegTagged'     : [ DetachedLLFilter("PhiKKNeg", PhiKKNegTagged, nickname = 'PhiKK') ],
              'Ds2PiPhiKKPosTagged'   : [ BCombiner('PhiKKPos', [ PhiKKPosTagged, BachelorPions ], nickname = "PhiKK", decay = ds2phipi ) ],
              'Ds2PiPhiKKNegTagged'   : [ BCombiner('PhiKKNeg', [ PhiKKNegTagged, BachelorPions ], nickname = "PhiKK", decay = ds2phipi ) ]
              }
            }
        
        from HltLine.HltLine import Hlt2Line
        for turbo in turboversions:
          for category, stages in stagemap.iteritems():
            for (nickname, algos) in self.algorithms(stages).iteritems():
                linename = 'PID' + nickname
                l0reqdict = self.getProp("L0Req")
                hlt1reqdict = self.getProp('Hlt1Req')
                l0cuts = l0reqdict.get(nickname, l0reqdict.get(category, None))
                hlt1cuts = hlt1reqdict.get(nickname, hlt1reqdict.get(category, None))
                doturbo = (len(turbo)>0)
                Hlt2Line(linename + turbo,
                         prescale = self.prescale,
                         algos = algos, postscale = self.postscale,
                         L0DU = l0cuts,
                         HLT1 = hlt1cuts,
                         Turbo = doturbo)
          from Configurables import HltANNSvc
          HltANNSvc().Hlt2SelectionID.update({ 'Hlt2PID' + n + turbo + 'Decision' : v + 200*(len(turbo)>0) for n, v in {
            'DetJPsiMuMuPosTagged'  : 50020,
            'DetJPsiMuMuNegTagged'  : 50021,
            'DetPhiMuMuPosTagged'   : 50022,
            'DetPhiMuMuNegTagged'   : 50023,
            'B2KJPsiMuMuPosTagged'  : 50024,
            'B2KJPsiMuMuNegTagged'  : 50025,
            'Ds2PiPhiMuMuPosTagged' : 50026,
            'Ds2PiPhiMuMuNegTagged' : 50027,
            'Lb2LcMuNu'             : 50028,
            'DetJPsiEEPosTagged'    : 50029,
            'DetJPsiEENegTagged'    : 50030,
            'B2KJPsiEEPosTagged'    : 50031,
            'B2KJPsiEENegTagged'    : 50032,
            'DetJPsiEEL0PosTagged'  : 50033,
            'DetJPsiEEL0NegTagged'  : 50034,
            'B2KJPsiEEL0PosTagged'  : 50035,
            'B2KJPsiEEL0NegTagged'  : 50036,
            'Ks2PiPiLL'             : 50037,
            'Ks2PiPiDD'             : 50038,
            'Lambda2PPiLL'          : 50039,
            'Lambda2PPiDD'          : 50040,
            'Lambda2PPiLLhighPT'    : 50041,
            'Lambda2PPiDDhighPT'    : 50042,
            'Lambda2PPiLLisMuon'    : 50043,
            'Lambda2PPiDDisMuon'    : 50044,
            'Lb2LcPi'               : 50045,
            'Sc02LcPi'              : 50046,
            'Scpp2LcPi'             : 50047,
            'D02KPiPiPi'            : 50048,
            'D02KPiPiPiTag'         : 50049,
            'D02KPiTag'             : 50050,
            'Lambda2PPiLLveryhighPT': 50051,
            'Lambda2PPiDDveryhighPT': 50052,
            'DetJPsiPPPosTagged'    : 50053,
            'DetJPsiPPNegTagged'    : 50054,
            'B2KJPsiPPPosTagged'    : 50055,
            'B2KJPsiPPNegTagged'    : 50056,
            'DetPhiKKUnbiased'      : 50057,
            'Ds2PiPhiKKUnbiased'    : 50058,
            'DetPhiKKPosTagged'     : 50059,
            'DetPhiKKNegTagged'     : 50060,
            'Ds2PiPhiKKPosTagged'   : 50061,
            'Ds2PiPhiKKNegTagged'   : 50062
            }.iteritems()})
