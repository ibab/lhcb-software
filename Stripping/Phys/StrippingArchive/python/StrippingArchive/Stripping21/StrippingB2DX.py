'''
B->DX selection for stripping. Uses selection framework.

B->DX,
where X=pi,K,K*,rho0,rho+,phi,
and   D=D+,D0,Ds.
The allowed decay modes of the intermediates are
D+ -> hhh (h=pi,K all combinations except KKK),
Ds -> hhh (h=pi,K all combinations except KKK),
D0 -> hh (h=pi,K all combinations).
D0 -> hhhh (K3pi FAV and DCS, and KKpipi are considered),
D0 -> Kpipi0
D0 -> Kshh (h=pi,K all combinations)

This selection deliberately uses no RICH information, since the channels in
the B->DX family are kinematically identical to each other and are the dominant
non-combinatoric backgrounds to each other.

In all cases where Cabibbo-favored and DCS decays lead to the same final
state, only CF is taken: e.g. K-Pi+ combination is always called D0, instead
of producing two candidates, D0 and D0bar. In B combinations, both suppressed
and favored modes are included (e.g. B+ -> D0bar K+ and B+ -> D0 K+ and c.c.)

Authors: A. Poluektov
         D0->hhhh and D0->KPiPi0 selections by Aurelien Martens.
         LambdaB->LambdaC K/pi selections by Rolf Oldeman.

'''

__author__  = ["Jacopo Nardulli", "Vava Gligorov", "Anton Poluektov",
               "Aurelien Martens", "Rolf Oldeman" ]
__date__    = "11/02/2011"
__version__ = ""

from Gaudi.Configuration import *
from GaudiConfUtils.ConfigurableGenerators import FilterDesktop, CombineParticles
from PhysSelPython.Wrappers import Selection, DataOnDemand, MergedSelection
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder, checkConfig
from StandardParticles import ( StdNoPIDsPions,
                                StdNoPIDsKaons,
                                StdNoPIDsDownPions,
                                StdNoPIDsProtons,
                                StdAllNoPIDsPions,
                                StdLooseMergedPi0,
                                StdLooseResolvedPi0 )

from copy import copy

moduleName = "B2DX"

config = {                                  # Default configuration dictionary

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
	      "PhotonCL"     : 0.25,         # Confidence level for Pi0 photons
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
	      "APtMin"		   : 1500.,            # Minumum Pt of a combination (MeV)
	    }, 
	    "BWithD2KPiPi0Cuts" : {                 # B->DX cuts with D->KPiPi0
    	      "BachelorChi2Max"    : 4.,            # maximum bachelor track chi2
    	      "BachelorPtMin"      : 500.,          # bachelor track Pt cut (MeV)
    	      "BachelorPMin"       : 5000.,         # bachelor track P cut (MeV)
    	      "BachelorMIPChi2Min" : 16.,           # minimum IP chi2 for a bachelor track
	      "VtxChi2Max"         : 9.,            # B vertex Chi2
	      "IPChi2Max"          : 16.,           # chi2 of B impact parameter to the related PV
	      "LTMin"              : 0.2,           # Minimum B lifetime
	      "DIRAMin"            : 0.9999,        # DIRA of the B to the related PV
	      "CombDMass"          : 500,           # Mass window for a combination (MeV)
	      "APtMin"		   : 1500.,            # Minumum Pt of a combination (MeV)
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
	      "CombMassMin"        : 5100,          # Lower mass of a combination (MeV)
	      "CombMassMax"        : 6200,          # Upper mass of a combination (MeV) 
	      "APtMin"		   : 1500.,         # Minumum Pt of a combination (MeV)
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
	    "HLT"              : "HLT_PASS_RE('Hlt2Topo.*Decision')", 
#	    "HLT"              : None, 
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
}


class B2DXConf(LineBuilder) :
    __configuration_keys__ = ("KstarCuts",
                              "PhiCuts",
                              "RhoCuts",
                              "KsCuts",
                              "VChargedCuts",
                              "Lambda0Cuts", 
                              "D2hhCuts",
                              "D2hhhCuts",
                              "D2hhhhCuts",
                              "D2KPiPi0Cuts",
                              "D2KshhCuts",
                              "D2KshCuts",
                              "LambdaCCuts",
                              "BCuts",
                              "BWithD2KPiPi0Cuts",
                              "B2DV0Cuts",
                              "B2DVChargedCuts",
                              "LambdaBCuts",
                              "LambdaB2DphCuts",
                              "UnbiasedBCuts",
                              "Prescales",
                              "CheckPV",
                              "HLT",
                              "MaxTracksInEvent", 
                              "MergedLines")

    def __init__(self, moduleName, config) :

        LineBuilder.__init__(self, moduleName, config)

        from Configurables import LoKi__VoidFilter as VoidFilter

#        from Configurables import LoKi__Hybrid__CoreFactory as CoreFactory
#        modules = CoreFactory('CoreFactory').Modules
#        for i in ['LoKiTracks.decorators']:
#            if i not in modules : modules.append(i)
#            if i in modules : modules.remove(i)

        Ks = makeKs(moduleName, config["KsCuts"])

        D2hh = makeD2hh(moduleName, config["D2hhCuts"])
        D2hhh = makeD2hhh(moduleName, config["D2hhhCuts"])
        D2hhhh = makeD2hhhh(moduleName, config["D2hhhhCuts"])
        D2hhWS = makeD2hhWS(moduleName, config["D2hhCuts"])
        D2hhhWS = makeD2hhhWS(moduleName, config["D2hhhCuts"])
        D2hhhhWS = makeD2hhhhWS(moduleName, config["D2hhhhCuts"])

        D2KPiPi0Merged = makeD2KPiPi0(moduleName, "Merged", config["D2KPiPi0Cuts"],
                                      Pi0Sel = StdLooseMergedPi0)
        D2KPiPi0Resolved = makeD2KPiPi0(moduleName, "Resolved", config["D2KPiPi0Cuts"],
                                        Pi0Sel = StdLooseResolvedPi0)
        D2Kshh = makeD2Kshh(moduleName, Ks, config["D2KshhCuts"])
        D2Ksh = makeD2Ksh(moduleName, Ks, config["D2KshCuts"])

        D2KPiPi0MergedWS = makeD2KPiPi0WS(moduleName, "Merged", config["D2KPiPi0Cuts"],
                                      Pi0Sel = StdLooseMergedPi0)
        D2KPiPi0ResolvedWS = makeD2KPiPi0WS(moduleName, "Resolved", config["D2KPiPi0Cuts"],
                                        Pi0Sel = StdLooseResolvedPi0)
        D2KshhWS = makeD2KshhWS(moduleName, Ks, config["D2KshhCuts"])
        LambdaC = makeLambdaC(moduleName, config["LambdaCCuts"])

        DSelections = {
          "D2hh"     : D2hh,
          "D2hhWS"   : D2hhWS,
          "D2hhh"    : D2hhh,
          "D2hhhWS"  : D2hhh,
          "D2hhhh"   : D2hhhh,
          "D2hhhhWS" : D2hhhhWS,
          "D2KPiPi0Merged"   : D2KPiPi0Merged,
          "D2KPiPi0Resolved" : D2KPiPi0Resolved,
          "D2Kshh"   : D2Kshh, 
          "D2Ksh"   : D2Ksh, 
          "D2KPiPi0MergedWS"   : D2KPiPi0MergedWS,
          "D2KPiPi0ResolvedWS" : D2KPiPi0ResolvedWS,
          "D2KshhWS"   : D2KshhWS
        }
        
        Pi0 = makePi0(moduleName)

        KstarNoMW = makeKstarNoMW(moduleName, config["KstarCuts"])
        Kstar = makeKstar(moduleName, KstarNoMW, config["KstarCuts"])
        KstarCharged = makeKstarCharged(moduleName, Pi0, config["VChargedCuts"])

        PhiNoMW = makePhiNoMW(moduleName, config["PhiCuts"])
        Phi = makePhi(moduleName, PhiNoMW, config["PhiCuts"])

        RhoNoMW = makeRhoNoMW(moduleName, config["RhoCuts"])
        Rho = makeRho(moduleName, RhoNoMW, config["RhoCuts"])
        RhoCharged = makeRhoCharged(moduleName, Pi0, config["VChargedCuts"])

        Lambda02ppi = makeLambda02ppi(moduleName, config["Lambda0Cuts"])
        Lambda02pK  = makeLambda02pK(moduleName, config["Lambda0Cuts"])

        # List of B selections
        BSelections = []

        for name, dsel in DSelections.iteritems() :

            list = []

            if name in ["D2hhh", "D2Ksh"] :
                selection1 = makeB02DK(moduleName, name, dsel, config["BCuts"])
                selection2 = makeB02DPi(moduleName, name, dsel, config["BCuts"])
                selection3 = makeB02DRhoCharged(moduleName, name, dsel, RhoCharged, config["B2DVChargedCuts"])
                selection4 = makeB02DKstarCharged(moduleName, name, dsel, KstarCharged, config["B2DVChargedCuts"])

                list += [ selection1, selection2, selection3, selection4 ]

		if name == "D2hhh" : 
            	    selection5 = makeB2DKstar(moduleName, name, Kstar, dsel, config["B2DV0Cuts"])
            	    selection6 = makeB2DPhi(moduleName, name, Phi, dsel, config["B2DV0Cuts"])
            	    selection7 = makeB2DRho(moduleName, name, Rho, dsel, config["B2DV0Cuts"])
		    list += [ selection5, selection6, selection7 ]

            elif name in ["D2hhhWS"] :
                selection1 = makeB02DKWS(moduleName, name, dsel, D2hhhWS, config["BCuts"])
                selection2 = makeB02DPiWS(moduleName, name, dsel, D2hhhWS, config["BCuts"])
                list += [ selection1, selection2]
            else :
                selection1 = makeB2D0K(moduleName, name, dsel, config["BCuts"])
                selection2 = makeB2D0Pi(moduleName, name, dsel, config["BCuts"])
                list += [ selection1, selection2]

            if name in ["D2hh", "D2Kshh", "D2hhWS", "D2KshhWS"] :
                selection3 = makeB02D0Kstar(moduleName, name, KstarNoMW, dsel, config["BCuts"])
                selection4 = makeB02D0Phi(moduleName, name, PhiNoMW, dsel, config["BCuts"])
                selection5 = makeB02D0Rho(moduleName, name, RhoNoMW, dsel, config["BCuts"])
                list += [ selection3, selection4, selection5 ]
                
                if name in [ "D2hh", "D2hhWS" ] : 
            	    selection6 = makeLambdaB2D0Lambda(moduleName, name, "pPi", dsel, Lambda02ppi, config["LambdaB2DphCuts"])
            	    selection7 = makeLambdaB2D0Lambda(moduleName, name, "pK",  dsel, Lambda02pK, config["LambdaB2DphCuts"])
            	    list += [ selection6, selection7 ]

            elif name in ["D2hhhh", "D2hhhhWS"] :
                selection3 = makeB02D0Kstar(moduleName, name, Kstar, dsel, config["BCuts"])
                selection4 = makeB02D0Phi(moduleName, name, Phi, dsel, config["BCuts"])
                selection5 = makeB02D0Rho(moduleName, name, Rho, dsel, config["BCuts"])
                list += [ selection3, selection4, selection5 ]

            elif name in ["D2KPiPi0Merged", "D2KPiPi0Resolved", "D2KPiPi0MergedWS", "D2KPiPi0ResolvedWS"] :
                selection3 = makeB02D0Kstar(moduleName, name, Kstar, dsel, config["BWithD2KPiPi0Cuts"])
                selection4 = makeB02D0Phi(moduleName, name, Phi, dsel, config["BWithD2KPiPi0Cuts"])
                selection5 = makeB02D0Rho(moduleName, name, Rho, dsel, config["BWithD2KPiPi0Cuts"])
                list += [ selection3, selection4, selection5 ]

	    if (config["MergedLines"]) : 

        	BSel = MergedSelection(moduleName + "With" + name + "Merged", RequiredSelections = list )

        	B2DXSideband = FilterDesktop(Code = "ALL")

    		BSidebandSel = Selection(moduleName + "With" + name, Algorithm = B2DXSideband, RequiredSelections = [ BSel ] )

        	line = StrippingLine(BSidebandSel.name()+"Line", prescale = config["Prescales"][name] ,
                                 selection = BSidebandSel,
                                 checkPV = config["CheckPV"],
                                 FILTER = {
                                   "Code" : "(recSummaryTrack(LHCb.RecSummary.nLongTracks, TrLONG) < %s )" % config["MaxTracksInEvent"][name], 
                                   "Preambulo" : [ "from LoKiTracks.decorators import *" ]
                                 }, 
                                 HLT = config["HLT"] )
        	self.registerLine( line )

    	    else : 
		for sel in list : 

        	    B2DXSideband = FilterDesktop(Code = "ALL")

    		    BSidebandSel = Selection("Sel" + sel.name(), Algorithm = B2DXSideband, RequiredSelections = [ sel ] )

        	    line = StrippingLine(sel.name()+"Line", prescale = config["Prescales"][name] ,
                                 selection = BSidebandSel,
                                 checkPV = config["CheckPV"],
                                 FILTER = {
                                   "Code" : "(recSummaryTrack(LHCb.RecSummary.nLongTracks, TrLONG) < %s )" % config["MaxTracksInEvent"][name], 
                                   "Preambulo" : [ "from LoKiTracks.decorators import *" ]
                                 }, 
                                 HLT = config["HLT"] )
        	    self.registerLine( line )

        selection1 = makeLambdaB2LambdaCK(moduleName, LambdaC, config["LambdaBCuts"])
        selection2 = makeLambdaB2LambdaCPi(moduleName, LambdaC, config["LambdaBCuts"])

        LambdaBSel = MergedSelection(moduleName + "WithLambdaMerged", RequiredSelections = [ selection1, selection2 ] )

        B2DXSideband = FilterDesktop(Code = "ALL")

        LambdaBSidebandSel = Selection(moduleName + "WithLambda", Algorithm = B2DXSideband,
                                       RequiredSelections = [ LambdaBSel ] )

        line = StrippingLine(LambdaBSidebandSel.name()+"Line", prescale = config["Prescales"]["Lambda"] ,
                             selection = LambdaBSidebandSel,
                             checkPV = config["CheckPV"],
                             FILTER = {
                                 "Code" : "(recSummaryTrack(LHCb.RecSummary.nLongTracks, TrLONG) < %s )" % config["MaxTracksInEvent"]["Lambda"], 
                                 "Preambulo" : [ "from LoKiTracks.decorators import *" ]
                             }, 
                             HLT = config["HLT"] )

        self.registerLine( line )

        #make unbiased selections
        unbiasedSelection = makeUnbiasedB2DPi("Unbiased" + moduleName, "D2hhh", D2hhh, config["UnbiasedBCuts"]) #NeedsAdditionalString?

        HLT1TIS = makeTISTOSSel("HLT1TISSelFor" + moduleName + "WithUnbiasedBs2DsPi", unbiasedSelection, "Hlt1Global%TIS")
        HLT2TIS = makeTISTOSSel("HLT2TISSelFor" + moduleName + "WithUnbiasedBs2DsPi", HLT1TIS, "Hlt2Global%TIS")

        line = StrippingLine(moduleName + "WithUnbiasedB2DPiLine", prescale = config["Prescales"]["Unbiased"] ,
                             selection = HLT2TIS,
                             checkPV = config["CheckPV"],
                             FILTER = {
                                 "Code" : "(recSummaryTrack(LHCb.RecSummary.nLongTracks, TrLONG) < %s )" % config["MaxTracksInEvent"]["Unbiased"], 
                                 "Preambulo" : [ "from LoKiTracks.decorators import *" ]
                             } ) 
        self.registerLine( line )


