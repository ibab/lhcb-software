"""
Line config dictionaries for the Gamma from Trees WG


Dictionary names should have the same name as the corresponding
StrippingSelections file containing the line builder instance.

   e.g  StrippingPromptCharm linebuilder should be added as:

      PromptCharm = { ... }
"""

from GaudiKernel.SystemOfUnits import *

B2DX = {
    'BUILDERTYPE' : 'B2DXConf',
    'CONFIG' : {                                  # Default configuration dictionary
    	    "KstarCuts" : {                 # Kstar->Kpi cuts 
    	      "DauChi2Max"   : 4.,          # maximum track chi2
    	      "DauPtMin"     : 300.,        # track Pt cut (MeV)
    	      "DauPMin"      : 2000.,       # track P cut (MeV)
    	      "DauMIPChi2Min": 4.,          # minimum IP chi2 for a track
	      "VtxChi2Max"   : 12.,         # vertex chi2
	      "MIPChi2DVMin" : 4.,          # min chi2 distance to any PV
	      "PtMin"	     : 1000,        # Minimum Pt of the Kstar candidate
	      "DMass"	     : 150,         # Mass window (MeV)
    	    }, 
    	    "Lambda0Cuts" : {               # "Lambda0" (in fact, ppi and pK combinations) cuts
    	      "DauChi2Max"   : 4.,          # maximum track chi2
    	      "DauPtMin"     : 300.,        # track Pt cut (MeV)
    	      "DauPMin"      : 2000.,       # track P cut (MeV)
    	      "DauMIPChi2Min": 4.,          # minimum IP chi2 for a track
	      "VtxChi2Max"   : 12.,         # vertex chi2
	      "MIPChi2DVMin" : 4.,          # min chi2 distance to any PV
	      "PtMin"	     : 1000,        # Minimum Pt of the candidate
    	    }, 
    	    "PhiCuts" : {                   # phi->KK cuts
    	      "DauChi2Max"   : 4.,          # maximum track chi2
    	      "DauPtMin"     : 250.,        # track Pt cut (MeV)
    	      "DauPMin"      : 2000.,       # track P cut (MeV)
    	      "DauMIPChi2Min": 4.,          # minimum IP chi2 for a track
	      "VtxChi2Max"   : 12.,         # vertex chi2
	      "MIPChi2DVMin" : 4.,          # min chi2 distance to any PV
	      "PtMin"	     : 1000,        # Minimum Pt of the candidate 
	      "DMass"	     : 150,         # Mass window (MeV)
    	    }, 
    	    "RhoCuts" : {                   # rho->pipi cuts
    	      "DauChi2Max"   : 4.,          # maximum track chi2
    	      "DauPtMin"     : 250.,        # track Pt cut (MeV)
    	      "DauPMin"      : 2000.,       # track P cut (MeV)
    	      "DauMIPChi2Min": 4.,          # minimum IP chi2 for a track
	      "VtxChi2Max"   : 12.,         # vertex chi2
	      "MIPChi2DVMin" : 4.,          # min chi2 distance to any PV
	      "PtMin"	     : 1000,        # Minimum Pt of the candidate 
	      "DMass"	     : 150,         # Mass window (MeV)
    	    }, 
    	    "VChargedCuts" : {              # [rho+ -> pipi0]cc and [Kstar+ -> Kpi0]cc cuts
    	      "DauChi2Max"   : 4.,          # maximum track chi2
    	      "DauPtMin"     : 500.,        # track Pt cut (MeV)
    	      "DauPMin"      : 2000.,       # track P cut (MeV)
    	      "DauMIPChi2Min": 4.,          # minimum IP chi2 for a track
	      "Pi0PtMin"     : 500,         # minimum Pi0 Pt (MeV)
	      "Pi0PMin"	     : 2000,        # minimum Pi0 P (MeV)
	      "PtMin"	     : 1500,        # Minimum Pt of the candidate 
	      "DMass"	     : 150,         # Mass window (MeV) 
    	    }, 
    	    "KsCuts" : {                    # Ks cuts (both DD and LL)
    	      "DauChi2Max"   : 4.,          # maximum track chi2
    	      "DauPtMin"     : 250.,        # track Pt cut (MeV)
    	      "DauPMin"      : 2000.,       # track P cut (MeV)
    	      "DauMIPChi2Min": 4.,          # minimum IP chi2 for a track
	      "KsCombDMass"  : 50.,         # MeV,  Ks inv. mass range for pion combinations before vertex fitting
	      "KsDMass"      : 30.,         # MeV,  Ks inv. mass range after vertex fitting
	      "KsVtxChi2Max" : 16.,         # Vertex chi2 for Ks
	      "KsFlightChi2" : 4.,          # Ks flight chi2
    	    }, 
    	    "D2hhCuts" : {                   # D->hh cuts
    	      "DauChi2Max"   : 4.,           # maximum track chi2
    	      "DauPtMin"     : 250.,         # track Pt cut (MeV)
    	      "DauPMin"      : 2000.,        # track P cut (MeV)
    	      "DauMIPChi2Min": 4.,           # minimum IP chi2 for a track
    	      "CombDMass"    : 110.,         # mass window (MeV) for a combination
    	      "DMass"        : 100.,         # mass window (MeV) after vertex fit
    	      "VtxChi2Max"   : 6.,           # D vertex Chi2
    	      "VDChi2Min"    : 100.,         # D flight distance Chi2
	      "PtMin"        : 1000.,        # Minimum Pt of the D (MeV)
	      "MaxDauMIPChi2": 40.,          # IP chi2 for at least one track
	      "DIRAMin"	     : 0.9,          # minimum D DIRA 
	      "DOCAMax"      : 0.6,          # maximum DOCA of track combinations (mm)
	      "IPChi2Min"    : 0.,           # Minimum IP chi2 
    	    }, 
    	    "D2hhhCuts" : {                  # D->hhh cuts
    	      "DauChi2Max"   : 4.,           # maximum track chi2
    	      "DauPtMin"     : 250.,         # track Pt cut (MeV)
    	      "DauPMin"      : 2000.,        # track P cut (MeV)
    	      "DauMIPChi2Min": 4.,           # minimum IP chi2 for a track
    	      "CombDMass"    : 110.,         # mass window (MeV) for a combination
    	      "DMass"        : 100.,         # mass window (MeV) after vertex fit
    	      "VtxChi2Max"   : 9.,           # D vertex Chi2
    	      "VDChi2Min"    : 36.,          # D flight distance chi2
	      "PtMin"        : 1500.,        # Minimum Pt of the D (MeV)
	      "MaxDauMIPChi2": 40.,          # IP chi2 for at least one track
	      "DIRAMin"	     : 0.9,          # minimum D DIRA
	      "DOCAMax"      : 0.6,          # maximum DOCA of track combinations (mm)
	      "IPChi2Min"    : 0.,           # Minimum IP chi2
    	    }, 
    	    "D2hhhhCuts" : {                 # D->hhhh cuts
    	      "DauChi2Max"   : 4.,           # maximum track chi2
    	      "PionPtMin"    : 150.,         # pion Pt cut (MeV)
    	      "PionPMin"     : 2000.,        # pion P cut (MeV)
    	      "KaonPtMin"    : 250.,         # kaon Pt cut (MeV)
    	      "KaonPMin"     : 3000.,        # kaon P cut (MeV)
    	      "DauMIPChi2Min": 4.,           # minimum IP chi2 for a track
    	      "CombMassMin"  : 1758.,        # lower mass cut for a combination (MeV)
    	      "CombMassMax"  : 1978.,        # upper mass cut for a combination (MeV)
    	      "MassMin"      : 1768.,        # lower mass cut after vertex fit (MeV)
    	      "MassMax"      : 1968.,        # upper mass cut after vertex fit (MeV)
    	      "VtxChi2Max"   : 9.,           # D vertex Chi2
    	      "VDChi2Min"    : 36.,          # D flight distance chi2
	      "PtMin"        : 2000.,        # Minimum Pt of the D (MeV)
	      "PMin"         : 2000.,        # Minimum P of the D (MeV)
	      "MaxDauMIPChi2": 40.,          # IP chi2 for at least one track
	      "DIRAMin"	     : 0.9,          # minimum D DIRA 
	      "DOCAMax"      : 0.6,          # maximum DOCA of track combinations (mm)
    	    }, 
    	    "D2KshhCuts" : {                 # D->Kshh cuts
    	      "DauChi2Max"   : 4.,           # maximum track chi2
    	      "DauPtMin"     : 250.,         # track Pt cut (MeV)
    	      "DauPMin"      : 2000.,        # track P cut (MeV)
    	      "DauMIPChi2Min": 4.,           # minimum IP chi2 for a track
    	      "CombDMass"    : 110.,         # mass window (MeV) for a combination
    	      "DMass"        : 100.,         # mass window (MeV) after vertex fit
    	      "VtxChi2Max"   : 9.,           # D vertex Chi2
    	      "VDChi2Min"    : 100.,         # D flight distance chi2
	      "PtMin"        : 1500.,        # Minimum Pt of the D (MeV)
	      "PMin"         : 1500.,        # Minimum P of the D (MeV)
	      "DIRAMin"	     : 0.9,          # minimum D DIRA
	      "DOCAMax"      : 0.6,          # maximum DOCA of charged track combinations (excl. Ks, mm)
	      "IPChi2Min"    : 0.,           # Minimum IP chi2
    	    }, 
    	    "D2KshCuts" : {                  # [D+ -> Ksh+]cc cuts
    	      "DauChi2Max"   : 4.,           # maximum track chi2
    	      "DauPtMin"     : 500.,         # track Pt cut (MeV)
    	      "DauPMin"      : 2000.,        # track P cut (MeV)
    	      "DauMIPChi2Min": 16.,          # minimum IP chi2 for a track
    	      "CombDMass"    : 110.,         # mass window (MeV) for a combination
    	      "DMass"        : 100.,         # mass window (MeV) after vertex fit
    	      "VtxChi2Max"   : 9.,           # D vertex Chi2
    	      "VDChi2Min"    : 36.,          # D flight distance chi2
	      "PtMin"        : 1500.,        # Minimum Pt of the D (MeV)
	      "DIRAMin"	     : 0.9,          # minimum D DIRA 
	      "IPChi2Min"    : 0.,           # Minimum IP chi2
    	    }, 
    	    "D2KPiPi0Cuts" : {               # D->Kpipi0 cuts
    	      "DauChi2Max"   : 4.,           # maximum track chi2
    	      "DauPtMin"     : 250.,         # track Pt cut (MeV)
    	      "DauPMin"      : 2000.,        # track P cut (MeV)
    	      "DauMIPChi2Min": 4.,           # minimum IP chi2 for a track
    	      "CombMassMin"  : 1708.,        # lower mass cut for a combination (MeV)
    	      "CombMassMax"  : 2028.,        # upper mass cut for a combination (MeV)
    	      "MassMin"      : 1718.,        # lower mass cut after vertex fit (MeV)
    	      "MassMax"      : 2018.,        # upper mass cut after vertex fit (MeV)
    	      "VtxChi2Max"   : 9.,           # D vertex Chi2
    	      "VDChi2Min"    : 36.,          # D flight distance chi2 
	      "PtMin"        : 2000.,        # Minimum Pt of the D (MeV) 
	      "PMin"         : 2000.,        # Minimum P of the D (MeV)
	      "Pi0PtMin"     : 600,          # Minimum Pt of pi0 (MeV)
	      "Pi0PMin"      : 2000,         # Minimum P of pi0 (MeV)
	      "MaxDauMIPChi2": 40.,          # IP chi2 for at least one track
	      "DIRAMin"	     : 0.9,          # minimum D DIRA  
	      "DOCAMax"      : 0.6,          # maximum DOCA of charged track combinations (mm)
    	    }, 
    	    "LambdaCCuts" : {                # [LambdaC->pKpi]cc cuts
    	      "DauChi2Max"   : 4.,           # maximum track chi2
    	      "DauPtMin"     : 250.,         # track Pt cut (MeV)
    	      "DauPMin"      : 2000.,        # track P cut (MeV)
    	      "DauMIPChi2Min": 4.,           # minimum IP chi2 for a track
    	      "CombMassMin"  : 2176.,        # lower mass cut for a combination (MeV)
    	      "CombMassMax"  : 2396.,        # upper mass cut for a combination (MeV)
    	      "MassMin"      : 2186.,        # lower mass cut after vertex fit (MeV)
    	      "MassMax"      : 2386.,        # upper mass cut after vertex fit (MeV)
    	      "VtxChi2Max"   : 9.,           # LambdaC vertex Chi2
    	      "VDChi2Min"    : 36.,          # LambdaC flight distance chi2
	      "PtMin"        : 1000.,        # Minimum Pt of the D (MeV)
	      "DIRAMin"	     : 0.9,          # minimum D DIRA
	      "DOCAMax"      : 0.6,          # maximum DOCA of charged track combinations (mm) 
	      "IPChi2Min"    : 0.,           # Minimum IP chi2
    	    }, 
	    "BCuts" : {                             # B->DX cuts (except for special cases below)
    	      "BachelorChi2Max"    : 4.,            # maximum bachelor track chi2
    	      "BachelorPtMin"      : 500.,          # bachelor track Pt cut (MeV)
    	      "BachelorPMin"       : 5000.,         # bachelor track P cut (MeV)
    	      "BachelorMIPChi2Min" : 16.,           # minimum IP chi2 for a bachelor track
	      "VtxChi2Max"         : 9.,            # B vertex Chi2
	      "IPChi2Max"          : 16.,           # chi2 of B impact parameter to the related PV
	      "LTMin"              : 0.2,           # Minimum B lifetime
	      "DIRAMin"            : 0.9999,        # DIRA of the B to the related PV
	      "CombDMass"          : 500,           # Mass window for a combination (MeV)
	      "APtMin"		   : 0.,            # Minumum Pt of a combination (MeV)
	    }, 
	    "B2DV0Cuts" : {                         # B->D(hhh)V0 (V0=Kstar,rho,phi) cuts
	      "VtxChi2Max"         : 9.,            # B vertex Chi2
	      "VDChi2Min"          : 80,            # Chi2 of B vertex separation from the related PV
	      "IPChi2Max"          : 12.,           # chi2 of B impact parameter to the related PV
	      "LTMin"              : 0.0,           # Minimum B lifetime 
	      "DIRAMin"            : 0.9999,        # DIRA of the B to the related PV
	      "CombDMass"          : 500,           # Mass window for a combination (MeV)
	      "APtMin"		   : 1500.,         # Minumum Pt of a combination (MeV) 
	    }, 
	    "B2DVChargedCuts" : {                   # B->D-(hhh)rho+ and B->D-(hhh)Kstar+ cuts
	      "VtxChi2Max"         : 6.,            # B vertex Chi2
	      "IPChi2Max"          : 16.,           # chi2 of B impact parameter to the related PV
	      "LTMin"              : 0.2,           # Minimum B lifetime
	      "DIRAMin"            : 0.99995,       # DIRA of the B to the related PV
	      "CombDMass"          : 500,           # Mass window for a combination (MeV)
	      "APtMin"		   : 1500.,         # Minumum Pt of a combination (MeV)
	    }, 
	    "LambdaBCuts" : {                       # LambdaB->LambdaCh cuts
    	      "BachelorChi2Max"    : 4.,            # maximum bachelor track chi2
    	      "BachelorPtMin"      : 500.,          # bachelor track Pt cut (MeV)
    	      "BachelorPMin"       : 5000.,         # bachelor track P cut (MeV)
    	      "BachelorMIPChi2Min" : 16.,           # minimum IP chi2 for a bachelor track
	      "VtxChi2Max"         : 9.,            # B vertex Chi2
	      "IPChi2Max"          : 16.,           # chi2 of B impact parameter to the related PV
	      "LTMin"              : 0.2,           # Minimum B lifetime
	      "DIRAMin"            : 0.9999,        # DIRA of the B to the related PV
	      "CombMassMin"        : 4800,          # Lower mass of a combination (MeV)
	      "CombMassMax"        : 5900,          # Upper mass of a combination (MeV) 
	      "APtMin"		   : 0.,            # Minumum Pt of a combination (MeV)
	    }, 
	    "LambdaB2DphCuts" : {                   # LambdaB->D(hh)ppi and LambdaB->D(hh)pK cuts 
	      "VtxChi2Max"         : 9.,            # LambdaB vertex Chi2
	      "IPChi2Max"          : 12.,           # chi2 of LambdaB impact parameter to the related PV
	      "VDChi2Min"          : 80,            # Chi2 of LambdaB vertex separation from the related PV
	      "DVDChi2Min"         : 100,           # Chi2 of daughter D vertex separation from the related PV 
	      "DIRAMin"            : 0.99995,       # DIRA of the B to the related PV
	      "LTMin"              : 0.0,           # Minimum LambdaB lifetime
	      "CombDMass"          : 500,           # Mass window for a combination (MeV) 
	      "APtMin"		   : 1500.,         # Minumum Pt of a combination (MeV) 
	    }, 
	    "UnbiasedBCuts" : {                     # Lifetime-unbiased B->D(hhh)h selection
    	      "BachelorChi2Max"    : 4.,            # maximum track chi2
    	      "BachelorPtMin"      : 500.,          # track Pt cut (MeV)
    	      "BachelorPMin"       : 5000.,         # track P cut (MeV)
    	      "BachelorMIPChi2Min" : 0.,            # minimum IP chi2 for a track
    	      "DMass" 		   : 100.,          # Mass window for a D combination (MeV)
	      "VtxChi2Max"         : 9.,            # B vertex Chi2
	      "IPChi2Max"          : 250.,          # chi2 of B impact parameter to the related PV
	      "LTMin"              : -1000.,        # Chi2 of B vertex separation from the related PV
	      "DIRAMin"            : -1.,           # DIRA of the B to the related PV
	      "CombDMass"          : 500,           # Mass window for a B combination (MeV)
	    }, 
	    "Prescales" : {                         # Prescales for individual lines
	      "D2hh"     : 1.,
	      "D2hhWS"   : 0.1,
	      "D2hhh"    : 1., 
	      "D2hhhWS"  : 0.1, 
	      "D2hhhh"   : 1., 
	      "D2hhhhWS" : 0.1, 
	      "D2Ksh"    : 1., 
	      "D2Kshh"   : 1., 
	      "D2KshhWS"   : 0.1, 
	      "D2KPiPi0Merged"   : 1., 
	      "D2KPiPi0Resolved" : 1., 
	      "D2KPiPi0MergedWS"   : 0.1, 
	      "D2KPiPi0ResolvedWS" : 0.1, 
	      "Lambda"   : 1., 
	      "Unbiased" : 0.1, 
	    }, 
	    "CheckPV"	       : True,              # PV requirement
	    "MaxTracksInEvent" : {                  # GECs for individual lines (max. number of long tracks)
	      "D2hh"     : 500, 
	      "D2hhWS"   : 500, 
	      "D2hhh"    : 500, 
	      "D2hhhWS"  : 500, 
	      "D2hhhh"   : 500, 
	      "D2hhhhWS" : 500, 
	      "D2Ksh"    : 500, 
	      "D2Kshh"   : 500, 
	      "D2KshhWS" : 500, 
	      "D2KPiPi0Merged"   : 500, 
	      "D2KPiPi0Resolved" : 500, 
	      "D2KPiPi0MergedWS"   : 500, 
	      "D2KPiPi0ResolvedWS" : 500, 
	      "Lambda"   : 500, 
	      "Unbiased" : 500
	    }, 
 	    "MergedLines" : True                    # If True, lines with the same D mode a merged
	},
    'STREAMS' : [ 'Bhadron' ],
    'WGs' : [ 'GammaFromTrees' ]
}

