# MODULES
from GaudiKernel.SystemOfUnits import *
from PhysConf.Filters import LoKi_Filters
from PhysSelPython.Wrappers import AutomaticData, Selection, SelectionSequence
from Configurables import FilterDesktop, CombineParticles
from Configurables import GaudiSequencer

# DATA
MuonLoc = 'Phys/StdAllLooseMuons/Particles'
MuonSel = AutomaticData(Location = MuonLoc)
LooseJPsiLoc = 'Phys/StdLooseJpsi2MuMu/Particles'
LooseJpsiSel = AutomaticData(Location = LooseJPsiLoc)
LooseD0Loc = 'Phys/StdLooseD02KPi/Particles'
LooseD0Sel = AutomaticData(Location = LooseD0Loc)
    
def exampleJpsiSelection(name):

    # FILTER
    JpsiFilt = LoKi_Filters(
        L0DU_Code = "L0_CHANNEL_RE('.*Muon')",
        HLT_CODE = "HLT_PASS_RE( 'Hlt1DiMuonHighMassDecision' ) & HLT_PASS_RE( 'Hlt2DiMuonJPsi.*Decision' )",
        STRIP_Code = "HLT_PASS_RE( 'StrippingFullDSTDiMuonJpsi2MuMuTOSLineDecision' )"
        )
    JpsiFilter = JpsiFilt.sequence("JpsiFilter")

    # RECONSTRUCTION (NEEDHAM)
    _AlgJpsi2MuMu = FilterDesktop("AlgJpsi2MuMu")
    _AlgJpsi2MuMu.Code = "(ADMASS('J/psi(1S)') < 100*MeV) & (VFASPF(VCHI2/VDOF) < 20)"
    _AlgJpsi2MuMu.Code += " & (MINTREE('mu+'==ABSID, PT) > 0.7*GeV) & (MINTREE('mu+'==ABSID, P) > 8*GeV)"
    _AlgJpsi2MuMu.Code += " & (MAXTREE('mu+'==ABSID, TRCHI2DOF) < 4)"
    
    JpsiSel = Selection("JpsiSel",
                        Algorithm = _AlgJpsi2MuMu,
                        RequiredSelections = [LooseJpsiSel])
    JpsiSelSeq = SelectionSequence("JpsiSelSeq",TopSelection = JpsiSel)
    
    # GAUDI SEQUENCE
    JpsiSeq = GaudiSequencer(name)
    JpsiSeq.Members = [ JpsiFilter, JpsiSelSeq.sequence() ]
    return JpsiSeq, JpsiSelSeq.outputLocation()

def examplePsi2SSelection(name):

    # FILTER
    Psi2SFilt = LoKi_Filters(
        L0DU_Code = "L0_CHANNEL_RE('.*Muon')",
        HLT_CODE = "HLT_PASS_RE( 'Hlt1DiMuonHighMassDecision' ) & HLT_PASS_RE( 'Hlt2DiMuonPsi2S.*Decision' )",
        STRIP_Code = "HLT_PASS_RE( 'StrippingFullDSTDiMuonPsi2MuMuTOSLineDecision' )"
        )
    Psi2SFilter = Psi2SFilt.sequence("Psi2SFilter")
    
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
    Psi2SSelSeq = SelectionSequence("Psi2SSelSeq",TopSelection = Psi2SSel)

    # GAUDI SEQUENCE
    Psi2SSeq = GaudiSequencer(name)
    Psi2SSeq.Members = [ Psi2SFilter, Psi2SSelSeq.sequence() ]
    return Psi2SSeq, Psi2SSelSeq.outputLocation()

def exampleUps1SSelection(name):

    # FILTER 
    Ups1SFilt = LoKi_Filters(
        L0DU_Code = "L0_CHANNEL_RE('.*Muon')",
        HLT_CODE = "HLT_PASS_RE( 'Hlt1DiMuonHighMassDecision' ) & HLT_PASS_RE( 'Hlt2DiMuonBDecision' )",
        STRIP_Code = "HLT_PASS_RE( 'StrippingFullDSTDiMuonDiMuonHighMassLineDecision' )"
        )
    Ups1SFilter = Ups1SFilt.sequence("Ups1SFilter")

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
    Ups1SSelSeq = SelectionSequence("Ups1SSelSeq",TopSelection = Ups1SSel)

    # GAUDI SEQUENCE
    Ups1SSeq = GaudiSequencer(name)
    Ups1SSeq.Members = [ Ups1SFilter, Ups1SSelSeq.sequence() ]
    return Ups1SSeq, Ups1SSelSeq.outputLocation()