def makeD2hh(moduleName, config) :

    __configuration_keys__ = ("DauChi2Max", "DauPtMin", "DauPMin", "DauMIPChi2Min",
                              "PtMin", "CombDMass", "DMass", "VtxChi2Max", "VDChi2Min",
                              "DIRAMin", "DOCAMax", "IPChi2Min", "MaxDauMIPChi2")

    checkConfig(__configuration_keys__, config)

    StdPi = StdNoPIDsPions
    StdK = StdNoPIDsKaons

    Daughtercut = "((TRCHI2DOF<%(DauChi2Max)s) & " \
    "(PT > %(DauPtMin)s*MeV) & (P > %(DauPMin)s*MeV) & " \
    "(MIPCHI2DV(PRIMARY) > %(DauMIPChi2Min)s))" % config

    D2pipi = CombineParticles(DecayDescriptors = [ "D0 -> pi- pi+" ], 
			      DaughtersCuts =  { "pi+"      : Daughtercut, "K+"       : Daughtercut }, 
			      CombinationCut = \
			    "(ADAMASS('D0') < %(CombDMass)s*MeV) & " \
			    "(APT>%(PtMin)s*MeV) & " \
			    "(AMAXDOCA('LoKi::TrgDistanceCalculator') < %(DOCAMax)s) & " \
			    "AHASCHILD(((ABSID == 'K+') | (ABSID == 'pi+')) & (MIPCHI2DV(PRIMARY) > %(MaxDauMIPChi2)s))" \
			    % config, 
			      MotherCut = "(ADMASS('D0') < %(DMass)s*MeV) " \
			    "& (VFASPF(VCHI2/VDOF)<%(VtxChi2Max)s) & (BPVVDCHI2 > %(VDChi2Min)s) & " \
			    "(BPVDIRA > %(DIRAMin)s) & (BPVIPCHI2() > %(IPChi2Min)s)" % config
			      )

    D2Kpi = copy(D2pipi)
    D2Kpi.DecayDescriptors = [ "[D0 -> K- pi+]cc" ]

    D2KK = copy(D2pipi)
    D2KK.DecayDescriptors = [ "D0 -> K- K+" ]

    D2pipiSel = Selection("D2pipiFor" + moduleName, Algorithm = D2pipi, RequiredSelections = [ StdPi ] )
    D2KpiSel = Selection("D2KpiFor" + moduleName, Algorithm = D2Kpi, RequiredSelections = [ StdPi, StdK ] )
    D2KKSel = Selection("D2KKFor" + moduleName, Algorithm = D2KK, RequiredSelections = [ StdK ] )

    DSel = MergedSelection("D2hhFor" + moduleName,
                           RequiredSelections = [ D2pipiSel, D2KpiSel, D2KKSel ] )

    return DSel

def makeD2hhWS(moduleName, config) :

    __configuration_keys__ = ("DauChi2Max", "DauPtMin", "DauPMin", "DauMIPChi2Min",
                              "PtMin", "CombDMass", "DMass", "VtxChi2Max", "VDChi2Min",
                              "DIRAMin", "DOCAMax", "IPChi2Min", "MaxDauMIPChi2")

    checkConfig(__configuration_keys__, config)

    StdPi = StdNoPIDsPions
    StdK = StdNoPIDsKaons

    Daughtercut = "((TRCHI2DOF<%(DauChi2Max)s) & " \
    "(PT > %(DauPtMin)s*MeV) & (P > %(DauPMin)s*MeV) & " \
    "(MIPCHI2DV(PRIMARY) > %(DauMIPChi2Min)s))" % config

    D2pipi = CombineParticles()
    D2pipi.DecayDescriptors = [ "[D0 -> pi- pi-]cc" ]
    D2pipi.DaughtersCuts =  { "pi+"      : Daughtercut,
                              "K+"       : Daughtercut }

    D2pipi.CombinationCut = \
    "(ADAMASS('D0') < %(CombDMass)s*MeV) & " \
    "(APT>%(PtMin)s*MeV) & " \
    "(AMAXDOCA('LoKi::TrgDistanceCalculator') < %(DOCAMax)s) & " \
    "AHASCHILD(((ABSID == 'K+') | (ABSID == 'pi+')) & (MIPCHI2DV(PRIMARY) > %(MaxDauMIPChi2)s))" \
    % config

    D2pipi.MotherCut = "(ADMASS('D0') < %(DMass)s*MeV) " \
    "& (VFASPF(VCHI2/VDOF)<%(VtxChi2Max)s) & (BPVVDCHI2 > %(VDChi2Min)s) & (BPVDIRA > %(DIRAMin)s) & (BPVIPCHI2() > %(IPChi2Min)s)" % config

    D2pipiSel = Selection("D2pipiWSFor" + moduleName, Algorithm = D2pipi, RequiredSelections = [ StdPi ] )

    D2Kpi = copy(D2pipi)
    D2Kpi.DecayDescriptors = [ "[D0 -> K- pi-]cc" ]
    D2KpiSel = Selection("D2KpiWSFor" + moduleName, Algorithm = D2Kpi, RequiredSelections = [ StdPi, StdK ] )

    D2KK = copy(D2pipi)
    D2KK.DecayDescriptors = [ "[D0 -> K- K-]cc" ]
    D2KKSel = Selection("D2KKWSFor" + moduleName, Algorithm = D2KK, RequiredSelections = [ StdK ] )

    DSel = MergedSelection("D2hhWSFor" + moduleName,
                           RequiredSelections = [ D2pipiSel, D2KpiSel, D2KKSel ] )

    return DSel


