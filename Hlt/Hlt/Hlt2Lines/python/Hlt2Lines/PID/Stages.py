# Based on Phys/StrippingSelections/python/StrippingSelections/StrippingCalib/StrippingPIDCalib.py
from Inputs import Muons, NoPIDsMuons, Electrons, NoPIDsElectrons, L0Electrons, NoDLLCutL0Electrons, NoPIDsKaons, NoPIDsPions, KsLL, KsDD, NoPIDsDownPions
from Inputs import NoPIDsProtons, NoPIDsDownProtons

# For tag and probe we need to make sure that we use exactly one tag and one probe track...
from Hlt2Lines.Utilities.Hlt2Filter import Hlt2ParticleFilter
# Tag filter
class TagFilter(Hlt2ParticleFilter):
    def __init__(self, name, input, mode, nickname, muons):
        if mode == 1:
            charge_cut = "(Q<0)"
        else:
            charge_cut = "(Q>0)"
        muon_cut = "(ISMUON)" if muons else "(PIDe > %(TagPIDe)s)"
        name += 'Tag'

        cut = (charge_cut + " & " + muon_cut + 
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
          muons = True
          l = "mu"
        elif nickname == "JPsiEE":
          decay = "J/psi(1S) -> e+ e-"
          muons = False
          l = "e"
        else:
          decay = "phi(1020) -> mu+ mu-"
          muons = True
          l = "mu"
        name = "PID" + name + "Tagged"
       
        dc = {
            l + "+" : "ALL",
            l + "-" : "ALL"
            }
        cc = ("(in_range(%(LLCombMLow)s, AM, %(LLCombMHigh)s)) & (ACHI2DOCA(1,2) < %(LLCombMaxDocaChi2)s)")
        mc = ("(ALL)")
        from HltTracking.HltPVs import PV3D
        Hlt2Combiner.__init__(self, name,
            decay,
            [ TagFilter(name, taginput, mode, nickname, muons), ProbeFilter(name, probeinput, mode, nickname) ],
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
    def __init__(self, name, inputs, nickname = None, decay = "[B+ -> J/psi(1S) K+]cc"):
        cc = ("(in_range(%(LLhCombMLow)s, AM, %(LLhCombMHigh)s))")
        mc = ("(VFASPF(VCHI2)<%(LLhVChi2)s) & (BPVVDCHI2 > %(LLhVDChi2)s) & (BPVIPCHI2()<%(LLhMaxIPChi2)s)")
        if nickname is None:
          nickname = name
        from HltTracking.HltPVs import PV3D
        Hlt2Combiner.__init__(self, name,
            decay,
            inputs,
            shared = True,
            nickname = nickname,
            dependencies = [ PV3D('Hlt2') ],
            #tistos = 'LLhTisTos',
            DaughtersCuts = { },
            CombinationCut = cc,
            MotherCut = mc,
            Preambulo = [ ])

# The filter for the input particles
class BachelorFilter(Hlt2ParticleFilter):
    def __init__(self, name, input):
        cut = ("(TRCHI2DOF < %(BachTrChi2DoF)s)" +
               "& (PT > %(BachPt)s)" +
               "& (P > %(BachP)s)" +
               "& (MIPCHI2DV(PRIMARY) > %(BachIPChi2)s)")

        from HltTracking.HltPVs import PV3D
        Hlt2ParticleFilter.__init__(self, name, cut, [input], nickname = name,
                                    dependencies = [PV3D('Hlt2')],
                                    UseP2PVRelations = False, shared = True)

# Extra filtering for the J/psi which are not combined with kaons to make B
class DetachedLLFilter(Hlt2ParticleFilter):
    def __init__(self, name, input, nickname = None):
        if nickname is None:
          nickname = name
        cut = ("(VFASPF(VCHI2)<%(DetLLVChi2)s)" +
               " & (PT > %(DetLLPt)s)" +
               " & (BPVVDCHI2 > %(DetLLVDChi2)s)")
        from HltTracking.HltPVs import PV3D
        Hlt2ParticleFilter.__init__(self, name, cut, [ input ],
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
    def __init__(self, name, input):
        cut = ("( ADWM( 'KS0' , WM( 'pi+' , 'pi-') ) > %(LambdaKsVeto)s ) & " +
               "(ADMASS('Lambda0') < %(LambdaMWindow)s )")
        from HltTracking.HltPVs import PV3D
        Hlt2ParticleFilter.__init__(self, 'PID' + name, cut, [ input ],
                                    dependencies = [ PV3D('Hlt2') ],
                                    shared = True,
                                    nickname = name,
                                    UseP2PVRelations = False)

class LambdaCombiner(Hlt2Combiner):
    def __init__(self, name, inputs, ismuon = False, ll = True):
        ismuonstr = "(ISMUON) & " if ismuon else ""
        dispcutstr = "(BPVLTIME() > %(LambdaLLTau)s) & " if ll else "(BPVVDZ > %(LambdaDDBPVVDZ)s) & "
        dc = {
            "p+"  : ismuonstr + "(P > %(LambdaProtonP)s ) & ( TRCHI2DOF < %(LambdaProtonTrChi2)s ) & ( MIPCHI2DV(PRIMARY) > %(LambdaProtonMinIPChi2)s )",
            "pi-" : "(P > %(LambdaPionP)s ) & ( TRCHI2DOF < %(LambdaPionTrChi2)s ) & ( MIPCHI2DV(PRIMARY) > %(LambdaPionMinIPChi2)s )"
            }
        cc = ("(ADAMASS('Lambda0') < %(LambdaAMWindow)s)")
        mc = ("(ADMASS('Lambda0') < %(LambdaMWindow)s ) & " +
              "(VFASPF(VCHI2PDOF) < %(LambdaVChi2)s) & " +
              dispcutstr + 
              "(BPVVDCHI2 > %(LambdaVDChi2)s ) & " + 
              "( ADWM( 'KS0' , WM( 'pi+' , 'pi-') ) > %(LambdaKsVeto)s )")
        from HltTracking.HltPVs import PV3D
        Hlt2Combiner.__init__(self, 'PID' + name,
                              "[Lambda0 -> p+ pi-]cc",
                              inputs,
                              shared = True,
                              nickname = name,
                              dependencies = [ PV3D('Hlt2') ],
                              DaughtersCuts = dc,
                              CombinationCut = cc,
                              MotherCut = mc,
                              Preambulo = [ ])

from HltLine.HltDecodeRaw import DecodeL0CALO
JPsiMuMuPosTagged = LLCombiner("JPsiMuMuPos",  Muons, NoPIDsMuons, 0, "JPsiMuMu")
JPsiMuMuNegTagged = LLCombiner("JPsiMuMuNeg",  Muons, NoPIDsMuons, 1, "JPsiMuMu")
JPsiEEPosTagged   = LLCombiner("JPsiEEPos",    Electrons, NoPIDsElectrons, 0, "JPsiEE")
JPsiEENegTagged   = LLCombiner("JPsiEENeg",    Electrons, NoPIDsElectrons, 1, "JPsiEE")
JPsiEEL0PosTagged = LLCombiner("JPsiEEL0Pos",  L0Electrons, NoDLLCutL0Electrons, 0, "JPsiEE", [DecodeL0CALO])
JPsiEEL0NegTagged = LLCombiner("JPsiEEL0Neg",  L0Electrons, NoDLLCutL0Electrons, 1, "JPsiEE", [DecodeL0CALO])
PhiMuMuPosTagged  = LLCombiner("PhiMuMuPos",   Muons, NoPIDsMuons, 0, "PhiMuMu")
PhiMuMuNegTagged  = LLCombiner("PhiMuMuNeg",   Muons, NoPIDsMuons, 1, "PhiMuMu")
#from Hlt2Lines.CharmHad.Stages import SharedNoPIDDetachedChild_K as FilteredKaons
#FilteredKaons     = BachelorFilter("SharedKaonsForPIDB2JPsiK", NoPIDsKaons)  # TODO perhaps use the generic shared NoPIDs particles instead?
#FilteredPions     = BachelorFilter("SharedPionsForPIDDs2PhiPi", NoPIDsPions) # TODO use shared charm daughters?
#FilteredDownPions = BachelorFilter("SharedDownPionsForPID", NoPIDsDownPions)
FilteredProtons = NoPIDsProtons
FilteredDownProtons = NoPIDsDownProtons
