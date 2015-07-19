# Based on Phys/StrippingSelections/python/StrippingSelections/StrippingCalib/StrippingPIDCalib.py
from Inputs import Muons, NoPIDsMuons, Electrons, NoPIDsElectrons, L0Electrons, NoDLLCutL0Electrons, NoPIDsKaons, NoPIDsPions, KsLL, KsDD, NoPIDsDownPions
from Inputs import NoPIDsProtons, NoPIDsDownProtons, Protons, Kaons

# For tag and probe we need to make sure that we use exactly one tag and one probe track...
from Hlt2Lines.Utilities.Hlt2Filter import Hlt2ParticleFilter
# Tag filter
class TagFilter(Hlt2ParticleFilter):
    def __init__(self, name, input, mode, nickname, extracut = ""):
        if mode == 1:
            charge_cut = "(Q<0)"
        else:
            charge_cut = "(Q>0)"
        name += 'Tag'

        cut = (charge_cut + extracut + 
               " & (TRCHI2DOF <%(TagTrChi2)s )" +
               " & (ISLONG)" +
               " & (P >%(TagP)s )" +
               " & (PT >%(TagPt)s )" +
               " & (MIPCHI2DV(PRIMARY)>%(TagMinIPChi2)s )")
        Hlt2ParticleFilter.__init__(self, name, cut, [input], nickname = nickname, shared = True)

# Probe filter
class ProbeFilter(Hlt2ParticleFilter):
    def __init__(self, name, input, mode, nickname):
        if mode == 1:
           charge_cut = "(Q>0)"
        else:
            charge_cut = "(Q<0)"
        name += 'Probe'
        cut = (charge_cut +
               " & (TRCHI2DOF <%(ProbeTrChi2)s )" +
               " & (ISLONG)" + 
               " & (P >%(ProbeP)s )" +
               " & (PT >%(ProbePt)s )" +
               " & (MIPCHI2DV(PRIMARY)>%(ProbeMinIPChi2)s )")
        Hlt2ParticleFilter.__init__(self, name, cut, [input], tistos = 'ProbeTisTos', nickname = nickname, shared = True)

# The class that creates the Hlt2Combiner
from Hlt2Lines.Utilities.Hlt2Combiner import Hlt2Combiner
class LLCombiner(Hlt2Combiner):
    def __init__(self, name, taginput, probeinput, mode, nickname, dependencies = [ ]):
        if nickname == "JPsiMuMu":
          decay = "J/psi(1S) -> mu+ mu-"
          extra_cut = " & (ISMUON)"
          lp = "mu+"
          lm = "mu-"
        elif nickname == "JPsiEE":
          decay = "J/psi(1S) -> e+ e-"
          extra_cut = " & (PIDe > %(TagPIDe)s)"
          lp = "e+"
          lm = "e-"
        elif nickname == "JPsiPP":
          decay = "J/psi(1S) -> p+ p~-"
          extra_cut = " & (PIDp > %(TagPIDp)s)"
          lp = "p+"
          lm = "p~-"
        elif nickname == "PhiMuMu":
          decay = "phi(1020) -> mu+ mu-"
          extra_cut = " & (ISMUON)"
          lp = "mu+"
          lm = "mu-"
        elif nickname == "PhiKK":
          decay = "phi(1020) -> K+ K-"
          extra_cut = " & (PIDK > %(TagPIDK)s)"
          lp = "K+"
          lm = "K-"
        else:
          print "Don't know how to handle: " + nickname

        name = "PID" + name + "Tagged"
       
        dc = {
            lp : "ALL",
            lm : "ALL"
            }
        cc = ("(in_range(%(LLCombAMLow)s, AM, %(LLCombAMHigh)s)) & (ACHI2DOCA(1,2) < %(LLCombMaxDocaChi2)s)")
        mc = ("(in_range(%(LLCombMLow)s, M, %(LLCombMHigh)s))")
        from HltTracking.HltPVs import PV3D
        Hlt2Combiner.__init__(self, name,
            decay,
            [ TagFilter(name, taginput, mode, nickname, extra_cut), ProbeFilter(name, probeinput, mode, nickname) ],
            dependencies = [ PV3D('Hlt2') ] + dependencies,
            #tistos = 'LLTisTos',
            DaughtersCuts = dc,
            CombinationCut = cc,
            MotherCut = mc,
            Preambulo = [ ],
            shared = True,
            nickname = nickname)