def makeD2hhh(moduleName, config) :

    __configuration_keys__ = ("DauChi2Max", "DauPtMin", "DauPMin", "DauMIPChi2Min",
                              "PtMin", "CombDMass", "DMass", "VtxChi2Max", "VDChi2Min",
                              "MaxDauMIPChi2", "DIRAMin", "DOCAMax", "IPChi2Min")

    checkConfig(__configuration_keys__, config)

    StdPi = StdNoPIDsPions
    StdK = StdNoPIDsKaons

    Daughtercut = "((TRCHI2DOF<%(DauChi2Max)s) & " \
    "(PT > %(DauPtMin)s*MeV) & (P > %(DauPMin)s*MeV) & " \
    "(MIPCHI2DV(PRIMARY) > %(DauMIPChi2Min)s))" % config

    D2pipipi = CombineParticles()
    D2pipipi.DecayDescriptors = [ "[D+ -> pi- pi+ pi+]cc" ]
    D2pipipi.DaughtersCuts =  { "pi+"     : Daughtercut,
                                "K+"      : Daughtercut }

    D2pipipi.CombinationCut = \
    "((ADAMASS('D+') < %(CombDMass)s*MeV) | (ADAMASS('D_s+') < %(CombDMass)s*MeV)) & " \
    "(APT>%(PtMin)s*MeV) & " \
    "(AMAXDOCA('LoKi::TrgDistanceCalculator') < %(DOCAMax)s) & " \
    "AHASCHILD(((ABSID == 'K+') | (ABSID == 'pi+')) & (MIPCHI2DV(PRIMARY) > %(MaxDauMIPChi2)s))" \
    % config

    D2pipipi.MotherCut = "((ADMASS('D+') < %(DMass)s*MeV) | (ADMASS('D_s+') < %(DMass)s*MeV))" \
    "& (VFASPF(VCHI2/VDOF)<%(VtxChi2Max)s) & (BPVVDCHI2 > %(VDChi2Min)s) & (BPVDIRA > %(DIRAMin)s) & (BPVIPCHI2() > %(IPChi2Min)s)" % config

    D2pipipiSel = Selection("D2pipipiFor" + moduleName, Algorithm = D2pipipi, RequiredSelections = [ StdPi ] )

    D2Kpipi = copy(D2pipipi)
    D2Kpipi.DecayDescriptors = [ "[D+ -> K- pi+ pi+]cc" ]
    D2KpipiSel = Selection("D2KpipiFor" + moduleName, Algorithm = D2Kpipi, RequiredSelections = [ StdPi, StdK ] )

    D2KKpi = copy(D2pipipi)
    D2KKpi.DecayDescriptors = [ "[D+ -> K- K+ pi+]cc" ]
    D2KKpiSel = Selection("D2KKpiFor" + moduleName, Algorithm = D2KKpi, RequiredSelections = [ StdPi, StdK ] )

    D2piKK = copy(D2pipipi)
    D2piKK.DecayDescriptors = [ "[D+ -> pi- K+ K+]cc" ]
    D2piKKSel = Selection("D2piKKFor" + moduleName, Algorithm = D2piKK, RequiredSelections = [ StdPi, StdK ] )

    D2pipiK = copy(D2pipipi)
    D2pipiK.DecayDescriptors = [ "[D+ -> pi- pi+ K+]cc" ]
    D2pipiKSel = Selection("D2pipiKFor" + moduleName, Algorithm = D2pipiK, RequiredSelections = [ StdPi, StdK ] )

    DSel = MergedSelection("D2hhhFor" + moduleName,
                           RequiredSelections = [ D2pipipiSel, D2KpipiSel, D2KKpiSel, D2piKKSel, D2pipiKSel ] )

    return DSel


def makeD2hhhWS(moduleName, config) :

    __configuration_keys__ = ("DauChi2Max", "DauPtMin", "DauPMin", "DauMIPChi2Min",
                              "PtMin", "CombDMass", "DMass", "VtxChi2Max", "VDChi2Min",
                              "MaxDauMIPChi2", "DIRAMin", "DOCAMax", "IPChi2Min")

    checkConfig(__configuration_keys__, config)

    StdPi = StdNoPIDsPions
    StdK = StdNoPIDsKaons

    Daughtercut = "((TRCHI2DOF<%(DauChi2Max)s) & " \
    "(PT > %(DauPtMin)s*MeV) & (P > %(DauPMin)s*MeV) & " \
    "(MIPCHI2DV(PRIMARY) > %(DauMIPChi2Min)s))" % config

    D2pipipi = CombineParticles()
    D2pipipi.DecayDescriptors = [ "[D+ -> pi+ pi+ pi+]cc" ]
    D2pipipi.DaughtersCuts =  { "pi+"     : Daughtercut,
                                "K+"      : Daughtercut }

    D2pipipi.CombinationCut = \
    "((ADAMASS('D+') < %(CombDMass)s*MeV) | (ADAMASS('D_s+') < %(CombDMass)s*MeV)) & " \
    "(APT>%(PtMin)s*MeV) & " \
    "(AMAXDOCA('LoKi::TrgDistanceCalculator') < %(DOCAMax)s) & " \
    "AHASCHILD(((ABSID == 'K+') | (ABSID == 'pi+')) & (MIPCHI2DV(PRIMARY) > %(MaxDauMIPChi2)s))" \
    % config

    D2pipipi.MotherCut = "((ADMASS('D+') < %(DMass)s*MeV) | (ADMASS('D_s+') < %(DMass)s*MeV))" \
    "& (VFASPF(VCHI2/VDOF)<%(VtxChi2Max)s) & (BPVVDCHI2 > %(VDChi2Min)s) & (BPVDIRA > %(DIRAMin)s) & (BPVIPCHI2() > %(IPChi2Min)s)" % config

    D2pipipiSel = Selection("D2pipipiWSFor" + moduleName, Algorithm = D2pipipi, RequiredSelections = [ StdPi ] )

    D2Kpipi = copy(D2pipipi)
    D2Kpipi.DecayDescriptors = [ "[D+ -> K+ pi+ pi+]cc" ]
    D2KpipiSel = Selection("D2KpipiWSFor" + moduleName, Algorithm = D2Kpipi, RequiredSelections = [ StdPi, StdK ] )

    D2KKpi = copy(D2pipipi)
    D2KKpi.DecayDescriptors = [ "[D+ -> K+ K+ pi+]cc" ]
    D2KKpiSel = Selection("D2KKpiWSFor" + moduleName, Algorithm = D2KKpi, RequiredSelections = [ StdPi, StdK ] )

    DSel = MergedSelection("D2hhhWSFor" + moduleName,
                           RequiredSelections = [ D2pipipiSel, D2KpipiSel, D2KKpiSel ] )

    return DSel

def makeKs(moduleName, config) : 

    __configuration_keys__ = ("DauChi2Max", "DauPtMin", "DauPMin", "DauMIPChi2Min",
                              "KsCombDMass", "KsDMass", "KsVtxChi2Max", "KsFlightChi2")

    checkConfig(__configuration_keys__, config)

    StdPi          = StdNoPIDsPions
    StdDownPi      = StdNoPIDsDownPions
    
    Daughtercut = "((TRCHI2DOF<%(DauChi2Max)s) & " \
    "(PT > %(DauPtMin)s*MeV) & (P > %(DauPMin)s*MeV) & " \
    "(MIPCHI2DV(PRIMARY) > %(DauMIPChi2Min)s))" % config

    KsLL = CombineParticles()
    KsLL.DecayDescriptor = "KS0 -> pi+ pi-"
    KsLL.DaughtersCuts = { "pi+" : Daughtercut }
    KsLL.CombinationCut = "(ADAMASS('KS0')<%(KsCombDMass)s*MeV)" % config
    KsLL.MotherCut = "(ADMASS('KS0')<%(KsDMass)s*MeV) & (VFASPF(VCHI2/VDOF)<%(KsVtxChi2Max)s) & (BPVVDCHI2 > %(KsFlightChi2)s)" % config

    KsDD = copy(KsLL)
    KsDD.MotherCut = "(ADMASS('KS0')<%(KsDMass)s*MeV) & (VFASPF(VCHI2/VDOF)<%(KsVtxChi2Max)s)" % config

    KsDDSel = Selection("KsDDFor" + moduleName, Algorithm = KsDD, RequiredSelections = [ StdDownPi ] )
    KsLLSel = Selection("KsLLFor" + moduleName, Algorithm = KsLL, RequiredSelections = [ StdPi ] )
    return MergedSelection("KsFor" + moduleName, RequiredSelections = [ KsDDSel, KsLLSel ] )

def makeD2Kshh(moduleName, KsSel, config) :

    __configuration_keys__ = ("DauChi2Max", "DauPtMin", "DauPMin", "DauMIPChi2Min",
                              "PtMin", "CombDMass", "DMass", "VtxChi2Max", "VDChi2Min",
                              "PMin", "DIRAMin", "DOCAMax", "IPChi2Min")

    checkConfig(__configuration_keys__, config)

    StdPi          = StdNoPIDsPions
    StdK           = StdNoPIDsKaons

    Daughtercut = "((TRCHI2DOF<%(DauChi2Max)s) & " \
    "(PT > %(DauPtMin)s*MeV) & (P > %(DauPMin)s*MeV) & " \
    "(MIPCHI2DV(PRIMARY) > %(DauMIPChi2Min)s))" % config

    D2Kspipi = CombineParticles()
    D2Kspipi.DecayDescriptors = ["D0 -> KS0 pi+ pi-" ]
    D2Kspipi.DaughtersCuts = { "pi+" : Daughtercut,
                               "K+"  : Daughtercut,
                               "KS0" : "ALL" }

    D2Kspipi.CombinationCut = \
    "(ADAMASS('D0') < %(CombDMass)s*MeV) & " \
    "(APT>%(PtMin)s*MeV) & " \
    "(ADOCA(2,3) < %(DOCAMax)s)" \
    % config

    D2Kspipi.MotherCut = "(ADMASS('D0') < %(DMass)s*MeV) " \
    "& (VFASPF(VCHI2/VDOF)<%(VtxChi2Max)s) & (BPVVDCHI2 > %(VDChi2Min)s) & (BPVDIRA > %(DIRAMin)s) & (BPVIPCHI2() > %(IPChi2Min)s)" % config

    D2KspipiSel = Selection("D2KspipiFor" + moduleName, Algorithm = D2Kspipi, RequiredSelections = [ KsSel, StdPi ] )

    D2KsKK = copy(D2Kspipi)
    D2KsKK.DecayDescriptors = [ "D0 -> KS0 K+ K-" ]
    D2KsKKSel = Selection("D2KsKKFor" + moduleName, Algorithm = D2KsKK, RequiredSelections = [ KsSel, StdK ] )

    D2KsKPi = copy(D2Kspipi)
    D2KsKPi.DecayDescriptors = [ "[D0 -> KS0 K+ pi-]cc" ]
    D2KsKPiSel = Selection("D2KsKPiFor" + moduleName, Algorithm = D2KsKPi, RequiredSelections = [ KsSel, StdK, StdPi ] )

    DSel = MergedSelection("D2KshhFor" + moduleName,
                           RequiredSelections = [ D2KspipiSel, D2KsKKSel, D2KsKPiSel ] )

    return DSel


def makeD2KshhWS(moduleName, KsSel, config) :

    __configuration_keys__ = ("DauChi2Max", "DauPtMin", "DauPMin", "DauMIPChi2Min",
                              "PtMin", "CombDMass", "DMass", "VtxChi2Max", "VDChi2Min",
                              "PMin", "DIRAMin", "DOCAMax", "IPChi2Min")

    checkConfig(__configuration_keys__, config)

    StdPi          = StdNoPIDsPions
    StdK           = StdNoPIDsKaons

    Daughtercut = "((TRCHI2DOF<%(DauChi2Max)s) & " \
    "(PT > %(DauPtMin)s*MeV) & (P > %(DauPMin)s*MeV) & " \
    "(MIPCHI2DV(PRIMARY) > %(DauMIPChi2Min)s))" % config

    D2Kspipi = CombineParticles()
    D2Kspipi.DecayDescriptors = ["[D0 -> KS0 pi+ pi+]cc" ]
    D2Kspipi.DaughtersCuts = { "pi+" : Daughtercut,
                               "K+"  : Daughtercut,
                               "KS0" : "ALL" }

    D2Kspipi.CombinationCut = \
    "(ADAMASS('D0') < %(CombDMass)s*MeV) & (ADOCA(2,3) < %(DOCAMax)s) & " \
    "(APT>%(PtMin)s*MeV)" \
    % config

    D2Kspipi.MotherCut = "(ADMASS('D0') < %(DMass)s*MeV) " \
    "& (VFASPF(VCHI2/VDOF)<%(VtxChi2Max)s) & (BPVVDCHI2 > %(VDChi2Min)s) & (BPVDIRA > %(DIRAMin)s) & (BPVIPCHI2() > %(IPChi2Min)s)" % config

    D2KspipiSel = Selection("D2KspipiWSFor" + moduleName, Algorithm = D2Kspipi, RequiredSelections = [ KsSel, StdPi ] )

    D2KsKK = copy(D2Kspipi)
    D2KsKK.DecayDescriptors = [ "[D0 -> KS0 K+ K+]cc" ]
    D2KsKKSel = Selection("D2KsKKWSFor" + moduleName, Algorithm = D2KsKK, RequiredSelections = [ KsSel, StdK ] )

    D2KsKPi = copy(D2Kspipi)
    D2KsKPi.DecayDescriptors = [ "[D0 -> KS0 K+ pi+]cc" ]
    D2KsKPiSel = Selection("D2KsKPiWSFor" + moduleName, Algorithm = D2KsKPi, RequiredSelections = [ KsSel, StdK, StdPi ] )

    DSel = MergedSelection("D2KshhWSFor" + moduleName,
                           RequiredSelections = [ D2KspipiSel, D2KsKKSel, D2KsKPiSel ] )

    return DSel

