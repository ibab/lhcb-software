# MODULES
from GaudiKernel.SystemOfUnits import *
from PhysConf.Filters import LoKi_Filters
from PhysSelPython.Wrappers import AutomaticData, Selection, SelectionSequence
from Configurables import FilterDesktop, CombineParticles

# DATA
MuonLoc = 'Phys/StdAllLooseMuons/Particles'
MuonSel = AutomaticData(Location = MuonLoc)
JpsiLoc = 'Phys/StdLooseJpsi2MuMu/Particles'
JpsiSel = AutomaticData(Location = JpsiLoc)
    
def exampleJpsiSelection(name):

    # FILTER
    JpsiFilt = LoKi_Filters(
        L0DU_Code = "L0_CHANNEL_RE('.*Muon')",
        HLT_CODE = "HLT_PASS_RE( 'Hlt1DiMuonHighMassDecision' ) & HLT_PASS_RE( 'Hlt2DiMuonJPsi.*Decision' )",
        STRIP_Code = "HLT_PASS_RE( 'StrippingFullDSTDiMuonJpsi2MuMuTOSLineDecision' )"
        )

    # RECONSTRUCTION (NEEDHAM)
    _AlgJpsi2MuMu = FilterDesktop("AlgJpsi2MuMu")
    _AlgJpsi2MuMu.Code = "(ADMASS('J/psi(1S)') < 100*MeV) & (VFASPF(VCHI2/VDOF) < 20)"
    _AlgJpsi2MuMu.Code += " & (MINTREE('mu+'==ABSID, PT) > 0.7*GeV) & (MINTREE('mu+'==ABSID, P) > 8*GeV)"
    _AlgJpsi2MuMu.Code += " & (MAXTREE('mu+'==ABSID, TRCHI2DOF) < 4)"
    
    JpsiSel = Selection("JpsiSel",
                        Algorithm = _AlgJpsi2MuMu,
                        RequiredSelections = [JpsiSel])
    JpsiSeq = SelectionSequence(name,TopSelection = JpsiSel)
    
    return JpsiSeq

def examplePsi2SSelection(name):

    # FILTER
    Psi2SFilt = LoKi_Filters(
        L0DU_Code = "L0_CHANNEL_RE('.*Muon')",
        HLT_CODE = "HLT_PASS_RE( 'Hlt1DiMuonHighMassDecision' ) & HLT_PASS_RE( 'Hlt2DiMuonPsi2S.*Decision' )",
        STRIP_Code = "HLT_PASS_RE( 'StrippingFullDSTDiMuonPsi2MuMuTOSLineDecision' )"
        )
    
    # RECONSTRUCTION (NEEDHAM)
    _AlgPsi2S2MuMu = CombineParticles("AlgPsi2S2MuMu")
    _AlgPsi2S2MuMu.DecayDescriptor = "psi(2S) -> mu+ mu-"
    _AlgPsi2S2MuMu.DaughtersCuts = { "mu+" : "(PT > 1.2*GeV) & (P > 8*GeV) & (TRCHI2DOF < 4) & (PIDmu > 0)",
                                     "mu-" : "(PT > 1.2*GeV) & (P > 8*GeV) & (TRCHI2DOF < 4) & (PIDmu > 0)"}
    _AlgPsi2S2MuMu.CombinationCut = "(ADAMASS('psi(2S)') < 200*MeV)"
    _AlgPsi2S2MuMu.CombinationCut += " & (ADOCACHI2CUT(20,''))"
    _AlgPsi2S2MuMu.MotherCut = "(ADMASS('psi(2S)') < 100*MeV)"
    _AlgPsi2S2MuMu.MotherCut += " & (VFASPF(VPCHI2) > 0.001)"
    
    Psi2SSel = Selection("Psi2SSel",
                         Algorithm = _AlgPsi2S2MuMu,
                         RequiredSelections = [MuonSel])
    Psi2SSeq = SelectionSequence(name,TopSelection = Psi2SSel)

    return Psi2SSeq

