#
# B->DX selection for stripping. Uses selection framework. 
#
# B->DX,
# where X=pi,K,K*,phi,
# and D=D+,D0,Ds.
# The allowed decay modes of the intermediates are
# K* -> Kpi, pipi (to include the pho)
# phi-> KK
# D+ -> hhh (h=pi,K all combinations except KKK),
# Ds -> hhh (h=pi,K all combinations except KKK), 
# D0 -> hh (h=pi,K all combinations).
# D0 -> hhhh (K3pi FAV and DCS, and KKpipi are considered), 
# D0 -> Kpipi0
# D0 -> Kshh (h=pi,K all combinations)
# 
# The default selection is based on a chi2 cuts on the impact paramters
# and flight significances (wrt. the offline selections),
#
# This selection deliberately uses no RICH information, since the channels in
# the B->DX family are kinematically identical to each other and are the dominant
# non-combinatoric backgrounds to each other.  
#
# The selection is tuned on the L0xHLT1 sample of minbias, achieving 
# a reduction factor of 5/1000 for MC09 mbias data. 
# The signal efficiency for the various channels ranges between 80 and 95%.
# 
# Will further update the selection in stages.
#
# Changes in the version using selection framework: 
# 
# In this version of selections, the unnecessary combinatorics is removed. 
# In all cases where Cabibbo-favored and DCS decays lead to the same final 
# state, only CF is taken: e.g. K-Pi+ combination is always called D0, instead
# of producing two candidates, D0 and D0bar. In B combinations, both suppressed
# and favored modes are included (e.g. B+ -> D0bar K+ and B+ -> D0 K+ and c.c.)
#
# Authors: J. Nardulli & V. Gligorov 
# Converted to the particle selection framework by A. Poluektov
#

def B2DXLines(
        moduleName = "B2DXChi2Loose", 
        MinDAMass = 1748.,              # MeV
        MaxDAMass = 2088.,              # MeV
        MinDMass = 1768.,               # MeV
        MaxDMass = 2068.,               # MeV
        MaxDVertChi2DOF = 12.,          # D vertex Chi2
        MinDPVVDChi2 = 4.,              # Chi2 of D vertex separation from the related PV
        MinSignalDMass = 1840.,         # MeV
        MaxSignalDMass = 1895.,         # MeV
        MinSignalDsMass = 1944.,        # MeV
        MaxSignalDsMass = 1994.,        # MeV
	MinDPt = 1000.,                 # MeV
	MaxKstarVertChi2DOF = 12.,      # Used for Kstar, phi and rho: vertex chi2
	MinKstarIPChi2DV = 4.,          # Used for Kstar, phi and rho: min chi2 distance to any PV
	MaxBVertChi2DOF = 12.,          # B vertex Chi2
	MaxBPVIPChi2 = 49.,             # chi2 of B impact parameter to the related PV
	MinBPVVDChi2 = 4.,              # Chi2 of B vertex separation from the related PV
	MinBPVDIRA = 0.9995,            # DIRA of the B to the related PV
	MinBMass = 4800,                # MeV
	MaxBMass = 5900,                # MeV
	MinSignalBMass = 5229,          # MeV
	MaxSignalBMass = 5329,          # MeV
	MinSignalBsMass = 5316,         # MeV
	MaxSignalBsMass = 5416,         # MeV
	MaxDaughterChi2 = 10.,          # Max Chi2 for daughter tracks (D,Kstar,phi,rho decay products)
	MinDaughterPt = 250.,           # MeV
	MinDaughterP  = 2000.,          # MeV
	MinDaughterIPChi2DV = 4.,       # Min Chi2 distance to any PV
	MaxBachelorChi2 = 10.,          # Max Chi2 for bachelor K/pi track
	MinBachelorPt = 350.,           # MeV
	MinBachelorP  = 2000.,          # MeV
	MinBachelorIPChi2DV = 4.,       # Min Chi2 distance to any PV
	KsDAMass = 50.,                 # MeV,  Ks inv. mass range for pion combinations before vertex fitting
	KsDMass = 30.,                  # MeV,  Ks inv. mass range after vertex fitting
	MaxKsVertChi2DOF = 16.,         # Vertex chi2 for Ks
	MinD2hhhhDaughterPt = 400.,     # MeV
	MinD2kpipi0DaughterPt = 400.,   # MeV
	MinD2hhhhP = 3000.,             # MeV
	MinD2kpipi0P = 3000.,           # MeV
	MinD2kpipi0Pt = 2000.,          # MeV
	MinPi0Pt = 800,                 # MeV
	MinPi0P  = 4500,                # MeV
	SidebandPostscale = 1., 
	D2hhPrescale = 1., 
	D2hhhPrescale = 1., 
	D2hhhhPrescale = 1., 
	D2KsPrescale = 1., 
	D2pi0Prescale = 1.
    ) : 

    from Configurables import CombineParticles, FilterDesktop
    from StrippingConf.StrippingLine import StrippingLine
    from PhysSelPython.Wrappers import Selection, SelectionSequence, DataOnDemand, MergedSelection

    #Define the shared cuts
    Dcut = "(in_range(%(MinDMass)s*MeV, M, %(MaxDMass)s*MeV) & (VFASPF(VCHI2/VDOF)<%(MaxDVertChi2DOF)s) & (BPVVDCHI2 > %(MinDPVVDChi2)s)) " % locals()