def makeD2Ksh(moduleName, KsSel, config) :

    __configuration_keys__ = ("DauChi2Max", "DauPtMin", "DauPMin", "DauMIPChi2Min",
                              "PtMin", "CombDMass", "DMass", "VtxChi2Max", "VDChi2Min",
                              "DIRAMin", "IPChi2Min")

    checkConfig(__configuration_keys__, config)

    StdPi          = StdNoPIDsPions
    StdK           = StdNoPIDsKaons

    Daughtercut = "((TRCHI2DOF<%(DauChi2Max)s) & " \
    "(PT > %(DauPtMin)s*MeV) & (P > %(DauPMin)s*MeV) & " \
    "(MIPCHI2DV(PRIMARY) > %(DauMIPChi2Min)s))" % config

    D2Kspi = CombineParticles()
    D2Kspi.DecayDescriptors = ["[D+ -> KS0 pi+]cc" ]
    D2Kspi.DaughtersCuts = { "pi+" : Daughtercut,
                             "K+"  : Daughtercut,
                             "KS0" : "ALL" }

    D2Kspi.CombinationCut = \
    "((ADAMASS('D+') < %(CombDMass)s*MeV) | (ADAMASS('D_s+') < %(CombDMass)s*MeV)) & " \
    "(APT>%(PtMin)s*MeV)" \
    % config

    D2Kspi.MotherCut = "((ADMASS('D+') < %(DMass)s*MeV) | (ADMASS('D_s+') < %(DMass)s*MeV))" \
    "& (VFASPF(VCHI2/VDOF)<%(VtxChi2Max)s) & (BPVVDCHI2 > %(VDChi2Min)s) & (BPVDIRA > %(DIRAMin)s) & (BPVIPCHI2() > %(IPChi2Min)s)" % config

    D2KspiSel = Selection("D2KspiFor" + moduleName, Algorithm = D2Kspi, RequiredSelections = [ KsSel, StdPi ] )

    D2KsK = copy(D2Kspi)
    D2KsK.DecayDescriptors = [ "[D+ -> KS0 K+]cc" ]
    D2KsKSel = Selection("D2KsKFor" + moduleName, Algorithm = D2KsK, RequiredSelections = [ KsSel, StdK ] )

    DSel = MergedSelection("D2KshFor" + moduleName,
                           RequiredSelections = [ D2KspiSel, D2KsKSel ] )

    return DSel


def makeD2KPiPi0(moduleName, pi0name, config, Pi0Sel) :
    """
    Create and return a D0->KPiPi0 Selection object for merged pi0s.
    Starts from DataOnDemand 'Phys/StdNoPiDsKaons', 'Phys/StdNoPIDsPions' and 'Phys/StdLooseMergedPi0'.
    Arguments in the "config" dictionary:

    DauChi2Max            : Maximum chi2 for D0 daughter track.
    DauMIPChi2Min         : Minimum impact parameter (wrt PV) chi2 for D0 daughter track.
    DauPtMin              : Minimum transverse momentum for D0 daughter track (MeV).
    DauPMin               : Minimum momentum for D0 daughter track (MeV).
    Pi0PtMin              : Minimum transverse momentum for Pi0 daughter of D0 (MeV).
    Pi0PMin               : Minimum momentum for Pi0 daughter of D0 (MeV).
    PtMin                 : Minimum transverse momentum for D0 (MeV).
    VtxChi2Max            : Maximum chi2 for D0 vertex fit.
    VDChi2Min             : Minimum chi2 for decay vertex distance (wrt PV) of D0.
    CombMassMin           : Minimum mass for the D0 combination of daughters (MeV).
    CombMassMax           : Maximum mass for the D0 combination of daughters (MeV).
    MassMin               : Minimum mass for the D0 (MeV).
    MassMax               : Maximum mass for the D0 (MeV).
    """

    __configuration_keys__ = ("DauChi2Max", "DauPtMin", "DauPMin", "DauMIPChi2Min",
                              "PtMin", "CombMassMin", "CombMassMax", "MassMin", "MassMax",
                              "VtxChi2Max", "VDChi2Min", "Pi0PtMin", "Pi0PMin", "PMin",
                              "DIRAMin", "DOCAMax", "MaxDauMIPChi2", "PhotonCL")

    checkConfig(__configuration_keys__, config)

    _Kaon = StdNoPIDsKaons
    _Pion = StdNoPIDsPions
    _Pi0  = Pi0Sel

    _DDauKineCut = "(PT> %(DauPtMin)s *MeV) & (P> %(DauPMin)s *MeV)" % config
    _DDauChi2Cut = "(MIPCHI2DV(PRIMARY)> %(DauMIPChi2Min)s ) & (TRCHI2DOF< %(DauChi2Max)s )" % config
    _DDauCut = "( " + _DDauKineCut + " & " + _DDauChi2Cut + " )"
    
    if _Pi0 == StdLooseResolvedPi0 : 
	_Pi0Cut = "(PT> %(Pi0PtMin)s *MeV) & (P> %(Pi0PMin)s *MeV) & (CHILD(CL,1)> %(PhotonCL)s) & (CHILD(CL,2)> %(PhotonCL)s)" % config
    else : 
	_Pi0Cut = "(PT> %(Pi0PtMin)s *MeV) & (P> %(Pi0PMin)s *MeV)" % config

    _DCombCut = \
    "(AM> %(CombMassMin)s *MeV) & (AM< %(CombMassMax)s *MeV) & " \
    "(ADOCA(1,2) < %(DOCAMax)s) & " \
    "AHASCHILD(((ABSID == 'K+') | (ABSID == 'pi+')) & (MIPCHI2DV(PRIMARY) > %(MaxDauMIPChi2)s)) &" \
    "(APT> %(PtMin)s *MeV)" \
    % config

    _DCut = "(M> %(MassMin)s *MeV) & (M< %(MassMax)s *MeV) & (P>%(PMin)s) & " \
    "(PT> %(PtMin)s *MeV) & (VFASPF(VCHI2/VDOF)< %(VtxChi2Max)s ) & " \
    "(BPVVDCHI2> %(VDChi2Min)s ) & (BPVDIRA > %(DIRAMin)s)" % config

    _D0 = CombineParticles()
    _D0.DecayDescriptors = ["[D0 -> K- pi+ pi0]cc" ]
    _D0.DaughtersCuts = { "K-" : _DDauCut, "pi+": _DDauCut, "pi0": _Pi0Cut }
    _D0.CombinationCut = _DCombCut
    _D0.MotherCut = _DCut

    return Selection("D2KPiPi0" + pi0name + "For" + moduleName
                    ,Algorithm = _D0
                    ,RequiredSelections = [ _Kaon, _Pion, _Pi0 ]
                    )

def makeD2KPiPi0WS(moduleName, pi0name, config, Pi0Sel) :

    __configuration_keys__ = ("DauChi2Max", "DauPtMin", "DauPMin", "DauMIPChi2Min",
                              "PtMin", "CombMassMin", "CombMassMax", "MassMin", "MassMax",
                              "VtxChi2Max", "VDChi2Min", "Pi0PtMin", "Pi0PMin", "PMin",
                              "DIRAMin", "DOCAMax", "MaxDauMIPChi2", "PhotonCL")

    checkConfig(__configuration_keys__, config)

    _Kaon = StdNoPIDsKaons
    _Pion = StdNoPIDsPions
    _Pi0  = Pi0Sel

    _DDauKineCut = "(PT> %(DauPtMin)s *MeV) & (P> %(DauPMin)s *MeV)" % config
    _DDauChi2Cut = "(MIPCHI2DV(PRIMARY)> %(DauMIPChi2Min)s ) & (TRCHI2DOF< %(DauChi2Max)s )" % config
    _DDauCut = "( " + _DDauKineCut + " & " + _DDauChi2Cut + " )"

    if _Pi0 == StdLooseResolvedPi0 : 
	_Pi0Cut = "(PT> %(Pi0PtMin)s *MeV) & (P> %(Pi0PMin)s *MeV) & (CHILD(CL,1)> %(PhotonCL)s) & (CHILD(CL,2)> %(PhotonCL)s)" % config
    else : 
	_Pi0Cut = "(PT> %(Pi0PtMin)s *MeV) & (P> %(Pi0PMin)s *MeV)" % config

    _DCombCut = \
    "(AM> %(CombMassMin)s *MeV) & (AM< %(CombMassMax)s *MeV) & " \
    "(ADOCA(1,2) < %(DOCAMax)s) & " \
    "AHASCHILD(((ABSID == 'K+') | (ABSID == 'pi+')) & (MIPCHI2DV(PRIMARY) > %(MaxDauMIPChi2)s)) &" \
    "(APT> %(PtMin)s *MeV)" \
    % config

    _DCut = \
    "(M> %(MassMin)s *MeV) & (M< %(MassMax)s *MeV) & (P>%(PMin)s) & " \
    "(PT> %(PtMin)s *MeV) & (VFASPF(VCHI2/VDOF)< %(VtxChi2Max)s ) & " \
    "(BPVVDCHI2> %(VDChi2Min)s ) & (BPVDIRA > %(DIRAMin)s)" \
    % config

    _D0 = CombineParticles()
    _D0.DecayDescriptors = ["[D0 -> K+ pi+ pi0]cc" ]
    _D0.DaughtersCuts = { "K+" : _DDauCut, "pi+": _DDauCut, "pi0": _Pi0Cut }
    _D0.CombinationCut = _DCombCut
    _D0.MotherCut = _DCut

    return Selection("D2KPiPi0WS" + pi0name + "For" + moduleName
                    ,Algorithm = _D0
                    ,RequiredSelections = [ _Kaon, _Pion, _Pi0 ]
                    )


