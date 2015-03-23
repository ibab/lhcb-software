__author__  = 'Olli Lupton'
__date__    = '$Date: 2015-03-12$'
__version__ = '$Revision: 0.0 $'

from GaudiKernel.SystemOfUnits import GeV, MeV, mm
from GaudiKernel.SystemOfUnits import picosecond as ps
from Hlt2Lines.Utilities.Hlt2LinesConfigurableUser import Hlt2LinesConfigurableUser

class PIDLines(Hlt2LinesConfigurableUser):
    l0_muons = ['Muon','DiMuon']
    l0_electrons = ['Photon', 'Electron', 'Hadron', 'Muon', 'DiMuon']
    hlt1_muons = 'Hlt1(TrackAllL0|TrackMVA|TrackMuon|SingleMuon).*Decision'
    hlt1_electrons = 'Hlt1(TrackAllL0|TrackMVA|TrackMuon|SingleMuon|TrackPhoton|SingleElectron).*Decision'
    __slots__ = {'Prescale' :  {
                                'Hlt2PIDLambda2PPiLL' : 1.0,
                                'Hlt2PIDLambda2PPiDD' : 0.0,
                                'Hlt2PIDLambda2PPiDDhighP' : 0.0,
                                'Hlt2PIDLambda2PPiDDisMuon' : 0.0,
                                'Hlt2PIDKs2PiPiLL'    : 1.0,
                                'Hlt2PIDKs2PiPiDD'    : 0.0
                                },
                  'Common'   : {'L0ForMuons'    : [ ],
                                'L0ForElectrons': [ ],
                                'TagTrChi2'     : 3,
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
                                'LambdaAMWindow': 40 * MeV,
                                'LambdaMWindow' : 25 * MeV,
                                'LambdaVChi2'   : 16,
                                'LambdaVDChi2'  : 50,
                                'LambdaKsVeto'  : 20 * MeV,
                                'LambdaLLTau'   : 2.0 * ps,
                                'LambdaDDBPVVDZ': 2200 * mm
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
                                  "L0(" + "|".join(l0_muons) + ")Decision%TIS",
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
                                  "L0(" + "|".join(l0_electrons) + ")Decision%TIS",
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
                                  "L0(" + "|".join(l0_muons) + ")Decision%TIS",
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
                     }
                }
   
    def __apply_configuration__(self):
        from Stages import DetachedLLFilter, BCombiner, JPsiMuMuPosTagged, JPsiMuMuNegTagged, \
            JPsiEEPosTagged, JPsiEENegTagged, JPsiEEL0PosTagged, JPsiEEL0NegTagged, \
            PhiMuMuPosTagged, PhiMuMuNegTagged, KSFilter, \
            LambdaCombiner, LambdaFilter
        from Inputs import KsLL, KsDD, LambdaLL, LambdaDD, NoPIDsProtons, NoPIDsDownProtons

        from Hlt2Lines.CharmHad.Lines import CharmHadLines
        from Hlt2Lines.CharmHad.Stages import SharedNoPIDDetachedChild_K, SharedNoPIDDetachedChild_pi
        from Hlt2Lines.Utilities.Hlt2Stage import Hlt2ExternalStage
        FilteredKaons = Hlt2ExternalStage(CharmHadLines(), SharedNoPIDDetachedChild_K)
        FilteredPions = Hlt2ExternalStage(CharmHadLines(), SharedNoPIDDetachedChild_pi)

        from Inputs import NoPIDsDownPions as FilteredDownPions # TODO maybe filter these, disabled for now we we don't even run the PID

        ds2phipi = "[D_s+ -> phi(1020) pi+]cc"
        stagemap = {
            'Muon' : {
              'DetJPsiMuMuPosTagged'  : [ DetachedLLFilter("JPsiMuMu", JPsiMuMuPosTagged) ],
              'DetJPsiMuMuNegTagged'  : [ DetachedLLFilter("JPsiMuMu", JPsiMuMuNegTagged) ],
              'DetPhiMuMuPosTagged'   : [ DetachedLLFilter("PhiMuMu", PhiMuMuPosTagged) ],
              'DetPhiMuMuNegTagged'   : [ DetachedLLFilter("PhiMuMu", PhiMuMuNegTagged) ],
              'B2KJPsiMuMuSSTagged'   : [ BCombiner("JPsiMuMu", [ JPsiMuMuPosTagged, FilteredKaons ] ) ],
              'B2KJPsiMuMuOSTagged'   : [ BCombiner("JPsiMuMu", [ JPsiMuMuNegTagged, FilteredKaons ] ) ],
              'Ds2PiPhiMuMuSSTagged'  : [ BCombiner('PhiMuMu', [ PhiMuMuPosTagged, FilteredPions ], ds2phipi ) ],
              'Ds2PiPhiMuMuOSTagged'  : [ BCombiner('PhiMuMu', [ PhiMuMuNegTagged, FilteredPions ], ds2phipi ) ]
              },
            'Electron' : {
              'DetJPsiEEPosTagged'    : [ DetachedLLFilter("JPsiEE", JPsiEEPosTagged) ],
              'DetJPsiEENegTagged'    : [ DetachedLLFilter("JPsiEE", JPsiEENegTagged) ],
              'B2KJPsiEESSTagged'     : [ BCombiner('JPsiEE', [ JPsiEEPosTagged, FilteredKaons ] ) ],
              'B2KJPsiEEOSTagged'     : [ BCombiner('JPsiEE', [ JPsiEENegTagged, FilteredKaons ] ) ]
              },
            'ElectronFromL0' : {
              'DetJPsiEEL0PosTagged'  : [ DetachedLLFilter("JPsiEE", JPsiEEL0PosTagged) ],
              'DetJPsiEEL0NegTagged'  : [ DetachedLLFilter("JPsiEE", JPsiEEL0NegTagged) ],
              'B2KJPsiEEL0SSTagged'   : [ BCombiner('JPsiEE', [ JPsiEEL0PosTagged, FilteredKaons ] ) ],
              'B2KJPsiEEL0OSTagged'   : [ BCombiner('JPsiEE', [ JPsiEEL0NegTagged, FilteredKaons ] ) ]
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
              'Lambda2PPiDDisMuon'    : [ LambdaCombiner('Lambda2PPiDDisMuon', [ NoPIDsDownProtons, FilteredDownPions ], ismuon = True, ll = False ) ]
              }
            }
        
        from HltLine.HltLine import Hlt2Line
        for category, stages in stagemap.iteritems():
          for (nickname, algos) in self.algorithms(stages).iteritems():
              linename = 'PID' + nickname
              l0cuts = self.getProp("L0Req").get(category, None)
              hlt1cuts = self.getProp("Hlt1Req").get(category, None)
              Hlt2Line(linename,
                       prescale = self.prescale,
                       algos = algos, postscale = self.postscale,
                       L0DU = l0cuts,
                       HLT1 = hlt1cuts)
