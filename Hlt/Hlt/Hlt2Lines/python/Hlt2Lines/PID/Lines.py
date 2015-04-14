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
                                'Hlt2PIDLambda2PPiLL' : 1.0,
                                'Hlt2PIDLambda2PPiDD' : 0.0,
                                'Hlt2PIDLambda2PPiDDhighP' : 0.0,
                                'Hlt2PIDLambda2PPiDDisMuon' : 0.0,
                                'Hlt2PIDKs2PiPiLL'    : 1.0,
                                'Hlt2PIDKs2PiPiDD'    : 0.0
                                },
                  'Common'   : {'TagTrChi2'     : 3,
                                'TagP'          : 3 * GeV, # 6GeV in old stripping
                                'TagPt'         : 500 * MeV, # 1.5GeV in old stripping
                                'ProbeTrChi2'   : 5, # no cut for ee in old stripping, 3 for detached mumu
                                'ProbeP'        : 3 * GeV, # 3 GeV for all but B -> (J/psi->mumu)K where it's zero in old stripping
                                #'BachTrChi2DoF' : 3,
                                #'BachPt'        : 0, # no cut in old stripping
                                #'BachP'         : 0, # no cut in J/psi -> mumu in old stripping, 1 GeV for old J/psi->ee
                                #'BachIPChi2'    : 9, # old J/psi -> mumu cut of 25, 9 for J/psi -> ee
                                'LambdaProtonTrChi2' : 5,
                                'LambdaPionP'   : 2.0 * GeV,
                                'LambdaPionTrChi2' : 5,
                                'LambdaAMWindow': 50 * MeV,
                                'LambdaMWindow' : 25 * MeV,
                                'LambdaVChi2'   : 16,
                                'LambdaVDChi2'  : 50,
                                'LambdaKsVeto'  : 0 * MeV,
                                'LambdaLLTau'   : 2.0 * ps,
                                'LambdaDDBPVVDZ': 400 * mm
                                },
                 'Lambda2PPiLL' : {
                                'LambdaProtonP' : 2.0 * GeV,
                                'LambdaProtonMinIPChi2' : 35,
                                'LambdaPionMinIPChi2' : 35},
                 'Lambda2PPiDD' : {
                                'LambdaProtonP' : 2.0 * GeV,
                                'LambdaProtonMinIPChi2' : 35,
                                'LambdaPionMinIPChi2' : 35},
                 'Lambda2PPiLLhighP' : {
                                'LambdaProtonP'  : 40.0 * GeV,
                                'LambdaProtonMinIPChi2' : 25,
                                'LambdaPionMinIPChi2' : 25},
                 'Lambda2PPiDDhighP' : {
                                'LambdaProtonP'  : 40.0 * GeV,
                                'LambdaProtonMinIPChi2' : 25,
                                'LambdaPionMinIPChi2' : 25},
                 'Lambda2PPiLLisMuon' : {
                                'LambdaProtonP' : 2.0 * GeV,
                                'LambdaProtonMinIPChi2' : 25,
                                'LambdaPionMinIPChi2' : 25},
                 'Lambda2PPiDDisMuon' : {
                                'LambdaProtonP' : 2.0 * GeV,
                                'LambdaProtonMinIPChi2' : 25,
                                'LambdaPionMinIPChi2' : 25},
                 'JPsiMuMu' :  {'ProbePt'       : 800 * MeV,
                                'ProbeMinIPChi2': 10,
                                'ProbeTisTos'   : [
                                  #"L0(" + "|".join(l0_muons) + ")Decision%TIS",
                                  hlt1_muons + "%TIS"
                                  ],
                                'TagMinIPChi2'  : 25,
                                'LLCombMLow'    : (3096-200) * MeV,
                                'LLCombMHigh'   : (3096+200) * MeV,
                                'LLCombMaxDocaChi2' : 10,
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
                                  #"L0(" + "|".join(l0_electrons) + ")Decision%TIS",
                                  hlt1_electrons + "%TIS"
                                  ],
                                'TagMinIPChi2'  : 25,
                                'TagPIDe'       : 5.0,
                                'LLCombMLow'    : 2100,
                                'LLCombMHigh'   : 4300,
                                'LLCombMaxDocaChi2': 20,
                                'LLhCombMLow'   : (5279-1000) * MeV,
                                'LLhCombMHigh'  : (5279+1000) * MeV,
                                'LLhVChi2'      : 18,
                                'LLhVDChi2'     : 25,
                                'LLhMaxIPChi2'  : 100,
                                'DetLLVChi2'    : 18,
                                'DetLLPt'       : 2 * GeV,
                                'DetLLVDChi2'   : 50},
                 'PhiMuMu'  :  {'LLCombMLow'    : (1020-100) * MeV,
                                'LLCombMHigh'   : (1020+100) * MeV,
                                'LLCombMaxDocaChi2': 10,
                                'TagMinIPChi2'  : 0,
                                'ProbeMinIPChi2': 0,
                                'ProbePt'       : 300 * MeV,
                                'ProbeTisTos'   : [
                                  #"L0(" + "|".join(l0_muons) + ")Decision%TIS",
                                  hlt1_muons + '%TIS'
                                  ],
                                'LLhCombMLow'   : (1968 - 300) * MeV, # Ds+ -> phi pi+
                                'LLhCombMHigh'  : (1968 + 300) * MeV,
                                'LLhVChi2'      : 25,
                                'LLhVDChi2'     : 25,
                                'LLhMaxIPChi2'  : 25,
                                'DetLLVChi2'    : 25,
                                'DetLLPt'       : 0 * GeV,
                                'DetLLVDChi2'   : 25},
                 'Lb2LcMuNu' : {'LLhCombMLow'   : (0) * MeV,
                                'LLhCombMHigh'  : (5620 + 400) * MeV,
                                'LLhVChi2'      : 25,
                                'LLhVDChi2'     : 25,
                                'LLhMaxIPChi2'  : 200},
                 'Lb2LcPi'   : {'LLhCombMLow'   : (5620 - 400) * MeV,
                                'LLhCombMHigh'  : (5620 + 400) * MeV,
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
                                'TisTosSpec'               : "Hlt1(Two)?TrackMVADecision%TOS"}, # changed this
                 # Note the pointing cut is now in the tagger so that we can re-use
                 # the Lc candidates in the PIDCALIB lines for Lb -> Lc pi and Lb -> Lc mu nu
                 'Sc2LcPi' : {
                                'DeltaM_AM_MIN'            :  150.0 * MeV,
                                'DeltaM_MIN'               :  155.0 * MeV,
                                'DeltaM_AM_MAX'            :  180.0 * MeV,
                                'DeltaM_MAX'               :  175.0 * MeV,
                                'TagVCHI2PDOF_MAX'         :  100.0,
                                'BPVDIRA_MIN'              :  0.99985,
                                'TisTosSpec'               : "Hlt1(Two)?TrackMVADecision%TOS"}
                 }

    def __apply_configuration__(self):
        from Stages import DetachedLLFilter, BCombiner, JPsiMuMuPosTagged, JPsiMuMuNegTagged, \
            JPsiEEPosTagged, JPsiEENegTagged, JPsiEEL0PosTagged, JPsiEEL0NegTagged, \
            PhiMuMuPosTagged, PhiMuMuNegTagged, KSFilter, \
            LambdaCombiner, LambdaFilter, Lc2KPPi
        from Inputs import KsLL, KsDD, LambdaLL, LambdaDD, NoPIDsProtons, NoPIDsDownProtons, Muons

        from Hlt2Lines.CharmHad.Lines import CharmHadLines
        from Hlt2Lines.CharmHad.Stages import SharedNoPIDDetachedChild_K, SharedNoPIDDetachedChild_pi, SharedSoftTagChild_pi, TagDecay
        from Hlt2Lines.Utilities.Hlt2Stage import Hlt2ExternalStage
        FilteredKaons = Hlt2ExternalStage(CharmHadLines(), SharedNoPIDDetachedChild_K)
        FilteredPions = Hlt2ExternalStage(CharmHadLines(), SharedNoPIDDetachedChild_pi)
        SharedSoftPions = Hlt2ExternalStage(CharmHadLines(), SharedSoftTagChild_pi)

        from Inputs import NoPIDsDownPions as FilteredDownPions # TODO maybe filter these, disabled for now we we don't even run the PID

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
              'Lb2LcMuNu'             : [ BCombiner('Lb2LcMuNu', [ Lc2KPPi, Muons ], decay = '[Lambda_b0 -> Lambda_c+ mu-]cc') ]
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
              #'Lambda2PPiLL'          : [ LambdaCombiner('Lambda2PPiLL', [ FilteredProtons, FilteredPions ]) ],
              #'Lambda2PPiDD'          : [ LambdaCombiner('Lambda2PPiDD', [ FilteredDownProtons, FilteredDownPions ]) ],
              'Lambda2PPiLLhighP'     : [ LambdaCombiner('Lambda2PPiLLhighP', [ NoPIDsProtons, FilteredPions ], ll = True) ],
              'Lambda2PPiDDhighP'     : [ LambdaCombiner('Lambda2PPiDDhighP', [ NoPIDsDownProtons, FilteredDownPions ], ll = False) ],
              'Lambda2PPiLLisMuon'    : [ LambdaCombiner('Lambda2PPiLLisMuon', [ NoPIDsProtons, FilteredPions ], ismuon = True, ll = True) ],
              'Lambda2PPiDDisMuon'    : [ LambdaCombiner('Lambda2PPiDDisMuon', [ NoPIDsDownProtons, FilteredDownPions ], ismuon = True, ll = False ) ],
              'Lb2LcPi'               : [ BCombiner('Lb2LcPi', [ Lc2KPPi, FilteredPions ], decay = '[Lambda_b0 -> Lambda_c+ pi-]cc') ],
              'Sc02LcPi'              : [ TagDecay('Sc02LcPi', "[Sigma_c0 -> Lambda_c+ pi-]cc", inputs = [ Lc2KPPi, SharedSoftPions ],
                                            DaughtersCuts = { "Lambda_c+" : "(BPVDIRA > %(BPVDIRA_MIN)s )" }, nickname = 'Sc2LcPi') ],
              'Scpp2LcPi'             : [ TagDecay('Scpp2LcPi', "[Sigma_c++ -> Lambda_c+ pi+]cc", inputs = [ Lc2KPPi, SharedSoftPions ],
                                            DaughtersCuts = { "Lambda_c+" : "(BPVDIRA > %(BPVDIRA_MIN)s )" }, nickname = 'Sc2LcPi') ]
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
            'Scpp2LcPi'             : 50047
            }.iteritems()})