def makeD2hhhh(moduleName, config) :

    __configuration_keys__ = ("DauChi2Max", "PionPtMin", "PionPMin", "KaonPtMin", "KaonPMin", "DauMIPChi2Min",
                              "PtMin", "CombMassMin", "CombMassMax", "MassMin", "MassMax",
                              "VtxChi2Max", "VDChi2Min", "PMin", "MaxDauMIPChi2",
                              "DIRAMin", "DOCAMax")

    checkConfig(__configuration_keys__, config)

    StdPi = StdAllNoPIDsPions  # Because of low Pt cut
    StdK  = StdNoPIDsKaons

    DaughtercutPion = "((TRCHI2DOF<%(DauChi2Max)s) & " \
    "(PT > %(PionPtMin)s*MeV) & (P > %(PionPMin)s*MeV) & " \
    "(MIPCHI2DV(PRIMARY) > %(DauMIPChi2Min)s))" % config

    DaughtercutKaon = "((TRCHI2DOF<%(DauChi2Max)s) & " \
    "(PT > %(KaonPtMin)s*MeV) & (P > %(KaonPMin)s*MeV) & " \
    "(MIPCHI2DV(PRIMARY) > %(DauMIPChi2Min)s))" % config

    D2Kpipipi = CombineParticles()
    D2Kpipipi.DecayDescriptors = [ "[D0 -> K- pi+ pi- pi+]cc" ]
    D2Kpipipi.DaughtersCuts =  { "pi+"     : DaughtercutPion, "K+" : DaughtercutKaon }

    D2Kpipipi.CombinationCut = \
    "(in_range(%(CombMassMin)s*MeV, AM, %(CombMassMax)s*MeV)) & " \
    "(APT>%(PtMin)s*MeV) & " \
    "(AMAXDOCA('LoKi::TrgDistanceCalculator') < %(DOCAMax)s) & " \
    "AHASCHILD(((ABSID == 'K+') | (ABSID == 'pi+')) & (MIPCHI2DV(PRIMARY) > %(MaxDauMIPChi2)s))" \
    % config

    D2Kpipipi.MotherCut = "(in_range(%(MassMin)s*MeV, M, %(MassMax)s*MeV)) & (P>%(PMin)s) " \
    "& (VFASPF(VCHI2/VDOF)<%(VtxChi2Max)s) & (BPVVDCHI2 > %(VDChi2Min)s) & (BPVDIRA > %(DIRAMin)s)" % config

    D2KpipipiSel = Selection("D2KpipipiFor" + moduleName, Algorithm = D2Kpipipi, RequiredSelections = [ StdPi, StdK ] )

    D2KKpipi = copy(D2Kpipipi)
    D2KKpipi.DecayDescriptors = [ "D0 -> K- K+ pi- pi+" ]
    D2KKpipiSel = Selection("D2KKpipiFor" + moduleName, Algorithm = D2KKpipi, RequiredSelections = [ StdPi, StdK ] )

    D2pipipipi = copy(D2Kpipipi)
    D2pipipipi.DecayDescriptors = [ "D0 -> pi- pi+ pi- pi+" ]
    D2pipipipiSel = Selection("D2pipipipiFor" + moduleName, Algorithm = D2pipipipi, RequiredSelections = [ StdPi ] )

    DSel = MergedSelection("D2hhhhFor" + moduleName,
                           RequiredSelections = [ D2KpipipiSel, D2KKpipiSel, D2pipipipiSel ] )

    return DSel

def makeD2hhhhWS(moduleName, config) :

    __configuration_keys__ = ("DauChi2Max", "PionPtMin", "PionPMin", "KaonPtMin", "KaonPMin", "DauMIPChi2Min",
                              "PtMin", "CombMassMin", "CombMassMax", "MassMin", "MassMax",
                              "VtxChi2Max", "VDChi2Min", "PMin", "MaxDauMIPChi2",
                              "DIRAMin", "DOCAMax")

    checkConfig(__configuration_keys__, config)

    StdPi = StdAllNoPIDsPions  # Because of low Pt cut
    StdK  = StdNoPIDsKaons

    DaughtercutPion = "((TRCHI2DOF<%(DauChi2Max)s) & " \
    "(PT > %(PionPtMin)s*MeV) & (P > %(PionPMin)s*MeV) & " \
    "(MIPCHI2DV(PRIMARY) > %(DauMIPChi2Min)s))" % config

    DaughtercutKaon = "((TRCHI2DOF<%(DauChi2Max)s) & " \
    "(PT > %(KaonPtMin)s*MeV) & (P > %(KaonPMin)s*MeV) & " \
    "(MIPCHI2DV(PRIMARY) > %(DauMIPChi2Min)s))" % config

    D2Kpipipi = CombineParticles()
    D2Kpipipi.DecayDescriptors = [ "[D0 -> K+ pi+ pi- pi+]cc" ]
    D2Kpipipi.DaughtersCuts =  { "pi+"     : DaughtercutPion, "K+" : DaughtercutKaon }

    D2Kpipipi.CombinationCut = \
    "(in_range(%(CombMassMin)s*MeV, AM, %(CombMassMax)s*MeV)) & " \
    "(APT>%(PtMin)s*MeV) & " \
    "(AMAXDOCA('LoKi::TrgDistanceCalculator') < %(DOCAMax)s) & " \
    "AHASCHILD(((ABSID == 'K+') | (ABSID == 'pi+')) & (MIPCHI2DV(PRIMARY) > %(MaxDauMIPChi2)s))" \
    % config

    D2Kpipipi.MotherCut = "(in_range(%(MassMin)s*MeV, M, %(MassMax)s*MeV)) & (P>%(PMin)s) " \
    "& (VFASPF(VCHI2/VDOF)<%(VtxChi2Max)s) & (BPVVDCHI2 > %(VDChi2Min)s) & (BPVDIRA > %(DIRAMin)s)" % config

    D2KpipipiSel = Selection("D2KpipipiWSFor" + moduleName, Algorithm = D2Kpipipi, RequiredSelections = [ StdPi, StdK ] )

    DSel = MergedSelection("D2hhhhWSFor" + moduleName,
                           RequiredSelections = [ D2KpipipiSel ] )

    return DSel



def makeLambdaC(moduleName, config) :

    __configuration_keys__ = ("DauChi2Max", "DauPtMin", "DauPMin", "DauMIPChi2Min",
                              "PtMin", "CombMassMin", "CombMassMax", "MassMin", "MassMax",
                              "VtxChi2Max", "VDChi2Min", "DIRAMin", "DOCAMax", "IPChi2Min")

    checkConfig(__configuration_keys__, config)

    StdPi = StdNoPIDsPions
    StdK  = StdNoPIDsKaons
    Stdp  = StdNoPIDsProtons

    Lccut = "(VFASPF(VCHI2/VDOF)<%(VtxChi2Max)s) & (BPVVDCHI2 > %(VDChi2Min)s) & " \
    "(in_range(%(MassMin)s*MeV, M, %(MassMax)s*MeV)) & (BPVDIRA > %(DIRAMin)s) & (BPVIPCHI2() > %(IPChi2Min)s)" % config

    Lccombcut = \
    "(in_range(%(CombMassMin)s*MeV, AM, %(CombMassMax)s*MeV)) & " \
    "(APT>%(PtMin)s*MeV) &" \
    "(AMAXDOCA('LoKi::TrgDistanceCalculator') < %(DOCAMax)s)" \
    % config

    Daughtercut = "((TRCHI2DOF<%(DauChi2Max)s) & " \
    "(PT > %(DauPtMin)s*MeV) & (P > %(DauPMin)s*MeV) & " \
    "(MIPCHI2DV(PRIMARY) > %(DauMIPChi2Min)s))" % config

    Lc2pKPi = CombineParticles()
    Lc2pKPi.DecayDescriptors = [ "[Lambda_c+ -> p+ K- pi+]cc" ]
    Lc2pKPi.DaughtersCuts =  { "p+"        : Daughtercut, "K+"        : Daughtercut, "pi+"        : Daughtercut }
    Lc2pKPi.CombinationCut = Lccombcut
    Lc2pKPi.MotherCut = Lccut
    return Selection("LambdaC2pKPiFor" + moduleName, Algorithm = Lc2pKPi, RequiredSelections = [ Stdp, StdK, StdPi ] )

def makeKstarNoMW(moduleName, config) :

    __configuration_keys__ = ("DauChi2Max", "DauPtMin", "DauPMin", "DauMIPChi2Min",
                              "PtMin", "DMass", "VtxChi2Max", "MIPChi2DVMin")

    checkConfig(__configuration_keys__, config)

    StdPi = StdNoPIDsPions
    StdK  = StdNoPIDsKaons

    Daughtercut = "((TRCHI2DOF<%(DauChi2Max)s) & " \
    "(PT > %(DauPtMin)s*MeV) & " \
    "(P > %(DauPMin)s*MeV) & " \
    "(MIPCHI2DV(PRIMARY) > %(DauMIPChi2Min)s))" % config

    Kstar = CombineParticles()
    Kstar.DecayDescriptors = [ "[K*(892)0 -> K+ pi-]cc" ]
    Kstar.DaughtersCuts =  { "K+"        : Daughtercut,
                             "pi+"       : Daughtercut }

    Kstar.CombinationCut = "(APT>%(PtMin)s*MeV)" % config
    Kstar.MotherCut = "((VFASPF(VCHI2/VDOF) < %(VtxChi2Max)s) & " \
    "(MIPCHI2DV(PRIMARY) > %(MIPChi2DVMin)s))" % config

    return Selection("KstarNoMWFor" + moduleName,
                     Algorithm = Kstar, RequiredSelections = [ StdPi, StdK ] )


def makeKstar(moduleName, KstarNoMWSel, config) :

    Kstar = FilterDesktop()
    Kstar.Code = "(ADMASS('K*(892)0') < %(DMass)s *MeV)" % config
    return Selection("KstarFor" + moduleName,
                     Algorithm = Kstar, RequiredSelections = [ KstarNoMWSel ] )

def makePhiNoMW(moduleName, config) :

    __configuration_keys__ = ("DauChi2Max", "DauPtMin", "DauPMin", "DauMIPChi2Min",
                              "PtMin", "DMass", "VtxChi2Max", "MIPChi2DVMin")

    checkConfig(__configuration_keys__, config)

    StdK  = StdNoPIDsKaons

    Daughtercut = "((TRCHI2DOF<%(DauChi2Max)s) & " \
    "(PT > %(DauPtMin)s*MeV) & (P > %(DauPMin)s*MeV) & " \
    "(MIPCHI2DV(PRIMARY) > %(DauMIPChi2Min)s))" % config

    Phi = CombineParticles()
    Phi.DecayDescriptors = [ "phi(1020) -> K+ K-" ]
    Phi.DaughtersCuts =  { "K+"        : Daughtercut }

    Phi.CombinationCut = "(APT>%(PtMin)s*MeV)" % config
    Phi.MotherCut = "((VFASPF(VCHI2/VDOF) < %(VtxChi2Max)s) & " \
    "(MIPCHI2DV(PRIMARY) > %(MIPChi2DVMin)s))" % config

    return Selection("PhiNoMWFor" + moduleName,
                     Algorithm = Phi, RequiredSelections = [ StdK ] )

def makePhi(moduleName, PhiNoMWSel, config) :

    Phi = FilterDesktop()
    Phi.Code = "(ADMASS('phi(1020)') < %(DMass)s *MeV)" % config
    return Selection("PhiFor" + moduleName,
                     Algorithm = Phi, RequiredSelections = [ PhiNoMWSel ] )