def exampleUps1SSelection(name):

    # FILTER 
    Ups1SFilt = LoKi_Filters(
        L0DU_Code = "L0_CHANNEL_RE('.*Muon')",
        HLT_CODE = "HLT_PASS_RE( 'Hlt1DiMuonHighMassDecision' ) & HLT_PASS_RE( 'Hlt2DiMuonBDecision' )",
        STRIP_Code = "HLT_PASS_RE( 'StrippingFullDSTDiMuonDiMuonHighMassLineDecision' )"
        )

    # RECONSTRUCTION (NEEDHAM)
    _AlgUps1S2MuMu = CombineParticles("AlgUps1S2MuMu")
    _AlgUps1S2MuMu.DecayDescriptor = "Upsilon(1S) -> mu+ mu-"
    _AlgUps1S2MuMu.DaughtersCuts = { "mu+" : "(PT > 1.5*GeV) & (P > 8*GeV) & (TRCHI2DOF < 4) & (PIDmu > 0)",
                                     "mu-" : "(PT > 1.5*GeV) & (P > 8*GeV) & (TRCHI2DOF < 4) & (PIDmu > 0)"}
    _AlgUps1S2MuMu.CombinationCut = "(ADAMASS('Upsilon(1S)') < 500*MeV)"
    _AlgUps1S2MuMu.CombinationCut += " & (ADOCACHI2CUT(20,''))"
    _AlgUps1S2MuMu.MotherCut = "(ADMASS('Upsilon(1S)') < 400*MeV)"
    _AlgUps1S2MuMu.MotherCut += " & (VFASPF(VPCHI2) > 0.001)"
    
    Ups1SSel = Selection("Ups1SSel",
                         Algorithm = _AlgUps1S2MuMu,
                         RequiredSelections = [MuonSel])
    Ups1SSeq = SelectionSequence(name,TopSelection = Ups1SSel)

    return Ups1SSeq

def exampleUpsAllSSelection(name):

    # FILTER 
    UpsAllSFilt = LoKi_Filters(
        L0DU_Code = "L0_CHANNEL_RE('.*Muon')",
        HLT_CODE = "HLT_PASS_RE( 'Hlt1DiMuonHighMassDecision' ) & HLT_PASS_RE( 'Hlt2DiMuonBDecision' )",
        STRIP_Code = "HLT_PASS_RE( 'StrippingFullDSTDiMuonDiMuonHighMassLineDecision' )"
        )

    # RECONSTRUCTION (NEEDHAM)
    _AlgUpsAllS2MuMu = CombineParticles("AlgUpsAllS2MuMu")
    _AlgUpsAllS2MuMu.DecayDescriptor = "Upsilon(1S) -> mu+ mu-" # but 2S, 3S as well
    _AlgUpsAllS2MuMu.DaughtersCuts = { "mu+" : "(PT > 1.5*GeV) & (P > 8*GeV) & (TRCHI2DOF < 4) & (PIDmu > 0)",
                                       "mu-" : "(PT > 1.5*GeV) & (P > 8*GeV) & (TRCHI2DOF < 4) & (PIDmu > 0)"}
    _AlgUpsAllS2MuMu.CombinationCut = "(AM > 8*GeV) & (AM < 12*GeV)"
    _AlgUpsAllS2MuMu.CombinationCut += " & (ADOCACHI2CUT(20,''))"
    _AlgUpsAllS2MuMu.MotherCut = "(M > 8.5*GeV) & (M < 11.5*GeV)"
    _AlgUpsAllS2MuMu.MotherCut += " & (VFASPF(VPCHI2) > 0.001)"
    
    UpsAllSSel = Selection("UpsAllSSel",
                           Algorithm = _AlgUpsAllS2MuMu,
                           RequiredSelections = [MuonSel])
    UpsAllSSeq = SelectionSequence(name,TopSelection = UpsAllSSel)

    return UpsAllSSeq

def exampleZ0Selection(name):

    # FILTER
    Z0Filt = LoKi_Filters(
        L0DU_Code = "L0_CHANNEL_RE('.*Muon')",
        HLT_CODE = "HLT_PASS_RE( 'Hlt1DiMuonHighMassDecision' ) & HLT_PASS_RE( 'Hlt2DiMuonBDecision|Hlt2DiMuonZDecision' )",
        STRIP_Code = "HLT_PASS_RE( 'StrippingZ02MuMuLineDecision' )"
        )

    # RECONSTRUCTION (NEEDHAM)
    _AlgZ02MuMu = CombineParticles("AlgZ02MuMu")
    _AlgZ02MuMu.DecayDescriptor = "Z0 -> mu+ mu-"
    _AlgZ02MuMu.DaughtersCuts = { "mu+" : "(PT > 3*GeV) & (P > 8*GeV) & (TRCHI2DOF < 4) & (PIDmu > 0)",
                                  "mu-" : "(PT > 3*GeV) & (P > 8*GeV) & (TRCHI2DOF < 4) & (PIDmu > 0)"}
    _AlgZ02MuMu.CombinationCut = "(ADOCACHI2CUT(20,''))"
    _AlgZ02MuMu.MotherCut = "(MM > 40*GeV)"
    _AlgZ02MuMu.MotherCut += " & (VFASPF(VPCHI2) > 0.001)"
    
    Z0Sel = Selection("Z0Sel",
                      Algorithm = _AlgZ02MuMu,
                      RequiredSelections = [MuonSel])
    Z0Seq = SelectionSequence(name,TopSelection = Z0Sel)

    return Z0Seq