DstarD02Kpipi0 = {
    'BUILDERTYPE' : 'DstarD02Kpipi0Conf',
    'CONFIG' : {
  	"RSPrescale" : 0.25, 
  	"WSPrescale" : 0.0025
	},
    'STREAMS' : [ 'CharmControl' ],
    'WGs' : [ 'GammaFromTrees' ]
}

# line currently does not meet stripping requirements
B2D3H = {
    'BUILDERTYPE' : 'B2D3HAllLinesConf',

    'CONFIG' : {
        "PionMinP"             : 2000.,
        "PionMaxP"             : 500000.,
        "PionMinPT"            : 200.,
        "PionMinPTtight"       : 500.,      
        "PionMinIPChisq"       : 6.25,
        "PionPiDDLL"           : 12,
        "KaonMinP"             : 2000.,
        "KaonMaxP"             : 500000.,
        "KaonMinPT"            : 200.,
        "KaonMinPTtight"       : 500.,        
        "KaonMinIPChisq"       : 6.25,
        "ProtonMinP"             : 2000.,
        "ProtonMaxP"             : 500000.,
        "ProtonMinPT"            : 200.,
        "ProtonMinIPChisq"       : 6.25,
        "MinPT"                : 300.,
        "TrkChisq"             : 4.0,
        "TrkChisqtight"        : 4.0,        
        "Bach3HMassWindow"     : 3000,
        "Bach3HDocaMax"        : 0.6,
        "Bach3HVtxChisq"       : 8.0,
        "Bach3HMinPT"          : 1000.0, 
        "Bach3HIP2PV"          : 0.0,
        "Bach3HIPChisq2PV"     : 9.0,
        "Bach3HVtxSepChisq"    : 36.0,
        "Bach3HDiraPV"         : 0.98,
        "Bach3HZVtxSep"        : 0.0,
        "Bach3HDRPV"           : 0.1,
        "DMinPT"               : 1000,
        "DVtxChisq"            : 8.0,
        "DMassWindow"          : 70,
        "D0MassWindow"         : 70,
        "DsMassWindow"         : 70,
        "tightDMassWindow"     : 40,
        "DDocaMax"             : 0.6,
        "DIP2PV"               : 0.0,
        "DIPChisq2PV"          : 4.0,
        "DVtxSepChisq"         : 49.0,
        "DDiraPV"              : 0.98,
        "DZVtxSep"             : 0.0,
        "DDRPV"                : 0.1,
        "DStarMassWindow"      : 50,
        "DStarMinPT"           : 1000,
        "BMinPT"               : 0.0, 
        "BVtxChisq"            : 8.0, #was 6.0
        "BMassWindow"          : 300, 
        "tightBMassWindow"     : 300,
        "BIP2PV"               : 0.15,
        "BIPChisq2PV"          : 16.0,
        "BVtxSepChisq"         : 25.0,
        "BDiraPV"              : 0.99994,
        "BZVtxSep"             : 0.0,
        "BDZVtxSep"            : -1.0,  
        "BDRPV"                : 0.1,
        "MaxTracks"                     : 300,
        "B2D0PiPiPiAll_Prescale"        : 1.0,
        "B2D0PiPiPiAll_Postscale"       : 1.0,
        "B2DPiPiPiAll_Prescale"         : 1.0,
        "B2DPiPiPiAll_Postscale"        : 1.0,
        "B2DStarPiPiPiAll_Prescale"     : 1.0,
        "B2DStarPiPiPiAll_Postscale"    : 1.0,
        "B2D0KPiPiAll_Prescale"         : 1.0,
        "B2D0KPiPiAll_Postscale"        : 1.0,
        "B2DKPiPiAll_Prescale"          : 1.0,
        "B2DKPiPiAll_Postscale"         : 1.0,
        "B2DStarKPiPiAll_Prescale"      : 1.0,
        "B2DStarKPiPiAll_Postscale"     : 1.0,
        "B2DDAll_Prescale"              : 1.0,
        "B2DDAll_Postscale"             : 1.0,
        "B2DStarDAll_Prescale"          : 1.0,
        "B2DStarDAll_Postscale"         : 1.0,
        "UnbiasedB2DPiPiPiAll_Prescale"         : 0.3,
        "UnbiasedB2DPiPiPiAll_Postscale"        : 1.0,
        "WSB2D3H_Prescale"              : 0.1,
        "WSB2D3H_Postscale"             : 1.0,
        "B2DStarDKAll_Prescale"          : 1.0,
        "B2DStarDKAll_Postscale"         : 1.0
        }, 
    'STREAMS' : [ 'Bhadron' ],
    'WGs' : [ 'GammaFromTrees' ]
}