#    Dcut = "((VFASPF(VCHI2/VDOF)<%(MaxDVertChi2DOF)s) & (BPVVDCHI2 > %(MinDPVVDChi2)s)) " % locals()
    DSignalcut = "(in_range(%(MinSignalDMass)s*MeV, M, %(MaxSignalDMass)s*MeV)) | (in_range(%(MinSignalDsMass)s*MeV, M, %(MaxSignalDsMass)s*MeV))" % locals()

    Dcombcut = "((APT>%(MinDPt)s*MeV) & (in_range(%(MinDAMass)s*MeV, AM, %(MaxDAMass)s*MeV)))" % locals()
#    Dcombcut = "(APT>%(MinDPt)s*MeV)" % locals()
    KstarCombcut = "(APT>%(MinDPt)s*MeV)" % locals()

    KPhiMothercut = "((VFASPF(VCHI2/VDOF) < %(MaxKstarVertChi2DOF)s) & (MIPCHI2DV(PRIMARY) > %(MinKstarIPChi2DV)s))" % locals()

    Bcut = "((VFASPF(VCHI2/VDOF)<%(MaxBVertChi2DOF)s)  & (BPVIPCHI2() < %(MaxBPVIPChi2)s) & (BPVVDCHI2 > %(MinBPVVDChi2)s)  & (BPVDIRA > %(MinBPVDIRA)s))" % locals()
    Bcombcut = "(in_range(%(MinBMass)s*MeV, AM, %(MaxBMass)s*MeV))" % locals()
    BSignalcombcut = "(in_range(%(MinSignalBMass)s*MeV, M, %(MaxSignalBMass)s*MeV)) | (in_range(%(MinSignalBsMass)s*MeV, M, %(MaxSignalBsMass)s*MeV))" % locals()

    Daughtercut = "((TRCHI2DOF<%(MaxDaughterChi2)s) & (PT > %(MinDaughterPt)s*MeV) & (P > %(MinDaughterP)s*MeV) & (MIPCHI2DV(PRIMARY) > %(MinDaughterIPChi2DV)s))" % locals()
    Bachelorcut = "((TRCHI2DOF<%(MaxBachelorChi2)s) & (PT > %(MinBachelorPt)s*MeV) & (P > %(MinBachelorP)s*MeV) & (MIPCHI2DV(PRIMARY) > %(MinBachelorIPChi2DV)s))" % locals()
    PhiDaughterCut = "( 2 == NINTREE( (ABSID=='K+') & (" + Daughtercut + ")))" 


    # -----------------------------------------------------------------------
    # Standard particles 
    # -----------------------------------------------------------------------

    StdPi      = DataOnDemand(Location = "Phys/StdNoPIDsPions")
    StdDownPi  = DataOnDemand(Location = "Phys/StdNoPIDsDownPions")
    StdK       = DataOnDemand(Location = "Phys/StdNoPIDsKaons")
    StdResolvedPi0 = DataOnDemand(Location = "Phys/StdLooseResolvedPi0")
    StdMergedPi0   = DataOnDemand(Location = "Phys/StdLooseMergedPi0")
    StdPi0 = MergedSelection("Pi0For" + moduleName, RequiredSelections = [ StdResolvedPi0, StdMergedPi0 ])

    # -----------------------------------------------------------------------
    # Composite particles 
    # -----------------------------------------------------------------------


    # K_S, both DD and LL
    Ks = CombineParticles("KsFor" + moduleName)
    Ks.DecayDescriptor = "KS0 -> pi+ pi-"
    Ks.DaughtersCuts = { "pi+" : Daughtercut }
    Ks.CombinationCut = "(ADAMASS('KS0')<%(KsDAMass)s*MeV)" % locals()
    Ks.MotherCut = "(ADMASS('KS0')<%(KsDMass)s*MeV) & (VFASPF(VCHI2/VDOF)<%(MaxKsVertChi2DOF)s)" % locals()
    KsDDSel = Selection("SelKsDDFor" + moduleName, Algorithm = Ks, RequiredSelections = [ StdDownPi ] )
    KsLLSel = Selection("SelKsLLFor" + moduleName, Algorithm = Ks, RequiredSelections = [ StdPi ] )
    KsSel = MergedSelection("SelKsFor" + moduleName, RequiredSelections = [ KsDDSel, KsLLSel ] )

    # Kstar
    Kstar = CombineParticles("KstarFor" + moduleName)
    Kstar.DecayDescriptors = [ "[K*(892)0 -> K+ pi-]cc" ]
    Kstar.DaughtersCuts =  { "K+"        : Daughtercut,
                             "pi+"       : Daughtercut }
    Kstar.CombinationCut = KstarCombcut
    Kstar.MotherCut = KPhiMothercut
    KstarSel = Selection("SelKstarFor" + moduleName, Algorithm = Kstar, RequiredSelections = [ StdPi, StdK ] )

    # Rho
    Rho = CombineParticles("RhoFor" + moduleName)
    Rho.DecayDescriptors = [ "rho(770)0 -> pi+ pi-" ]
    Rho.DaughtersCuts =  { "pi+"       : Daughtercut }
    Rho.CombinationCut = KstarCombcut
    Rho.MotherCut = KPhiMothercut
    RhoSel = Selection("SelRhoFor" + moduleName, Algorithm = Rho, RequiredSelections = [ StdPi ] )

    # Phi
    Phi = CombineParticles("PhiFor" + moduleName)
    Phi.DecayDescriptor = "phi(1020) -> K+ K-"
    Phi.DaughtersCuts =  { "K+"        : Daughtercut}
    Phi.CombinationCut = KstarCombcut
    Phi.MotherCut = KPhiMothercut
    PhiSel = Selection("SelPhiFor" + moduleName, Algorithm = Phi, RequiredSelections = [ StdK ] )

    # Bachelor kaon/pion
    Bachelor = FilterDesktop("BachelorFor" + moduleName)
    Bachelor.Code = Bachelorcut
    BachelorPiSel = Selection("SelBachelorPiFor" + moduleName, Algorithm = Bachelor, RequiredSelections = [ StdPi ] )
    BachelorKSel  = Selection("SelBachelorKFor" + moduleName,  Algorithm = Bachelor, RequiredSelections = [ StdK  ] )

    # -----------------------------------------------------------------------
    # D mesons
    # -----------------------------------------------------------------------

    # D0 -> hh
    D2PiPi = CombineParticles("D2PiPiFor" + moduleName)
    D2PiPi.DecayDescriptors = [ "D0 -> pi- pi+" ]
    D2PiPi.DaughtersCuts =  { "K+"        : Daughtercut,"pi+"        : Daughtercut }
    D2PiPi.CombinationCut = Dcombcut
    D2PiPi.MotherCut = Dcut
    D2PiPiSel = Selection("SelD2PiPiFor" + moduleName, Algorithm = D2PiPi, RequiredSelections = [ StdPi ] )

    D2KPi = D2PiPi.clone("D2KPiFor" + moduleName, DecayDescriptors = [ "[D0 -> K- pi+]cc" ] )
    D2KPiSel = Selection("SelD2KPiFor" + moduleName, Algorithm = D2KPi, RequiredSelections = [ StdPi, StdK ] )

    D2KK  = D2PiPi.clone("D2KKFor" + moduleName , DecayDescriptors = [ "D0 -> K- K+" ] ) 
    D2KKSel = Selection("SelD2KKFor" + moduleName, Algorithm = D2KK, RequiredSelections = [ StdK ] )

    # D -> hhh

    D2KPiPi   = D2PiPi.clone("D2KPiPiFor"   + moduleName, DecayDescriptors = [ "[D+ -> K- pi+ pi+]cc" ] ) 
    D2KPiPiSel = Selection("SelD2KPiPiFor" + moduleName, Algorithm = D2KPiPi, RequiredSelections = [ StdPi, StdK ] )

    D2PiPiPi   = D2PiPi.clone("D2PiPiPiFor"   + moduleName, DecayDescriptors = [ "[D+ -> pi- pi+ pi+]cc" ] ) 
    D2PiPiPiSel = Selection("SelD2PiPiPiFor" + moduleName, Algorithm = D2PiPiPi, RequiredSelections = [ StdPi ] )

    D2KKPi     = D2PiPi.clone("D2KKPiFor"     + moduleName, DecayDescriptors = [ "[D+ -> K- K+ pi+]cc" ] ) 
    D2KKPiSel = Selection("SelD2KKPiFor" + moduleName, Algorithm = D2KKPi, RequiredSelections = [ StdPi, StdK ] )

    D2KPiPiDCS    = D2PiPi.clone("D2KPiPiDCSFor" + moduleName, DecayDescriptors = [ "[D+ -> pi- pi+ K+]cc" ] ) 
    D2KPiPiDCSSel = Selection("SelD2KPiPiDCSFor" + moduleName, Algorithm = D2KPiPiDCS, RequiredSelections = [ StdPi, StdK ] )

    # D -> hhhh
    
    D2KPiPiPi = CombineParticles("D2KPiPiPiFor" + moduleName)
    D2KPiPiPi.DecayDescriptors  = ["[D0 -> K- pi+ pi- pi+]cc" ]
    D2KPiPiPi.DaughtersCuts = { "K+"        : Daughtercut+ "& (PT>%(MinD2hhhhDaughterPt)s*MeV)" % locals(),
                                "pi+"       : Daughtercut+ "& (PT>%(MinD2hhhhDaughterPt)s*MeV)" % locals() }
    D2KPiPiPi.CombinationCut =  Dcombcut
    D2KPiPiPi.MotherCut = Dcut+" & (BPVDIRA>0) & (P>%(MinD2hhhhP)s*MeV)" % locals()
    D2KPiPiPiSel = Selection("SelD2KPiPiPiFor" + moduleName, Algorithm = D2KPiPiPi, RequiredSelections = [ StdPi, StdK ] )

    D2KKPiPi = D2KPiPiPi.clone("D2KKPiPiFor" + moduleName, DecayDescriptors  = ["D0 -> K- K+ pi- pi+"] )
    D2KKPiPiSel = Selection("SelD2KKPiPiFor" + moduleName, Algorithm = D2KKPiPi, RequiredSelections = [ StdPi, StdK ] )

    # D -> KPiPi0
    D2KPiPi0 = CombineParticles("D2KPiPi0For" + moduleName)
    D2KPiPi0.DecayDescriptors = [ "[D0 -> K- pi+ pi0]cc" ] 
    D2KPiPi0.DaughtersCuts = { "K-"        : Daughtercut + "& (PT>%(MinD2kpipi0DaughterPt)s*MeV)" % locals(),
                               "pi+"       : Daughtercut + "& (PT>%(MinD2kpipi0DaughterPt)s*MeV)" % locals(),
                               "pi0": "(PT>%(MinPi0Pt)s*MeV) & (P>%(MinPi0P)s*MeV)" % locals() }
    D2KPiPi0.CombinationCut =  ("(APT>%(MinD2kpipi0Pt)s*MeV) & " % locals()) + Dcombcut
    D2KPiPi0.MotherCut = Dcut+" & (BPVDIRA>0) & (P>%(MinD2kpipi0P)s*MeV)" % locals()
    D2KPiPi0Sel = Selection("SelD2KPiPi0For" + moduleName, Algorithm = D2KPiPi0, RequiredSelections = [ StdPi, StdK, StdPi0 ] )

    # D->Kshh
    D2KsPiPi = CombineParticles("D2KsPiPiFor" + moduleName)
    D2KsPiPi.DecayDescriptors = ["D0 -> KS0 pi+ pi-" ]
    D2KsPiPi.DaughtersCuts = { "pi+" : Daughtercut,"K+" : Daughtercut, "KS0" : "ALL" }
    D2KsPiPi.CombinationCut = Dcombcut
    D2KsPiPi.MotherCut = Dcut
    D2KsPiPiSel = Selection("SelD2KsPiPiFor" + moduleName, Algorithm = D2KsPiPi, RequiredSelections = [ KsSel, StdPi ] )

    D2KsKK = D2KsPiPi.clone("D2KsKKFor" + moduleName, DecayDescriptors = [ "D0 -> KS0 K+ K-" ] )
    D2KsKKSel = Selection("SelD2KsKKFor" + moduleName, Algorithm = D2KsKK, RequiredSelections = [ KsSel, StdK ] )

    D2KsKPi = D2KsPiPi.clone("D2KsKPiFor" + moduleName, DecayDescriptors = [ "[D0 -> KS0 K+ pi-]cc" ] )
    D2KsKPiSel = Selection("SelD2KsKPiFor" + moduleName, Algorithm = D2KsKPi, RequiredSelections = [ KsSel, StdK, StdPi ] )

    # Dictionary of D0 selections: "name" : "list of selections"
    # Selections under each "name" will be merged for a single B selection

    D0Selections = {
      "D2hh"   : [ D2KPiSel, D2PiPiSel, D2KKSel ], 
      "D2Ks"   : [ D2KsPiPiSel, D2KsKKSel, D2KsKPiSel ], 
      "D2pi0"  : [ D2KPiPi0Sel ], 
      "D2hhhh" : [ D2KPiPiPiSel, D2KKPiPiSel ]
    }

    # List of B selections

    BSelections = {}

    for name,selections in D0Selections.iteritems() : 

	BSelections[name] = []

        DSel = MergedSelection("Merged" + name + "For" + moduleName, 
                               RequiredSelections = selections )
    
	B2DPi = CombineParticles("B2DPiWith" + name + "For" + moduleName)
	B2DPi.DecayDescriptors = [ "[B+ -> D~0 pi+]cc", "[B+ -> D0 pi+]cc" ]
        B2DPi.DaughtersCuts = { "D0" : Dcut , 
                                "pi+" : Bachelorcut }
        B2DPi.CombinationCut = Bcombcut
        B2DPi.MotherCut = Bcut
        B2DPiSel = Selection("SelB2DPiWith" + name + "For" + moduleName, Algorithm = B2DPi, 
                        RequiredSelections = [ DSel, BachelorPiSel] )
        BSelections[name].append(B2DPiSel)

	B2DK = B2DPi.clone("B2DKWith" + name + "For" + moduleName, 
	                   DaughtersCuts = { "D0" : Dcut , 
	                                     "K+" : Bachelorcut }, 
	                   DecayDescriptors = [ "[B+ -> D~0 K+]cc", "[B+ -> D0 K+]cc" ] ) 
        B2DKSel = Selection("SelB2DKWith" + name + "For" + moduleName, Algorithm = B2DK, 
                        RequiredSelections = [ DSel, BachelorKSel] )
        BSelections[name].append(B2DKSel)

	B2DKstar = B2DPi.clone("B2DKstarWith" + name + "For" + moduleName, 
	                   DaughtersCuts = { "D0" : Dcut 
	                                   }, 
	                   DecayDescriptors = [ "[B0 -> D~0 K*(892)0]cc", "[B0 -> D0 K*(892)0]cc" ] ) 
        B2DKstarSel = Selection("SelB2DKstarWith" + name + "For" + moduleName, Algorithm = B2DKstar, 
                        RequiredSelections = [ DSel, KstarSel] )
        BSelections[name].append(B2DKstarSel)

	B2DRho = B2DPi.clone("B2DRhoWith" + name + "For" + moduleName, 
	                   DaughtersCuts = { "D0" : Dcut 
	                                   }, 
	                   DecayDescriptors = [ "B0 -> D~0 rho(770)0", "B0 -> D0 rho(770)0" ] ) 
        B2DRhoSel = Selection("SelB2DRhoWith" + name + "For" + moduleName, Algorithm = B2DRho, 
                        RequiredSelections = [ DSel, RhoSel] )
        BSelections[name].append(B2DRhoSel)

	B2DPhi = B2DPi.clone("B2DPhiWith" + name + "For" + moduleName, 
	                   DaughtersCuts = { "D0" : Dcut 
	                                   } , 
	                   DecayDescriptors = [ "B0 -> D~0 phi(1020)", "B0 -> D0 phi(1020)" ] ) 
        B2DPhiSel = Selection("SelB2DPhiWith" + name + "For" + moduleName, Algorithm = B2DPhi, 
                        RequiredSelections = [ DSel, PhiSel] )
        BSelections[name].append(B2DPhiSel)

    # Dictionary of charged D selections

    DcSelections = {
       "D2hhh"           : [ D2KPiPiSel, D2PiPiPiSel, D2KKPiSel, D2KPiPiDCSSel ]
    }

    for name,selections in DcSelections.iteritems() : 

	BSelections[name] = []

        DSel = MergedSelection("Merged" + name + "For" + moduleName, 
                               RequiredSelections = selections )

	B2DPi = CombineParticles("B2DPiWith" + name + "For" + moduleName)
	B2DPi.DecayDescriptors = [ "[B0 -> D- pi+]cc" ]
        B2DPi.DaughtersCuts = { "D0" : Dcut , 
                                "pi+" : Bachelorcut }
        B2DPi.CombinationCut = Bcombcut
        B2DPi.MotherCut = Bcut
        B2DPiSel = Selection("SelB2DPiWith" + name + "For" + moduleName, Algorithm = B2DPi, 
                        RequiredSelections = [ DSel, BachelorPiSel] )
        BSelections[name].append(B2DPiSel)

	B2DK = B2DPi.clone("B2DKWith" + name + "For" + moduleName, 
	                   DaughtersCuts = { "D0" : Dcut , 
	                                     "K+" : Bachelorcut }, 
	                   DecayDescriptors = [ "[B0 -> D- K+]cc" ] ) 
        B2DKSel = Selection("SelB2DKWith" + name + "For" + moduleName, Algorithm = B2DK, 
                        RequiredSelections = [ DSel, BachelorKSel] )
        BSelections[name].append(B2DKSel)

    # For now, we merge all B selections to create one line for each set of D modes

    BMergedSel = []

    for name,list in BSelections.iteritems() : 

	BSel = MergedSelection(moduleName + "With" + name, 
                               RequiredSelections = list )
        BMergedSel.append(BSel)

    ########################################################################
    ########################################################################
    #Now we define a second selection, for the signal box
    #The signal box is in all the relevant masses: B, D, phi, and K*
    #This is allowed since we are making all possible combinations in the decay descriptor
    #(at least we think we are, if anything was forgotten it shouldn't have been...)
    #For the B and the D the signal mass windows include both the B/Bs and D/Ds masses 
    ########################################################################
    ########################################################################

    B2DXSignal = FilterDesktop("B2DXSignalFilterFor" + moduleName)
    B2DXSignal.Code = BSignalcombcut + "&" + "INTREE(((ABSID == 'D0') | (ABSID == 'D+')) & " + DSignalcut + ")"

    B2DXSideband = FilterDesktop("B2DXSidebandFilterFor" + moduleName)
    B2DXSideband.Code = "ALL"

    lines = []

    for selection in BMergedSel : 

        BSignalSel = Selection(selection.name() + "Signal", Algorithm = B2DXSignal, RequiredSelections = [ selection ] ) 

        BSidebandSel = Selection(selection.name() + "Sideband", Algorithm = B2DXSideband, RequiredSelections = [ selection ] ) 

	prescaleFactor = 1.0
	if selection.name() == moduleName + "WithD2hh"  : prescaleFactor = "%(D2hhPrescale)s" % locals()
	if selection.name() == moduleName + "WithD2hhh" : prescaleFactor = "%(D2hhhPrescale)s" % locals()
	if selection.name() == moduleName + "WithD2hhhh": prescaleFactor = "%(D2hhhhPrescale)s" % locals()
	if selection.name() == moduleName + "WithD2Ks"  : prescaleFactor = "%(D2KsPrescale)s" % locals()
	if selection.name() == moduleName + "WithD2pi0" : prescaleFactor = "%(D2pi0Prescale)s" % locals()

        line = StrippingLine(selection.name()+"Line", prescale = prescaleFactor, 
                algos = [ BSidebandSel ], postscale = "%(SidebandPostscale)s" % locals() )

        lines.append( line )

        signalLine = StrippingLine(BSignalSel.name()+"Line", prescale = prescaleFactor, 
                algos = [ BSignalSel ] )
            
        lines.append( signalLine )

    return lines