def exampleUpsAllSSelection(name):

    # FILTER 
    UpsAllSFilt = LoKi_Filters(
        L0DU_Code = "L0_CHANNEL_RE('.*Muon')",
        HLT_CODE = "HLT_PASS_RE( 'Hlt1DiMuonHighMassDecision' ) & HLT_PASS_RE( 'Hlt2DiMuonBDecision' )",
        STRIP_Code = "HLT_PASS_RE( 'StrippingFullDSTDiMuonDiMuonHighMassLineDecision' )"
        )
    UpsAllSFilter = UpsAllSFilt.sequence("UpsAllSFilter")

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
    UpsAllSSelSeq = SelectionSequence("UpsAllSSelSeq",TopSelection = UpsAllSSel)

    # GAUDI SEQUENCE
    UpsAllSSeq = GaudiSequencer(name)
    UpsAllSSeq.Members = [ UpsAllSFilter, UpsAllSSelSeq.sequence() ]
    return UpsAllSSeq, UpsAllSSelSeq.outputLocation()

def exampleZ0Selection(name):

    # FILTER
    Z0Filt = LoKi_Filters(
        L0DU_Code = "L0_CHANNEL_RE('.*Muon')",
        HLT_CODE = "HLT_PASS_RE( 'Hlt1DiMuonHighMassDecision' ) & HLT_PASS_RE( 'Hlt2DiMuonBDecision|Hlt2DiMuonZDecision' )",
        STRIP_Code = "HLT_PASS_RE( 'StrippingZ02MuMuLineDecision' )"
        )
    Z0Filter = Z0Filt.sequence("Z0Filter")

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
    Z0SelSeq = SelectionSequence("Z0SelSeq",TopSelection = Z0Sel)

    # GAUDI SEQUENCE
    Z0Seq = GaudiSequencer(name)
    Z0Seq.Members = [ Z0Filter, Z0SelSeq.sequence() ]
    return Z0Seq, Z0SelSeq.outputLocation()

def exampleD0FromDstarSelection(name):

    # FILTER
    from PhysConf.Filters import LoKi_Filters
    D0Filt = LoKi_Filters(
        HLT_Code = "HLT_PASS( 'Hlt2CharmHadD02HH_D02KPiDecision' )"
        )
    D0Filter = D0Filt.sequence("D0Filter")
    
    # RECONSTRUCTION
    from CommonParticles.StdLooseDstarWithD2HH import StdLooseDstarWithD02KPi
    StdLooseDstarWithD02KPi.MotherCut = "(VFASPF(VCHI2/VDOF)<25) & (abs(M-MAXTREE('D0'==ABSID,M)-145.421)<4)"
    _AlgD02KPi = FilterDesktop("AlgD02KPi")
    _AlgD02KPi.Code = "(ADMASS('D0') < 50.*MeV) & (VFASPF(VCHI2) < 10.)"
    _AlgD02KPi.Code += " & (PT > 3.5*GeV)" # put in a good trigger line
    _AlgD02KPi.Code += " & (MINTREE('K-'==ABSID, PIDK) > 0) & (MAXTREE('pi+'==ABSID, PIDK) < 0)"
    D0Sel = Selection("D0Sel",
                      Algorithm = _AlgD02KPi,
                      RequiredSelections = [ LooseD0Sel ])
    D0SelSeq = SelectionSequence("D0SelSeq", TopSelection = D0Sel)
    
    # THERE CAN ONLY BE ONE (D0, so I know it's the one from the D*)
    code = "CONTAINS('" + D0Seq.outputLocation() + "')==1"
    onefilt = LoKi_Filters( VOID_Code = code )
    HIGHLANDER = onefilt.sequence('HIGHLANDER')

    # GAUDI SEQUENCE
    D0Seq = GaudiSequencer(name)
    D0Seq.Members = [ D0Filter, StdLooseDstarWithD02KPi, D0SelSeq.sequence(), HIGHLANDER ]
    return D0Seq, D0SelSeq.outputLocation()