Bu2D0h_D02KShh_NoPID = {
    'BUILDERTYPE' : 'StrippingBu2D0h_D02KShh_NoPIDConf',
    'CONFIG' : {#almost offline sel cuts 8/2/2011 (D daughter pi/K IPchi2s looser, Ks DD daughter pi PID cut looser)
                        'PrescaleLL'    : 1.0 
                        ,  'PostscaleLL'   : 1.0
                        #B cuts, LL seln
                       ,  'BFlightCHI2_LL'        : 169. 
                       ,  'BDIRA_LL'           : 0.99999     
                       ,  'BIPCHI2_LL'         : 9.         
                       ,  'BVertexCHI2_LL'     : 6.25
                       #Bachelor K cuts, LL seln
                       ,  'BachIPCHI2_LL'     : 16.
                       ,  'BachPt_LL'         : 1.0  
                       #D0 and D0 daughter pi cuts, LL seln
                       ,  'DVertexCHI2_LL'     : 4.     
                       ,  'DdaughterIPCHI2_LL' : 16.
                       #KS and KS daughter pi cuts, LL seln
                       ,  'KSFlightCHI2_LL'    : 4.
                       ,  'KSVertexCHI2_LL'    : 16.
                       ,  'KSdaughterPiIPCHI2_LL' : 9.
                       ,  'PrescaleDD'    : 1.0 
                       ,  'PostscaleDD'   : 1.0
                       #B cuts, DD seln
                       ,  'BFlightCHI2_DD'     : 100.
                       ,  'BDIRA_DD'           : 0.9999    
                       ,  'BIPCHI2_DD'         : 16.         
                       ,  'BVertexCHI2_DD'     : 9.
                       #Bachelor K cuts, DD seln
                       ,  'BachIPCHI2_DD'     : 6.25
                       ,  'BachPt_DD'         : 0.5 
                       #D0 and D0 daughter pi cuts, DD seln
                       ,  'DVertexCHI2_DD'     : 16.        
                       ,  'DdaughterIPCHI2_DD' : 9.
                        #KS and KS daughter pi cuts, DD seln
                       ,  'KSFlightCHI2_DD'    : 4.
                       ,  'KSVertexCHI2_DD'    : 16.
                       ,  'KSdaughterPiIPCHI2_DD' : 6.25
 	}, 
    'STREAMS' : [ 'Bhadron' ],
    'WGs' : [ 'GammaFromTrees' ]
}