def makeRhoNoMW(moduleName, config) :

    __configuration_keys__ = ("DauChi2Max", "DauPtMin", "DauPMin", "DauMIPChi2Min",
                              "PtMin", "DMass", "VtxChi2Max", "MIPChi2DVMin")

    checkConfig(__configuration_keys__, config)

    StdPi  = StdNoPIDsPions

    Daughtercut = "((TRCHI2DOF<%(DauChi2Max)s) & " \
    "(PT > %(DauPtMin)s*MeV) & (P > %(DauPMin)s*MeV) & " \
    "(MIPCHI2DV(PRIMARY) > %(DauMIPChi2Min)s))" % config

    Rho = CombineParticles()
    Rho.DecayDescriptors = [ "rho(770)0 -> pi+ pi-" ]
    Rho.DaughtersCuts =  { "pi+"        : Daughtercut }

    Rho.CombinationCut = "(APT>%(PtMin)s*MeV)" % config
    Rho.MotherCut = "((VFASPF(VCHI2/VDOF) < %(VtxChi2Max)s) & " \
    "(MIPCHI2DV(PRIMARY) > %(MIPChi2DVMin)s))" % config

    return Selection("RhoNoMWFor" + moduleName,
                     Algorithm = Rho, RequiredSelections = [ StdPi ] )

def makeRho(moduleName, RhoNoMWSel, config) :

    Rho = FilterDesktop()
    Rho.Code = "(ADMASS('rho(770)0') < %(DMass)s *MeV)" % config
    return Selection("RhoFor" + moduleName,
                     Algorithm = Rho, RequiredSelections = [ RhoNoMWSel ] )

def makePi0(moduleName) : 
    StdPi0Merged = StdLooseMergedPi0
    StdPi0Resolved = StdLooseResolvedPi0

    return MergedSelection("Pi0For" + moduleName, RequiredSelections = [ StdPi0Merged, StdPi0Resolved ] )


def makeRhoCharged(moduleName, Pi0Sel, config) :

    __configuration_keys__ = ("DauChi2Max", "DauPtMin", "DauPMin", "DauMIPChi2Min",
                              "Pi0PtMin", "Pi0PMin", "PtMin", "DMass")

    checkConfig(__configuration_keys__, config)

    from Configurables import CombineParticles

    StdPi  = StdNoPIDsPions

    Daughtercut = "((TRCHI2DOF<%(DauChi2Max)s) & " \
    "(PT > %(DauPtMin)s*MeV) & (P > %(DauPMin)s*MeV) & " \
    "(MIPCHI2DV(PRIMARY) > %(DauMIPChi2Min)s))" % config
    Pi0cut = "(PT> %(Pi0PtMin)s *MeV) & (P> %(Pi0PMin)s *MeV)" % config

    Rho = CombineParticles("CombineRhoChargedFor" + moduleName)
    Rho.ParticleCombiners.update ( { '' : 'ParticleAdder' } )

    Rho.DecayDescriptors = [ "[rho(770)+ -> pi+ pi0]cc" ]
    Rho.DaughtersCuts =  { "pi+"        : Daughtercut,
                           "pi0"        : Pi0cut }

    Rho.CombinationCut = "(APT>%(PtMin)s*MeV) & (ADAMASS('rho(770)+') < %(DMass)s *MeV)" % config
    Rho.MotherCut = "ALL"

    return Selection("RhoChargedFor" + moduleName,
                     Algorithm = Rho, RequiredSelections = [ StdPi, Pi0Sel ] )

def makeKstarCharged(moduleName, Pi0Sel, config) :

    __configuration_keys__ = ("DauChi2Max", "DauPtMin", "DauPMin", "DauMIPChi2Min",
                              "Pi0PtMin", "Pi0PMin", "PtMin", "DMass")

    checkConfig(__configuration_keys__, config)

    from Configurables import CombineParticles

    StdK  = StdNoPIDsKaons

    Daughtercut = "((TRCHI2DOF<%(DauChi2Max)s) & " \
    "(PT > %(DauPtMin)s*MeV) & (P > %(DauPMin)s*MeV) & " \
    "(MIPCHI2DV(PRIMARY) > %(DauMIPChi2Min)s))" % config
    Pi0cut = "(PT> %(Pi0PtMin)s *MeV) & (P> %(Pi0PMin)s *MeV)" % config

    Kstar = CombineParticles("CombineKstarChargedFor" + moduleName)
    Kstar.ParticleCombiners.update ( { '' : 'ParticleAdder' } )

    Kstar.DecayDescriptors = [ "[K*(892)+ -> K+ pi0]cc" ]
    Kstar.DaughtersCuts =  { "K+"        : Daughtercut,
                             "pi0"        : Pi0cut }

    Kstar.CombinationCut = "(APT>%(PtMin)s*MeV) & (ADAMASS('K*(892)+') < %(DMass)s *MeV)" % config
    Kstar.MotherCut = "ALL"

    return Selection("KstarChargedFor" + moduleName,
                     Algorithm = Kstar, RequiredSelections = [ StdK, Pi0Sel ] )


def makeB2D0Pi(moduleName, DName, D0Sel, config ) :

    StdPi  = StdNoPIDsPions

    Bachelorcut = "((TRCHI2DOF<%(BachelorChi2Max)s) & " \
    "(PT > %(BachelorPtMin)s*MeV) & (P > %(BachelorPMin)s*MeV) & " \
    "(MIPCHI2DV(PRIMARY) > %(BachelorMIPChi2Min)s))" % config

    B2DPi = CombineParticles()
    B2DPi.DecayDescriptors = [ "[B+ -> D~0 pi+]cc", "[B+ -> D0 pi+]cc" ]
    B2DPi.DaughtersCuts = { "pi+" : Bachelorcut }
    B2DPi.CombinationCut = "(ADAMASS('B+') < %(CombDMass)s *MeV) & (APT>%(APtMin)s *MeV)" % config

    B2DPi.MotherCut = "((VFASPF(VCHI2/VDOF)<%(VtxChi2Max)s)  & " \
    "(BPVIPCHI2() < %(IPChi2Max)s) & (BPVLTIME()>%(LTMin)s*ps) & " \
    "(BPVDIRA > %(DIRAMin)s))" % config

    return Selection("B2DPiWith" + DName + "For" + moduleName, Algorithm = B2DPi,
                        RequiredSelections = [ D0Sel, StdPi ] )


def makeB2D0K(moduleName, DName, D0Sel, config) :

    StdK  = StdNoPIDsKaons

    Bachelorcut = "((TRCHI2DOF<%(BachelorChi2Max)s) & " \
    "(PT > %(BachelorPtMin)s*MeV) & (P > %(BachelorPMin)s*MeV) & " \
    "(MIPCHI2DV(PRIMARY) > %(BachelorMIPChi2Min)s))" % config

    B2DK = CombineParticles()
    B2DK.DecayDescriptors = [ "[B+ -> D~0 K+]cc", "[B+ -> D0 K+]cc" ]
    B2DK.DaughtersCuts = { "K+" : Bachelorcut }
    B2DK.CombinationCut = "(ADAMASS('B+') < %(CombDMass)s *MeV) & (APT>%(APtMin)s *MeV)" % config

    B2DK.MotherCut = "((VFASPF(VCHI2/VDOF)<%(VtxChi2Max)s)  & " \
    "(BPVIPCHI2() < %(IPChi2Max)s) & (BPVLTIME()>%(LTMin)s*ps)  & " \
    "(BPVDIRA > %(DIRAMin)s))" % config

    return Selection("B2DKWith" + DName + "For" + moduleName, Algorithm = B2DK,
                        RequiredSelections = [ D0Sel, StdK ] )

def makeB02DPi(moduleName, DName, DSel, config ) :

    StdPi  = StdNoPIDsPions

    Bachelorcut = "((TRCHI2DOF<%(BachelorChi2Max)s) & " \
    "(PT > %(BachelorPtMin)s*MeV) & (P > %(BachelorPMin)s*MeV) & " \
    "(MIPCHI2DV(PRIMARY) > %(BachelorMIPChi2Min)s))" % config

    B2DPi = CombineParticles()
    B2DPi.DecayDescriptors = [ "[B0 -> D- pi+]cc" ]
    B2DPi.DaughtersCuts = { "pi+" : Bachelorcut }
    B2DPi.CombinationCut = "((ADAMASS('B0') < %(CombDMass)s *MeV) | (ADAMASS('B_s0') < %(CombDMass)s *MeV)) & (APT>%(APtMin)s *MeV)" % config

    B2DPi.MotherCut = "((VFASPF(VCHI2/VDOF)<%(VtxChi2Max)s)  & " \
    "(BPVIPCHI2() < %(IPChi2Max)s) & (BPVLTIME()>%(LTMin)s*ps)  & " \
    "(BPVDIRA > %(DIRAMin)s))" % config

    return Selection("B02DPiWith" + DName + "For" + moduleName, Algorithm = B2DPi,
                        RequiredSelections = [ DSel, StdPi ] )


def makeB02DRhoCharged(moduleName, DName, DSel, RhoChargedSel, config ) :

    from Configurables import OfflineVertexFitter
    from Configurables import CombineParticles

    B2DRho = CombineParticles("CombineB02DRhoChargedWith" + DName + "For" + moduleName)
    B2DRho.DecayDescriptors = [ "[B0 -> D- rho(770)+]cc" ]
    B2DRho.CombinationCut = "((ADAMASS('B0') < %(CombDMass)s *MeV) | (ADAMASS('B_s0') < %(CombDMass)s *MeV)) & (APT>%(APtMin)s *MeV)" % config
    B2DRho.addTool(OfflineVertexFitter)
    B2DRho.VertexFitters.update( { "" : "OfflineVertexFitter"} )
    B2DRho.OfflineVertexFitter.useResonanceVertex = False

    B2DRho.MotherCut = "((VFASPF(VCHI2/VDOF)<%(VtxChi2Max)s)  & " \
    "(BPVIPCHI2() < %(IPChi2Max)s) & (BPVLTIME()>%(LTMin)s*ps)  & " \
    "(BPVDIRA > %(DIRAMin)s))" % config

    return Selection("B02DRhoChargedWith" + DName + "For" + moduleName, Algorithm = B2DRho,
                        RequiredSelections = [ DSel, RhoChargedSel ] )

def makeB02DKstarCharged(moduleName, DName, DSel, KstarChargedSel, config ) :

    from Configurables import OfflineVertexFitter
    from Configurables import CombineParticles

    B2DKstar = CombineParticles("CombineB02DKstarChargedWith" + DName + "For" + moduleName)
    B2DKstar.DecayDescriptors = [ "[B0 -> D- K*(892)+]cc" ]
    B2DKstar.CombinationCut = "((ADAMASS('B0') < %(CombDMass)s *MeV) | (ADAMASS('B_s0') < %(CombDMass)s *MeV)) & (APT>%(APtMin)s *MeV)" % config
    B2DKstar.addTool(OfflineVertexFitter)
    B2DKstar.VertexFitters.update( { "" : "OfflineVertexFitter"} )
    B2DKstar.OfflineVertexFitter.useResonanceVertex = False

    B2DKstar.MotherCut = "((VFASPF(VCHI2/VDOF)<%(VtxChi2Max)s)  & " \
    "(BPVIPCHI2() < %(IPChi2Max)s) & (BPVLTIME()>%(LTMin)s*ps)  & " \
    "(BPVDIRA > %(DIRAMin)s))" % config

    return Selection("B02DKstarChargedWith" + DName + "For" + moduleName, Algorithm = B2DKstar,
                        RequiredSelections = [ DSel, KstarChargedSel ] )