# Class to add a kaon to a J/psi
class BCombiner(Hlt2Combiner):
    def __init__(self, name, inputs, nickname = None, decay = "[B+ -> J/psi(1S) K+]cc", corrm = False):
        cc = ("(in_range(%(LLhCombAMLow)s, AM, %(LLhCombAMHigh)s)) & "
            + "(APT1+APT2 > %(LLhSumPTMin)s)")
        if corrm:
          mc_mass = " & (in_range(%(LLhCombBPVCORRMLow)s, BPVCORRM, %(LLhCombBPVCORRMHigh)s))"
        else:
          mc_mass = " & (in_range(%(LLhCombMLow)s, M, %(LLhCombMHigh)s))"
        mc = ("(VFASPF(VCHI2)<%(LLhVChi2)s) & (BPVVDCHI2 > %(LLhVDChi2)s)"
              + " & (BPVIPCHI2()<%(LLhMaxIPChi2)s)"
              + " & (BPVDIRA > %(LLhMinBPVDIRA)s)"
              + mc_mass)

        dc = { }
        for daug in ['p+', 'pi+', 'K+']:
            dc[daug] = ("(PT > %(BachPt)s)" +
                        " & (P > %(BachP)s)" +
                        " & (MIPCHI2DV(PRIMARY) > %(BachIPChi2)s)")
        
        if nickname is None:
          nickname = name
        from HltTracking.HltPVs import PV3D
        Hlt2Combiner.__init__(self, 'PID' + name + 'BCombiner',
            decay,
            inputs,
            shared = True,
            nickname = nickname,
            dependencies = [ PV3D('Hlt2') ],
            tistos = 'CombTisTosSpec',
            DaughtersCuts = dc,
            CombinationCut = cc,
            MotherCut = mc,
            Preambulo = [ ])

# Extra filtering for the J/psi which are not combined with kaons to make B
class DetachedLLFilter(Hlt2ParticleFilter):
    def __init__(self, name, input, nickname = None):
        if nickname is None:
          nickname = name
        cut = ("(VFASPF(VCHI2)<%(DetLLVChi2)s)" +
               " & (PT > %(DetLLPt)s)" +
               " & (BPVVDCHI2 > %(DetLLVDChi2)s)" +
               " & (MIPCHI2DV(PRIMARY) > %(DetLLMinIPChi2)s)" +
               " & (BPVDIRA > %(DetLLMinBPVDIRA)s)")
        from HltTracking.HltPVs import PV3D
        Hlt2ParticleFilter.__init__(self, 'PID' + name, cut, [ input ],
                                    #tistos = 'LLhTisTos',
                                    nickname = nickname,
                                    dependencies = [ PV3D('Hlt2') ],
                                    shared = True,
                                    UseP2PVRelations = False)

class KSFilter(Hlt2ParticleFilter):
    def __init__(self, name, input):
        cut = ("(ADMASS ( 'KS0') < %(KsMassWindow)s ) & " + 
               "in_range ( 0 , VFASPF ( VCHI2 ) , %(KsVChi2)s ) & " +
               "( VFASPF ( VZ ) < %(KsDecayMaxZ)s ) & " +
               "(BPVVDCHI2 > %(KsVDChi2)s ) & " +
               "( ADWM( 'Lambda0' , WM( 'p+' , 'pi-') ) > %(KsLambdaVeto)s ) & " +
               "( ADWM( 'Lambda0' , WM( 'pi+' , 'p~-') ) > %(KsLambdaVeto)s )")
        from HltTracking.HltPVs import PV3D
        Hlt2ParticleFilter.__init__(self, 'PID' + name, cut, [ input ],
                                    dependencies = [ PV3D('Hlt2') ],
                                    shared = True,
                                    nickname = name,
                                    UseP2PVRelations = False)

class LambdaFilter(Hlt2ParticleFilter):
    def __init__(self, name, input, ismuon = False):
        ismuonstr = " & (CHILDCUT( ISMUONLOOSE , 1) )" if ismuon else ""
        cut = ("( ADWM( 'KS0' , WM( 'pi+' , 'pi-') ) > %(LambdaKsVeto)s ) & " +
               "(ADMASS('Lambda0') < %(LambdaMWindow)s ) & " +
               "(CHILDCUT( ( P > %(LambdaProtonP)s ) & (PT > %(LambdaProtonPT)s), 1) ) & " +
               "(MIPCHI2DV(PRIMARY) < %(LambdaIPChi2Max)s )" + 
               ismuonstr) # ASSUME proton is 1st child
        Hlt2ParticleFilter.__init__(self, 'PID' + name, cut, [ input ],
                                    shared = True,
                                    nickname = name,
                                    UseP2PVRelations = False)

class MassVetoFilter(Hlt2ParticleFilter):
    def __init__(self, name, inputs, vetoes = { }, windows = { }):
        cuts = ["( ( WM(" + ids + ") > %(MVETO_" + hypo + "_GT)s ) | ( WM(" + ids + ") < %(MVETO_" + hypo + "_LT)s ) )" for hypo, ids in vetoes.iteritems()]
        cuts += ["(in_range(%(M_" + comb + "_LOW)s, MASS(" + var + "), %(M_" + comb + "_HIGH)s))" for comb, var in windows.iteritems()]
        cut = (" & ".join(cuts))
        Hlt2ParticleFilter.__init__(self, 'PID' + name + 'Veto', cut, inputs,
                                    shared = True, nickname = name,
                                    UseP2PVRelations = False)

