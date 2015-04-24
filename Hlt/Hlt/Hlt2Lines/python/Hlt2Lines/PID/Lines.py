__author__  = 'Olli Lupton'
__date__    = '$Date: 2015-03-12$'
__version__ = '$Revision: 0.0 $'

from GaudiKernel.SystemOfUnits import GeV, MeV, mm
from GaudiKernel.SystemOfUnits import picosecond as ps
from Hlt2Lines.Utilities.Hlt2LinesConfigurableUser import Hlt2LinesConfigurableUser

class PIDLines(Hlt2LinesConfigurableUser):
    l0_muons = ['Muon','DiMuon']
    l0_electrons = ['Photon', 'Electron', 'Hadron', 'Muon', 'DiMuon']
    hlt1_muons = 'Hlt1(TrackAllL0|TrackMuon|SingleMuon|DiMuon|TrackMVA|TwoTrackMVA).*Decision'
    hlt1_electrons = 'Hlt1.*Decision'
    __slots__ = {'Prescale' :  {
                                'Hlt2PIDLambda2PPiLL'           : 1.0, # will go down
                                'Hlt2PIDLambda2PPiLLhighP'      : 1.0, # probably can't be 1.0
                                'Hlt2PIDLambda2PPiLLveryhighP'  : 1.0, # maybe P cut tuned so it can be 1.0?
                                'Hlt2PIDLambda2PPiLLisMuon'     : 1.0, # hopefully can be 1.0
                                'Hlt2PIDLambda2PPiDD'           : 0.0,
                                'Hlt2PIDLambda2PPiDDhighP'      : 0.0,
                                'Hlt2PIDLambda2PPiDDisMuon'     : 0.0,
                                'Hlt2PIDKs2PiPiLL'              : 1.0,
                                'Hlt2PIDKs2PiPiDD'              : 0.0,
                                'Hlt2PIDDetJPsiEEL0PosTagged'   : 0.0, # verdict seems to be that we
                                'Hlt2PIDDetJPsiEEL0NegTagged'   : 0.0, # will use the full calo and
                                'Hlt2PIDB2KJPsiEESSTagged'      : 0.0, # have no need for this suite
                                'Hlt2PIDB2KJPsiEEOSTagged'      : 0.0  # of lines
                                },
                  'Common'   : {'TagTrChi2'     : 3.0,
                                'TagP'          : 3 * GeV, # 6GeV in old stripping
                                'TagPt'         : 500 * MeV, # 1.5GeV in old stripping
                                'ProbeTrChi2'   : 5, # no cut for ee in old stripping, 3 for detached mumu
                                'ProbeP'        : 3 * GeV, # 3 GeV for all but B -> (J/psi->mumu)K where it's zero in old stripping
                                'BachPt'        : 0, # no cut in old stripping
                                'BachP'         : 0, # no cut in J/psi -> mumu in old stripping, 1 GeV for old J/psi->ee
                                'BachIPChi2'    : 9.0, # old J/psi -> mumu cut of 25, 9 for J/psi -> ee
                                'LambdaMWindow' : 25 * MeV,
                                'LambdaKsVeto'  : 20 * MeV
                                },
                 'Lambda2PPiLL' : {
                                'LambdaProtonP' : 2.0 * GeV},
                 'Lambda2PPiDD' : {
                                'LambdaProtonP' : 2.0 * GeV},
                 'Lambda2PPiLLhighP' : {
                                'LambdaProtonP' : 40.0 * GeV},
                 'Lambda2PPiDDhighP' : {
                                'LambdaProtonP' : 40.0 * GeV},
                 'Lambda2PPiLLveryhighP' : {
                                'LambdaProtonP' : 60.0 * GeV},
                 'Lambda2PPiDDveryhighP' : {
                                'LambdaProtonP' : 60.0 * GeV},
                 'Lambda2PPiLLisMuon' : {
                                'LambdaProtonP' : 2.0 * GeV},
                 'Lambda2PPiDDisMuon' : {
                                'LambdaProtonP' : 2.0 * GeV},
                 'JPsiMuMu' :  {'ProbePt'       : 800 * MeV,
                                'ProbeMinIPChi2': 10,
                                'ProbeTisTos'   : [
                                  "L0(" + "|".join(l0_muons) + ")Decision%TIS",
                                  hlt1_muons + "%TIS"
                                  ],
                                'TagMinIPChi2'  : 25,
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
                                'DetLLVChi2'    : 5,
                                'DetLLPt'       : 0 * GeV,
                                'DetLLVDChi2'   : 25},
                 'JPsiEE'   :  {'ProbePt'       : 500 * MeV,
                                'ProbeMinIPChi2': 9,
                                'ProbeTisTos'   : [
                                  "L0(" + "|".join(l0_electrons) + ")Decision%TIS",
                                  hlt1_electrons + "%TIS"
                                  ],
                                'TagMinIPChi2'  : 25,
                                'TagPIDe'       : 5.0,
                                'LLCombAMLow'   : 2050 * MeV,
                                'LLCombAMHigh'  : 4350 * MeV,
                                'LLCombMLow'    : 2100 * MeV,
                                'LLCombMHigh'   : 4300 * MeV,
                                'LLCombMaxDocaChi2': 20,
                                'LLhCombAMLow'  : (5279-1100) * MeV,
                                'LLhCombAMHigh' : (5279+1100) * MeV,
                                'LLhCombMLow'   : (5279-1000) * MeV,
                                'LLhCombMHigh'  : (5279+1000) * MeV,
                                'LLhVChi2'      : 18,
                                'LLhVDChi2'     : 25,
                                'LLhMaxIPChi2'  : 100,
                                'DetLLVChi2'    : 18,
                                'DetLLPt'       : 2 * GeV,
                                'DetLLVDChi2'   : 50},
                 'PhiMuMu'  :  {'LLCombAMLow'   : (1020-100) * MeV,
                                'LLCombAMHigh'  : (1020+100) * MeV,
                                'LLCombMLow'    : (1020-80) * MeV,
                                'LLCombMHigh'   : (1020+80) * MeV,
                                'LLCombMaxDocaChi2': 10,
                                'TagMinIPChi2'  : 9.0,
                                'ProbeMinIPChi2': 9.0,
                                'ProbePt'       : 300 * MeV,
                                'ProbeTisTos'   : [
                                  "L0(" + "|".join(l0_muons) + ")Decision%TIS",
                                  hlt1_muons + '%TIS'
                                  ],
                                'LLhCombAMLow'  : (1968 - 120) * MeV,
                                'LLhCombAMHigh' : (1968 + 120) * MeV,
                                'LLhCombMLow'   : (1968 - 100) * MeV, # Ds+ -> phi pi+
                                'LLhCombMHigh'  : (1968 + 100) * MeV,
                                'LLhVChi2'      : 25,
                                'LLhVDChi2'     : 25,
                                'LLhMaxIPChi2'  : 25,
                                'DetLLVChi2'    : 25,
                                'DetLLPt'       : 1.5 * GeV,
                                'DetLLVDChi2'   : 30},
                 'Lb2LcMuNu' : {'LLhCombAMLow'  : 0 * MeV,
                                'LLhCombAMHigh' : (5620 + 400) * MeV,
                                'LLhCombBPVCORRMHigh' : (5620 + 400) * MeV,
                                'LLhVChi2'      : 25,
                                'LLhVDChi2'     : 25,
                                'LLhMaxIPChi2'  : 200},
                 'Lb2LcPi'   : {'LLhCombAMLow'  : (5620 - 400) * MeV,
                                'LLhCombAMHigh' : (5620 + 400) * MeV,
                                'LLhCombMLow'   : (5620 - 350) * MeV,
                                'LLhCombMHigh'  : (5620 + 350) * MeV,
                                'LLhVChi2'      : 25,
                                'LLhVDChi2'     : 25,
                                'LLhMaxIPChi2'  : 25},
                 'Ks2PiPiLL' : {'KsMassWindow'  : 50 * MeV,
                                'KsVChi2'       : 16,
                                'KsDecayMaxZ'   : 2200 * mm,
                                'KsVDChi2'      : 25,
                                'KsLambdaVeto' : 9 * MeV},
                 'Ks2PiPiDD' : {'KsMassWindow'  : 50 * MeV,
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
                 # Now the combiner for the PID calib lines
                 # The pointing cut is tight to ensure a purer signal even without
                 # a PID cut on the proton
                 'Lc2KPPi' : {
                                'Trk_ALL_MIPCHI2DV_MIN'    :  6.0,
                                'Trk_2OF3_MIPCHI2DV_MIN'   :  9.0,
                                'Trk_1OF3_MIPCHI2DV_MIN'   :  12.0,
                                'Trk_ALL_PT_MIN'           :  200.0 * MeV,
                                'Trk_2OF3_PT_MIN'          :  400.0 * MeV,
                                'Trk_1OF3_PT_MIN'          :  400.0 * MeV,
                                'BPVVDCHI2_MIN'            :  10.0,
                                'BPVLTIME_MIN'             :  0.1 * ps,
                                'BPVDIRA_MIN'              :  0.0,
                                'ASUMPT_MIN'               :  2000 * MeV,
                                'AM_MIN'                   :  2201 * MeV,
                                'AM_MAX'                   :  2371 * MeV,
                                'Mass_M_MIN'               :  2211.0 * MeV,
                                'Mass_M_MAX'               :  2361.0 * MeV,
                                'VCHI2PDOF_MAX'            :  10.0, # took this one from CharmHad Common
                                'TisTosSpec'               :  [ ]},#"Hlt1(Two)?TrackMVADecision%TOS"}, # changed this
                 # Note the pointing cut is now in the tagger so that we can re-use
                 # the Lc candidates in the PIDCALIB lines for Lb -> Lc pi and Lb -> Lc mu nu
                 'Sc2LcPi' : {
                                'DeltaM_AM_MIN'            :  150.0 * MeV,
                                'DeltaM_MIN'               :  155.0 * MeV,
                                'DeltaM_AM_MAX'            :  275.0 * MeV,
                                'DeltaM_MAX'               :  270.0 * MeV,
                                'TagVCHI2PDOF_MAX'         :  100.0,
                                'BPVDIRA_MIN'              :  0.99985},
                 'D02KPiPiPi' : {
                                'AM_MAX'                   :  2100 * MeV,
                                'ASUMPT_MIN'               :  0.0 * MeV,
                                'ACHI2DOCA_MAX'            :  16.0,
                                'VCHI2PDOF_MAX'            :  15.0,
                                'BPVDIRA_MIN'              :  0.9999,
                                'BPVLTIME_MIN'             :  0.1 * ps,
                                'Mass_M_MIN'               :  1790 * MeV,
                                'Mass_M_MAX'               :  1940 * MeV,
                                'TisTosSpec'               :  [ ]},#"Hlt1(Two)?TrackMVADecision%TOS"},
                 'D02KPiPiPiTag' : {
                                'TagVCHI2PDOF_MAX'         :  15.0,
                                'DeltaM_AM_MIN'            :  0.0,
                                'DeltaM_AM_MAX'            :  190.0 * MeV,
                                'DeltaM_MIN'               :  0.0,
                                'DeltaM_MAX'               :  170.0 * MeV},
                 'D02KPi' : {   'Trk_ALL_PT_MIN'           : 800.0 * MeV,   # Stripping has 250
                                'Trk_ALL_P_MIN'            : 5.0 * GeV,     # Stripping has 2.0
                                'Trk_ALL_MIPCHI2DV_MIN'    : 2.0,           # Stripping has 16
                                # We curently have an implicit cut on track chi2/dof < 3 coming
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
                                'DeltaM_AM_MAX'            :  190.0 * MeV,  # Stripping has 165 
                                'DeltaM_MIN'               :  0.0,          # Stripping has 130
                                'DeltaM_MAX'               :  170.0 * MeV}  # Stripping has 155
                                                                            # Stripping has D* APT > 2.2 GeV
                 }

    def __apply_configuration__(self):
        from Stages import DetachedLLFilter, BCombiner, JPsiMuMuPosTagged, JPsiMuMuNegTagged, \
            JPsiEEPosTagged, JPsiEENegTagged, JPsiEEL0PosTagged, JPsiEEL0NegTagged, \
            PhiMuMuPosTagged, PhiMuMuNegTagged, KSFilter, \
            LambdaFilter, Lc2KPPi, D02K3Pi, D02KPi
        from Inputs import KsLL, KsDD, LambdaLL, LambdaDD, Muons

        from Hlt2Lines.CharmHad.Lines import CharmHadLines
        from Hlt2Lines.CharmHad.Stages import SharedNoPIDDetachedChild_K, SharedNoPIDDetachedChild_pi, SharedSoftTagChild_pi, TagDecay
        from Hlt2Lines.Utilities.Hlt2Stage import Hlt2ExternalStage
        FilteredKaons = Hlt2ExternalStage(CharmHadLines(), SharedNoPIDDetachedChild_K)
        FilteredPions = Hlt2ExternalStage(CharmHadLines(), SharedNoPIDDetachedChild_pi)
        SharedSoftPions = Hlt2ExternalStage(CharmHadLines(), SharedSoftTagChild_pi)

        ds2phipi = "[D_s+ -> phi(1020) pi+]cc"
        stagemap = {
            'Muon' : {
              'DetJPsiMuMuPosTagged'  : [ DetachedLLFilter("JPsiMuMuPos", JPsiMuMuPosTagged, nickname = "JPsiMuMu") ],
              'DetJPsiMuMuNegTagged'  : [ DetachedLLFilter("JPsiMuMuNeg", JPsiMuMuNegTagged, nickname = "JPsiMuMu") ],
              'DetPhiMuMuPosTagged'   : [ DetachedLLFilter("PhiMuMuPos", PhiMuMuPosTagged, nickname = "PhiMuMu") ],
              'DetPhiMuMuNegTagged'   : [ DetachedLLFilter("PhiMuMuNeg", PhiMuMuNegTagged, nickname = "PhiMuMu") ],
              'B2KJPsiMuMuSSTagged'   : [ BCombiner("JPsiMuMuSS", [ JPsiMuMuPosTagged, FilteredKaons ], nickname = "JPsiMuMu" ) ],
              'B2KJPsiMuMuOSTagged'   : [ BCombiner("JPsiMuMuOS", [ JPsiMuMuNegTagged, FilteredKaons ], nickname = "JPsiMuMu" ) ],
              'Ds2PiPhiMuMuSSTagged'  : [ BCombiner('PhiMuMuSS', [ PhiMuMuPosTagged, FilteredPions ], nickname = "PhiMuMu", decay = ds2phipi ) ],
              'Ds2PiPhiMuMuOSTagged'  : [ BCombiner('PhiMuMuOS', [ PhiMuMuNegTagged, FilteredPions ], nickname = "PhiMuMu", decay = ds2phipi ) ],
              'Lb2LcMuNu'             : [ BCombiner('Lb2LcMuNu', [ Lc2KPPi, Muons ], decay = '[Lambda_b0 -> Lambda_c+ mu-]cc', corrm = True) ]
              },
            'Electron' : {
              'DetJPsiEEPosTagged'    : [ DetachedLLFilter("JPsiEEPos", JPsiEEPosTagged, nickname = "JPsiEE") ],
              'DetJPsiEENegTagged'    : [ DetachedLLFilter("JPsiEENeg", JPsiEENegTagged, nickname = "JPsiEE") ],
              'B2KJPsiEESSTagged'     : [ BCombiner('JPsiEESS', [ JPsiEEPosTagged, FilteredKaons ], nickname = "JPsiEE" ) ],
              'B2KJPsiEEOSTagged'     : [ BCombiner('JPsiEEOS', [ JPsiEENegTagged, FilteredKaons ], nickname = "JPsiEE" ) ]
              },
            'ElectronFromL0' : {
              'DetJPsiEEL0PosTagged'  : [ DetachedLLFilter("JPsiEEL0Pos", JPsiEEL0PosTagged, nickname = "JPsiEE") ],
              'DetJPsiEEL0NegTagged'  : [ DetachedLLFilter("JPsiEEL0Neg", JPsiEEL0NegTagged, nickname = "JPsiEE") ],
              'B2KJPsiEEL0SSTagged'   : [ BCombiner('JPsiEEL0SS', [ JPsiEEL0PosTagged, FilteredKaons ], nickname = "JPsiEE" ) ],
              'B2KJPsiEEL0OSTagged'   : [ BCombiner('JPsiEEL0OS', [ JPsiEEL0NegTagged, FilteredKaons ], nickname = "JPsiEE" ) ]
              },
            'Other' : {
              'Ks2PiPiLL'             : [ KSFilter('Ks2PiPiLL', KsLL) ],
              'Ks2PiPiDD'             : [ KSFilter('Ks2PiPiDD', KsDD) ],
              'Lambda2PPiLL'          : [ LambdaFilter('Lambda2PPiLL', LambdaLL) ],
              'Lambda2PPiDD'          : [ LambdaFilter('Lambda2PPiDD', LambdaDD) ],
              'Lambda2PPiLLhighP'     : [ LambdaFilter('Lambda2PPiLLhighP', LambdaLL) ],
              'Lambda2PPiDDhighP'     : [ LambdaFilter('Lambda2PPiDDhighP', LambdaDD) ],
              'Lambda2PPiLLveryhighP' : [ LambdaFilter('Lambda2PPiLLveryhighP', LambdaLL) ],
              'Lambda2PPiDDveryhighP' : [ LambdaFilter('Lambda2PPiDDveryhighP', LambdaDD) ],
              'Lambda2PPiLLisMuon'    : [ LambdaFilter('Lambda2PPiLLisMuon', LambdaLL, ismuon = True) ],
              'Lambda2PPiDDisMuon'    : [ LambdaFilter('Lambda2PPiDDisMuon', LambdaDD, ismuon = True) ],
              'Lb2LcPi'               : [ BCombiner('Lb2LcPi', [ Lc2KPPi, FilteredPions ], decay = '[Lambda_b0 -> Lambda_c+ pi-]cc') ],
              # These next two need shared=True so the Turbo versions don't re-do the combination
              'Sc02LcPi'              : [ TagDecay('PIDSc02LcPi', "[Sigma_c0 -> Lambda_c+ pi-]cc", inputs = [ Lc2KPPi, SharedSoftPions ],
                                            DaughtersCuts = { "Lambda_c+" : "(BPVDIRA > %(BPVDIRA_MIN)s )" }, nickname = 'Sc2LcPi', shared = True) ],
              'Scpp2LcPi'             : [ TagDecay('PIDScpp2LcPi', "[Sigma_c++ -> Lambda_c+ pi+]cc", inputs = [ Lc2KPPi, SharedSoftPions ],
                                            DaughtersCuts = { "Lambda_c+" : "(BPVDIRA > %(BPVDIRA_MIN)s )" }, nickname = 'Sc2LcPi', shared = True) ],
              # D0 -> K3pi noPID line
              'D02KPiPiPi'            : [ D02K3Pi ],
              'D02KPiPiPiTag'         : [ TagDecay('PIDD02KPiPiPiTag', "[D*(2010)+ -> D0 pi+]cc", inputs = [ D02K3Pi, SharedSoftPions ], shared = True,
                                            nickname = 'D02KPiPiPiTag') ],

              # D0 -> Kpi noPID line
              'D02KPiTag'             : [ TagDecay('PIDD02KPiTag', "[D*(2010)+ -> D0 pi+]cc", inputs = [ D02KPi, SharedSoftPions ], shared = True,
                                            nickname = 'D02KPiTag') ]
              }
            }
        
        from HltLine.HltLine import Hlt2Line
        for turbo in ['','Turbo']:
          for category, stages in stagemap.iteritems():
            for (nickname, algos) in self.algorithms(stages).iteritems():
                linename = 'PID' + nickname
                l0cuts = self.getProp("L0Req").get(category, None)
                hlt1cuts = self.getProp("Hlt1Req").get(category, None)
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
            'B2KJPsiMuMuSSTagged'   : 50024,
            'B2KJPsiMuMuOSTagged'   : 50025,
            'Ds2PiPhiMuMuSSTagged'  : 50026,
            'Ds2PiPhiMuMuOSTagged'  : 50027,
            'Lb2LcMuNu'             : 50028,
            'DetJPsiEEPosTagged'    : 50029,
            'DetJPsiEENegTagged'    : 50030,
            'B2KJPsiEESSTagged'     : 50031,
            'B2KJPsiEEOSTagged'     : 50032,
            'DetJPsiEEL0PosTagged'  : 50033,
            'DetJPsiEEL0NegTagged'  : 50034,
            'B2KJPsiEEL0SSTagged'   : 50035,
            'B2KJPsiEEL0OSTagged'   : 50036,
            'Ks2PiPiLL'             : 50037,
            'Ks2PiPiDD'             : 50038,
            'Lambda2PPiLL'          : 50039,
            'Lambda2PPiDD'          : 50040,
            'Lambda2PPiLLhighP'     : 50041,
            'Lambda2PPiDDhighP'     : 50042,
            'Lambda2PPiLLisMuon'    : 50043,
            'Lambda2PPiDDisMuon'    : 50044,
            'Lb2LcPi'               : 50045,
            'Sc02LcPi'              : 50046,
            'Scpp2LcPi'             : 50047,
            'D02KPiPiPi'            : 50048,
            'D02KPiPiPiTag'         : 50049,
            'D02KPiTag'             : 50050,
            'Lambda2PPiLLveryhighP' : 50051,
            'Lambda2PPiDDveryhighP' : 50052
            }.iteritems()})