def makeB02DPiWS(moduleName, DName, DSel, DWSSel, config ) :

    StdPi  = StdNoPIDsPions

    Bachelorcut = "((TRCHI2DOF<%(BachelorChi2Max)s) & " \
    "(PT > %(BachelorPtMin)s*MeV) & (P > %(BachelorPMin)s*MeV) & " \
    "(MIPCHI2DV(PRIMARY) > %(BachelorMIPChi2Min)s))" % config

    B2DPiWS = CombineParticles()
    B2DPiWS.DecayDescriptors = [ "[B0 -> D- pi-]cc" ]
    B2DPiWS.DaughtersCuts = { "pi+" : Bachelorcut }
    B2DPiWS.CombinationCut = "((ADAMASS('B0') < %(CombDMass)s *MeV) | (ADAMASS('B_s0') < %(CombDMass)s *MeV)) & (APT>%(APtMin)s *MeV)" % config

    B2DPiWS.MotherCut = "((VFASPF(VCHI2/VDOF)<%(VtxChi2Max)s)  & " \
    "(BPVIPCHI2() < %(IPChi2Max)s) & (BPVLTIME()>%(LTMin)s*ps)  & " \
    "(BPVDIRA > %(DIRAMin)s))" % config

    B2DWSPi = copy(B2DPiWS)
    B2DWSPi.DecayDescriptors = [ "[B0 -> D- pi+]cc" ]

    B2DPiWSSel = Selection("B02DPiWSWith" + DName + "For" + moduleName, Algorithm = B2DPiWS,
                        RequiredSelections = [ DSel, StdPi ] )
    B2DWSPiSel = Selection("B02DWSPiWith" + DName + "For" + moduleName, Algorithm = B2DWSPi,
                        RequiredSelections = [ DWSSel, StdPi ] )
    B2DWSPiWSSel = Selection("B02DWSPiWSWith" + DName + "For" + moduleName, Algorithm = B2DPiWS,
                        RequiredSelections = [ DWSSel, StdPi ] )

    return MergedSelection("B02DPiWSMergedWith" + DName + "For" + moduleName,
                           RequiredSelections = [ B2DPiWSSel, B2DWSPiSel, B2DWSPiWSSel ] )


def makeUnbiasedB2DPi(moduleName, DName, DSel, config ) :

    StdPi  = StdAllNoPIDsPions  # Because of no IPchi2 cut

    Bachelorcut = "((TRCHI2DOF<%(BachelorChi2Max)s) & " \
    "(PT > %(BachelorPtMin)s*MeV) & (P > %(BachelorPMin)s*MeV) & " \
    "(MIPCHI2DV(PRIMARY) > %(BachelorMIPChi2Min)s))" % config

    Dcut = "((MINTREE(ABSID=='K+',PIDK)>0) & (MAXTREE(ABSID=='pi+',PIDK)<5))"

    B2DPi = CombineParticles()
    B2DPi.DecayDescriptors = [ "[B_s0 -> D- pi+]cc" ]
    B2DPi.DaughtersCuts = { "pi+" : Bachelorcut, "D-" : Dcut }
    B2DPi.CombinationCut = "((ADAMASS('B0') < %(CombDMass)s *MeV) | (ADAMASS('B_s0') < %(CombDMass)s *MeV))" % config

    B2DPi.MotherCut = "((VFASPF(VCHI2/VDOF)<%(VtxChi2Max)s) & " \
    "(BPVDIRA > %(DIRAMin)s))" % config

    return Selection("Bs2DsPiWith" + DName + "For" + moduleName, Algorithm = B2DPi,
                        RequiredSelections = [ DSel, StdPi ] )

def makeB02DK(moduleName, DName, DSel, config) :

    StdK  = StdNoPIDsKaons

    Bachelorcut = "((TRCHI2DOF<%(BachelorChi2Max)s) & " \
    "(PT > %(BachelorPtMin)s*MeV) & (P > %(BachelorPMin)s*MeV) & " \
    "(MIPCHI2DV(PRIMARY) > %(BachelorMIPChi2Min)s))" % config

    B2DK = CombineParticles()
    B2DK.DecayDescriptors = [ "[B0 -> D- K+]cc" ]
    B2DK.DaughtersCuts = { "K+" : Bachelorcut }
    B2DK.CombinationCut = "((ADAMASS('B0') < %(CombDMass)s *MeV) | (ADAMASS('B_s0') < %(CombDMass)s *MeV)) & (APT>%(APtMin)s *MeV)" % config

    B2DK.MotherCut = "((VFASPF(VCHI2/VDOF)<%(VtxChi2Max)s)  & " \
    "(BPVIPCHI2() < %(IPChi2Max)s) & (BPVLTIME()>%(LTMin)s*ps)  & " \
    "(BPVDIRA > %(DIRAMin)s))" % config

    return Selection("B02DKWith" + DName + "For" + moduleName, Algorithm = B2DK,
                        RequiredSelections = [ DSel, StdK ] )


def makeB02DKWS(moduleName, DName, DSel, DWSSel, config) :

    StdK  = StdNoPIDsKaons

    Bachelorcut = "((TRCHI2DOF<%(BachelorChi2Max)s) & " \
    "(PT > %(BachelorPtMin)s*MeV) & (P > %(BachelorPMin)s*MeV) & " \
    "(MIPCHI2DV(PRIMARY) > %(BachelorMIPChi2Min)s))" % config

    B2DKWS = CombineParticles()
    B2DKWS.DecayDescriptors = [ "[B0 -> D- K-]cc" ]
    B2DKWS.DaughtersCuts = { "pi+" : Bachelorcut }
    B2DKWS.CombinationCut = "((ADAMASS('B0') < %(CombDMass)s *MeV) | (ADAMASS('B_s0') < %(CombDMass)s *MeV)) & (APT>%(APtMin)s *MeV)" % config

    B2DKWS.MotherCut = "((VFASPF(VCHI2/VDOF)<%(VtxChi2Max)s)  & " \
    "(BPVIPCHI2() < %(IPChi2Max)s) & (BPVLTIME()>%(LTMin)s*ps)  & " \
    "(BPVDIRA > %(DIRAMin)s))" % config

    B2DWSK = copy(B2DKWS)
    B2DWSK.DecayDescriptors = [ "[B0 -> D- K+]cc" ]

    B2DKWSSel = Selection("B02DKWSWith" + DName + "For" + moduleName, Algorithm = B2DKWS,
                        RequiredSelections = [ DSel, StdK ] )
    B2DWSKSel = Selection("B02DWSKWith" + DName + "For" + moduleName, Algorithm = B2DWSK,
                        RequiredSelections = [ DWSSel, StdK ] )
    B2DWSKWSSel = Selection("B02DWSKWSWith" + DName + "For" + moduleName, Algorithm = B2DKWS,
                        RequiredSelections = [ DWSSel, StdK ] )

    return MergedSelection("B02DKWSMergedWith" + DName + "For" + moduleName,
                           RequiredSelections = [ B2DKWSSel, B2DWSKSel, B2DWSKWSSel ] )


def makeB2DKstar(moduleName, DName, DSel, KstarSel, config) :

    B2DKstar = CombineParticles()
    B2DKstar.DecayDescriptors = [ "[B- -> D- K*(892)0]cc", "[B+ -> D+ K*(892)0]cc" ]
    B2DKstar.CombinationCut = "(ADAMASS('B+') < %(CombDMass)s *MeV) & (APT>%(APtMin)s *MeV)" % config

    B2DKstar.MotherCut = "((VFASPF(VCHI2/VDOF)<%(VtxChi2Max)s)  & " \
    "(BPVIPCHI2() < %(IPChi2Max)s) & (BPVLTIME()>%(LTMin)s*ps)  & " \
    "(BPVDIRA > %(DIRAMin)s) & (BPVVDCHI2 > %(VDChi2Min)s))" % config

    return Selection("B2DKstarWith" + DName + "For" + moduleName, Algorithm = B2DKstar,
                        RequiredSelections = [ DSel, KstarSel ] )

def makeB2DRho(moduleName, DName, DSel, RhoSel, config) :

    B2DRho = CombineParticles()
    B2DRho.DecayDescriptors = [ "[B- -> D- rho(770)0]cc" ]
    B2DRho.CombinationCut = "(ADAMASS('B+') < %(CombDMass)s *MeV) & (APT>%(APtMin)s *MeV)" % config

    B2DRho.MotherCut = "((VFASPF(VCHI2/VDOF)<%(VtxChi2Max)s)  & " \
    "(BPVIPCHI2() < %(IPChi2Max)s) & (BPVLTIME()>%(LTMin)s*ps)  & " \
    "(BPVDIRA > %(DIRAMin)s) & (BPVVDCHI2 > %(VDChi2Min)s))" % config

    return Selection("B2DRhoWith" + DName + "For" + moduleName, Algorithm = B2DRho,
                        RequiredSelections = [ DSel, RhoSel ] )

def makeB2DPhi(moduleName, DName, DSel, PhiSel, config) :

    B2DPhi = CombineParticles()
    B2DPhi.DecayDescriptors = [ "[B- -> D- phi(1020)]cc" ]
    B2DPhi.CombinationCut = "(ADAMASS('B+') < %(CombDMass)s *MeV) & (APT>%(APtMin)s *MeV)" % config

    B2DPhi.MotherCut = "((VFASPF(VCHI2/VDOF)<%(VtxChi2Max)s)  & " \
    "(BPVIPCHI2() < %(IPChi2Max)s) & (BPVLTIME()>%(LTMin)s*ps)  & " \
    "(BPVDIRA > %(DIRAMin)s) & (BPVVDCHI2 > %(VDChi2Min)s))" % config

    return Selection("B2DPhiWith" + DName + "For" + moduleName, Algorithm = B2DPhi,
                        RequiredSelections = [ DSel, PhiSel ] )


def makeB02D0Kstar(moduleName, DName, KstarSel, D0Sel, config) :
    """
    Create and return a B0 -> D0 (Kpipi0) Kstar0 (Kpi) Selection object.
    Arguments:
    moduleName      : name of the module
    DName           : name of the D selection
    KstarSel        : Kstar0->K Pi Selection object.
    D0Sel           : D0 Selection object.
    VtxChi2Max      : Maximum chi2 for B0 vertex fit.
    MIPChi2Max      : Maximum chi2 for impact parameter (wrt PV) of B0.
    VDChi2Min       : Minimum chi2 for decay vertex distance (wrt PV) of B0.
    DIRAMin         : Minimum cosine of angle between reconstructed vector momentum and flight distance vector of B0.
    CombDMass       : Maximum mass difference for the B0 combination (MeV) from nominal B0 or B_s mass.
    """

    _BCombCut = "((ADAMASS('B0') < %(CombDMass)s *MeV) | (ADAMASS('B_s0') < %(CombDMass)s *MeV)) & (APT>%(APtMin)s *MeV)" % config

    _B0 = CombineParticles()
    _B0.DecayDescriptors  = ["[B0 -> D~0 K*(892)0]cc","[B0 -> D0 K*(892)0]cc" ]
    _B0.CombinationCut = _BCombCut
    _B0.MotherCut = "((VFASPF(VCHI2/VDOF)<%(VtxChi2Max)s)  & " \
    "(BPVIPCHI2() < %(IPChi2Max)s) & (BPVLTIME()>%(LTMin)s*ps)  & " \
    "(BPVDIRA > %(DIRAMin)s))" % config

    return Selection("B2D0KstarWith" + DName + "For" + moduleName
                     ,Algorithm = _B0
                     ,RequiredSelections = [D0Sel,KstarSel]
                    )