Bu2D0h_D02KShh_NoPID_WS = {
    'BUILDERTYPE' : 'StrippingBu2D0h_D02KShh_NoPID_WSConf',
    'CONFIG' : {#almost offline sel cuts 8/2/2011 (D daughter pi/K IPchi2s looser)
                        'PrescaleLL'    : 0.15 
                        ,  'PostscaleLL'   : 1.0
                        #B cuts, LL seln
                       ,  'BFlightCHI2_LL'        : 169. 
                       ,  'BDIRA_LL'           : 0.99999     
                       ,  'BIPCHI2_LL'         : 9.         
                       ,  'BVertexCHI2_LL'     : 6.25
                       #Bachelor K cuts, LL seln
                       ,  'BachIPCHI2_LL'     : 16.
                       ,  'BachPt_LL'         : 1.0  
                       #D0 and D0 daughter pi cuts, LL seln
                       ,  'DVertexCHI2_LL'     : 4.     
                       ,  'DdaughterIPCHI2_LL' : 16.
                       #KS and KS daughter pi cuts, LL seln
                       ,  'KSFlightCHI2_LL'    : 4.
                       ,  'KSVertexCHI2_LL'    : 16.
                       ,  'KSdaughterPiIPCHI2_LL' : 9.
                       ,  'PrescaleDD'    : 0.15 
                       ,  'PostscaleDD'   : 1.0
                       #B cuts, DD seln
                       ,  'BFlightCHI2_DD'     : 100.
                       ,  'BDIRA_DD'           : 0.9999    
                       ,  'BIPCHI2_DD'         : 16.         
                       ,  'BVertexCHI2_DD'     : 9.
                       #Bachelor K cuts, DD seln
                       ,  'BachIPCHI2_DD'     : 6.25
                       ,  'BachPt_DD'         : 0.5 
                       #D0 and D0 daughter pi cuts, DD seln
                       ,  'DVertexCHI2_DD'     : 16.        
                       ,  'DdaughterIPCHI2_DD' : 9.
                        #KS and KS daughter pi cuts, DD seln
                       ,  'KSFlightCHI2_DD'    : 4.
                       ,  'KSVertexCHI2_DD'    : 16.
                       ,  'KSdaughterPiIPCHI2_DD' : 6.25
 		}, 
    'STREAMS' : [ 'Bhadron' ],
    'WGs' : [ 'GammaFromTrees' ]
}