# Long term this should be moved to the filter in CharmHad...
class PIDGhostInFilter(Hlt2ParticleFilter):
    def __init__(self, name, inputs, pidVar = None, pidULim = False):
        if pidVar:
          pidCut = " & (%s %s %%(PID_LIM)s)" % (pidVar, ('<' if pidULim else '>'))
        else:
          pidCut = ""
        cut = ("(TRGHOSTPROB < %(Trk_GhostProb_MAX)s) & (PT > %(Trk_PT_MIN)s)"
             + " & (MIPCHI2DV(PRIMARY) > %(Trk_MinIPChi2)s)" + pidCut)
        Hlt2ParticleFilter.__init__(self, 'PID' + name + 'PIDGhostFilter', cut, inputs,
                                    shared = True, nickname = name, UseP2PVRelations = False)


class PromptIPChi2Filter(Hlt2ParticleFilter):
    def __init__(self, name, inputs):
        cut = ("(MIPCHI2DV(PRIMARY) < %(D0_BPVIPCHI2_Max)s)")
        Hlt2ParticleFilter.__init__(self, 'PID' + name + 'PromptIPChi2Filter', cut,
                                    inputs, shared = True, nickname = name,
                                    UseP2PVRelations = False)

class PromptFilter(Hlt2ParticleFilter):
    def __init__(self, name, inputs):
        cut = ("(BPVDIRA > %(BPVDIRA_MIN)s) & (BPVLTIME() > %(BPVLTIME_MIN)s )")
        Hlt2ParticleFilter.__init__(self, 'PID' + name + 'PromptFilter', cut, inputs,
                                    shared = True, nickname = name, UseP2PVRelations = False,
                                    tistos = 'PromptTisTosSpec')

class UnbiasedPhi(Hlt2Combiner):
    def __init__(self, name, inputs):
        dc = {
            'K+' : "(MIPCHI2DV(PRIMARY) > %(Trk_ALL_MIPCHI2DV_MIN)s) & "
                 + "(PT > %(Trk_ALL_PT_MIN)s)"
            }
        cc = ("(ADAMASS('phi(1020)') < %(AMWindow)s) & "
            + "(ACHI2DOCA(1,2) < %(MaxDocaChi2)s) & "
            + "((APT1+APT2) > %(ASUMPT_MIN)s) & "
            + "(AHASCHILD(MIPCHI2DV(PRIMARY) > %(Trk_1OF2_MIPCHI2DV_MIN)s)) & "
            + "(AHASCHILD(PT > %(Trk_1OF2_PT_MIN)s))")
        mc = ("(ADMASS('phi(1020)') < %(MWindow)s)")
        
        from HltTracking.HltPVs import PV3D
        Hlt2Combiner.__init__(self, 'PID' + name,
                              "phi(1020) -> K+ K-",
                              inputs,
                              shared = True,
                              nickname = name,
                              dependencies = [ PV3D('Hlt2') ],
                              DaughtersCuts = dc,
                              CombinationCut = cc,
                              MotherCut = mc,
                              Preambulo = [ ])

UnbiasedPhiKK = UnbiasedPhi("PhiKKUnbiased", [ NoPIDsKaons ])

from HltLine.HltDecodeRaw import DecodeL0CALO
JPsiMuMuPosTagged = LLCombiner("JPsiMuMuPos",  Muons, NoPIDsMuons, 0, "JPsiMuMu")
JPsiMuMuNegTagged = LLCombiner("JPsiMuMuNeg",  Muons, NoPIDsMuons, 1, "JPsiMuMu")
JPsiEEPosTagged   = LLCombiner("JPsiEEPos",    Electrons, NoPIDsElectrons, 0, "JPsiEE")
JPsiEENegTagged   = LLCombiner("JPsiEENeg",    Electrons, NoPIDsElectrons, 1, "JPsiEE")
JPsiEEL0PosTagged = LLCombiner("JPsiEEL0Pos",  L0Electrons, NoDLLCutL0Electrons, 0, "JPsiEE", [DecodeL0CALO])
JPsiEEL0NegTagged = LLCombiner("JPsiEEL0Neg",  L0Electrons, NoDLLCutL0Electrons, 1, "JPsiEE", [DecodeL0CALO])
PhiMuMuPosTagged  = LLCombiner("PhiMuMuPos",   Muons, NoPIDsMuons, 0, "PhiMuMu")
PhiMuMuNegTagged  = LLCombiner("PhiMuMuNeg",   Muons, NoPIDsMuons, 1, "PhiMuMu")