def makeB02D0Phi(moduleName, DName, PhiSel, D0Sel, config) :

    """
    Create and return a B0 -> D0 (Kpipi0) Phi(1020) (KK) Selection object.
    Arguments:
    moduleName      : name of the module
    DName           : name of the D selection
    PhiSel          : Phi Selection object.
    D0Sel           : D0 Selection object.
    VtxChi2Max      : Maximum chi2 for B0 vertex fit.
    MIPChi2Max      : Maximum chi2 for impact parameter (wrt PV) of B0.
    VDChi2Min       : Minimum chi2 for decay vertex distance (wrt PV) of B0.
    DIRAMin         : Minimum cosine of angle between reconstructed vector momentum and flight distance vector of B0.
    CombDMass       : Maximum mass difference for the B0 combination (MeV) from nominal B0 or B_s mass.
    """

    _BCombCut = "((ADAMASS('B0') < %(CombDMass)s *MeV) | (ADAMASS('B_s0') < %(CombDMass)s *MeV)) & (APT>%(APtMin)s *MeV)" % config

    _B0 = CombineParticles()
    _B0.DecayDescriptors  = ["B0 -> D~0 phi(1020)","B0 -> D0 phi(1020)" ]
    _B0.CombinationCut = _BCombCut
    _B0.MotherCut = "((VFASPF(VCHI2/VDOF)<%(VtxChi2Max)s)  & " \
    "(BPVIPCHI2() < %(IPChi2Max)s) & (BPVLTIME()>%(LTMin)s*ps)  & " \
    "(BPVDIRA > %(DIRAMin)s))" % config

    return Selection("B2D0PhiWith" + DName + "For" + moduleName
                     ,Algorithm = _B0
                     ,RequiredSelections = [D0Sel,PhiSel]
                    )

def makeB02D0Rho(moduleName, DName, RhoSel, D0Sel, config) :

    """
    Create and return a B0 -> D0 (Kpipi0) Rho(770)0 (pipi) Selection object.
    Arguments:
    moduleName      : name of the module
    DName           : name of the D selection
    RhoSel          : Rho Selection object.
    D0Sel           : D0 Selection object.
    VtxChi2Max      : Maximum chi2 for B0 vertex fit.
    MIPChi2Max      : Maximum chi2 for impact parameter (wrt PV) of B0.
    VDChi2Min       : Minimum chi2 for decay vertex distance (wrt PV) of B0.
    DIRAMin         : Minimum cosine of angle between reconstructed vector momentum and flight distance vector of B0.
    CombDMass       : Maximum mass difference for the B0 combination (MeV) from nominal B0 or B_s mass.
    """

    _BCombCut = "((ADAMASS('B0') < %(CombDMass)s *MeV) | (ADAMASS('B_s0') < %(CombDMass)s *MeV)) & (APT>%(APtMin)s *MeV)" % config

    _B0 = CombineParticles()
    _B0.DecayDescriptors  = ["B0 -> D~0 rho(770)0","B0 -> D0 rho(770)0" ]
    _B0.CombinationCut = _BCombCut
    _B0.MotherCut = "((VFASPF(VCHI2/VDOF)<%(VtxChi2Max)s)  & " \
    "(BPVIPCHI2() < %(IPChi2Max)s) & (BPVLTIME()>%(LTMin)s*ps)  & " \
    "(BPVDIRA > %(DIRAMin)s))" % config

    return Selection("B2D0RhoWith" + DName + "For" + moduleName
                     ,Algorithm = _B0
                     ,RequiredSelections = [D0Sel,RhoSel]
                    )

def makeLambdaB2LambdaCPi(moduleName, LambdaCSel, config ) :

    StdPi  = StdNoPIDsPions

    Bachelorcut = "((TRCHI2DOF<%(BachelorChi2Max)s) & " \
    "(PT > %(BachelorPtMin)s*MeV) & (P > %(BachelorPMin)s*MeV) & " \
    "(MIPCHI2DV(PRIMARY) > %(BachelorMIPChi2Min)s))" % config

    LambdaB2LambdaCPi = CombineParticles()
    LambdaB2LambdaCPi.DecayDescriptors = [ "[Lambda_b0 -> Lambda_c+ pi-]cc" ]
    LambdaB2LambdaCPi.DaughtersCuts = { "pi+" : Bachelorcut }
    LambdaB2LambdaCPi.CombinationCut = "(in_range(%(CombMassMin)s*MeV, AM, %(CombMassMax)s*MeV)) & (APT>%(APtMin)s *MeV)" % config

    LambdaB2LambdaCPi.MotherCut = "((VFASPF(VCHI2/VDOF)<%(VtxChi2Max)s)  & " \
    "(BPVIPCHI2() < %(IPChi2Max)s) & (BPVLTIME()>%(LTMin)s*ps) & " \
    "(BPVDIRA > %(DIRAMin)s))" % config

    return Selection("LambdaB2LambdaCPiFor" + moduleName, Algorithm = LambdaB2LambdaCPi,
                        RequiredSelections = [ LambdaCSel, StdPi ] )


def makeLambdaB2LambdaCK(moduleName, LambdaCSel, config ) :

    StdK  = StdNoPIDsKaons

    Bachelorcut = "((TRCHI2DOF<%(BachelorChi2Max)s) & " \
    "(PT > %(BachelorPtMin)s*MeV) & (P > %(BachelorPMin)s*MeV) & " \
    "(MIPCHI2DV(PRIMARY) > %(BachelorMIPChi2Min)s))" % config

    LambdaB2LambdaCK = CombineParticles()
    LambdaB2LambdaCK.DecayDescriptors = [ "[Lambda_b0 -> Lambda_c+ K-]cc" ]
    LambdaB2LambdaCK.DaughtersCuts = { "K+" : Bachelorcut }
    LambdaB2LambdaCK.CombinationCut = "(in_range(%(CombMassMin)s*MeV, AM, %(CombMassMax)s*MeV)) & (APT>%(APtMin)s *MeV)" % config

    LambdaB2LambdaCK.MotherCut = "((VFASPF(VCHI2/VDOF)<%(VtxChi2Max)s)  & " \
    "(BPVIPCHI2() < %(IPChi2Max)s) & (BPVLTIME()>%(LTMin)s*ps) & " \
    "(BPVDIRA > %(DIRAMin)s))" % config

    return Selection("LambdaB2LambdaCKFor" + moduleName, Algorithm = LambdaB2LambdaCK,
                        RequiredSelections = [ LambdaCSel, StdK ] )

def makeTISTOSSel(name, sel, trigger ) :
    from Configurables import TisTosParticleTagger

    tisTosFilter = TisTosParticleTagger(name + "Filter")
    tisTosFilter.TisTosSpecs = { trigger : 0}
    # the rest ist only to speed it up... (TISTOSsing only done with tracking system)
    tisTosFilter.ProjectTracksToCalo = False
    tisTosFilter.CaloClustForCharged = False
    tisTosFilter.CaloClustForNeutral = False
    tisTosFilter.TOSFrac = { 4:0.0, 5:0.0 }

    return Selection(name, Algorithm = tisTosFilter, RequiredSelections = [ sel ] )

def makeLambda02ppi(moduleName, config) :

    __configuration_keys__ = ("DauChi2Max", "DauPtMin", "DauPMin", "DauMIPChi2Min",
                              "PtMin", "VtxChi2Max", "MIPChi2DVMin")

    checkConfig(__configuration_keys__, config)

    StdPi = StdNoPIDsPions
    StdP  = StdNoPIDsProtons

    Daughtercut = "((TRCHI2DOF<%(DauChi2Max)s) & " \
    "(PT > %(DauPtMin)s*MeV) & " \
    "(P > %(DauPMin)s*MeV) & " \
    "(MIPCHI2DV(PRIMARY) > %(DauMIPChi2Min)s))" % config

    Lambda0 = CombineParticles()
    Lambda0.DecayDescriptors = [ "[Lambda0 -> p+ pi-]cc" ]
    Lambda0.DaughtersCuts =  { "p+"        : Daughtercut + " & (PIDp>0)",
                               "pi+"       : Daughtercut }

    Lambda0.CombinationCut = "(APT>%(PtMin)s*MeV)" % config
    Lambda0.MotherCut = "((VFASPF(VCHI2/VDOF) < %(VtxChi2Max)s) & " \
    "(MIPCHI2DV(PRIMARY) > %(MIPChi2DVMin)s))" % config

    return Selection("Lambda02ppiFor" + moduleName,
                     Algorithm = Lambda0, RequiredSelections = [ StdPi, StdP ] )

def makeLambda02pK(moduleName, config) :

    __configuration_keys__ = ("DauChi2Max", "DauPtMin", "DauPMin", "DauMIPChi2Min",
                              "PtMin", "VtxChi2Max", "MIPChi2DVMin")

    checkConfig(__configuration_keys__, config)

    StdK  = StdNoPIDsKaons
    StdP  = StdNoPIDsProtons

    Daughtercut = "((TRCHI2DOF<%(DauChi2Max)s) & " \
    "(PT > %(DauPtMin)s*MeV) & " \
    "(P > %(DauPMin)s*MeV) & " \
    "(MIPCHI2DV(PRIMARY) > %(DauMIPChi2Min)s))" % config

    Lambda0 = CombineParticles()
    Lambda0.DecayDescriptors = [ "[Lambda0 -> p+ K-]cc" ]
    Lambda0.DaughtersCuts =  { "p+"        : Daughtercut + " & (PIDp>0)",
                               "K+"        : Daughtercut}

    Lambda0.CombinationCut = "(APT>%(PtMin)s*MeV)" % config
    Lambda0.MotherCut = "((VFASPF(VCHI2/VDOF) < %(VtxChi2Max)s) & " \
    "(MIPCHI2DV(PRIMARY) > %(MIPChi2DVMin)s))" % config

    return Selection("Lambda02pKFor" + moduleName,
                     Algorithm = Lambda0, RequiredSelections = [ StdK, StdP ] )


def makeLambdaB2D0Lambda(moduleName, DName, LambdaName, D0Sel, LambdaSel, config) :

    _B0 = CombineParticles()
    _B0.DecayDescriptors  = ["[Lambda_b0 -> D~0 Lambda0]cc","[Lambda_b0 -> D0 Lambda0]cc" ]
#    _B0.CombinationCut = "(in_range(%(CombMassMin)s*MeV, AM, %(CombMassMax)s*MeV)) & (APT>2000*MeV)" % config
    _B0.CombinationCut = "(ADAMASS('Lambda_b0') < %(CombDMass)s *MeV) & (APT>%(APtMin)s *MeV)" % config
    _B0.MotherCut = "((VFASPF(VCHI2/VDOF)<%(VtxChi2Max)s)  & " \
    "(BPVIPCHI2() < %(IPChi2Max)s) & (BPVLTIME()>%(LTMin)s*ps)  & " \
    "(BPVDIRA > %(DIRAMin)s) & (BPVVDCHI2 > %(VDChi2Min)s))" % config
    _B0.DaughtersCuts = { "D0" : "(BPVVDCHI2 > %(DVDChi2Min)s)" % config }

    return Selection("LambdaB2D0" + LambdaName + "With" + DName + "For" + moduleName
                     , Algorithm = _B0
                     , RequiredSelections = [D0Sel, LambdaSel]
                    )
