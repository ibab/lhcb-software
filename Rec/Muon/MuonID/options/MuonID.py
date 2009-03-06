#//-----------------------------------------------------------------
#// MuonID v5r2
#// June 2007 Added a SmartRef to the muonTracks 
#//           Miriam
#// MuonID v5r1 
#// May 2007 new muonTrack object created for each MuonPID
#//          new parameter in the opts file to enlarge the FOI if needed
#//          Miriam, Erica
#//
#// Configure MuonID v4r6
#// February 2007 new DLL binning for pions
#//               fixed the number of Landau parameters for muons
#// February 2007 changed the muonPID key (Chris Jones)
#//  October 2006 new FOI tuning for DC06
#//               new DLL tuning for DC06 - same binning
#//           Erica, Miriam
#//  April 2006
#//  New version for DC06 (MuonIDFOI and MuonIDDLL options merged) 
#//            Erica, Miriam 
#//  muon id tuning march 2005
#//  4 parameters per region/station
#//            jtmn,massafferri 
#//-----------------------------------------------------------------
from Configurables import MuonID

#// MuonID depth in stations for the momentum bin, below means:
#// 3GeV -> 6GeV : station M2.and.M3, 6->10 GeV M2.and.M3.and.M4.or.M5
#// , >10GeV M2.and.M3.and.M4.and.M5
#// MuonID preselection momentum (MeV/c)
MuonID().PreSelMomentum = 3000.
#// MuonID momentum cut ranges (MeV/c)
MuonID().MomentumCuts = [ 6000. , 10000. ]

#// MuonID FOI parameters

MuonID().FOIfactor = 1.2

MuonID().XFOIParameter1 = [5.5, 4.0, 3.3, 2.8,
                         5.2, 3.6, 2.4, 2.4,
                         5.7, 4.4, 2.8, 2.3,
                         5.1, 3.1, 2.3, 2.1,
                         5.8, 3.4, 2.6, 2.8]

MuonID().XFOIParameter2 = [11., 3., 1., 1.,
                         31., 28., 21., 17.,
                         30., 31., 27., 22.,
                         28., 33., 35., 47.,
                         31., 39., 56., 151.]

MuonID().XFOIParameter3 = [0.2, 0.08, 0.03, 0.1,
                         0.06, 0.08, 0.10, 0.15,
                         0.04, 0.06, 0.09, 0.12,
                         0.08, 0.15, 0.23, 0.36,
                         0.07, 0.14, 0.24, 0.49]

MuonID().YFOIParameter1 = [2.8, 1.7, -153., 1.9,
                         3.3, 2.1, 1.7, 1.6,
                         3.6, 2.8, 1.9, 1.8,
                         4.4, 3.3, 2.2, 2.2,
                         4.8, 3.9, 2.6, 2.3]

MuonID().YFOIParameter2 = [3., 2., 156., 0.,
                         17., 15., 9., 5.,
                         26., 25., 16., 15.,
                         30., 49., 57., 92.,
                         32., 55., 96., 166.]

MuonID().YFOIParameter3 = [0.03, 0.02, 0.00, 0.09,
                         0.13, 0.19, 0.19, 0.24, 
                         0.11, 0.19, 0.21, 0.32,
                         0.10, 0.22, 0.30, 0.52,
                         0.08, 0.20, 0.34, 0.52]


#//-----------------------------------------------------------------
#// Parameters of the Landau functions
#//-----------------------------------------------------------------
MuonID().distMuon  = [0.311, 1.349, 0.524, 0.0020, 17., 10.6, 0.04, 4.1, 1.64]

MuonID().distPion  = [11., -12., 0.2029, -0.026, 0.06, 0.59, 0.008, -29., 41.]