#FilteredDownProtons = NoPIDsDownProtons

from Hlt2Lines.CharmHad.Stages import DetachedInParticleFilter
JPsiPPPosTagged   = LLCombiner("JPsiPPPos",    Protons, NoPIDsProtons, 0, "JPsiPP")
JPsiPPNegTagged   = LLCombiner("JPsiPPNeg",    Protons, NoPIDsProtons, 1, "JPsiPP")
PhiKKPosTagged    = LLCombiner("PhiKKPos",     Kaons, NoPIDsKaons, 0, "PhiKK")
PhiKKNegTagged    = LLCombiner("PhiKKNeg",     Kaons, NoPIDsKaons, 1, "PhiKK")

from Hlt2Lines.CharmHad.Lines import CharmHadLines
from Hlt2Lines.CharmHad.Stages import DetachedHHHCombiner, SharedNoPIDDetachedChild_p, SharedDetachedLcChild_K, \
    SharedDetachedLcChild_pi, MassFilter, DV4BCombiner, SharedNoPIDDetachedChild_pi, SharedNoPIDDetachedChild_K, \
    DetachedD02HHCombiner, SharedSoftTagChild_pi
from Hlt2Lines.Utilities.Hlt2Stage import Hlt2ExternalStage

SharedNoPIDPions  = Hlt2ExternalStage(CharmHadLines(), SharedNoPIDDetachedChild_pi)
SharedNoPIDKaons  = Hlt2ExternalStage(CharmHadLines(), SharedNoPIDDetachedChild_K)
SharedSoftPions   = Hlt2ExternalStage(CharmHadLines(), SharedSoftTagChild_pi)

LcTightChild_K  = PIDGhostInFilter("LcTightChild_K",
                    [ Hlt2ExternalStage(CharmHadLines(), SharedDetachedLcChild_K) ],
                    'PIDK', False)
LcTightChild_pi = PIDGhostInFilter("LcTightChild_pi",
                    [ Hlt2ExternalStage(CharmHadLines(), SharedDetachedLcChild_pi) ],
                    'PIDK', True)

LcTightChild_p  = PIDGhostInFilter("LcTightChild_p",
                    [ Hlt2ExternalStage(CharmHadLines(), SharedNoPIDDetachedChild_p) ])

BachelorKaons = DetachedInParticleFilter('PIDSharedBachelor_K', [ SharedNoPIDKaons ], 'PIDK' )
BachelorPions = DetachedInParticleFilter('PIDSharedBachelor_pi', [ SharedNoPIDPions ], 'PIDK', True)
BachelorMuons = DetachedInParticleFilter('PIDSharedBachelor_mu', [ Muons ], 'PIDmu')

# Make the Lc+ -> K- p+ pi+ candidates without proton PID, to be combined with more tracks...
# Share as much of the 'CharmHad' code as possible
Lc2KPPi_Loose = MassFilter('PIDLc2KPPi', inputs = [
  DetachedHHHCombiner('PIDLc2KPPi',
    decay = "[Lambda_c+ -> K- p+ pi+]cc",
    inputs = [
      LcTightChild_p,
      LcTightChild_K,
      LcTightChild_pi
      ],
    nickname = 'Lc2KPPi')
  ], shared = True, nickname = "Lc2KPPi")

# Totally dependent on knowing the order of your decay descriptor...
Lc2KPPi = MassVetoFilter('Lc2KPPi', [Lc2KPPi_Loose], vetoes = {
              "Kpipi" : "'K-', 'pi+', 'pi+'",
              "KKpi"  : "'K-', 'K+',  'pi+'"
            }, windows = {
              "Kpi" : "1, 3"
              })

Lc2KPPi_Prompt = PromptFilter('Lc2KPPiPrompt', [Lc2KPPi])

D02KPi = PromptIPChi2Filter('D02KPi', [MassFilter('PIDD02KPi', inputs = [ 
  DetachedD02HHCombiner('PIDD02KPi',
    decay = "[D0 -> K- pi+]cc",
    inputs = [ SharedNoPIDPions, SharedNoPIDKaons ],
    nickname = 'D02KPi',
    shared = True
    )
  ], shared = True, nickname = 'D02KPi')])

# Make the D0 -> K- pi+ pi- pi+ candidates without any PID
# These will be used both for tagged and untagged lines
D02K3Pi = MassFilter('PIDD02KPiPiPi',
    inputs = [
      DV4BCombiner('PIDD02KPiPiPi',
        inputs = [ SharedNoPIDPions, SharedNoPIDKaons ],
        decay = ['[D0 -> K- pi+ pi+ pi-]cc'],
        nickname = 'D02KPiPiPi',
        shared = True
        )
      ],
    nickname = 'D02KPiPiPi',
    shared = True
    )
