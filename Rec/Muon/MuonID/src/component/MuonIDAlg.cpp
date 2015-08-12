// Include files
#include <cstdio>
#include <map>
#include <vector>

// from Gaudi
#include "DetDesc/Condition.h"

#include "Event/RecHeader.h"

// local
#include "MuonIDAlg.h"
#include "TF1.h"
#include <cmath>
#include "Math/ProbFuncMathCore.h"
#include "vdt/exp.h"

//boost
#include <boost/assign/list_of.hpp>


//-----------------------------------------------------------------------------
// Implementation file for class : MuonIDAlg
//
// first version
// 02/05/2002 : David Hutchcroft
//
// new FOI parameterization, acceptance cut for inner chamber holes,
// back to original algorithm; code expects all five chambers, even though
// it only checks hits in M2-M5 by now.
// 09/04/2003 : jtmn, miriam
//
// new Event Model version: calculates de muon LL for muons and non muons
// and the number of tracks that share hits, which are the new members of
// the muonPID object.
// 14/12/2005 : Erica Polycarpo, Miriam Gandelman
//
// new FOI uses 3 parameters
// 16/10/2006 : Erica Polycarpo, Miriam Gandelman
//
// small fixes (DLL binning for pions, Landau with 9 parameters, muonPID key)
// 08/02/2007 : Erica Polycarpo, Miriam Gandelman
//
// added the method makeMuonTrack to create a track object for each MuonPID
// added a foifactor to enlarge the foi if needed. default = 1.
// 07/05/2007 : Erica Polycarpo, Miriam Gandelman
//
// added a smartRef to the muonTracks
// 30/06/2007 : Miriam Gandelman
//
// MuonPID info made also available in muonTracks (PreSelMom, InAcceptance,IsMuon...)
// New property FindQuality. Adds 'quality' info to tracks. This quality can be chi2 from a
//                           fit with hits in FOI and a state from seed.
// New property AllMuonTracks. Build muonTracks with all info for all seeds even if they
// are not Muon Candidates.
// 28/04/2009 : Xabier Cid Vidal, Jose Angel Hernando Morata, Erica Polycarpo
//
// Introduction of IsMuonLoose as candidate selector.
//===================================================
// OR of the hits in FOI (instead of the AND) defines the IsMuonLoose boolean variable
// DLL is built and MuonTrack are stored if IsMuonLoose=true
// IsMuon info is still stored in MuonPID objects
// IsMuonLoose and IsMuon can be used with/without weights (settable via property).
// New property: Weight_flag
// 10/05/2009: X.Cid Vidal, S.Furcas, J.A.Hernando Morata, G. Lanfranchi, E. Polycarpo.
//
// New DLL definition:
//=====================
//  Use the distance built with the closest hit to make a hypothesis test for muons and
//  non-muons; the distance distributions are binned in p-bins and muon detector regions;
//  the integral of the distributions is used to evaluate the compatibility of a given track
//  with the muon (Probmu) /non-muon (Prob non-mu) hypotheses.
//  Prob(mu) and Prob(non-mu) in the case of correct hypothesis are p-independent.
//  Prob(mu) and Prob(non-mu) are stored in the MuonPID object.
//
// New properties: DLL_flag (to pass from the old DLL to the binned-integrated DLL)
//                 Parameters of the Landaus for Muons and non-Muons for the binned DLL.
// 03/06/2009: G. Lanfranchi, S. Furcas.
//
// Conditions database
//=====================
// Some global properties and DLL_flag=1 parameters may be read from conditions database.
// Or alternatively(OverrideDB=true or database not found), still from options file
// 16/01/2010: J. H. Lopes
//
// Implementation of the hyperbolic tangent mapping of distances:
//=====================
// 02/02/2010: J. Helder Lopes
// Replace Landau fits by histograms. The closest distance is mapped into [0,1]
// by hyperbolic tangent, with a conversion factor per region/momentum bin.
//
//=====================
// 21/02/2011: J. Helder Lopes
// Implement muon probability tuned on 2010 data and using cumulative histograms
// of the hyperbolic tangent of the distances. NonMuon prob still from Landau fits.
// This corresponds to DLL_Flag = 4 and is used with 2011 and 2010 data if configured
// from options.  DLL_Flag = 3 should not be used for 2011 and 2010 data anymore
//=====================
//05/05/2011: X. Cid Vidal
// Implement the use of KalmanFoI in MuonIDAlg. This implies the use of MuIDTool 
// and some other independent tools, also for distance calculation. New tool introduced 
// for NShared calculation. Default behaviour left as before, with possibility to switch
// with a flag.
// Introduction of UseUncrossed hits option. Default true. If false, in regions where
// both crossed logical channels are present, force the presence of both to accept a hit for
// MuonID.
//=====================
// 15/08/2011: J. Helder Lopes
// Remove use of random weights for IsMuon or IsMuonLoose
// Keep, however, weight 0 for hits in M4 if p<3.5 and hits in M5 if p<4.2
// Property Weight_flag continues in database, bit is not used
//=====================
// 25/08/2011: J. Helder Lopes
// Implemented IsMuonTight, which is IsMuon but using only hits with x,y crossing
// Option UseUncrossed now affects only probabilities, dlls, NShared... 
// Does not affect IsMuonLoose or IsMuon or IsMuonTight.
// MuonPID in Event/RecEvent was modified to save the new IsMuonTight info. Also mutrack saves this info.
//=====================

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( MuonIDAlg )

double land2(Double_t *x, Double_t *par);


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
MuonIDAlg::MuonIDAlg( const std::string& name,
                      ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator ),
    m_mudet ( NULL ),
    m_myMuIDTool ( NULL ),
    m_DistMuIDTool ( NULL ),
    m_NSharedTool ( NULL ),
    m_IsMuonTool ( NULL ),
    m_IsMuonLooseTool ( NULL )


{

  // If you add or change a property, change also the options file AND the ConfiguredMuonIDs.py !
  // Source of track to ID
  declareProperty("TrackLocation",
                  m_TracksPath = LHCb::TrackLocation::Default);

  declareProperty("useTtrack", m_useTtrack = kFALSE);

  // Destination of MuonPID
  declareProperty("MuonIDLocation",
                  m_MuonPIDsPath = LHCb::MuonPIDLocation::Default);

  // Destination of MuonTracks
  declareProperty("MuonTrackLocation",
                  m_MuonTracksPath = LHCb::TrackLocation::Muon);

  // Destination of MuonTracks (all tracks)
  declareProperty("MuonTrackLocationAll",
                  m_MuonTracksPathAll = LHCb::TrackLocation::Muon+"/AllMuonTracks");

  // Ignore MuonID info from conditions database. Use the ones from options file:
  declareProperty("OverrideDB",m_OverrideDB=false);

  // Use or not uncrossed logical channels in the non-pad detector areas
  declareProperty("UseUncrossed",m_use_uncrossed=true);

  // Pre-selection momentum
  declareProperty( "PreSelMomentum", m_PreSelMomentum = 3000.0);

  // Different depths of stations considered in different momentum ranges
  declareProperty( "MomentumCuts", m_MomentumCuts );

  // function that defines the field of interest size
  // here momentum is scaled to Gaudi::Units::GeV....
  // new formula: p(1) + p(2)*momentum + p(3)*exp(-p(4)*momentum)

  declareProperty( "XFOIParameter1", m_xfoiParam1 );
  declareProperty( "XFOIParameter2", m_xfoiParam2 );
  declareProperty( "XFOIParameter3", m_xfoiParam3 );
  declareProperty( "YFOIParameter1", m_yfoiParam1 );
  declareProperty( "YFOIParameter2", m_yfoiParam2 );
  declareProperty( "YFOIParameter3", m_yfoiParam3 );

  declareProperty("FOIfactor",m_foifactor = 1.2);

  declareProperty("distMuon",m_distMuon );
  declareProperty("distPion",m_distPion );

  //want to find quality?
  declareProperty("FindQuality", m_FindQuality = true);
  
  //use default FoI or Kalman?
  declareProperty("KalmanFoI", m_kalman_foi = false);

  //Create container with all muonTracks (even if not in acceptance or !IsMuon)
  declareProperty("AllMuonTracks", m_DoAllMuonTracks = false);
  //declare which quantity you want to get stored
  declareProperty("myMuIDTool",m_myMuIDToolName="Chi2MuIDTool");
  // Which property for quality (added as extrainfo)?
  if (m_myMuIDToolName=="Chi2MuIDTool") m_use_dist=false;
  else m_use_dist=true;

  //--------------------
  // flag to use DLL:
  //--------------------
  // 0 -- default
  // 1 -- binned distance with closest hit + integral of Landau fit
  // 3 -- binned tanh(distance) with closest hit(Muon) + integral. Flag 2 used by MuonPIDChecker for monitoring Prob Mu
  // 4 -- binned tanh(distance) with closest hit(Muon) + integral of Landau fit(NonMuon).
  //-----------------------------
  declareProperty("DLL_flag",m_dllFlag = 1);
  
  if (m_kalman_foi && m_dllFlag == 0){
    warning() << 
      "DLL flag 0 (use of all hits in FoI for DLL computation) incompatible with Kalman FoI. Setting DLL flag to 1 (use of closest hit)" 
              << endmsg;
    m_dllFlag = 1;
  }
  

  // Muons - Region1:
  declareProperty( "MuLandauParameterR1_1", m_MuLanParR1_1 );
  declareProperty( "MuLandauParameterR1_2", m_MuLanParR1_2 );
  declareProperty( "MuLandauParameterR1_3", m_MuLanParR1_3 );
  declareProperty( "MuLandauParameterR1_4", m_MuLanParR1_4 );
  declareProperty( "MuLandauParameterR1_5", m_MuLanParR1_5 );
  declareProperty( "MuLandauParameterR1_6", m_MuLanParR1_6 );
  declareProperty( "MuLandauParameterR1_7", m_MuLanParR1_7 );

  // Muons - Region2:
  declareProperty( "MuLandauParameterR2_1", m_MuLanParR2_1 );
  declareProperty( "MuLandauParameterR2_2", m_MuLanParR2_2 );
  declareProperty( "MuLandauParameterR2_3", m_MuLanParR2_3 );
  declareProperty( "MuLandauParameterR2_4", m_MuLanParR2_4 );
  declareProperty( "MuLandauParameterR2_5", m_MuLanParR2_5 );

  // Muons - Region3:
  declareProperty( "MuLandauParameterR3_1", m_MuLanParR3_1 );
  declareProperty( "MuLandauParameterR3_2", m_MuLanParR3_2 );
  declareProperty( "MuLandauParameterR3_3", m_MuLanParR3_3 );
  declareProperty( "MuLandauParameterR3_4", m_MuLanParR3_4 );
  declareProperty( "MuLandauParameterR3_5", m_MuLanParR3_5 );

  // Muons - Region4:
  declareProperty( "MuLandauParameterR4_1", m_MuLanParR4_1 );
  declareProperty( "MuLandauParameterR4_2", m_MuLanParR4_2 );
  declareProperty( "MuLandauParameterR4_3", m_MuLanParR4_3 );
  declareProperty( "MuLandauParameterR4_4", m_MuLanParR4_4 );
  declareProperty( "MuLandauParameterR4_5", m_MuLanParR4_5 );

  // Non-Muons - Region 1-2-3-4:
  declareProperty( "NonMuLandauParameterR1", m_NonMuLanParR1 );
  declareProperty( "NonMuLandauParameterR2", m_NonMuLanParR2 );
  declareProperty( "NonMuLandauParameterR3", m_NonMuLanParR3 );
  declareProperty( "NonMuLandauParameterR4", m_NonMuLanParR4 );

  declareProperty( "step", m_x = 0.2); // step for the integral
  declareProperty( "nMax_bin", m_nMax = 4000 ); // number of steps

  declareProperty( "nMupBinsR1", m_nMupBinsR1 = 6);
  declareProperty( "nMupBinsR2", m_nMupBinsR2 = 4);
  declareProperty( "nMupBinsR3", m_nMupBinsR3 = 4);
  declareProperty( "nMupBinsR4", m_nMupBinsR4 = 4);

  declareProperty( "MupBinsR1", m_MupBinsR1);
  declareProperty( "MupBinsR2", m_MupBinsR2);
  declareProperty( "MupBinsR3", m_MupBinsR3);
  declareProperty( "MupBinsR4", m_MupBinsR4);

  // hyperbolic tangent mapping of distances:

  // tanh scale factors
  declareProperty( "tanhScaleFactorsR1", m_tanhScaleFactorsR1);
  declareProperty( "tanhScaleFactorsR2", m_tanhScaleFactorsR2);
  declareProperty( "tanhScaleFactorsR3", m_tanhScaleFactorsR3);
  declareProperty( "tanhScaleFactorsR4", m_tanhScaleFactorsR4);

  // tanh(dist2) histograms contents
  declareProperty( "tanhCumulHistoMuonR1_1", m_tanhCumulHistoMuonR1_1);
  declareProperty( "tanhCumulHistoMuonR1_2", m_tanhCumulHistoMuonR1_2);
  declareProperty( "tanhCumulHistoMuonR1_3", m_tanhCumulHistoMuonR1_3);
  declareProperty( "tanhCumulHistoMuonR1_4", m_tanhCumulHistoMuonR1_4);
  declareProperty( "tanhCumulHistoMuonR1_5", m_tanhCumulHistoMuonR1_5);
  declareProperty( "tanhCumulHistoMuonR1_6", m_tanhCumulHistoMuonR1_6);
  declareProperty( "tanhCumulHistoMuonR1_7", m_tanhCumulHistoMuonR1_7);

  declareProperty( "tanhCumulHistoMuonR2_1", m_tanhCumulHistoMuonR2_1);
  declareProperty( "tanhCumulHistoMuonR2_2", m_tanhCumulHistoMuonR2_2);
  declareProperty( "tanhCumulHistoMuonR2_3", m_tanhCumulHistoMuonR2_3);
  declareProperty( "tanhCumulHistoMuonR2_4", m_tanhCumulHistoMuonR2_4);
  declareProperty( "tanhCumulHistoMuonR2_5", m_tanhCumulHistoMuonR2_5);

  declareProperty( "tanhCumulHistoMuonR3_1", m_tanhCumulHistoMuonR3_1);
  declareProperty( "tanhCumulHistoMuonR3_2", m_tanhCumulHistoMuonR3_2);
  declareProperty( "tanhCumulHistoMuonR3_3", m_tanhCumulHistoMuonR3_3);
  declareProperty( "tanhCumulHistoMuonR3_4", m_tanhCumulHistoMuonR3_4);
  declareProperty( "tanhCumulHistoMuonR3_5", m_tanhCumulHistoMuonR3_5);

  declareProperty( "tanhCumulHistoMuonR4_1", m_tanhCumulHistoMuonR4_1);
  declareProperty( "tanhCumulHistoMuonR4_2", m_tanhCumulHistoMuonR4_2);
  declareProperty( "tanhCumulHistoMuonR4_3", m_tanhCumulHistoMuonR4_3);
  declareProperty( "tanhCumulHistoMuonR4_4", m_tanhCumulHistoMuonR4_4);
  declareProperty( "tanhCumulHistoMuonR4_5", m_tanhCumulHistoMuonR4_5);


  // tanh(dist2) histograms contents
  // # For the moment, non-muons dist per momentum bin come from the same dist per region
  declareProperty( "tanhCumulHistoNonMuonR1_1", m_tanhCumulHistoNonMuonR1_1);
  declareProperty( "tanhCumulHistoNonMuonR1_2", m_tanhCumulHistoNonMuonR1_2);
  declareProperty( "tanhCumulHistoNonMuonR1_3", m_tanhCumulHistoNonMuonR1_3);
  declareProperty( "tanhCumulHistoNonMuonR1_4", m_tanhCumulHistoNonMuonR1_4);
  declareProperty( "tanhCumulHistoNonMuonR1_5", m_tanhCumulHistoNonMuonR1_5);
  declareProperty( "tanhCumulHistoNonMuonR1_6", m_tanhCumulHistoNonMuonR1_6);
  declareProperty( "tanhCumulHistoNonMuonR1_7", m_tanhCumulHistoNonMuonR1_7);

  declareProperty( "tanhCumulHistoNonMuonR2_1", m_tanhCumulHistoNonMuonR2_1);
  declareProperty( "tanhCumulHistoNonMuonR2_2", m_tanhCumulHistoNonMuonR2_2);
  declareProperty( "tanhCumulHistoNonMuonR2_3", m_tanhCumulHistoNonMuonR2_3);
  declareProperty( "tanhCumulHistoNonMuonR2_4", m_tanhCumulHistoNonMuonR2_4);
  declareProperty( "tanhCumulHistoNonMuonR2_5", m_tanhCumulHistoNonMuonR2_5);

  declareProperty( "tanhCumulHistoNonMuonR3_1", m_tanhCumulHistoNonMuonR3_1);
  declareProperty( "tanhCumulHistoNonMuonR3_2", m_tanhCumulHistoNonMuonR3_2);
  declareProperty( "tanhCumulHistoNonMuonR3_3", m_tanhCumulHistoNonMuonR3_3);
  declareProperty( "tanhCumulHistoNonMuonR3_4", m_tanhCumulHistoNonMuonR3_4);
  declareProperty( "tanhCumulHistoNonMuonR3_5", m_tanhCumulHistoNonMuonR3_5);

  declareProperty( "tanhCumulHistoNonMuonR4_1", m_tanhCumulHistoNonMuonR4_1);
  declareProperty( "tanhCumulHistoNonMuonR4_2", m_tanhCumulHistoNonMuonR4_2);
  declareProperty( "tanhCumulHistoNonMuonR4_3", m_tanhCumulHistoNonMuonR4_3);
  declareProperty( "tanhCumulHistoNonMuonR4_4", m_tanhCumulHistoNonMuonR4_4);
  declareProperty( "tanhCumulHistoNonMuonR4_5", m_tanhCumulHistoNonMuonR4_5);

}

//=============================================================================
// Destructor
//=============================================================================
MuonIDAlg::~MuonIDAlg() {}

//=============================================================================
// Initialisation. Check parameters
//=============================================================================
StatusCode MuonIDAlg::initialize() {

  // Sets up various tools and services
  const StatusCode sc = GaudiAlgorithm::initialize();
  if ( sc.isFailure() ) { return sc; }

  info()  << "==> Initialise: Input tracks in: " << m_TracksPath << endmsg;
  info()  << "                Output MuonPID in: " << m_MuonPIDsPath<< endmsg;

  // Check the presence of global MuonID parameters in the loaded conditions database
  if(existDet<DataObject>(detSvc(),"Conditions/ParticleID/Muon/PreSelMomentum" )){
    if (msgLevel(MSG::DEBUG) ) debug()  << "Initialise: Conditions database with muon ID info found"  << endmsg;

    if(m_OverrideDB){  // Keep values from options file. Ignore database data
      info() << "Initialise: OverrideDB=true. Data from Conditions database will be ignored." << endmsg;
      info() << "Initialise:                  Using values read from options file" << endmsg;
    }
    else {  // Use database values instead of options file ones.
      // THE CONDITIONS AND THEIR REGISTERING SHOULD BE DONE AFTER GaudiAlgorithm::initialize() !!
      
      //Global Muon ParticleID Conditions from database
      Condition * PreSelMomentum;
      Condition * MomentumCuts;
      Condition * XFOIParameters;
      Condition * YFOIParameters;
      Condition * FOIfactor;
      Condition * DLL_flag;

      // Register condition and read parameters values
      registerCondition<MuonIDAlg>("Conditions/ParticleID/Muon/PreSelMomentum", PreSelMomentum);
      registerCondition<MuonIDAlg>("Conditions/ParticleID/Muon/MomentumCuts", MomentumCuts);
      registerCondition<MuonIDAlg>("Conditions/ParticleID/Muon/XFOIParameters", XFOIParameters);
      registerCondition<MuonIDAlg>("Conditions/ParticleID/Muon/YFOIParameters", YFOIParameters);
      registerCondition<MuonIDAlg>("Conditions/ParticleID/Muon/FOIfactor", FOIfactor);
      registerCondition<MuonIDAlg>("Conditions/ParticleID/Muon/DLL_flag", DLL_flag);

      // perform an update without waiting the next BeginEvent incident.
      // Needed before the param values can be read
      StatusCode scupdMgr = runUpdate();
      if ( scupdMgr.isFailure() ) return Error(" Unable to update global conditions from database ",scupdMgr);


      // Now read parameters values
      if (msgLevel(MSG::DEBUG) ) debug()  << "Initialise: Reading global parameters from conditions database:"  << endmsg;
      m_PreSelMomentum = PreSelMomentum->param<double>("PreSelMomentum");
      if (msgLevel(MSG::DEBUG) ) debug()  << "==> PreSelMomentum:" << m_PreSelMomentum << endmsg;

      // Different depths of stations considered in different momentum ranges
      m_MomentumCuts = MomentumCuts->paramVect<double>("MomentumCuts");
      if (msgLevel(MSG::DEBUG) ) debug()  << "==> MomentumCuts:" << m_MomentumCuts << endmsg;

      // function that defines the field of interest size
      // here momentum is scaled to Gaudi::Units::GeV....
      // new formula: p(1) + p(2)*momentum + p(3)*exp(-p(4)*momentum)
      m_xfoiParam1 = XFOIParameters->paramVect<double>("XFOIParameters1");
      if (msgLevel(MSG::DEBUG) ) debug()  << "==> XFOIParameters1:" << m_xfoiParam1 << endmsg;
      m_xfoiParam2 = XFOIParameters->paramVect<double>("XFOIParameters2");
      if (msgLevel(MSG::DEBUG) ) debug()  << "==> XFOIParameters2:" << m_xfoiParam2 << endmsg;
      m_xfoiParam3 = XFOIParameters->paramVect<double>("XFOIParameters3");
      if (msgLevel(MSG::DEBUG) ) debug()  << "==> XFOIParameters3:" << m_xfoiParam3 << endmsg;

      m_yfoiParam1 = YFOIParameters->paramVect<double>("YFOIParameters1");
      if (msgLevel(MSG::DEBUG) ) debug()  << "==> YFOIParameters1:" << m_yfoiParam1 << endmsg;
      m_yfoiParam2 = YFOIParameters->paramVect<double>("YFOIParameters2");
      if (msgLevel(MSG::DEBUG) ) debug()  << "==> YFOIParameters2:" << m_yfoiParam2 << endmsg;
      m_yfoiParam3 = YFOIParameters->paramVect<double>("YFOIParameters3");
      if (msgLevel(MSG::DEBUG) ) debug()  << "==> YFOIParameters3:" << m_yfoiParam3 << endmsg;

      m_foifactor = FOIfactor->param<double>("FOIfactor");
      if (msgLevel(MSG::DEBUG) ) debug()  << "==> FOIfactor:" << m_foifactor << endmsg;

      int dllFlag  = DLL_flag->param<int>("DLL_flag");
      if(dllFlag != m_dllFlag) info() << "Initialise: OverrideDB=false but dllFlag in options file (="
                                      << m_dllFlag << ") not equal dllFlag in database (="  << dllFlag 
   				      << "). Using dllFlag from options file" << endmsg;
      //m_dllFlag = dllFlag;
      if (msgLevel(MSG::DEBUG) ) debug()  << "==> DLL_flag:" << m_dllFlag << endmsg;

      if(1 == m_dllFlag){
      
      // Check the presence of a MuonID parameters for dllFlag==1 in the loaded conditions database
      if(existDet<DataObject>(detSvc(),"Conditions/ParticleID/Muon/MuLandauParameterR1" )){
        if (msgLevel(MSG::DEBUG) ) debug()  << "Initialise: Conditions database with muon ID info for  DLL_flag == 1 found"  
                                            << endmsg;
    
      // Muon ParticleID Conditions for DLL_flag = 1 from database
        Condition * nMupBins;
        Condition * MupBins;

        Condition * MuLandauParameterR1;
        Condition * MuLandauParameterR2;
        Condition * MuLandauParameterR3;
        Condition * MuLandauParameterR4;

        Condition * NonMuLandauParameterR1;
        Condition * NonMuLandauParameterR2;
        Condition * NonMuLandauParameterR3;
        Condition * NonMuLandauParameterR4;

        Condition * step;
        Condition * nMax_bin;

        // Register condition and read parameters values
        registerCondition<MuonIDAlg>("Conditions/ParticleID/Muon/nMupBins", nMupBins);
        registerCondition<MuonIDAlg>("Conditions/ParticleID/Muon/MupBins", MupBins);
        registerCondition<MuonIDAlg>("Conditions/ParticleID/Muon/MuLandauParameterR1", MuLandauParameterR1);
        registerCondition<MuonIDAlg>("Conditions/ParticleID/Muon/MuLandauParameterR2", MuLandauParameterR2);
        registerCondition<MuonIDAlg>("Conditions/ParticleID/Muon/MuLandauParameterR3", MuLandauParameterR3);
        registerCondition<MuonIDAlg>("Conditions/ParticleID/Muon/MuLandauParameterR4", MuLandauParameterR4);
        registerCondition<MuonIDAlg>("Conditions/ParticleID/Muon/NonMuLandauParameterR1",NonMuLandauParameterR1);
        registerCondition<MuonIDAlg>("Conditions/ParticleID/Muon/NonMuLandauParameterR2",NonMuLandauParameterR2);
        registerCondition<MuonIDAlg>("Conditions/ParticleID/Muon/NonMuLandauParameterR3",NonMuLandauParameterR3);
        registerCondition<MuonIDAlg>("Conditions/ParticleID/Muon/NonMuLandauParameterR4",NonMuLandauParameterR4);
        registerCondition<MuonIDAlg>("Conditions/ParticleID/Muon/step", step);
        registerCondition<MuonIDAlg>("Conditions/ParticleID/Muon/nMax_bin", nMax_bin);

        // perform an update without waiting the next BeginEvent incident.
        // Needed before the param values can be read
        scupdMgr = runUpdate();
        if ( scupdMgr.isFailure() )
          return Error(" Unable to update DLL_flag = 1 conditions from database ",scupdMgr);

        if (msgLevel(MSG::DEBUG) ) debug()  << "Initialise: Reading parameters for DLL_flag=1 mode from conditions database:"  
                                            << endmsg;
        m_nMupBinsR1 = nMupBins->param<int>("nMupBinsR1");
        if (msgLevel(MSG::VERBOSE) ) verbose()  << "==> nMupBinsR1:" << m_nMupBinsR1 << endmsg;
        m_nMupBinsR2 = nMupBins->param<int>("nMupBinsR2");
        if (msgLevel(MSG::VERBOSE) ) verbose()  << "==> nMupBinsR2:" << m_nMupBinsR2 << endmsg;
        m_nMupBinsR3 = nMupBins->param<int>("nMupBinsR3");
        if (msgLevel(MSG::VERBOSE) ) verbose()  << "==> nMupBinsR3:" << m_nMupBinsR3 << endmsg;
        m_nMupBinsR4 = nMupBins->param<int>("nMupBinsR4");
        if (msgLevel(MSG::VERBOSE) ) verbose()  << "==> nMupBinsR4:" << m_nMupBinsR4 << endmsg;

        m_MupBinsR1 = MupBins->paramVect<double>("MupBinsR1");
        if (msgLevel(MSG::VERBOSE) ) verbose()  << "==> MupBinsR1:" << m_MupBinsR1 << endmsg;
        m_MupBinsR2 = MupBins->paramVect<double>("MupBinsR2");
        if (msgLevel(MSG::VERBOSE) ) verbose()  << "==> MupBinsR2:" << m_MupBinsR2 << endmsg;
        m_MupBinsR3 = MupBins->paramVect<double>("MupBinsR3");
        if (msgLevel(MSG::VERBOSE) ) verbose()  << "==> MupBinsR3:" << m_MupBinsR3 << endmsg;
        m_MupBinsR4 = MupBins->paramVect<double>("MupBinsR4");
        if (msgLevel(MSG::VERBOSE) ) verbose()  << "==> MupBinsR4:" << m_MupBinsR4 << endmsg;

        // Muons - Region1:
        m_MuLanParR1_1 = MuLandauParameterR1->paramVect<double>("MuLandauParameterR1_1");
        if (msgLevel(MSG::VERBOSE) ) verbose()  << "==> MuLandauParameterR1_1:" << m_MuLanParR1_1 << endmsg;
        m_MuLanParR1_2 = MuLandauParameterR1->paramVect<double>("MuLandauParameterR1_2");
        if (msgLevel(MSG::VERBOSE) ) verbose()  << "==> MuLandauParameterR1_2:" << m_MuLanParR1_2 << endmsg;
        m_MuLanParR1_3 = MuLandauParameterR1->paramVect<double>("MuLandauParameterR1_3");
        if (msgLevel(MSG::VERBOSE) ) verbose()  << "==> MuLandauParameterR1_3:" << m_MuLanParR1_3 << endmsg;
        m_MuLanParR1_4 = MuLandauParameterR1->paramVect<double>("MuLandauParameterR1_4");
        if (msgLevel(MSG::VERBOSE) ) verbose()  << "==> MuLandauParameterR1_4:" << m_MuLanParR1_4 << endmsg;
        m_MuLanParR1_5 = MuLandauParameterR1->paramVect<double>("MuLandauParameterR1_5");
        if (msgLevel(MSG::VERBOSE) ) verbose()  << "==> MuLandauParameterR1_5:" << m_MuLanParR1_5 << endmsg;
        m_MuLanParR1_6 = MuLandauParameterR1->paramVect<double>("MuLandauParameterR1_6");
        if (msgLevel(MSG::VERBOSE) ) verbose()  << "==> MuLandauParameterR1_6:" << m_MuLanParR1_6 << endmsg;
        m_MuLanParR1_7 = MuLandauParameterR1->paramVect<double>("MuLandauParameterR1_7");
        if (msgLevel(MSG::VERBOSE) ) verbose()  << "==> MuLandauParameterR1_7:" << m_MuLanParR1_7 << endmsg;

        // Muons - Region2:
        m_MuLanParR2_1 = MuLandauParameterR2->paramVect<double>("MuLandauParameterR2_1");
        if (msgLevel(MSG::VERBOSE) ) verbose()  << "==> MuLandauParameterR2_1:" << m_MuLanParR2_1 << endmsg;
        m_MuLanParR2_2 = MuLandauParameterR2->paramVect<double>("MuLandauParameterR2_2");
        if (msgLevel(MSG::VERBOSE) ) verbose()  << "==> MuLandauParameterR2_2:" << m_MuLanParR2_2 << endmsg;
        m_MuLanParR2_3 = MuLandauParameterR2->paramVect<double>("MuLandauParameterR2_3");
        if (msgLevel(MSG::VERBOSE) ) verbose()  << "==> MuLandauParameterR2_3:" << m_MuLanParR2_3 << endmsg;
        m_MuLanParR2_4 = MuLandauParameterR2->paramVect<double>("MuLandauParameterR2_4");
        if (msgLevel(MSG::VERBOSE) ) verbose()  << "==> MuLandauParameterR2_4:" << m_MuLanParR2_4 << endmsg;
        m_MuLanParR2_5 = MuLandauParameterR2->paramVect<double>("MuLandauParameterR2_5");
        if (msgLevel(MSG::VERBOSE) ) verbose()  << "==> MuLandauParameterR2_5:" << m_MuLanParR2_5 << endmsg;

        // Muons - Region3:
        m_MuLanParR3_1 = MuLandauParameterR3->paramVect<double>("MuLandauParameterR3_1");
        if (msgLevel(MSG::VERBOSE) ) verbose()  << "==> MuLandauParameterR3_1:" << m_MuLanParR3_1 << endmsg;
        m_MuLanParR3_2 = MuLandauParameterR3->paramVect<double>("MuLandauParameterR3_2");
        if (msgLevel(MSG::VERBOSE) ) verbose()  << "==> MuLandauParameterR3_2:" << m_MuLanParR3_2 << endmsg;
        m_MuLanParR3_3 = MuLandauParameterR3->paramVect<double>("MuLandauParameterR3_3");
        if (msgLevel(MSG::VERBOSE) ) verbose()  << "==> MuLandauParameterR3_3:" << m_MuLanParR3_3 << endmsg;
        m_MuLanParR3_4 = MuLandauParameterR3->paramVect<double>("MuLandauParameterR3_4");
        if (msgLevel(MSG::VERBOSE) ) verbose()  << "==> MuLandauParameterR3_4:" << m_MuLanParR3_4 << endmsg;
        m_MuLanParR3_5 = MuLandauParameterR3->paramVect<double>("MuLandauParameterR3_5");
        if (msgLevel(MSG::VERBOSE) ) verbose()  << "==> MuLandauParameterR3_5:" << m_MuLanParR3_5 << endmsg;

        // Muons - Region4:
        m_MuLanParR4_1 = MuLandauParameterR4->paramVect<double>("MuLandauParameterR4_1");
        if (msgLevel(MSG::VERBOSE) ) verbose()  << "==> MuLandauParameterR4_1:" << m_MuLanParR4_1 << endmsg;
        m_MuLanParR4_2 = MuLandauParameterR4->paramVect<double>("MuLandauParameterR4_2");
        if (msgLevel(MSG::VERBOSE) ) verbose()  << "==> MuLandauParameterR4_2:" << m_MuLanParR4_2 << endmsg;
        m_MuLanParR4_3 = MuLandauParameterR4->paramVect<double>("MuLandauParameterR4_3");
        if (msgLevel(MSG::VERBOSE) ) verbose()  << "==> MuLandauParameterR4_3:" << m_MuLanParR4_3 << endmsg;
        m_MuLanParR4_4 = MuLandauParameterR4->paramVect<double>("MuLandauParameterR4_4");
        if (msgLevel(MSG::VERBOSE) ) verbose()  << "==> MuLandauParameterR4_4:" << m_MuLanParR4_4 << endmsg;
        m_MuLanParR4_5 = MuLandauParameterR4->paramVect<double>("MuLandauParameterR4_5");
        if (msgLevel(MSG::VERBOSE) ) verbose()  << "==> MuLandauParameterR4_5:" << m_MuLanParR4_5 << endmsg;


        // Non-Muons - Region 1-2-3-4:
        m_NonMuLanParR1 = NonMuLandauParameterR1->paramVect<double>("NonMuLandauParameterR1");
        if (msgLevel(MSG::VERBOSE) ) verbose()  << "==> NonMuLandauParameterR1:" << m_NonMuLanParR1 << endmsg;
        m_NonMuLanParR2 = NonMuLandauParameterR2->paramVect<double>("NonMuLandauParameterR2");
        if (msgLevel(MSG::VERBOSE) ) verbose()  << "==> NonMuLandauParameterR2:" << m_NonMuLanParR2 << endmsg;
        m_NonMuLanParR3 = NonMuLandauParameterR3->paramVect<double>("NonMuLandauParameterR3");
        if (msgLevel(MSG::VERBOSE) ) verbose()  << "==> NonMuLandauParameterR3:" << m_NonMuLanParR3 << endmsg;
        m_NonMuLanParR4 = NonMuLandauParameterR4->paramVect<double>("NonMuLandauParameterR4");
        if (msgLevel(MSG::VERBOSE) ) verbose()  << "==> NonMuLandauParameterR4:" << m_NonMuLanParR4 << endmsg;

        // step for the integral
        m_x = step->param<double>("step");
        if (msgLevel(MSG::DEBUG) ) debug()  << "==> step:" << m_x << endmsg;

        // number of steps
        m_nMax = nMax_bin->param<int>("nMax_bin");
        if (msgLevel(MSG::DEBUG) ) {
          debug()  << "==> nMax_bin:" << m_nMax << endmsg;
          debug()  << endmsg;
	}
      }
      else {  
        warning()  << "Initialise: Parameters for dllFlag==1 not found in Conditions database."
                   << " Using values from options file"  << endmsg;
      }	
      }
      else if(4 == m_dllFlag){
      // Muon ParticleID Conditions for DLL_flag = 4 from database

      // Check the presence of a MuonID parameters for dllFlag==4 in the loaded conditions database
      if(existDet<DataObject>(detSvc(),"Conditions/ParticleID/Muon/tanhScaleFactors" )){
        if (msgLevel(MSG::DEBUG) ) debug()  << 
          "Initialise: Conditions database with muon ID info for dllFlag==4 (ProbMu from tanh(dist) ) found"  << endmsg;
        Condition *  nMupBins;
        Condition *  MupBins;     

        registerCondition<MuonIDAlg>("Conditions/ParticleID/Muon/nMupBins", nMupBins);
        registerCondition<MuonIDAlg>("Conditions/ParticleID/Muon/MupBins", MupBins);
	
        Condition *  tanhScaleFactors;
        registerCondition<MuonIDAlg>("Conditions/ParticleID/Muon/tanhScaleFactors", tanhScaleFactors);

        Condition *  tanhCumulHistoMuonR1;
        registerCondition<MuonIDAlg>("Conditions/ParticleID/Muon/tanhCumulHistoMuonR1", tanhCumulHistoMuonR1);
	
        Condition *  tanhCumulHistoMuonR2;
        registerCondition<MuonIDAlg>("Conditions/ParticleID/Muon/tanhCumulHistoMuonR2", tanhCumulHistoMuonR2);

        Condition *  tanhCumulHistoMuonR3;
        registerCondition<MuonIDAlg>("Conditions/ParticleID/Muon/tanhCumulHistoMuonR3", tanhCumulHistoMuonR3);

        Condition *  tanhCumulHistoMuonR4;	
        registerCondition<MuonIDAlg>("Conditions/ParticleID/Muon/tanhCumulHistoMuonR4", tanhCumulHistoMuonR4);
	
        // perform an update without waiting the next BeginEvent incident.
        // Needed before the param values can be read
        scupdMgr = runUpdate();
        if ( scupdMgr.isFailure() )
          return Error(" Unable to update DLL_flag = 4 tanh conditions from database ",scupdMgr);

        if (msgLevel(MSG::DEBUG) ) debug()  << "Initialise: Reading parameters for DLL_flag=4 mode from conditions database:"  
                                            << endmsg;
        m_nMupBinsR1 = nMupBins->param<int>("nMupBinsR1");
        if (msgLevel(MSG::DEBUG) ) debug()  << "==> nMupBinsR1:" << m_nMupBinsR1 << endmsg;
        m_nMupBinsR2 = nMupBins->param<int>("nMupBinsR2");
        if (msgLevel(MSG::DEBUG) ) debug()  << "==> nMupBinsR2:" << m_nMupBinsR2 << endmsg;
        m_nMupBinsR3 = nMupBins->param<int>("nMupBinsR3");
        if (msgLevel(MSG::DEBUG) ) debug()  << "==> nMupBinsR3:" << m_nMupBinsR3 << endmsg;
        m_nMupBinsR4 = nMupBins->param<int>("nMupBinsR4");
        if (msgLevel(MSG::DEBUG) ) debug()  << "==> nMupBinsR4:" << m_nMupBinsR4 << endmsg;

        m_MupBinsR1 = MupBins->paramVect<double>("MupBinsR1");
        if (msgLevel(MSG::DEBUG) ) debug()  << "==> MupBinsR1:" << m_MupBinsR1 << endmsg;
        m_MupBinsR2 = MupBins->paramVect<double>("MupBinsR2");
        if (msgLevel(MSG::DEBUG) ) debug()  << "==> MupBinsR2:" << m_MupBinsR2 << endmsg;
        m_MupBinsR3 = MupBins->paramVect<double>("MupBinsR3");
        if (msgLevel(MSG::DEBUG) ) debug()  << "==> MupBinsR3:" << m_MupBinsR3 << endmsg;
        m_MupBinsR4 = MupBins->paramVect<double>("MupBinsR4");
        if (msgLevel(MSG::DEBUG) ) debug()  << "==> MupBinsR4:" << m_MupBinsR4 << endmsg;
	
	m_tanhScaleFactorsR1 = tanhScaleFactors->paramVect<double>("tanhScaleFactorsR1");
        if (msgLevel(MSG::VERBOSE) ) verbose()  << "==> tanhScaleFactorsR1:" << m_tanhScaleFactorsR1 << endmsg;
	m_tanhScaleFactorsR2 = tanhScaleFactors->paramVect<double>("tanhScaleFactorsR2");
        if (msgLevel(MSG::VERBOSE) ) verbose()  << "==> tanhScaleFactorsR2:" << m_tanhScaleFactorsR2 << endmsg;
	m_tanhScaleFactorsR3 = tanhScaleFactors->paramVect<double>("tanhScaleFactorsR3");
        if (msgLevel(MSG::VERBOSE) ) verbose()  << "==> tanhScaleFactorsR3:" << m_tanhScaleFactorsR3 << endmsg;
	m_tanhScaleFactorsR4 = tanhScaleFactors->paramVect<double>("tanhScaleFactorsR4");
        if (msgLevel(MSG::VERBOSE) ) verbose()  << "==> tanhScaleFactorsR4:" << m_tanhScaleFactorsR4 << endmsg;
	
	m_tanhCumulHistoMuonR1_1 = tanhCumulHistoMuonR1->paramVect<double>("tanhCumulHistoMuonR1_1");
        if (msgLevel(MSG::VERBOSE) ) verbose()  << "==> tanhCumulHistoMuonR1_1:" << m_tanhCumulHistoMuonR1_1 << endmsg;
	m_tanhCumulHistoMuonR1_2 = tanhCumulHistoMuonR1->paramVect<double>("tanhCumulHistoMuonR1_2");
        if (msgLevel(MSG::VERBOSE) ) verbose()  << "==> tanhCumulHistoMuonR1_2:" << m_tanhCumulHistoMuonR1_2 << endmsg;
	m_tanhCumulHistoMuonR1_3 = tanhCumulHistoMuonR1->paramVect<double>("tanhCumulHistoMuonR1_3");
        if (msgLevel(MSG::VERBOSE) ) verbose()  << "==> tanhCumulHistoMuonR1_3:" << m_tanhCumulHistoMuonR1_3 << endmsg;
	m_tanhCumulHistoMuonR1_4 = tanhCumulHistoMuonR1->paramVect<double>("tanhCumulHistoMuonR1_4");
        if (msgLevel(MSG::VERBOSE) ) verbose()  << "==> tanhCumulHistoMuonR1_4:" << m_tanhCumulHistoMuonR1_4 << endmsg;
	m_tanhCumulHistoMuonR1_5 = tanhCumulHistoMuonR1->paramVect<double>("tanhCumulHistoMuonR1_5");
        if (msgLevel(MSG::VERBOSE) ) verbose()  << "==> tanhCumulHistoMuonR1_5:" << m_tanhCumulHistoMuonR1_5 << endmsg;
	m_tanhCumulHistoMuonR1_6 = tanhCumulHistoMuonR1->paramVect<double>("tanhCumulHistoMuonR1_6");
        if (msgLevel(MSG::VERBOSE) ) verbose()  << "==> tanhCumulHistoMuonR1_6:" << m_tanhCumulHistoMuonR1_6 << endmsg;
	m_tanhCumulHistoMuonR1_7 = tanhCumulHistoMuonR1->paramVect<double>("tanhCumulHistoMuonR1_7");
        if (msgLevel(MSG::VERBOSE) ) verbose()  << "==> tanhCumulHistoMuonR1_7:" << m_tanhCumulHistoMuonR1_7 << endmsg;

	m_tanhCumulHistoMuonR2_1 = tanhCumulHistoMuonR2->paramVect<double>("tanhCumulHistoMuonR2_1");
        if (msgLevel(MSG::VERBOSE) ) verbose()  << "==> tanhCumulHistoMuonR2_1:" << m_tanhCumulHistoMuonR2_1 << endmsg;
	m_tanhCumulHistoMuonR2_2 = tanhCumulHistoMuonR2->paramVect<double>("tanhCumulHistoMuonR2_2");
        if (msgLevel(MSG::VERBOSE) ) verbose()  << "==> tanhCumulHistoMuonR2_2:" << m_tanhCumulHistoMuonR2_2 << endmsg;
	m_tanhCumulHistoMuonR2_3 = tanhCumulHistoMuonR2->paramVect<double>("tanhCumulHistoMuonR2_3");
        if (msgLevel(MSG::VERBOSE) ) verbose()  << "==> tanhCumulHistoMuonR2_3:" << m_tanhCumulHistoMuonR2_3 << endmsg;
	m_tanhCumulHistoMuonR2_4 = tanhCumulHistoMuonR2->paramVect<double>("tanhCumulHistoMuonR2_4");
        if (msgLevel(MSG::VERBOSE) ) verbose()  << "==> tanhCumulHistoMuonR2_4:" << m_tanhCumulHistoMuonR2_4 << endmsg;
	m_tanhCumulHistoMuonR2_5 = tanhCumulHistoMuonR2->paramVect<double>("tanhCumulHistoMuonR2_5");
        if (msgLevel(MSG::VERBOSE) ) verbose()  << "==> tanhCumulHistoMuonR2_5:" << m_tanhCumulHistoMuonR2_5 << endmsg;

	m_tanhCumulHistoMuonR3_1 = tanhCumulHistoMuonR3->paramVect<double>("tanhCumulHistoMuonR3_1");
        if (msgLevel(MSG::VERBOSE) ) verbose()  << "==> tanhCumulHistoMuonR3_1:" << m_tanhCumulHistoMuonR3_1 << endmsg;
	m_tanhCumulHistoMuonR3_2 = tanhCumulHistoMuonR3->paramVect<double>("tanhCumulHistoMuonR3_2");
        if (msgLevel(MSG::VERBOSE) ) verbose()  << "==> tanhCumulHistoMuonR3_2:" << m_tanhCumulHistoMuonR3_2 << endmsg;
	m_tanhCumulHistoMuonR3_3 = tanhCumulHistoMuonR3->paramVect<double>("tanhCumulHistoMuonR3_3");
        if (msgLevel(MSG::VERBOSE) ) verbose()  << "==> tanhCumulHistoMuonR3_3:" << m_tanhCumulHistoMuonR3_3 << endmsg;
	m_tanhCumulHistoMuonR3_4 = tanhCumulHistoMuonR3->paramVect<double>("tanhCumulHistoMuonR3_4");
        if (msgLevel(MSG::VERBOSE) ) verbose()  << "==> tanhCumulHistoMuonR3_4:" << m_tanhCumulHistoMuonR3_4 << endmsg;
	m_tanhCumulHistoMuonR3_5 = tanhCumulHistoMuonR3->paramVect<double>("tanhCumulHistoMuonR3_5");
        if (msgLevel(MSG::VERBOSE) ) verbose()  << "==> tanhCumulHistoMuonR3_5:" << m_tanhCumulHistoMuonR3_5 << endmsg;

	m_tanhCumulHistoMuonR4_1 = tanhCumulHistoMuonR4->paramVect<double>("tanhCumulHistoMuonR4_1");
        if (msgLevel(MSG::VERBOSE) ) verbose()  << "==> tanhCumulHistoMuonR4_1:" << m_tanhCumulHistoMuonR4_1 << endmsg;
	m_tanhCumulHistoMuonR4_2 = tanhCumulHistoMuonR4->paramVect<double>("tanhCumulHistoMuonR4_2");
        if (msgLevel(MSG::VERBOSE) ) verbose()  << "==> tanhCumulHistoMuonR4_2:" << m_tanhCumulHistoMuonR4_2 << endmsg;
	m_tanhCumulHistoMuonR4_3 = tanhCumulHistoMuonR4->paramVect<double>("tanhCumulHistoMuonR4_3");
        if (msgLevel(MSG::VERBOSE) ) verbose()  << "==> tanhCumulHistoMuonR4_3:" << m_tanhCumulHistoMuonR4_3 << endmsg;
	m_tanhCumulHistoMuonR4_4 = tanhCumulHistoMuonR4->paramVect<double>("tanhCumulHistoMuonR4_4");
        if (msgLevel(MSG::VERBOSE) ) verbose()  << "==> tanhCumulHistoMuonR4_4:" << m_tanhCumulHistoMuonR4_4 << endmsg;
	m_tanhCumulHistoMuonR4_5 = tanhCumulHistoMuonR4->paramVect<double>("tanhCumulHistoMuonR4_5");
        if (msgLevel(MSG::VERBOSE) ) verbose()  << "==> tanhCumulHistoMuonR4_5:" << m_tanhCumulHistoMuonR4_5 << endmsg;
        }
        else {  
          warning()  << "Initialise: Parameters for dllFlag==4 MuonProb not found in Conditions database."
                     << " Using values from options file"  << endmsg;
        }	
	
	// For the non-muon hypothesis, use the previous Landau fitings, as in dllFlag==1
        // Check the presence of a MuonID parameters for dllFlag==1 in the loaded conditions database
        if(existDet<DataObject>(detSvc(),"Conditions/ParticleID/Muon/NonMuLandauParameterR1" )){
        if (msgLevel(MSG::DEBUG) ) debug()  
          << "Initialise: Conditions database with muon ID info (ProbNonMu from previous Landau fits ) found"  << endmsg;
	
        Condition * NonMuLandauParameterR1;
        Condition * NonMuLandauParameterR2;
        Condition * NonMuLandauParameterR3;
        Condition * NonMuLandauParameterR4;

        Condition * step;
        Condition * nMax_bin;

        registerCondition<MuonIDAlg>("Conditions/ParticleID/Muon/NonMuLandauParameterR1",NonMuLandauParameterR1);
        registerCondition<MuonIDAlg>("Conditions/ParticleID/Muon/NonMuLandauParameterR2",NonMuLandauParameterR2);
        registerCondition<MuonIDAlg>("Conditions/ParticleID/Muon/NonMuLandauParameterR3",NonMuLandauParameterR3);
        registerCondition<MuonIDAlg>("Conditions/ParticleID/Muon/NonMuLandauParameterR4",NonMuLandauParameterR4);
        registerCondition<MuonIDAlg>("Conditions/ParticleID/Muon/step", step);
        registerCondition<MuonIDAlg>("Conditions/ParticleID/Muon/nMax_bin", nMax_bin);

        // perform an update without waiting the next BeginEvent incident.
        // Needed before the param values can be read
        scupdMgr = runUpdate();
        if ( scupdMgr.isFailure() )
          return Error(" Unable to update DLL_flag = 4  conditions from database ",scupdMgr);
	  
        // Non-Muons - Region 1-2-3-4:
        m_NonMuLanParR1 = NonMuLandauParameterR1->paramVect<double>("NonMuLandauParameterR1");
        if (msgLevel(MSG::DEBUG) ) debug()  << "==> NonMuLandauParameterR1:" << m_NonMuLanParR1 << endmsg;
        m_NonMuLanParR2 = NonMuLandauParameterR2->paramVect<double>("NonMuLandauParameterR2");
        if (msgLevel(MSG::DEBUG) ) debug()  << "==> NonMuLandauParameterR2:" << m_NonMuLanParR2 << endmsg;
        m_NonMuLanParR3 = NonMuLandauParameterR3->paramVect<double>("NonMuLandauParameterR3");
        if (msgLevel(MSG::DEBUG) ) debug()  << "==> NonMuLandauParameterR3:" << m_NonMuLanParR3 << endmsg;
        m_NonMuLanParR4 = NonMuLandauParameterR4->paramVect<double>("NonMuLandauParameterR4");
        if (msgLevel(MSG::DEBUG) ) debug()  << "==> NonMuLandauParameterR4:" << m_NonMuLanParR4 << endmsg;

        // step for the integral
        m_x = step->param<double>("step");
        if (msgLevel(MSG::DEBUG) ) debug()  << "==> step:" << m_x << endmsg;

        // number of steps
        m_nMax = nMax_bin->param<int>("nMax_bin");
        if (msgLevel(MSG::DEBUG) ) {
          debug()  << "==> nMax_bin:" << m_nMax << endmsg;
          debug()  << endmsg;
        }
        
        }
        else {  
          warning()  << "Initialise: Parameters for dllFlag==4 NonMuonProb not found in Conditions database."
                     << " Using values from options file"  << endmsg;
        }	
	
      }
      else {
        error() << "Initialise: OverrideDB=False but no parameters in database for dllFlag = " 
                << m_dllFlag << endmsg;
        return Error("Initialise: Failed to configure MuonID dll parameters");
      }
    }
  }
  else {
    warning()  << "Initialise: Conditions database with muon ID info not found."
               << " Using values from options file"  << endmsg;
  }

// Associate tanh parameters with more generic containers
  m_tanhScaleFactors.push_back(&m_tanhScaleFactorsR1);
  m_tanhScaleFactors.push_back(&m_tanhScaleFactorsR2);
  m_tanhScaleFactors.push_back(&m_tanhScaleFactorsR3);
  m_tanhScaleFactors.push_back(&m_tanhScaleFactorsR4);

  m_tanhCumulHistoMuonR1.push_back(&m_tanhCumulHistoMuonR1_1);
  m_tanhCumulHistoMuonR1.push_back(&m_tanhCumulHistoMuonR1_2);
  m_tanhCumulHistoMuonR1.push_back(&m_tanhCumulHistoMuonR1_3);
  m_tanhCumulHistoMuonR1.push_back(&m_tanhCumulHistoMuonR1_4);
  m_tanhCumulHistoMuonR1.push_back(&m_tanhCumulHistoMuonR1_5);
  m_tanhCumulHistoMuonR1.push_back(&m_tanhCumulHistoMuonR1_6);
  m_tanhCumulHistoMuonR1.push_back(&m_tanhCumulHistoMuonR1_7);

  m_tanhCumulHistoMuonR2.push_back(&m_tanhCumulHistoMuonR2_1);
  m_tanhCumulHistoMuonR2.push_back(&m_tanhCumulHistoMuonR2_2);
  m_tanhCumulHistoMuonR2.push_back(&m_tanhCumulHistoMuonR2_3);
  m_tanhCumulHistoMuonR2.push_back(&m_tanhCumulHistoMuonR2_4);
  m_tanhCumulHistoMuonR2.push_back(&m_tanhCumulHistoMuonR2_5);

  m_tanhCumulHistoMuonR3.push_back(&m_tanhCumulHistoMuonR3_1);
  m_tanhCumulHistoMuonR3.push_back(&m_tanhCumulHistoMuonR3_2);
  m_tanhCumulHistoMuonR3.push_back(&m_tanhCumulHistoMuonR3_3);
  m_tanhCumulHistoMuonR3.push_back(&m_tanhCumulHistoMuonR3_4);
  m_tanhCumulHistoMuonR3.push_back(&m_tanhCumulHistoMuonR3_5);

  m_tanhCumulHistoMuonR4.push_back(&m_tanhCumulHistoMuonR4_1);
  m_tanhCumulHistoMuonR4.push_back(&m_tanhCumulHistoMuonR4_2);
  m_tanhCumulHistoMuonR4.push_back(&m_tanhCumulHistoMuonR4_3);
  m_tanhCumulHistoMuonR4.push_back(&m_tanhCumulHistoMuonR4_4);
  m_tanhCumulHistoMuonR4.push_back(&m_tanhCumulHistoMuonR4_5);

  m_tanhCumulHistoMuon.push_back(&m_tanhCumulHistoMuonR1);
  m_tanhCumulHistoMuon.push_back(&m_tanhCumulHistoMuonR2);
  m_tanhCumulHistoMuon.push_back(&m_tanhCumulHistoMuonR3);
  m_tanhCumulHistoMuon.push_back(&m_tanhCumulHistoMuonR4);

  m_tanhCumulHistoNonMuonR1.push_back(&m_tanhCumulHistoNonMuonR1_1);
  m_tanhCumulHistoNonMuonR1.push_back(&m_tanhCumulHistoNonMuonR1_2);
  m_tanhCumulHistoNonMuonR1.push_back(&m_tanhCumulHistoNonMuonR1_3);
  m_tanhCumulHistoNonMuonR1.push_back(&m_tanhCumulHistoNonMuonR1_4);
  m_tanhCumulHistoNonMuonR1.push_back(&m_tanhCumulHistoNonMuonR1_5);
  m_tanhCumulHistoNonMuonR1.push_back(&m_tanhCumulHistoNonMuonR1_6);
  m_tanhCumulHistoNonMuonR1.push_back(&m_tanhCumulHistoNonMuonR1_7);

  m_tanhCumulHistoNonMuonR2.push_back(&m_tanhCumulHistoNonMuonR2_1);
  m_tanhCumulHistoNonMuonR2.push_back(&m_tanhCumulHistoNonMuonR2_2);
  m_tanhCumulHistoNonMuonR2.push_back(&m_tanhCumulHistoNonMuonR2_3);
  m_tanhCumulHistoNonMuonR2.push_back(&m_tanhCumulHistoNonMuonR2_4);
  m_tanhCumulHistoNonMuonR2.push_back(&m_tanhCumulHistoNonMuonR2_5);

  m_tanhCumulHistoNonMuonR3.push_back(&m_tanhCumulHistoNonMuonR3_1);
  m_tanhCumulHistoNonMuonR3.push_back(&m_tanhCumulHistoNonMuonR3_2);
  m_tanhCumulHistoNonMuonR3.push_back(&m_tanhCumulHistoNonMuonR3_3);
  m_tanhCumulHistoNonMuonR3.push_back(&m_tanhCumulHistoNonMuonR3_4);
  m_tanhCumulHistoNonMuonR3.push_back(&m_tanhCumulHistoNonMuonR3_5);

  m_tanhCumulHistoNonMuonR4.push_back(&m_tanhCumulHistoNonMuonR4_1);
  m_tanhCumulHistoNonMuonR4.push_back(&m_tanhCumulHistoNonMuonR4_2);
  m_tanhCumulHistoNonMuonR4.push_back(&m_tanhCumulHistoNonMuonR4_3);
  m_tanhCumulHistoNonMuonR4.push_back(&m_tanhCumulHistoNonMuonR4_4);
  m_tanhCumulHistoNonMuonR4.push_back(&m_tanhCumulHistoNonMuonR4_5);

  m_tanhCumulHistoNonMuon.push_back(&m_tanhCumulHistoNonMuonR1);
  m_tanhCumulHistoNonMuon.push_back(&m_tanhCumulHistoNonMuonR2);
  m_tanhCumulHistoNonMuon.push_back(&m_tanhCumulHistoNonMuonR3);
  m_tanhCumulHistoNonMuon.push_back(&m_tanhCumulHistoNonMuonR4);


  counter("nGoodTracksForMuonID");
  counter("nMuonPIDs");
  counter("nInAcceptance");
  counter("nMomentumPresel");
  counter("nIsMuonLoose");
  counter("nIsMuon");
  counter("nIsMuonTight");
  m_mullfail=0;
  m_NStation = 0;
  m_NRegion = 0;
  MuonBasicGeometry basegeometry( detSvc(),msgSvc());
  m_NStation= basegeometry.getStations();
  m_NRegion = basegeometry.getRegions();
  int i=0;
  while(i<m_NStation){
    m_stationNames.push_back(basegeometry.getStationName(i));
    if (msgLevel(MSG::DEBUG) ) debug()   <<" station "<<i<<" "<<m_stationNames[i]<<endmsg;
    if("M2"==m_stationNames[i]) m_iM2 = i;
    i++;
  }
  //info() << "Index of station M2: " << m_iM2 <<endmsg;

  // If no M1, switch m_FindQuality to false to avoid using Chi2MuIDTool, which was  not fixed to no M1 case JHL 12/10/2013
  if(0 == m_iM2) {
    m_FindQuality = false;
    if (msgLevel(MSG::DEBUG) ) debug()   <<"FindQuality set to false. Chi2MuIDTool not yet fixed to no M1 case" << endmsg;
  }

  // set the size of the local vectors
  m_padSizeX.resize(m_NStation * m_NRegion);
  m_padSizeY.resize(m_NStation * m_NRegion);
  m_regionInnerX.resize(m_NStation);
  m_regionOuterX.resize(m_NStation);
  m_regionInnerY.resize(m_NStation);
  m_regionOuterY.resize(m_NStation);

  // fill local arrays of pad sizes and region sizes
  m_mudet=getDet<DeMuonDetector>("/dd/Structure/LHCb/DownstreamRegion/Muon");
  int station,region;
  for(station = 0 ; station < m_NStation ; station++ ){   // JHLJHL 30/08/2013
    m_regionInnerX[station] = m_mudet->getInnerX(station);
    m_regionOuterX[station] = m_mudet->getOuterX(station);
    m_regionInnerY[station] = m_mudet->getInnerY(station);
    m_regionOuterY[station] = m_mudet->getOuterY(station);
    m_stationZ[station] = m_mudet->getStationZ(station);
    for(region = 0 ; region < m_NRegion ; region++ ){
      int i  = station * m_NRegion + region;
      m_padSizeX[i]=m_mudet->getPadSizeX(station,region);
      m_padSizeY[i]=m_mudet->getPadSizeY(station,region);

      if(m_padSizeX[i]==0){
        return Error( "Muon Chamber Pad Size could not be retrieved !!!" );
      }
    }
  }

  if ( msgLevel(MSG::DEBUG) ) {
    debug()  << "-Geometry information ----------------"<< endmsg;
    for(station = 0 ; station < m_NStation ; station++ ){
      debug()  << "Station " << m_stationNames[station] << " Outer X = "  << m_regionOuterX[station]
               << " Outer Y = " << m_regionOuterY[station] 
               << "\tInner X = "  << m_regionInnerX[station] << " Inner Y = " << m_regionInnerY[station]
               << "\tstationZ = " <<  m_stationZ[station] <<  endmsg;
    }
  }
  

  if( m_MomentumCuts.empty() ||
      m_xfoiParam1.size() != 20 ||   // 20 = 5 stationx x 4 regions// JHLJHL 30/08/2013
      m_xfoiParam2.size() != 20 ||
      m_xfoiParam3.size() != 20 ||
      m_yfoiParam1.size() != 20 ||
      m_yfoiParam2.size() != 20 ||
      m_yfoiParam3.size() != 20){
    return Error(format("OPTIONS initialising MuonID: missing or wrong size for MomentumCuts or foi parameters")); // JHLJHL 30/08/2013

  }

  // Parameters in database or option files assumes five muon stations. In case of no M1, shift the values properly:  JHL 11/10/2013 
  if(0 == m_iM2){
     for(station = 0 ; station < m_NStation ; station++ ){   // m_NStation==4 
        for(region=0; region<m_NRegion; ++region){
           int i  = station * m_NRegion + region;
           int ii = (station + 1) * m_NRegion + region;
           m_xfoiParam1[i] = m_xfoiParam1[ii];
           m_xfoiParam2[i] = m_xfoiParam2[ii];
           m_xfoiParam3[i] = m_xfoiParam3[ii];
           m_yfoiParam1[i] = m_yfoiParam1[ii];
           m_yfoiParam2[i] = m_yfoiParam2[ii];
           m_yfoiParam3[i] = m_yfoiParam3[ii];
        }
     }
     if (msgLevel(MSG::DEBUG) ){
        debug()  << "FOI parameters reset for the case without M1 (last four entries will be ignored)"  <<endmsg;
        debug()  << "==> XFOIParameters1:" << m_xfoiParam1 << endmsg;
        debug()  << "==> XFOIParameters2:" << m_xfoiParam2 << endmsg;
        debug()  << "==> XFOIParameters3:" << m_xfoiParam3 << endmsg;
        debug()  << "==> YFOIParameters1:" << m_yfoiParam1 << endmsg;
        debug()  << "==> YFOIParameters2:" << m_yfoiParam2 << endmsg;
        debug()  << "==> YFOIParameters3:" << m_yfoiParam3 << endmsg;
     }
   }


  if( m_MomentumCuts.size() != 2 )
    return Error(" OPTIONS are wrong: size of MomentumCuts vector is not correct");

  if (msgLevel(MSG::DEBUG) ) {
    debug()  << "MuonIDAlg::  Momentum bins are (MeV/c) " <<endmsg;
    debug()  << " PreSelMomentum = "<<  m_PreSelMomentum << endmsg;
  }

  std::vector<double>::const_iterator iMom;
  if (msgLevel(MSG::DEBUG) ){
    for(iMom = m_MomentumCuts.begin() ; iMom != m_MomentumCuts.end() ; iMom++){
      debug()  << "Mom. cuts = " << *iMom << endmsg ;
    }
  }


  if( m_distPion.size() != 9 || m_distMuon.size() != 9)
    return Error("OPTIONS are wrong: size of m_distPion or m_distMuon vector is not correct");

  // GL&SF: Check that Landaus parameters are properly uploaded:
  m_parLandauMu = 6;
  m_parLandauNonMu = 3;

  if( m_MuLanParR1_1.size() != m_parLandauMu || m_MuLanParR1_2.size() != m_parLandauMu ||
      m_MuLanParR1_3.size() != m_parLandauMu || m_MuLanParR1_4.size() != m_parLandauMu ||
      m_MuLanParR1_5.size() != m_parLandauMu || m_MuLanParR1_6.size() != m_parLandauMu ||
      m_MuLanParR1_5.size() != m_parLandauMu ) {
    return Error("OPTIONS initialising Landau Parameters for muons in R1 are missing");
  }
  if( m_MuLanParR2_1.size() != m_parLandauMu || m_MuLanParR2_2.size() != m_parLandauMu ||
      m_MuLanParR2_3.size() != m_parLandauMu || m_MuLanParR2_4.size() != m_parLandauMu ||
      m_MuLanParR2_5.size() != m_parLandauMu ) {
    return Error("OPTIONS initialising Landau Parameters for muons in R2 are missing");
  }
  if( m_MuLanParR3_1.size() != m_parLandauMu || m_MuLanParR3_2.size() != m_parLandauMu ||
      m_MuLanParR3_3.size() != m_parLandauMu || m_MuLanParR3_4.size() != m_parLandauMu ||
      m_MuLanParR3_5.size() != m_parLandauMu ){
    return Error("OPTIONS initialising Landau Parameters for muons in R3 are missing");
  }
  if( m_MuLanParR4_1.size() != m_parLandauMu || m_MuLanParR4_2.size() != m_parLandauMu ||
      m_MuLanParR4_3.size() != m_parLandauMu || m_MuLanParR4_4.size() != m_parLandauMu ||
      m_MuLanParR4_5.size() != m_parLandauMu ) {
    return Error("OPTIONS initialising Landau Parameters for muons in R4 are missing");
  }

  if( m_NonMuLanParR1.size() != m_parLandauNonMu || m_NonMuLanParR2.size() != m_parLandauNonMu ||
      m_NonMuLanParR3.size() != m_parLandauNonMu || m_NonMuLanParR4.size() != m_parLandauNonMu ) {
    return Error("OPTIONS initialising Landau Parameters for non-muons are missing");
  }

  // GL&SF: Check bin size:
  if( (m_MupBinsR1.size()!=(unsigned)m_nMupBinsR1) || (m_MupBinsR2.size()!=(unsigned)m_nMupBinsR2) ||
      (m_MupBinsR3.size()!=(unsigned)m_nMupBinsR3) || (m_MupBinsR4.size()!=(unsigned)m_nMupBinsR4)){
    return Error("OPTIONS initialising momentum bins for muons are missing");
  }

  // GL&SF: Check that dll flag is properly set:
  if (m_dllFlag==0) info() << " dllFlag=0 -----> DLL standard (old method) " << endmsg;
  if (m_dllFlag==1) info() << " dllFlag=1 -----> DLL  new (binned-integrated method) " << endmsg;
  if (m_dllFlag==3) info() << " dllFlag=3 -----> DLL  new (hyperbolic tangent mapping) " << endmsg;
  if (m_dllFlag==4) info() << " dllFlag=4 -----> DLL with muLL tuned on 2010 data (hyperbolic tangent mapping) and previous nonMuLL (Landau fittings 2009)" 
                           << endmsg;

  if (m_dllFlag<0 || m_dllFlag>4 || m_dllFlag==2)
    return Error("DLL flag set to a not existing value: allowed values are: 0=DLL old, 1=DLL integrated, 3=hyperbolic tangent mapping, 4=new mu prob tuned on 2010 data");
    
  if(3 == m_dllFlag) warning() << "dllFlag = 3 cannot be used with datatype 2010 or 2011. Check that this is not the case." 
                               << endmsg;

  // GL&SF: Check that parameters of the integral are fine
  if ((int)(m_x*m_nMax) !=800) return Error(format("DLL integral cannot be calculated, parameters are wrong: x, N %8.3f, %8.3f",
                                                   m_x,m_nMax));

  // GL&SF: Calculate Landaus normalizations:
  StatusCode sc2 = calcLandauNorm();
  if ( sc2.isFailure() ) return Error(" Normalizations of Landaus not properly set ",sc2);

  // Print tanh(dist2) parameters
  if((4 == m_dllFlag || 3 == m_dllFlag) && msgLevel(MSG::VERBOSE)){
    debug() << "Initialize: tanh(dist2) parameters:" << endmsg;
    debug() << "Initialize: m_nMupBinsR1: " << m_nMupBinsR1 << endmsg;
    for(int iR=0;iR<4;++iR){
      int npBins=m_tanhScaleFactors[iR]->size();
      debug() << "Region " << iR+1 <<  " npBins: " << npBins << endmsg;
      for(int ipBin=0;ipBin<npBins;++ipBin){
        debug() << "    ipBin+1:  " << ipBin+1 << " tanhScaleFactor: "
                << (*m_tanhScaleFactors[iR])[ipBin] << " Muon prob tanh(dist2) bin contents:" << endmsg;
        debug() << "\t";
	int nDistBins = (*(*m_tanhCumulHistoMuon[iR])[ipBin]).size();
        for(int idBin=0;idBin<nDistBins;++idBin){
          debug() << (*(*m_tanhCumulHistoMuon[iR])[ipBin])[idBin] << " ";
        }
        debug() << endmsg;
	if(4 == m_dllFlag) continue;
        debug() <<  " Non Muon prob tanh(dist2) bin contents:" << endmsg;
        debug() << "\t";
	nDistBins = (*(*m_tanhCumulHistoNonMuon[iR])[ipBin]).size();
        for(int idBin=0;idBin<nDistBins;++idBin){
          debug() << (*(*m_tanhCumulHistoNonMuon[iR])[ipBin])[idBin] << " ";
        }
        debug() << endmsg;
      }
    }
  }

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode MuonIDAlg::execute() {

  if (msgLevel(MSG::DEBUG) ) debug()  << "==> Execute" << endmsg;
  LHCb::RecHeader* header=NULL;
  long evt=-1;
  long run=-1;
  if ( msgLevel(MSG::DEBUG) && exist<LHCb::RecHeader>
       (LHCb::RecHeaderLocation::Default)) {
    header = get<LHCb::RecHeader>(LHCb::RecHeaderLocation::Default);
    evt = header->evtNumber();
    run = header->runNumber();
    debug() << "Run " << run << " Event " << evt << endmsg;
  }
  m_nmu = 0;

  //XCV: if kalman foi, this will be done by SmartMuonMeasProvider
  if (!m_kalman_foi){  
    StatusCode sc = fillCoordVectors();
    if(sc.isFailure()){
      return sc;
    }
  }

  LHCb::Tracks* trTracks = get<LHCb::Tracks>(m_TracksPath);

  if (msgLevel(MSG::DEBUG) ) debug()  << "Number of input tracks for MuonID: " << trTracks->size() << endmsg;

  // CRJ : Handle the case MuonPID and MuonTrack data is already on the TES
  LHCb::MuonPIDs * pMuids = getOrCreate<LHCb::MuonPIDs,LHCb::MuonPIDs>(m_MuonPIDsPath);
  if ( !pMuids->empty() )
  {
    pMuids->clear();
    Warning( "MuonPIDs already exist at '" + m_MuonPIDsPath + "' -> Will Replace",
             StatusCode::SUCCESS,1 ).ignore();
  }
  LHCb::Tracks * mutracks = getOrCreate<LHCb::Tracks,LHCb::Tracks>(m_MuonTracksPath);
  if ( !mutracks->empty() )
  {
    mutracks->clear();
    Warning( "Muon Tracks already exist at '" + m_MuonTracksPath + "' -> Will Replace",
             StatusCode::SUCCESS,1 ).ignore();
  }

  // CRJ : Set Muon PID data version
  pMuids->setVersion(1);

  //LHCb::Tracks * mutracks_all = m_DoAllMuonTracks ? new LHCb::Tracks() : 0;
  LHCb::Tracks * mutracks_all = new LHCb::Tracks();

  LHCb::Tracks::const_iterator iTrack;
  for( iTrack = trTracks->begin() ; iTrack != trTracks->end() ; iTrack++){
    // in the clone killed output we want only
    // unique && (matched || forward || downstream)
    if(!(*iTrack)->checkFlag(LHCb::Track::Clone)  &&
       ((*iTrack)->checkType(LHCb::Track::Long) ||
        ((*iTrack)->checkType(LHCb::Track::Ttrack) && m_useTtrack) ||
        (*iTrack)->checkType(LHCb::Track::Downstream))){
 
     counter("nGoodTracksForMuonID")++;

      m_mutrack=LHCb::Track();
      
      if(m_kalman_foi){
        //get first state
         const LHCb::State * state1 = &((*iTrack)->firstState());
         if(state1) m_MomentumPre = state1->p();
         else{
           Warning(" Failed to get 1st state from track ").ignore();
           m_MomentumPre = 0;
         }
         
         // get state closest to M1 for extrapolation
         const LHCb::State * state = &((*iTrack)->closestState(9450.));
         if(state) m_Momentum = state->p();
         else {
           Warning(" Failed to get state from track ").ignore();
           m_Momentum = 0;
         }
      }
      

      //XCV: if kalman foi, this will be done by DistMuonIDTool
      else{
        // do the track extrapolations
        StatusCode sc0 = trackExtrapolate(*iTrack);
        if ( sc0.isFailure() ){
          Warning(" trackExtrapolate failed for track ",StatusCode::SUCCESS,0).ignore();
          if (msgLevel(MSG::DEBUG) ) debug()<< " trackExtrapolate failed for track " << *iTrack << endmsg;
          continue;
        }
      }

      // Do the complete ID, calculating IsMuon and likelihoods
      LHCb::MuonPID * pMuid = new LHCb::MuonPID;
      pMuid->setIDTrack(*iTrack);
      StatusCode sc = doID(pMuid);
      if(sc.isFailure()){
        Warning(" doID failed for track ",StatusCode::SUCCESS,0).ignore();
        if (msgLevel(MSG::DEBUG) ) debug()<< " doID failed for track " << *iTrack << endmsg;
      }

      // Save MuonPIDs only if track is in acceptance, unless DoAllMuonTracks is true
      if(!m_DoAllMuonTracks && (!pMuid->InAcceptance())){
        if (msgLevel(MSG::DEBUG) ) debug()<< " Track with key " << (*iTrack)->key() << " not in acceptance. Not saving MuonPID"  << endmsg;
        delete pMuid;
        continue;
      }
 
      counter("nMuonPIDs")++;

      pMuids->insert( pMuid, (*iTrack)->key() );
      if (msgLevel(MSG::DEBUG) ) debug()<< " added MuonPID object with key " << pMuid->key()  << endmsg;

      LHCb::Track* mutrack = 0;

      // Build mutrack if IsMuonLoose is 1
      if (pMuid->IsMuonLoose()) {
        mutrack = makeMuonTrack(*pMuid);
        pMuid->setMuonTrack( mutrack );
        mutracks->insert( mutrack, (*iTrack)->key() );
      }

      // If required, save all tracks. If tracks already created, simply clone them
      if (m_DoAllMuonTracks) {
        LHCb::Track* mutrack_all;
        if (pMuid->IsMuonLoose())  mutrack_all=mutrack->clone();
        else mutrack_all = makeMuonTrack(*pMuid);
        mutracks_all->insert( mutrack_all, (*iTrack)->key() );
      }
      
      // XCV: If kalman foi, nshared will be calculated afterwards, once all the distances have been calculated
      if (!m_kalman_foi){
        sc = calcSharedHits(pMuid, pMuids);
        if (sc.isFailure()){
          Warning(" calcSharedHits failed for track ",StatusCode::SUCCESS,0).ignore();
          if (msgLevel(MSG::DEBUG) ) debug()<<" calcSharedHits failed for track " << *iTrack << endmsg;
        }
      }
      
    } // long tracks
  }  // loop over tracks
  
  // Debug : muon identification event summary
  if (msgLevel(MSG::DEBUG) ) {
    debug()  << "MuonIDAlg:: Number of MuonPID objects created: " << pMuids->size()
             << endmsg;
    debug()  << "MuonIDAlg:: Number of tracks with IsMuonLoose = True : " << m_nmu
             << endmsg;
  }

  if (msgLevel(MSG::DEBUG) ) debug()  << "execute:: Muon Tracks registered  " << endmsg;

  // Register the PIDTracksIsMuonLoose container to the TES
  if (m_DoAllMuonTracks){ 
      put(mutracks_all,m_MuonTracksPathAll);
  }
  else delete mutracks_all;
  if (msgLevel(MSG::DEBUG) ) debug()  << "execute:: All Muon Tracks registered or deleted " << endmsg;

  //XCV: once the muon tracks have been put in TES, store in the muonPIDs and tracks the NShared info
  if (m_kalman_foi) NSharedTool()->fillNShared();
  //XCV: if kalman foi, this will be done by SmartMuonMeasProvider
  else clearCoordVectors();

  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode MuonIDAlg::finalize() {

  if (msgLevel(MSG::DEBUG) ) {
    debug()  << "==> Finalize" << endmsg;
  }
  info()  << "==> Total number of tracks with IsMuonLoose=1 failing likelihood (p<0): " <<
    m_mullfail << endmsg;


  // Execute base class method
  return GaudiAlgorithm::finalize();
}

//=============================================================================
// fill vectors of x,y,z positions for the MuonCoords
//=============================================================================
StatusCode MuonIDAlg::fillCoordVectors(){

  clearCoordVectors(); // sets up Coord vectors of vectors

  // get the MuonCoords for each station in turn
  LHCb::MuonCoords* coords = get<LHCb::MuonCoords>(LHCb::MuonCoordLocation::MuonCoords);
  if ( coords==0 ) return Error(" Cannot retrieve MuonCoords ");

  // loop over the coords
  LHCb::MuonCoords::const_iterator iCoord;
  for ( iCoord = coords->begin() ; iCoord != coords->end() ; iCoord++ ){
    int region = (*iCoord)->key().region();
    int station = (*iCoord)->key().station();
    double x,dx,y,dy,z,dz;
    LHCb::MuonTileID tile=(*iCoord)->key();
    
    StatusCode sc = m_mudet->Tile2XYZ(tile,x,dx,y,dy,z,dz);
    if (sc.isFailure()){
      Warning(" Failed to get x,y,z of tile ",StatusCode::SUCCESS).ignore();
      if (msgLevel(MSG::DEBUG) ) debug()<< "Failed to get x,y,z of tile " << tile << endmsg;
      continue;
    }
    m_coordPos[station*m_NRegion+region].
      push_back(coordExtent_(x,dx,y,dy,*iCoord));
    if (msgLevel(MSG::VERBOSE) ) verbose() << "fillCoordVectors: station: " << station << " region: " << region << " tile: " << tile << " z: " << z << endmsg;
  }

  return StatusCode::SUCCESS;
}

//=====================================
// Find Landau Parameters:
// G. Lanfranchi & S. Furcas
// 23-5-2009
//=====================================
StatusCode MuonIDAlg::find_LandauParam(const double& p,
                                       const std::vector<int>& trackRegion, double *parMu, double *parNonMu){
  // Track region is defined in M2 or M3
  int region=trackRegion[m_iM2]; // M2 JHLJHL Check M2 and M3 indices if no M1 30/08/2013
  if (region<0) region=trackRegion[m_iM2+1]; // M3
  if (region==0){//Region 1
    if (p>m_PreSelMomentum && p<m_MupBinsR1[0]) {for (int i=0;i<6;i++){parMu[i] = m_MuLanParR1_1[i]; } }
    if (p>m_MupBinsR1[0] && p<m_MupBinsR1[1]) {for (int i=0;i<6;i++){parMu[i] = m_MuLanParR1_2[i]; } }
    if (p>m_MupBinsR1[1] && p<m_MupBinsR1[2]) {for (int i=0;i<6;i++){parMu[i] = m_MuLanParR1_3[i]; } }
    if (p>m_MupBinsR1[2] && p<m_MupBinsR1[3]) {for (int i=0;i<6;i++){parMu[i] = m_MuLanParR1_4[i]; } }
    if (p>m_MupBinsR1[3] && p<m_MupBinsR1[4]) {for (int i=0;i<6;i++){parMu[i] = m_MuLanParR1_5[i]; } }
    if (p>m_MupBinsR1[4] && p<m_MupBinsR1[5]) {for (int i=0;i<6;i++){parMu[i] = m_MuLanParR1_6[i]; } }
    if (p>m_MupBinsR1[5]) {for (int i=0;i<6;i++){parMu[i] = m_MuLanParR1_7[i]; } }
    for (int i=0;i<3;i++){parNonMu[i] = m_NonMuLanParR1[i]; }
  }else if (region==1){// Region 2
    if (p>m_PreSelMomentum  && p<m_MupBinsR2[0]) {for (int i=0;i<6;i++){parMu[i] = m_MuLanParR2_1[i]; } }
    if (p>m_MupBinsR2[0] && p<m_MupBinsR2[1]) {for (int i=0;i<6;i++){parMu[i] = m_MuLanParR2_2[i]; } }
    if (p>m_MupBinsR2[1] && p<m_MupBinsR2[2]) {for (int i=0;i<6;i++){parMu[i] = m_MuLanParR2_3[i]; } }
    if (p>m_MupBinsR2[2] && p<m_MupBinsR2[3]) {for (int i=0;i<6;i++){parMu[i] = m_MuLanParR2_4[i]; } }
    if (p>m_MupBinsR2[3] ) {for (int i=0;i<6;i++){parMu[i] = m_MuLanParR2_5[i]; } }
    for (int i=0;i<3;i++){parNonMu[i] = m_NonMuLanParR2[i]; }
  }else if (region==2){// Region 3
    if (p>m_PreSelMomentum  && p<m_MupBinsR3[0]) {for (int i=0;i<6;i++){parMu[i] = m_MuLanParR3_1[i]; } }
    if (p>m_MupBinsR3[0] && p<m_MupBinsR3[1]) {for (int i=0;i<6;i++){parMu[i] = m_MuLanParR3_2[i]; } }
    if (p>m_MupBinsR3[1] && p<m_MupBinsR3[2]) {for (int i=0;i<6;i++){parMu[i] = m_MuLanParR3_3[i]; } }
    if (p>m_MupBinsR3[2] && p<m_MupBinsR3[3]) {for (int i=0;i<6;i++){parMu[i] = m_MuLanParR3_4[i]; } }
    if (p>m_MupBinsR3[3] ) {for (int i=0;i<6;i++){parMu[i] = m_MuLanParR3_5[i]; } }
    for (int i=0;i<3;i++){parNonMu[i] = m_NonMuLanParR3[i]; }
  }else if (region==3){// Region 4
    if (p>m_PreSelMomentum  && p<m_MupBinsR4[0]) {for (int i=0;i<6;i++){parMu[i] = m_MuLanParR4_1[i]; } }
    if (p>m_MupBinsR4[0]  && p<m_MupBinsR4[1]) {for (int i=0;i<6;i++){parMu[i] = m_MuLanParR4_2[i]; } }
    if (p>m_MupBinsR4[1]  && p<m_MupBinsR4[2]) {for (int i=0;i<6;i++){parMu[i] = m_MuLanParR4_3[i]; } }
    if (p>m_MupBinsR4[2]  && p<m_MupBinsR4[3]) {for (int i=0;i<6;i++){parMu[i] = m_MuLanParR4_4[i]; } }
    if (p>m_MupBinsR4[3] ) {for (int i=0;i<6;i++){parMu[i] = m_MuLanParR4_5[i]; } }
    for (int i=0;i<3;i++){parNonMu[i] = m_NonMuLanParR4[i]; }
  } else return Error(" Not valid region ",StatusCode::FAILURE,1);
  return StatusCode::SUCCESS;
}

//=============================================================================
// Clear the coord vector
//=============================================================================
void MuonIDAlg::clearCoordVectors(){
  m_coordPos.clear();
  m_coordPos.resize(m_NStation * m_NRegion);
  m_muonMap.clear();
}

//=============================================================================
// Do the identification
//=============================================================================
StatusCode MuonIDAlg::doID(LHCb::MuonPID *pMuid){

  if (msgLevel(MSG::DEBUG) )debug()  << "doID: ------------"  << endmsg;
  // Initializes data members
  pMuid->setIsMuon(0);
  pMuid->setIsMuonLoose(0);
  pMuid->setIsMuonTight(0);
  pMuid->setNShared(0);
  pMuid->setPreSelMomentum(0);
  pMuid->setInAcceptance(0);
  pMuid->setMuonLLMu(-10000.);
  pMuid->setMuonLLBg(-10000.);


  // find the momentum bin we are in
  // it can be 0, 1 or 2
  int momentumBin = 0;

  // m_PreSelMomentum = 3000 MeV
  if (m_MomentumPre> m_PreSelMomentum && m_MomentumPre < m_MomentumCuts[0]) {momentumBin = 0;}
  if (m_MomentumPre >= m_MomentumCuts[0] && m_MomentumPre < m_MomentumCuts[1]) {momentumBin = 1;}
  if (m_MomentumPre >= m_MomentumCuts[1]) {momentumBin = 2;}

  if (msgLevel(MSG::DEBUG) )debug()  << " p = " << m_MomentumPre << " bin = "   << momentumBin << endmsg;

  StatusCode sc;
    
  bool myIsMuon=false;
  bool myIsMuonLoose=false;
  bool myIsMuonTight=false;

    if (m_kalman_foi){
    // Kalman filter FoI method
    // Is good seed? 
    // Check PreSelMom and Acceptance
    const LHCb::Track* mother = pMuid->idTrack();
    sc = DistMuIDTool()->isGoodSeed(*mother);
    if (sc.getCode()==101){
      if (DistMuIDTool()->isTrackInAcceptance(*mother)) pMuid->setInAcceptance(1);
    }
    else{
      pMuid->setPreSelMomentum(1);
      if (sc.getCode()!=102) pMuid->setInAcceptance(1);
    }
    
    if (msgLevel(MSG::DEBUG) ) {
      debug() << " pMuid->PreSelMomentum()=" << pMuid->PreSelMomentum() << endmsg;
      debug() << " pMuid->InAcceptance()=" << pMuid->InAcceptance() << endmsg;
    }

    if(pMuid->InAcceptance())  counter("nInAcceptance")++;
    if(pMuid->PreSelMomentum())  counter("nMomentumPresel")++;

    if(sc.isFailure()){
      if (msgLevel(MSG::DEBUG) ) debug() << " Track failed preselection " << endmsg;
      return StatusCode::SUCCESS;    
    }
    
    m_mutrack = LHCb::Track(mother->key());
    //add mother track to ancestors
    m_mutrack.addToAncestors(mother);

    
    // Is it a muon candidate?
    sc = DistMuIDTool()->muonCandidate(*mother,m_mutrack);
    if (!(sc.isSuccess()||sc.getCode()==203))
    {
      Warning("Error when finding muon hits",sc).ignore();
      return StatusCode::SUCCESS;
    }

    myIsMuon = IsMuonTool()->IsMuonCandidate(m_mutrack);
    myIsMuonLoose = IsMuonLooseTool()->IsMuonCandidate(m_mutrack);
  }
  

  else{
    
    // First do a preselection:
    // track is in acceptance? Track has minimum momentum?
    bool passed = true;
    sc = preSelection( pMuid,passed );
    if(sc.isFailure()){
      Warning(" preSelection failed to MuonPID object ",sc).ignore();
      if (msgLevel(MSG::DEBUG) ) debug()<<" preSelection failed to MuonPID object" << pMuid << endmsg;
      return sc;
    }

    if(pMuid->InAcceptance())  counter("nInAcceptance")++;
    if(pMuid->PreSelMomentum())  counter("nMomentumPresel")++;

    if (msgLevel(MSG::DEBUG) ) debug() << "doID: Track key: " << pMuid->idTrack()->key() << " InAcceptance: " << pMuid->InAcceptance() 
        << " PreSelMomentum: " << pMuid->PreSelMomentum() << endmsg;

    // OK: track failed preselection say so and return
    if(!passed){
      if (msgLevel(MSG::DEBUG) ) debug() << " Track failed preselection " << endmsg;
      return StatusCode::SUCCESS;
    }

    // find the coordinates in the fields of interest
    sc = setCoords( pMuid );
    if(sc.isFailure()){
      Warning(" setCoords failed to MuonPID object ",sc).ignore();
      if (msgLevel(MSG::DEBUG) ) debug()<<" setCoords failed to MuonPID object" << pMuid << endmsg;
      return sc;
    }

    // apply ID: depends on the track momentum
    // bin 0 M2.and.M3
    // bin 1 M2.and.M3.and.(M4.or.M5)
    // bin 2 M2.and.M3.and.M4.and.M5

    std::vector<int> stations;
    for(int ist=m_iM2;ist<m_NStation;ist++){   // JHLJHL   Changed to go from m_iM2 to m_NStation 30/08/2013
      if (m_occupancyAll[ist]>0) stations.push_back(ist);
    }
    if (msgLevel(MSG::DEBUG)){
       debug() << " occupancyAll[ist]: " << m_occupancyAll << endmsg;
       debug() << " stations with hits in FOI: " << stations << endmsg;
    }

    //GL & SF: IsMuon/IsMuonLoose definition (with/without weights):
    //JHL: Remove use of weights 15/08/2011
    myIsMuonLoose = IsMuonLoose(stations,m_MomentumPre);  
    myIsMuon = IsMuon(stations,m_MomentumPre);

    std::vector<int> stationsWithCrossing;
    for(int ist=m_iM2;ist<m_NStation;ist++){
      if (m_occupancyWithCrossing[ist]>0) stationsWithCrossing.push_back(ist);
    }
    if (msgLevel(MSG::DEBUG) ) {
        debug() << " occupancyWithCrossing[ist]: " << m_occupancyWithCrossing << endmsg;
        debug() << " stations with hits in FOI requiring crossing: " << stationsWithCrossing << endmsg;
    }
    myIsMuonTight = IsMuon(stationsWithCrossing,m_MomentumPre);
  }

  if (msgLevel(MSG::DEBUG) ) {
    debug()<< " myIsMuon="<<myIsMuon<<" myIsMuonLoose="<<myIsMuonLoose<<" myIsMuonTight="<<myIsMuonTight<<endmsg;
  }
  
  
  pMuid->setIsMuon(myIsMuon);
  pMuid->setIsMuonLoose(myIsMuonLoose);
  pMuid->setIsMuonTight(myIsMuonTight);

  if(!m_kalman_foi){  
    // calculate MuProb
    double MuProb = calcMuProb( pMuid );
    if (msgLevel(MSG::DEBUG) ) debug()  << " MuProb= " << MuProb <<endmsg;
  }
  
  // calculate Muon DLL
  if(m_dllFlag == 0){   //  (old Erica&Miriam code):
    sc = calcMuonLL( pMuid );
    if (msgLevel(MSG::DEBUG) ) debug() << " calcMuonLL failed (P<0) to MuonPID object " << pMuid << endmsg;
    if(sc.isFailure()){
      if (myIsMuonLoose) m_mullfail++;
    }
  }  else if(m_dllFlag == 1){             // DLL with distance+integral:
    sc = calcMuonLL_dist(pMuid,m_MomentumPre);
    if(sc.isFailure()){
      if (msgLevel(MSG::DEBUG) ) debug() << " calcMuonLL(binned) failed (P<0) to MuonPID object " << pMuid << endmsg;
      if (myIsMuonLoose) m_mullfail++;
    }
  } else  if(m_dllFlag == 3){ // DLL with tanh(dist2) histograms
    sc = calcMuonLL_tanhdist(pMuid,m_MomentumPre);
    if(sc.isFailure()){
      if (msgLevel(MSG::DEBUG) ) debug() 
        << " calcMuonLL(tanh mapping) failed (P<0) to MuonPID object " << pMuid << endmsg;
      if (myIsMuonLoose) m_mullfail++;
    }
  } else  if(m_dllFlag == 4){ // DLL with tanh(dist2) 2010 data histograms for muons and 2009 landau fittings for non-muons
    sc = calcMuonLL_tanhdist_landau(pMuid,m_MomentumPre);
    if(sc.isFailure()){
      if (msgLevel(MSG::DEBUG) ) debug() 
        << " calcMuonLL (dllFlag == 4) failed (P<0) to MuonPID object " << pMuid << endmsg;
      if (myIsMuonLoose) m_mullfail++;    
    }
  }


  //increment number of IsMuonLoose=true tracks for monitoring
  if(pMuid->IsMuonLoose()) m_nmu++;
  if(pMuid->IsMuonLoose()) counter("nIsMuonLoose")++;
  if(pMuid->IsMuon()) counter("nIsMuon")++;
  if(pMuid->IsMuonTight()) counter("nIsMuonTight")++;

  return StatusCode::SUCCESS;
}



//=============================================================================
// Find out if st myst is in input array of stations
//=============================================================================
bool MuonIDAlg::stInStations(const int myst,const std::vector<int>& stations)
{

  for (std::vector<int>::const_iterator it = stations.begin();
       it != stations.end(); ++it)
  {
    int ist = *it;
    if (ist==myst) return true;
  }
  return false;
}


//=============================================================================
// Common IsMuon (with weights) requirements from set
// of stations with hits in FOI
// modified  Sara & Gaia
// Modified by J. Helder to remove use of weights
//=============================================================================

bool MuonIDAlg::IsMuon(const std::vector<int>& stations,const double& p)
{

  const double pr1=m_PreSelMomentum;
  const double pr2=m_MomentumCuts[0];
  const double pr3=m_MomentumCuts[1];

  double mom=p;
  //double mom=p/Gaudi::Units::GeV;

  if (msgLevel(MSG::DEBUG) ) {
    debug()<<"IsMuon"<<endmsg;
    debug()<<"pr1="<<pr1<<endmsg;
    debug()<<"pr2="<<pr2<<endmsg;
    debug()<<"pr3="<<pr3<<endmsg;
    debug()<<"IsMuon p="<<mom<<endmsg;
  }

  if (mom>pr1 && mom<pr2)  // JHLJHL: Check indices for M2 and M3 30/08/2013
  {
    if (stInStations(m_iM2,stations) && stInStations(m_iM2+1,stations)) return true;
  }

  else if (mom>pr2 && mom<pr3)
  {
    if (stInStations(m_iM2,stations) && stInStations(m_iM2+1,stations) &&
        (stInStations(m_iM2+2,stations) || stInStations(m_iM2+3,stations))) return true;
  }
  else if (mom>pr3)
  {
    if (stInStations(m_iM2,stations) && stInStations(m_iM2+1,stations)
        && stInStations(m_iM2+2,stations) && stInStations(m_iM2+3, stations) ) return true;
  }

  return false;
}


//=============================================================================
// Common IsMuonLoose (with weights) requirements from set
// of stations with hits in FOI
// modified  Sara & Gaia
// Modified by J. Helder to remove use of weights
//=============================================================================
bool MuonIDAlg::IsMuonLoose(const std::vector<int>& stations,const double& p)
{
  //double mom=p/Gaudi::Units::GeV;
  double mom=p;
  std::vector<int> vstations_rel1 = boost::assign::list_of(m_iM2)(m_iM2+1)(m_iM2+2);   // JHLJHL: Check indices... 30/08/2013
  std::vector<int> vstations_rel2 = boost::assign::list_of(m_iM2)(m_iM2+1)(m_iM2+2)(m_iM2+3);

  if (msgLevel(MSG::DEBUG) ) {
    debug()<<"vstations_rel1"<<vstations_rel1<<endmsg;
    debug()<<"vstations_rel2"<<vstations_rel2<<endmsg;
  }

  const double pr1=m_PreSelMomentum;
  const double pr2=m_MomentumCuts[0];

  // JHL: Aug. 2011: Don't use random weights but assume hits in M4 and M5 for p < 3.5 GeV and hits in M5 for p<4.2 GeV are not signal
  bool w[5] = {true,true,true,false,false};
  if(mom>3500) w[3]=true;
  if(mom>4200) w[4]=true;

  if (msgLevel(MSG::DEBUG) ) {
    debug()<<"IsMuonLoose"<<endmsg;
    debug()<<"pr1="<<pr1<<endmsg;
    debug()<<"pr2="<<pr2<<endmsg;
    debug()<<"p IsMuonLoose="<<mom<<endmsg;
    debug() << "weights: " << w[0] << " " << w[1] << " " << w[2] << " " << w[3] << " " << w[4] <<endmsg;
  }

  if (mom>pr1 && mom<pr2)
  {
    int j=0;
    for (std::vector<int>::const_iterator it = stations.begin();
         it != stations.end(); ++it)
    {
      int ist = *it;
      if (stInStations(ist,vstations_rel1)&&w[ist]) j+=1;
    }
    if (msgLevel(MSG::DEBUG) ) debug()<<"first bin, j="<<j<<endmsg;
    if (j>1) return true;
  }

  else if (mom>pr2)
  {
    int j=0;
    for (std::vector<int>::const_iterator it = stations.begin();
         it != stations.end(); ++it)
    {
      int ist = *it;
      if (stInStations(ist,vstations_rel2)&&w[ist]) j+=1;
    }
    if (msgLevel(MSG::DEBUG) ) debug()<<"second bin, j="<<j<<endmsg;
    if (j>2) return true;
  }
  return false;
}

//=============================================================================
// calculate MuProb
//=============================================================================
// if found a muon make a probability from the DxDz matching
double MuonIDAlg::calcMuProb(LHCb::MuonPID * pMuid){

  if(pMuid->IsMuonLoose()){
    // slope calculated in M2 and M3
    m_xMatchStation = m_iM2;  // JHLJHL: Check M2 and M3 indices, ... for the case with no M1! 30/08/2013
    // find slope difference between track and Coords in M2-M3
    double coordSlopeX = ((m_CoordX[m_xMatchStation] -
                           m_CoordX[m_xMatchStation+1])/
                          (m_stationZ[m_xMatchStation] -
                           m_stationZ[m_xMatchStation+1]));
    double dSlopeX = fabs( m_trackSlopeX - coordSlopeX );
    if (msgLevel(MSG::DEBUG) ) debug()<<"calcMuProb: m_xMatchStation: " << m_xMatchStation << " m_CoordX: " 
        << m_CoordX[m_xMatchStation] << " - " << m_CoordX[m_xMatchStation+1] << " m_stationZ: " <<  m_stationZ[m_xMatchStation] << " - " <<  m_stationZ[m_xMatchStation+1] << " dSlopeX: " << dSlopeX <<endmsg; 

    // formula to make this a probability is
    // dSlopeX < 0.005 = 1.0
    // 0.005 < dSlopeX < 0.085 = 1. - ( (dSlopeX-0.005) / 0.08 )
    // 0.085 < dSlopeX = 0.
    if( dSlopeX < 0.005 ) {
      return 1.0;
    }else if( dSlopeX > 0.085 ){
      return 0.0;
    } else {
      return (1.0 - ( (dSlopeX-0.005) / 0.08 ) );
    }
  }else{
    // not passed selection as a muon so probability is zero
    return 0.0;
  }
}


StatusCode MuonIDAlg::calcMuonLL_dist(LHCb::MuonPID * pMuid, const double& p){
  //=============================================================================
  // comment: Calculate the muon DLL with the integral of the binned closest distance:
  // authors: G. Lanfranchi & S. Furcas,
  // date:    10/5/09
  //=============================================================================

  pMuid->setMuonLLMu(-10000.);
  pMuid->setMuonLLBg(-10000.);

  // calculate dll only for IsMuonLoose:
  if ( !pMuid->IsMuonLoose() ) return StatusCode::SUCCESS;

  // Initialize some variables:
  double myDist=-1.;
  double ProbMu=-1.;
  double ProbNonMu = -1.;
  double parMu[6];
  double parNonMu[3];
  for(int i=0;i<6;i++){parMu[i]=0;}
  for(int i=0;i<3;i++){parNonMu[i]=0;}
  std::vector<int> trackRegion(m_NStation,-1);

  // Calculate Distance using the closest hit:
  myDist = calc_closestDist(pMuid,p,closest_region);
  if (myDist<=0){
    Warning("Closest Distance <= 0 ",StatusCode::SUCCESS).ignore();
    return StatusCode::FAILURE; 
  }

  //EP: Store dist to fill Muon Track extra info
  m_dist_out=myDist;


  //find track region
  trackRegion=findTrackRegions();


  // Find Landau's parameters for a given track:
  StatusCode sc = find_LandauParam(p, trackRegion, parMu, parNonMu);
  if (sc.isFailure()) {
    Warning(" Find Landau Parameters: no valid region",StatusCode::SUCCESS).ignore();
    return StatusCode::FAILURE;
  }


  // Calculate Prob(mu) and Prob(non-mu) for a given track;
  ProbMu = calc_ProbMu(myDist, parMu);
  if (ProbMu<0) return Error("ProbMu <0", StatusCode::FAILURE);

  ProbNonMu = calc_ProbNonMu(myDist, parNonMu);
  if (ProbNonMu<0) return Error("ProbNonMu <0", StatusCode::FAILURE);

  // Set in the MuonPID object the ProbMu & ProbNonMu (Not the Log!)
  pMuid->setMuonLLMu(log(ProbMu));
  pMuid->setMuonLLBg(log(ProbNonMu));

  if (msgLevel(MSG::DEBUG) ) {
    debug() << "calcMuonLL_dist: region: " << trackRegion << " momentum: " << p
            << " dist: " << myDist << " ProbMu: " << ProbMu << " ProbNonMu: "
            << ProbNonMu << " DLL: " << log(ProbMu/ProbNonMu) << endmsg;
  }
  return StatusCode::SUCCESS;
}

StatusCode MuonIDAlg::calcMuonLL_tanhdist(LHCb::MuonPID * pMuid, const double& p){
  //=============================================================================
  // comment: Calculate the muon DLL with the cumulative histos of the hyperbolic
  //          tangent of the closest distance, per region and momentum bins:
  // authors: J. Helder Lopes
  // date:    13/10/09
  //=============================================================================

  static const double atanh05 = std::atanh(0.5);
  
  pMuid->setMuonLLMu(-10000.);
  pMuid->setMuonLLBg(-10000.);

  // calculate dll only for IsMuonLoose:
  if ( !pMuid->IsMuonLoose() ) return StatusCode::SUCCESS;

  // Initialize some variables:
  double myDist=-1.;
  double ProbMu=-1.;
  double ProbNonMu = -1.;
  std::vector<int> trackRegion(m_NStation,-1);

  // Calculate Distance using the closest hit:
  myDist = calc_closestDist(pMuid,p,closest_region);
  if (myDist<=0){
    Warning("Closest Distance < 0 ",StatusCode::SUCCESS).ignore();
    return StatusCode::FAILURE; 
  }

  //EP: Store dist to fill Muon Track extra info
  m_dist_out=myDist;


  //find track region
  trackRegion=findTrackRegions();

  int region=trackRegion[m_iM2]; // M2 // JHLJHL: Check M2 and M3 indices in case of no M1. 30/08/2013
  if (region<0) region=trackRegion[m_iM2+1]; // M3
  
  if (region<0) {
    Warning("calcMuonLL_tanhdist: no valid region",StatusCode::SUCCESS).ignore();
    return StatusCode::FAILURE;
  }


  // Determine the momentum bin for this region
  int pBin=GetPbin(p, region);
  double tanhdist;
  // Calculate tanh(dist). The effective scale factor is after dividing by arc tanh(0.5)
  tanhdist = tanh(myDist/(*(m_tanhScaleFactors[region]))[pBin]*atanh05);

  // Calculate Prob(mu) and Prob(non-mu) for a given track;
  ProbMu = calc_ProbMu_tanh(tanhdist, pBin, region );
  if (ProbMu<0) return Error("ProbMu <0", StatusCode::FAILURE);

  ProbNonMu = calc_ProbNonMu_tanh(tanhdist, pBin, region );
  if (ProbNonMu<0) return Error("ProbNonMu <0", StatusCode::FAILURE);


  // Set in the MuonPID object the ProbMu & ProbNonMu (Note the Log!)
  pMuid->setMuonLLMu(log(ProbMu));
  pMuid->setMuonLLBg(log(ProbNonMu));

  if (msgLevel(MSG::DEBUG) ) {
    debug() << "calcMuonLL_tanhdist: region: " << region << " momentum: " << p
            << " pBin: " <<  pBin << " dist " << myDist << " tanh(dist2): "
            << tanhdist << " ProbMu: " << ProbMu << " ProbNonMu: " << ProbNonMu
            <<" DLL: " << log(ProbMu/ProbNonMu) << endmsg;
  }

  return StatusCode::SUCCESS;
}

StatusCode MuonIDAlg::calcMuonLL_tanhdist_landau(LHCb::MuonPID * pMuid, const double& p){
  //=============================================================================
  // comment: Calculate the muon DLL using cumulative histos of the hyperbolic
  //          tangent of the closest distance tuned on 2010 data for muon hypothesis 
  //          and previous landau fittings to 2009 MC for non-muon hypothesis, 
  //          per region and momentum bins:
  // authors: J. Helder Lopes
  // date:    16/02/2011
  //=============================================================================

  static const double atanh05 = std::atanh(0.5);

  pMuid->setMuonLLMu(-10000.);
  pMuid->setMuonLLBg(-10000.);

  // calculate dll only for IsMuonLoose:
  if ( !pMuid->IsMuonLoose() ) return StatusCode::SUCCESS;

  // Initialize some variables:
  double myDist=-1.;
  double ProbMu=-1.;
  double ProbNonMu = -1.;
  double parMu[6];  // These won't be used here... Only parNonMu
  double parNonMu[3];
  for(int i=0;i<6;i++){parMu[i]=0;}
  for(int i=0;i<3;i++){parNonMu[i]=0;}
  std::vector<int> trackRegion(m_NStation,-1);

  // Calculate Distance using the closest hit:
  myDist = calc_closestDist(pMuid,p,closest_region);
  if (myDist<=0){
    Warning("Closest Distance < 0 ",StatusCode::SUCCESS).ignore();
    return StatusCode::FAILURE; 
  }
  
  //EP: Store dist to fill Muon Track extra info
  m_dist_out=myDist;


  //find track region
  trackRegion=findTrackRegions();

  int region=trackRegion[m_iM2]; // M2   JHLJHL Check indices ... 30/08/2013
  if (region<0) region=trackRegion[m_iM2+1]; // M3

  // Find Landau's parameters for a given track:
  StatusCode sc = find_LandauParam(p, trackRegion, parMu, parNonMu);
  if (sc.isFailure()) {
    Warning(" Find Landau Parameters: no valid region",StatusCode::SUCCESS).ignore();
    return StatusCode::FAILURE;
  }

  // Determine the momentum bin for this region
  int pBin=GetPbin(p, region);
  double tanhdist;
  // Calculate tanh(dist). The effetive scale factor is after dividing by arc tanh(0.5)
  tanhdist = tanh(myDist/(*(m_tanhScaleFactors[region]))[pBin]*atanh05);

  // Calculate Prob(mu)  for a given track using tanh(dist);
  ProbMu = calc_ProbMu_tanh(tanhdist, pBin, region );
  if (ProbMu<0) return Error("ProbMu <0", StatusCode::FAILURE);

  // Calculate ProbNonMu using Landau fits 
  ProbNonMu = calc_ProbNonMu(myDist, parNonMu);
  if (ProbNonMu<0) return Error("ProbNonMu <0", StatusCode::FAILURE);

  // Using histograms it's not unexpected that some bins are empty. Use very small prob as a protection for the log
  if(ProbMu<1.e-30) {
    if (msgLevel(MSG::DEBUG) ) debug() << "calcMuonLL_tanhdist_landau: Found null ProbMu: " << ProbMu << endmsg;
    ProbMu = 1.e-30;
  }
  if(ProbNonMu<1.e-30) {
    if (msgLevel(MSG::DEBUG) ) debug() << "calcMuonLL_tanhdist_landau: Found null ProbNonMu: " << ProbNonMu << endmsg;
    ProbNonMu = 1.e-30;
  }

  // Set in the MuonPID object the ProbMu & ProbNonMu (Note the Log!)
  pMuid->setMuonLLMu(log(ProbMu));
  pMuid->setMuonLLBg(log(ProbNonMu));

  if (msgLevel(MSG::DEBUG) ) {
    debug() << "calcMuonLL_tanhdist_landau: region: " << region << " momentum: " << p
            << " pBin: " <<  pBin << " dist " << myDist << " tanh(dist2): "
            << tanhdist << " ProbMu: " << ProbMu << " ProbNonMu: " << ProbNonMu
            <<" DLL: " << log(ProbMu/ProbNonMu) << endmsg;
  }

  return StatusCode::SUCCESS;
}

//=============================================================================
// Calculate the muon DLL
//=============================================================================
StatusCode MuonIDAlg::calcMuonLL(LHCb::MuonPID * muonid){

  if ( !muonid->IsMuonLoose() ) {
    return StatusCode::SUCCESS;
  }

  const LHCb::Track* pTrack = muonid->idTrack();
  // do the track extrapolations
  StatusCode sc = trackExtrapolate(pTrack);
  if (!sc){
    Warning(" trackExtrapolate fails for track ",sc,0).ignore();
    if (msgLevel(MSG::DEBUG) ) debug()<< "trackExtrapolate fails for track" << pTrack << endmsg;
    muonid->setMuonLLMu(-10000.);
    muonid->setMuonLLBg(-10000.);
    return StatusCode::FAILURE;
  }

  unsigned int nhits=0;
  double dist = 0.;
  std::vector<LHCb::MuonCoord*> & mcoord = m_muonMap[muonid];
  std::vector<LHCb::MuonCoord*>::const_iterator iCoord;
  for ( iCoord = mcoord.begin() ; iCoord != mcoord.end() ; iCoord++ ){

    double x,dx,y,dy,z,dz;
    LHCb::MuonTileID tile=(*iCoord)->key();
    sc = m_mudet->Tile2XYZ(tile,x,dx,y,dy,z,dz);
    if (sc.isFailure()){
      Warning(" Failed to get x,y,z of tile ",StatusCode::SUCCESS,0).ignore();
      if (msgLevel(MSG::DEBUG) ) debug()<< "Failed to get x,y,z of tile " << tile << endmsg;
      continue;
    }

    int station = (*iCoord)->key().station();
    if (station >= m_iM2 ) {  // JHL: station == 0 is OK in the case of no M1  12/Oct./2013 
      dist = dist + pow(((x - m_trackX[station])/dx),2) +
        pow(((y - m_trackY[station])/dy),2) ;
      nhits++;
    }
  }

  if( 0 < nhits ) dist = dist / nhits;
  //EP: Store dist to fill Muon Track extra info
  m_dist_out=dist;

  double LklhMu = m_distMuon[0] * (TMath::Landau (dist,m_distMuon[1],m_distMuon[2])) +
    m_distMuon[3] * (TMath::Landau (dist,m_distMuon[4],m_distMuon[5])) +
    m_distMuon[6] * (TMath::Landau (dist,m_distMuon[7],m_distMuon[8]));
  double LklhPi = m_distPion[0] * (TMath::Landau (dist,m_distPion[1],m_distPion[2])) + m_distPion[3] * (TMath::Landau (dist,m_distPion[4],m_distPion[5])) +
    m_distPion[6] * (TMath::Landau (dist,m_distPion[7],m_distPion[8]));

  if (LklhMu<0 || LklhPi<0) return StatusCode::FAILURE;
  muonid->setMuonLLMu(log(LklhMu));
  muonid->setMuonLLBg(log(LklhPi));

  return StatusCode::SUCCESS;

}

//=============================================================================
// calculate the number of tracks that share hits
//=============================================================================
StatusCode MuonIDAlg::calcSharedHits( LHCb::MuonPID* muonid, LHCb::MuonPIDs * pMuids ) {

  // if not a muon candidate do nothing
  if ( !muonid->IsMuonLoose() ) return StatusCode::SUCCESS;

  // calculate the distance from the hit to the extrapolated position
  StatusCode sc = calcDist(muonid);
  if( sc.isFailure() ) {
    muonid->setNShared(100);
    return Warning( "calcDist 1 failure", sc );
  }

  double dist1 = m_dist;
  if (msgLevel(MSG::DEBUG) ) debug()  << " mdist 1=  " << m_dist << endmsg;

  // loop over the muonIDs
  LHCb::MuonPIDs::const_iterator iMuon;
  for( iMuon = pMuids->begin(); pMuids->end() != iMuon; ++iMuon ) {

    // skip if this muonID is the same as the input or if IsMuonLoose is false
    if ( *iMuon == muonid || !(*iMuon)->IsMuonLoose() ) continue;

    // see if there are shared hits between the muonIDs
    if ( compareHits( muonid, *iMuon ) ){

      // get dist for this muonID
      sc = calcDist(*iMuon);
      if( sc.isFailure() ) {
        Warning(" calcDist 2 failure",StatusCode::SUCCESS,0).ignore();
        continue;
      }

      double dist2 = m_dist;
      if (msgLevel(MSG::DEBUG) ) debug()  << " mdist 2=  " << m_dist << endmsg;
      // the muonID which gets the number of shared hits is the one
      // which has the biggest dist
      if( dist2 < dist1 ) {
        int nNSH = muonid->nShared();
        muonid->setNShared(nNSH+1);
      }
      else{
        int nNSH = (*iMuon)->nShared();
        (*iMuon)->setNShared(nNSH+1);
      }
    }
  }
  if (msgLevel(MSG::DEBUG) ) debug()  << "nShared=  " <<  muonid->nShared() << endmsg;

  return StatusCode::SUCCESS;
}

//=============================================================================
//  Method to compare the Coords of the muonIDs
//============================================================================
bool MuonIDAlg::compareHits( LHCb::MuonPID* muonid1, LHCb::MuonPID* muonid2 ){

  bool theSame = false;

  std::vector<LHCb::MuonCoord*> & mcoord1 = m_muonMap[muonid1];
  std::vector<LHCb::MuonCoord*>::const_iterator iCoord1;
  std::vector<LHCb::MuonCoord*> & mcoord2 = m_muonMap[muonid2];
  std::vector<LHCb::MuonCoord*>::const_iterator iCoord2;
  for( iCoord1 = mcoord1.begin() ; iCoord1 != mcoord1.end() ; iCoord1++ ){
    for( iCoord2 = mcoord2.begin() ; iCoord2 != mcoord2.end() ; iCoord2++ ){
      if( (*iCoord1)->key() == (*iCoord2)->key() ) {
        if ( ((*iCoord1)->key().station()) >= (unsigned int)m_iM2 )  theSame = true; // JHL: station == 0 is OK in the case of no M1  12/Oct./2013
      }
    }
  }

  return theSame;

}

//=============================================================================
//  Method to calculate the distance from the hit to the extrapolated position
//============================================================================
StatusCode MuonIDAlg::calcDist( LHCb::MuonPID* muonid ){

  m_dist = 0.;

  if ( !muonid->IsMuonLoose() ) {
    return StatusCode::SUCCESS;
  }
  const LHCb::Track* pTrack = muonid->idTrack();
  // do the track extrapolations
  StatusCode sc = trackExtrapolate(pTrack);
  if(!sc){
    Warning(" trackExtrapolate fails for track ",sc,0).ignore();
    if (msgLevel(MSG::DEBUG) ) debug()<< " trackExtrapolate fails for track " <<  pTrack << endmsg;
    return StatusCode::FAILURE;
  }

  std::vector<LHCb::MuonCoord*> & mcoord = m_muonMap[muonid];
  std::vector<LHCb::MuonCoord*>::const_iterator iCoord;
  double mCoordX[5] = {0.,0.,0.,0.,0.};
  double mCoordY[5] = {0.,0.,0.,0.,0.};
  double mCoordDX[5] = {0.,0.,0.,0.,0.};
  double mCoordDY[5] = {0.,0.,0.,0.,0.};
  for( iCoord = mcoord.begin() ; iCoord != mcoord.end() ; iCoord++ ){
    double x,dx,y,dy,z,dz;
    LHCb::MuonTileID tile=(*iCoord)->key();
    sc = m_mudet->Tile2XYZ(tile,x,dx,y,dy,z,dz);
    if(sc.isFailure()){
      Warning(" Failed to get x,y,z of tile ",sc).ignore();
      if (msgLevel(MSG::DEBUG) ) debug()<< "Failed to get x,y,z of tile " << tile << endmsg;
      continue;
    }
    int station = (*iCoord)->key().station();
    if(mCoordX[station] == 0) {
      // this is the first coord found
      mCoordX[station] = x;
      mCoordY[station] = y;
      // difference between x,y and the extrapolated x,y
      mCoordDX[station] = ( x - m_trackX[station] ) / dx;
      mCoordDY[station] = ( y - m_trackY[station] ) / dy;

    }else{
      // get best match in X and Y
      if( (x - m_trackX[station])*(x - m_trackX[station]) +
          (y - m_trackY[station])*(y - m_trackY[station])  <
          pow((mCoordX[station] - m_trackX[station]),2) +
          pow((mCoordY[station] - m_trackY[station]),2) ){
        // this Coord is a better match
        mCoordX[station] = x;
        mCoordY[station] = y;
        mCoordDX[station] = ( x - m_trackX[station] ) / dx;
        mCoordDY[station] = ( y - m_trackY[station] ) / dy;
      }
    }
  } // end of Coords loop

  // calculate the square of the distances
  int nstn = 0;
  for( int stn = 0 ; stn < 5; stn++ ){  // No M1 case: 5 here refers to dimension of mCoordDX arrays JHL 12/Oct./2013
    if (msgLevel(MSG::DEBUG) ) {
      debug() << "calcDist: station: "<< stn << " mCoordDX = " << mCoordDX[stn] 
              << " mCoordDY = " << mCoordDY[stn] <<  endmsg;
    }
    if ( mCoordDX[stn] != 0. ) {
      nstn++;
      m_dist += mCoordDX[stn]*mCoordDX[stn] + mCoordDY[stn]*mCoordDY[stn];
    }
  }
  m_dist = m_dist / nstn; //divide by the number of station

  return StatusCode::SUCCESS;
}

//=============================================================================
// apply the preselection
//=============================================================================
StatusCode MuonIDAlg::preSelection(LHCb::MuonPID * pMuid, bool &passed){
  passed = true;

  // compare momentum and position to acceptance
  // use the first state on track for the preSel
  if (m_MomentumPre < m_PreSelMomentum ){
    // failed preselection momentum cut
    pMuid->setPreSelMomentum(0);
    passed = false;
  }else{
    pMuid->setPreSelMomentum(1);
  }
  pMuid->setInAcceptance(1);
  // in first and last station acceptance  
  // JHLJHL: Change 4 -> m_NStation-1, in case of no M1. In this case 0 -> M2. Check how this affects InAcceptance... 30/08/2013
  if (msgLevel(MSG::DEBUG) ) {
    debug()  << "trackX0 = " << m_trackX[0] << endmsg;
    debug()  << "trackX4 = " << m_trackX[m_NStation-1] << endmsg;
    debug()  << "trackY0 = " << m_trackY[0] << endmsg;
    debug()  << "trackY4 = " << m_trackY[m_NStation-1] << endmsg;
  }
  if(  ! (fabs(m_trackX[0]) <  m_regionOuterX[0] &&
          fabs(m_trackY[0]) <  m_regionOuterY[0] )  ||
       ! (fabs(m_trackX[m_NStation-1]) <
          m_regionOuterX[m_NStation-1] &&
          fabs(m_trackY[m_NStation-1]) <
          m_regionOuterY[m_NStation-1] )
       ) {
    // outside M1 - M5 region
    pMuid->setInAcceptance(0);
    passed = false;
  }

  // Inner acceptance
  if(   (fabs(m_trackX[0]) <  m_regionInnerX[0] &&
         fabs(m_trackY[0]) <  m_regionInnerY[0] )  ||
        (fabs(m_trackX[m_NStation-1]) <
         m_regionInnerX[m_NStation-1] &&
         fabs(m_trackY[m_NStation-1]) <
         m_regionInnerY[m_NStation-1] ) ) {

    // inside M1 - M5 chamber hole
    pMuid->setInAcceptance(0);
    passed = false;
  }


  return StatusCode::SUCCESS;
}

//=============================================================================
// get the coords
//=============================================================================
StatusCode MuonIDAlg::setCoords(LHCb::MuonPID *pMuid){

  if (msgLevel(MSG::VERBOSE) ) verbose()  << "setCoords:"  << endmsg;
  int station;
  for(station = 0 ; station < m_NStation ; station++){
    int region;
    for(region = 0 ; region < m_NRegion ; region++){
      int i  = station * m_NRegion + region;
      if( !m_coordPos[i].empty() ){

      if (msgLevel(MSG::VERBOSE) ) verbose()  << "           station " << station << " region " << region
                                          << " mapInRegion: " << m_mudet->mapInRegion(station,region ) << endmsg;

      //TODO: compute this once, build lookup table...
      double sr_foiX = 0.; // foiX for this station, region... upto factor 'dx'
      double sr_foiY = 0.; // foiY for this station, region... upto factor 'dy'
	  std::tie(sr_foiX,sr_foiY) = foiXY( station, region, m_MomentumPre );

        for(auto itPos = m_coordPos[i].begin();
            itPos != m_coordPos[i].end();
            itPos++){

          double x = itPos->m_x;
          double dx = itPos->m_dx;
          double y = itPos->m_y;
          double dy = itPos->m_dy;

          double foiXDim = sr_foiX * dx * m_foifactor;
          double foiYDim = sr_foiY * dy * m_foifactor;

          // check if the hit is in the window
          if(  ( fabs( x - m_trackX[station] ) < foiXDim ) &&
               ( fabs( y - m_trackY[station] ) < foiYDim )  ) {

            if (msgLevel(MSG::VERBOSE) ) {
              verbose()  << "              FOIfactor : " << m_foifactor << endmsg;

              verbose()  << "              ratioX = " << fabs( x - m_trackX[station])
                       << " ratioY = " << fabs( y - m_trackY[station]) << " foiXDim = "
                       << foiXDim <<" foiYDim = " << foiYDim <<endmsg;
              verbose()  << "              padX = " << dx << " padY = " << dy << endmsg;
            }

            // it is in the window
            // add the hit to the MuonPID
            m_occupancyAll[station]++;  // Occupancy without requiring x,y crossing

            // Check x,y crossing for this hit
            bool uncrossed = ((m_mudet->mapInRegion(station,region ) != 1) && ((itPos->m_pCoord)->uncrossed()));

            if ( !uncrossed ) m_occupancyWithCrossing[station]++; // Occupancy requiring x,y crossing
            if (msgLevel(MSG::VERBOSE) ) {
                verbose()  << "              Checking crossing: uncrossed: " << ((itPos->m_pCoord)->uncrossed()) << " add hit? " << !uncrossed
                         << " m_occupancyAll = " << m_occupancyAll[station]
                         << " m_occupancyWithCrossing = " << m_occupancyWithCrossing[station] << endmsg;
            }

            // Fill m_muonMap and m_CoordX according to m_use_uncrossed option
            if ( !m_use_uncrossed && uncrossed ) continue;
	    m_muonMap[pMuid].push_back(itPos->m_pCoord);

            if (msgLevel(MSG::VERBOSE) )  verbose()  << "              m_muonMap size: " << m_muonMap[pMuid].size()  << endmsg;

            // also need track angle in X in M2-M3 for angle matching
            int nCoord = m_occupancyAll[station];
            if(!m_use_uncrossed) nCoord = m_occupancyWithCrossing[station];
            if(1 == nCoord) {
              // this is the first coord found
              m_CoordX[station] = x;
            }else{
              // get best match X
              if( fabs(x - m_trackX[station]) <
                  fabs(m_CoordX[station] - m_trackX[station]) ){
                // this Coord is a better match
                m_CoordX[station] = x;
              }
            }
          }
        }// itPos
      }
    } //region
  } //station
  return StatusCode::SUCCESS;
}


StatusCode MuonIDAlg::get_closest(LHCb::MuonPID *pMuid, double *closest_x, 
                                  double *closest_y, double *closest_region){
  //=============================================================================
  // comment: get the closest hit
  // authors: G. Lanfranchi & S. Furcas,
  // date: 10/5/09
  //=============================================================================

  double foiXDim,foiYDim;
  int nhits=0;

  for (int ista=0; ista<5; ista++){  // No M1 case: 5 here is OK, these arrays have dimension 5 JHL 12/Oct./2013
    closest_x[ista] = -1;
    closest_y[ista] = -1;
    small_dist[ista] = 10000000.;
    Fdist[ista] = 0;
    closest_region[ista] = -1;
  }

  std::vector<LHCb::MuonCoord*> & mcoord = m_muonMap[pMuid];
  std::vector<LHCb::MuonCoord*>::const_iterator iCoord;
  for ( iCoord = mcoord.begin() ; iCoord != mcoord.end() ; iCoord++ ){

    double x,dx,y,dy,z,dz;
    LHCb::MuonTileID tile=(*iCoord)->key();
    StatusCode sc =
      m_mudet->Tile2XYZ(tile,x,dx,y,dy,z,dz);
    if (sc.isFailure()){
      Warning(" Failed to get x,y,z of tile ",sc).ignore();
      if (msgLevel(MSG::DEBUG) ) debug()<< "Failed to get x,y,z of tile " << tile << endmsg;
      continue;
    }

    int station = (*iCoord)->key().station();
    int region = (*iCoord)->key().region();
    std::tie(foiXDim,foiYDim) = foiXY( station, region, m_MomentumPre );
    foiXDim *= dx*m_foifactor;
    foiYDim *= dy*m_foifactor;

    // only for M2-M3-M4-M5:
    if (station >= m_iM2 ) {  // JHL: station == 0 is OK in the case of no M1  12/Oct./2013
      if(  ( fabs( x - m_trackX[station] ) < foiXDim ) &&
           ( fabs( y - m_trackY[station] ) < foiYDim )  ) {
        
        Fdist[station] =(((x - m_trackX[station])/dx) * ((x - m_trackX[station])/dx))
          +(((y - m_trackY[station])/dy) * ((y - m_trackY[station])/dy));

        nhits++;

        if(Fdist[station] < small_dist[station]){

          small_dist[station] = Fdist[station];
          closest_x[station]  = (x - m_trackX[station]) / dx;
          closest_y[station]  = (y - m_trackY[station]) / dy;
          closest_region[station] = region;

        }//Fdist
      }//foi
    }//station
  }//coord

  return StatusCode::SUCCESS;
}



//=====================================================================
std::vector<int> MuonIDAlg::findTrackRegions(){
  //=====================================================================
  // comment: Returns the muon detector region of the extrapolated track;
  // authors: G. Lanfranchi & S. Furcas & X. Cid Vidal,
  // date:    10/3/11
  //=====================================================================
    
  std::vector<int> trackRegion(m_NStation,-1);
  // Region of the track extrapolated:
  
  if (m_kalman_foi)
  {
    StatusCode sc = DistMuIDTool()->findTrackRegions(m_mutrack,trackRegion);
    if (sc.isFailure() && msgLevel(MSG::DEBUG) ) debug() << " Error finding track regions "<< endmsg;
    return trackRegion;
  }
  
  for (int sta=0;sta<m_NStation; sta++){
  
    int chnum = -1;
    int regnum = -1;
    
    m_mudet->Pos2StChamberNumber(m_trackX[sta],m_trackY[sta],sta,chnum,regnum).ignore();
    trackRegion[sta] = regnum;
    if (trackRegion[sta]<0 && msgLevel(MSG::DEBUG) )
      debug() << format(" Track extrapolation in station %d gives not-existent region ",sta)
              << endmsg;
  }
  
  if (msgLevel(MSG::DEBUG)) debug() << "TrackRegions" << trackRegion << endmsg;
  return trackRegion;
}


//=====================================================================
double MuonIDAlg::calc_closestDist(LHCb::MuonPID *pMuid, const double& p, double *closest_region){
  //=====================================================================
  // comment: Returns the squared distance calculated with closest hit
  // authors: G. Lanfranchi & S. Furcas,
  // date:    10/5/09
  //=====================================================================

  double closest_dist=0;

  std::vector<int> *occupancy;
  if(m_use_uncrossed) occupancy = &m_occupancyAll;
  else occupancy = &m_occupancyWithCrossing;

  if (msgLevel(MSG::DEBUG) )debug()  << "calc_closestDist: occupancy: " << (*occupancy) << endmsg;
  
  if (m_kalman_foi){
      
    StatusCode sc = DistMuIDTool()->muonQuality(m_mutrack, closest_dist);
    if (sc.isFailure())
    {
      Warning("DistMuonIDTool: ComputeDistance failed ",sc).ignore();
      return -1;
    }
    return closest_dist;
  }

  StatusCode sc = get_closest(pMuid,closest_x,closest_y,closest_region);

  if ( sc.isFailure() ){
    Warning(" Closest_hit failed ",sc).ignore();
    return -1;
  }

  if (msgLevel(MSG::DEBUG) ){
    debug()  << "calc_closestDist: station closest_region \t closest_x \t closest_y" << endmsg;
    for(int ist=0;ist<5;++ist) debug()  << ist << " " << closest_region[ist] << "\t" << closest_x[ist] << "\t" << closest_y[ist] << endmsg;
  }

  int iM2=m_iM2, iM3=m_iM2+1, iM4=m_iM2+2, iM5=m_iM2+3;

  if(p>m_PreSelMomentum && p<m_MomentumCuts[0]){ // JHLJHL: Check station indices whith no M1 configuration 30/08/2013
    //3 or 2 stations
    if((*occupancy)[iM2]>0 && (*occupancy)[iM3]>0 && (*occupancy)[iM4]>0){//M2 &&M3 && M4
      closest_dist = (closest_x[iM2]*closest_x[iM2]+closest_y[iM2]*closest_y[iM2]) +
        (closest_x[iM3]*closest_x[iM3]+closest_y[iM3]*closest_y[iM3])+
        (closest_x[iM4]*closest_x[iM4]+closest_y[iM4]*closest_y[iM4]);
      closest_dist = closest_dist/3.;
      return closest_dist;
    }
    if((*occupancy)[iM2]>0 && (*occupancy)[iM3]>0){//M2 &&M3
      closest_dist = (closest_x[iM2]*closest_x[iM2]+closest_y[iM2]*closest_y[iM2]) +
        (closest_x[iM3]*closest_x[iM3]+closest_y[iM3]*closest_y[iM3]);
      closest_dist = closest_dist/2.;
      return closest_dist;
    }
    if((*occupancy)[iM2]>0 && (*occupancy)[iM4]>0){//M2 &&M4
      closest_dist = (closest_x[iM2]*closest_x[iM2]+closest_y[iM2]*closest_y[iM2]) +
        (closest_x[iM4]*closest_x[iM4]+closest_y[iM4]*closest_y[iM4]);
      closest_dist = closest_dist/2.;
      return closest_dist;
    }
    if((*occupancy)[iM3]>0 && (*occupancy)[iM4]>0){//M3 &&M4
      closest_dist = (closest_x[iM3]*closest_x[iM3]+closest_y[iM3]*closest_y[iM3]) +
        (closest_x[iM4]*closest_x[iM4]+closest_y[iM4]*closest_y[iM4]);
      closest_dist = closest_dist/2.;
      return closest_dist;
    }
  }//3-6

  if(p>m_MomentumCuts[0]){
    if((*occupancy)[iM2]>0 && (*occupancy)[iM3]>0 && (*occupancy)[iM4]>0 && (*occupancy)[iM5]>0){
      //M2 &&M3&&M4&&M5
      closest_dist = (closest_x[iM2]*closest_x[iM2]+closest_y[iM2]*closest_y[iM2]) +
        (closest_x[iM3]*closest_x[iM3]+closest_y[iM3]*closest_y[iM3])+
        (closest_x[iM4]*closest_x[iM4]+closest_y[iM4]*closest_y[iM4])+
        (closest_x[iM5]*closest_x[iM5]+closest_y[iM5]*closest_y[iM5]);
      closest_dist = closest_dist/4.;
      return closest_dist;
    }
    if((*occupancy)[iM2]>0 && (*occupancy)[iM3]>0 && (*occupancy)[iM4]){//M2 && M3 && M4
      closest_dist = (closest_x[iM2]*closest_x[iM2]+closest_y[iM2]*closest_y[iM2]) +
        (closest_x[iM3]*closest_x[iM3]+closest_y[iM3]*closest_y[iM3])+
        (closest_x[iM4]*closest_x[iM4]+closest_y[iM4]*closest_y[iM4]);
      closest_dist = closest_dist/3.;
      return closest_dist;
    }
    if((*occupancy)[iM2]>0 && (*occupancy)[iM3]>0 && (*occupancy)[iM5]){//M2 && M3 && M5
      closest_dist = (closest_x[iM2]*closest_x[iM2]+closest_y[iM2]*closest_y[iM2]) +
        (closest_x[iM3]*closest_x[iM3]+closest_y[iM3]*closest_y[iM3]) +
        (closest_x[iM5]*closest_x[iM5]+closest_y[iM5]*closest_y[iM5]);
      closest_dist = closest_dist/3.;
      return closest_dist;
    }
    if((*occupancy)[iM3]>0 && (*occupancy)[iM4]>0 && (*occupancy)[iM5]>0){//M3 &&M4 && M5
      closest_dist = (closest_x[iM3]*closest_x[iM3]+closest_y[iM3]*closest_y[iM3]) +
        (closest_x[iM4]*closest_x[iM4]+closest_y[iM4]*closest_y[iM4])+
        (closest_x[iM5]*closest_x[iM5]+closest_y[iM5]*closest_y[iM5]);
      closest_dist = closest_dist/3.;
      return closest_dist;
    }
    if((*occupancy)[iM2]>0 && (*occupancy)[iM4]>0 && (*occupancy)[iM5]>0){//M2 &&M4 && M5
      closest_dist = (closest_x[iM2]*closest_x[iM2]+closest_y[iM2]*closest_y[iM2]) +
        (closest_x[iM4]*closest_x[iM4]+closest_y[iM4]*closest_y[iM4])+
        (closest_x[iM5]*closest_x[iM5]+closest_y[iM5]*closest_y[iM5]);
      closest_dist = closest_dist/3.;
      return closest_dist;
    }
  }

  if(!closest_dist) return 0;
  return closest_dist;

}

int MuonIDAlg::GetPbin(double p, int region){
  std::vector<double> * pBins=0;
  switch( region+1 ){
  case 1:{ pBins = &m_MupBinsR1; break;}
  case 2:{ pBins = &m_MupBinsR2; break;}
  case 3:{ pBins = &m_MupBinsR3; break;}
  case 4:{ pBins = &m_MupBinsR4; break;}
  }
  if (msgLevel(MSG::VERBOSE) ) verbose() << "GetPbin: region+1 " << region+1 << " p " <<  p << " pBins vector address: " 
                                     << pBins << endmsg;
  if(0 == pBins) throw GaudiException("GetPbin: No match to a pBins vector. Null pBins pointer", "",StatusCode::FAILURE);
  for(unsigned int iBin=0; iBin<pBins->size();++iBin){
    if (msgLevel(MSG::VERBOSE) ) verbose() << "GetPbin:\tiBin " <<  iBin << " pBins[iBin] " << (*pBins)[iBin] << endmsg;
    if(p < (*pBins)[iBin]) return iBin;
  }
  return pBins->size();  // last bin. npBins goes from 0 to (Gaias npBin)+1
}

//=====================================================================
double MuonIDAlg::calc_ProbMu_tanh(const double& tanhdist0, int pBin, int region){
  int nDistBins = (*(*m_tanhCumulHistoMuon[region])[pBin]).size();
  int itanhdist=int(tanhdist0*nDistBins);

  //if(itanhdist>nDistBins)itanhdist--;
  if(itanhdist>=nDistBins) itanhdist=nDistBins-1;

  if (msgLevel(MSG::DEBUG) ) debug() << "calc_ProbMu_tanh: region " << region << " pBin " << pBin << " tanh(dist) " << tanhdist0
          << " itanhdist " << itanhdist << " ProbMu " << (*((*(m_tanhCumulHistoMuon[region]))[pBin]))[itanhdist] << endmsg;
  return (*((*(m_tanhCumulHistoMuon[region]))[pBin]))[itanhdist];
}

//=====================================================================
double MuonIDAlg::calc_ProbNonMu_tanh(const double& tanhdist0, int pBin, int region){
  int nDistBins = (*(*m_tanhCumulHistoNonMuon[region])[pBin]).size();
  int itanhdist=int(tanhdist0*nDistBins);
  //if(itanhdist>nDistBins)itanhdist--;
  if(itanhdist>=nDistBins) itanhdist=nDistBins-1;
  if (msgLevel(MSG::DEBUG) ) debug() << "calc_ProbNonMu_tanh: region " << region << " pBin " << pBin << " tanh(dist) " 
                                     << tanhdist0 << " itanhdist " << itanhdist << " ProbNonMu " 
                                     << (*((*(m_tanhCumulHistoNonMuon[region]))[pBin]))[itanhdist] << endmsg;
  return (*((*(m_tanhCumulHistoNonMuon[region]))[pBin]))[itanhdist];
}

//=====================================================================
double MuonIDAlg::calc_ProbMu(const double& dist0, const double *parMu){
  //=====================================================================
  // comment: Returns for a given track of dist0 the probability to be a muon;
  // authors: G. Lanfranchi & S. Furcas,
  // date:    10/5/09
  //=====================================================================

  double ProbMu=0;
  TF1 myF("myF",land2,0,m_x*m_nMax,5);
  myF.SetParameters(parMu[0],parMu[1],parMu[2],parMu[3],parMu[4]);
  ProbMu = myF.Integral(0,dist0);

  if(parMu[5]>0){
    return ProbMu = ProbMu/parMu[5];
  }else{
    Warning(" normalization out of control ",StatusCode::FAILURE).ignore();
    return -1;
  }

}


double MuonIDAlg::calc_ProbNonMu(const double& dist0, const double *parNonMu){
  //=====================================================================
  // comment: Returns for a given track of dist0 the probability to be a non-muon;
  // authors: G. Lanfranchi & S. Furcas,
  // date:    10/5/09
  // Modified by Helder Lopes on January 2014 to solve https://sft.its.cern.ch/jira/browse/ROOT-5985
  //=====================================================================

  double Prob = ROOT::Math::landau_cdf(dist0, parNonMu[1], parNonMu[0]) - ROOT::Math::landau_cdf(0, parNonMu[1], parNonMu[0]);

  if(parNonMu[2]>0){
    if (msgLevel(MSG::DEBUG) ) debug() << "probnmu, parNonMu[2] : "<< Prob <<","<< parNonMu[2] << endmsg;
    return Prob = Prob/parNonMu[2];
  }else{
    Warning("ProbNonMu: normalization out of control ",StatusCode::FAILURE).ignore();
    return -1;
  }
}

StatusCode MuonIDAlg::calcLandauNorm(){
  //=====================================================================
  // comment: Normalizations for Landaus for Muons and Non-Muons
  // authors: G. Lanfranchi & S. Furcas,
  // date:    10/5/09
  //=====================================================================

  double par[5];
  double parnmu[3];
  for(int i=1;i<5;i++){par[i]=0;}
  for(int i=1;i<3;i++){parnmu[i]=0;}
  double Norm = -1;

  // For muons, only used if m_dllFlag == 1
  if(m_dllFlag == 1){
    //===================
    //  Muons - Region 1
    //===================
    for(int i=0;i<5;i++){par[i]=m_MuLanParR1_1[i];}
    Norm = calcNorm(par);
    if (Norm<0 || Norm==0) return Error("normalization of Muon R1 bin1 out of control");
    m_MuLanParR1_1[5] = Norm;

    for(int i=0;i<5;i++){par[i]=m_MuLanParR1_2[i];}
    Norm = calcNorm(par);
    if (Norm<0 || Norm==0) return Error("normalization of Muon R1 bin2 out of control");
    m_MuLanParR1_2[5] = Norm;

    for(int i=0;i<5;i++){par[i]=m_MuLanParR1_3[i];}
    Norm = calcNorm(par);
    if (Norm<0 || Norm==0) return Error("normalization of Muon R1 bin3 out of control");
    m_MuLanParR1_3[5] = Norm;

    for(int i=0;i<5;i++){par[i]=m_MuLanParR1_4[i];}
    Norm = calcNorm(par);
    if (Norm<0 || Norm==0) return Error("normalization of Muon R1 bin4 out of control");
    m_MuLanParR1_4[5] = Norm;

    for(int i=0;i<5;i++){par[i]=m_MuLanParR1_5[i];}
    Norm = calcNorm(par);
    if (Norm<0 || Norm==0) return Error("normalization of Muon R1 bin5 out of control");
    m_MuLanParR1_5[5] = Norm;

    for(int i=0;i<5;i++){par[i]=m_MuLanParR1_6[i];}
    Norm = calcNorm(par);
    if (Norm<0 || Norm==0) return Error("normalization of Muon R1 bin6 out of control");
    m_MuLanParR1_6[5] = Norm;

    for(int i=0;i<5;i++){par[i]=m_MuLanParR1_7[i];}
    Norm = calcNorm(par);
    if (Norm<0 || Norm==0) return Error("normalization of Muon R1 bin7 out of control");
    m_MuLanParR1_7[5] = Norm;

    //====================
    //  Muons - Region 2
    //====================

    for(int i=0;i<5;i++){par[i]=m_MuLanParR2_1[i];}
    Norm = calcNorm(par);
    if (Norm<0 || Norm==0) return Error("normalization of Muon R2 bin1 out of control");
    m_MuLanParR2_1[5] = Norm;

    for(int i=0;i<5;i++){par[i]=m_MuLanParR2_2[i];}
    Norm = calcNorm(par);
    if (Norm<0 || Norm==0) return Error("normalization of Muon R2 bin2 out of control");
    m_MuLanParR2_2[5] = Norm;

    for(int i=0;i<5;i++){par[i]=m_MuLanParR2_3[i];}
    Norm = calcNorm(par);
    if (Norm<0 || Norm==0) return Error("normalization of Muon R2 bin3 out of control");
    m_MuLanParR2_3[5] = Norm;

    for(int i=0;i<5;i++){par[i]=m_MuLanParR2_4[i];}
    Norm = calcNorm(par);
    if (Norm<0 || Norm==0) return Error("normalization of Muon R2 bin4 out of control");
    m_MuLanParR2_4[5] = Norm;

    for(int i=0;i<5;i++){par[i]=m_MuLanParR2_5[i];}
    Norm = calcNorm(par);
    if (Norm<0 || Norm==0) return Error("normalization of Muon R2 bin5 out of control");
    m_MuLanParR2_5[5] = Norm;

    //====================
    //  Muons - Region 3
    //====================

    for(int i=0;i<5;i++){par[i]=m_MuLanParR3_1[i];}
    Norm = calcNorm(par);
    if (Norm<0 || Norm==0) return Error("normalization of Muon R3 bin1 out of control");
    m_MuLanParR3_1[5] = Norm;

    for(int i=0;i<5;i++){par[i]=m_MuLanParR3_2[i];}
    Norm = calcNorm(par);
    if (Norm<0 || Norm==0) return Error("normalization of Muon R3 bin2 out of control");
    m_MuLanParR3_2[5] = Norm;

    for(int i=0;i<5;i++){par[i]=m_MuLanParR3_3[i];}
    Norm = calcNorm(par);
    if (Norm<0 || Norm==0) return Error("normalization of Muon R3 bin3 out of control");
    m_MuLanParR3_3[5] = Norm;

    for(int i=0;i<5;i++){par[i]=m_MuLanParR3_4[i];}
    Norm = calcNorm(par);
    if (Norm<0 || Norm==0) return Error("normalization of Muon R3 bin4 out of control");
    m_MuLanParR3_4[5] = Norm;

    for(int i=0;i<5;i++){par[i]=m_MuLanParR3_5[i];}
    Norm = calcNorm(par);
    if (Norm<0 || Norm==0) return Error("normalization of Muon R3 bin5 out of control");
    m_MuLanParR3_5[5] = Norm;

    //====================
    //  Muons - Region 4
    //====================

    for(int i=0;i<5;i++){par[i]=m_MuLanParR4_1[i];}
    Norm = calcNorm(par);
    if (Norm<0 || Norm==0) return Error("normalization of Muon R4 bin1 out of control");
    m_MuLanParR4_1[5] = Norm;

    for(int i=0;i<5;i++){par[i]=m_MuLanParR4_2[i];}
    Norm = calcNorm(par);
    if (Norm<0 || Norm==0) return Error("normalization of Muon R4 bin2 out of control");
    m_MuLanParR4_2[5] = Norm;

    for(int i=0;i<5;i++){par[i]=m_MuLanParR4_3[i];}
    Norm = calcNorm(par);
    if (Norm<0 || Norm==0) return Error("normalization of Muon R4 bin3 out of control");
    m_MuLanParR4_3[5] = Norm;

    for(int i=0;i<5;i++){par[i]=m_MuLanParR4_4[i];}
    Norm = calcNorm(par);
    if (Norm<0 || Norm==0) return Error("normalization of Muon R4 bin4 out of control");
    m_MuLanParR4_4[5] = Norm;

    for(int i=0;i<5;i++){par[i]=m_MuLanParR4_5[i];}
    Norm = calcNorm(par);
    if (Norm<0 || Norm==0) return Error("normalization of Muon R4 bin5 out of control");
    m_MuLanParR4_5[5] = Norm;
  }

  //=========================
  //  Non-Muons - Region 1
  //=========================

  for(int i=0;i<3;i++){parnmu[i]=m_NonMuLanParR1[i];}
  Norm = calcNorm_nmu(parnmu);
  if (Norm<0 || Norm==0) return Error("normalization of Non-Muon R1 out of control");
  m_NonMuLanParR1[2] = Norm;

  for(int i=0;i<3;i++){parnmu[i]=m_NonMuLanParR2[i];}
  Norm = calcNorm_nmu(parnmu);
  if (Norm<0 || Norm==0)  return Error("normalization of Non-Muon R2 out of control");
  m_NonMuLanParR2[2] = Norm;

  for(int i=0;i<3;i++){parnmu[i]=m_NonMuLanParR3[i];}
  Norm = calcNorm_nmu(parnmu);
  if (Norm<0 || Norm==0)  return Error("normalization of Non-Muon R3 out of control");
  m_NonMuLanParR3[2] = Norm;

  for(int i=0;i<3;i++){parnmu[i]=m_NonMuLanParR4[i];}
  Norm = calcNorm_nmu(parnmu);
  if (Norm<0 || Norm==0)  return Error("normalization of Non-Muon R4 out of control");
  m_NonMuLanParR4[2] = Norm;

  return StatusCode::SUCCESS;

}

//=====================================================================
double MuonIDAlg::calcNorm(double *par){
  //=====================================================================
  // comment: Calculate Normalizations for muons
  // authors: G. Lanfranchi & S. Furcas,
  // date:    10/5/09
  //=====================================================================

  double Norm = 0.;

  TF1 myF("myF",land2,0,m_x*m_nMax,5);
  myF.SetParameters(par[0],par[1],par[2],par[3],par[4]);
  Norm = myF.Integral(0,m_x*m_nMax);

  return Norm;
}


double MuonIDAlg::calcNorm_nmu(double *par){
  //=====================================================================
  // comment: Calculate Normalizations for non-muons
  // authors: G. Lanfranchi & S. Furcas,
  // date:    10/5/09
  // Modified by Helder Lopes on January 2014 to solve https://sft.its.cern.ch/jira/browse/ROOT-5985
  //=====================================================================

  double Norm =  ROOT::Math::landau_cdf(m_x*m_nMax,par[1],par[0]) - ROOT::Math::landau_cdf(0.,par[1],par[0]);

  return Norm;
}
//=============================================================================
// Track extrapolation
//=============================================================================
StatusCode MuonIDAlg::trackExtrapolate(const LHCb::Track *pTrack){

  resetTrackLocals();

  // get first state for the preSel cut
  const LHCb::State * state1 = &(pTrack->firstState());
  // get state closest to M1 for extrapolation
  const LHCb::State * state = &(pTrack->closestState(9450.));

  if(!state1) return Error(" Failed to get 1st state from track ");
  if(!state)  return Error(" Failed to get state from track ");

  // get the momentum (MeV/c)
  m_Momentum = state->p();
  m_MomentumPre = state1->p();

  // get the track slope
  m_trackSlopeX = state->tx();

  // Project the state into the muon stations
  int station;
  for(station = 0; station < m_NStation ; station++){
    // x(z') = x(z) + (dx/dz * (z' - z))
    m_trackX.push_back(state->x() + ( state->tx() *
                                      (m_stationZ[station] - state->z()) ));
    m_trackY.push_back(state->y() + ( state->ty() *
                                      (m_stationZ[station] - state->z()) ));
    if (msgLevel(MSG::DEBUG) ) debug() << "trackExtrapolate: station: " << station << " m_trackX: " 
        << m_trackX[station] << " m_trackY: " << m_trackY[station] << endmsg;
  }

  return StatusCode::SUCCESS;
}

//=============================================================================
// return the FOI in x in a station and region for momentum (in MeV/c)
// this is a simpler version of the parameterization:
// foi = par0 + par2*exp(-par3*p)
//=============================================================================
std::pair<double,double> MuonIDAlg::foiXY(int station, int region, double p) const
{
  int i  = station * m_NRegion + region;
  if (p < 1000000. ){
    auto pGeV = p/Gaudi::Units::GeV;
    std::array<double,2> pp = { -m_xfoiParam3[ i ]*pGeV , 
                                -m_yfoiParam3[ i ]*pGeV };
    return { ( m_xfoiParam1[ i ] + m_xfoiParam2[ i ]* vdt::fast_exp(pp[0]) ), 
             ( m_yfoiParam1[ i ] + m_yfoiParam2[ i ]* vdt::fast_exp(pp[1]) ) };

  }else{
    return { m_xfoiParam1[ i ],
             m_yfoiParam1[ i ]  } ;
  }

  //in the future optimize this checking that 2*dx =m_padSizeX[station * m_NRegion + region]
  //then eliminates dx from function
}

//=============================================================================
// reset vectors
//=============================================================================
void MuonIDAlg::resetTrackLocals(){
  m_Momentum = -1.;
  m_MomentumPre = -1.;
  m_trackSlopeX = 0.;
  m_trackX.clear();
  m_trackY.clear();
  m_occupancyAll.clear();
  m_occupancyWithCrossing.clear();
  m_CoordX.clear();
  m_occupancyAll.resize(m_NStation,0);
  m_occupancyWithCrossing.resize(m_NStation,0);
  m_CoordX.resize(m_NStation,0.);
  m_dist_out = -1;
}

//=============================================================================
// make the muon segments
//=============================================================================
LHCb::Track* MuonIDAlg::makeMuonTrack(const LHCb::MuonPID& mupid){

  const LHCb::Track* mother = mupid.idTrack();
  double Quality=-1;
  LHCb::Track* mtrack;

  //XCV: if Kalman Foi, can use directly the existing muon track!
  if (m_kalman_foi){    
    if (m_FindQuality) {
      StatusCode sc;
      if (m_use_dist){
        // distance already calculated!
        sc = StatusCode::SUCCESS;
        if (mupid.IsMuonLoose()) Quality = m_dist_out;
        else Quality = -1.0;
      }

      // get chi2 value
      else{
        if (m_mutrack.states().size()) sc = myMuIDTool()->muonQuality(m_mutrack,Quality);
        else sc=StatusCode::FAILURE;
      }
      
      if (sc.isSuccess()) {
        if (msgLevel(MSG::DEBUG) ) debug()<<"\t Quality="<< Quality<<endmsg;
      }
      else {
        if (msgLevel(MSG::DEBUG) ) debug()<<"Error when fitting track"<<endmsg;
      }
    }
  
    if (m_mutrack.states().size()) mtrack = m_mutrack.cloneWithKey();
    else 
    {
      mtrack = new LHCb::Track( mupid.key() );
      mtrack->addToStates( mother->closestState(9450.) );
    }
  }
  
  else{
    
    mtrack = new LHCb::Track( mupid.key() );
    std::vector<LHCb::MuonCoord*> & mcoord = m_muonMap[(LHCb::MuonPID*) &mupid];
    //add mother track to ancestors
    mtrack->addToAncestors(*mother);
    mtrack->addToStates( mother->closestState(9450.) );
    std::vector<LHCb::MuonCoord*>::const_iterator iCoord;      
    std::vector<LHCb::LHCbID> ids_init;
      
    //charge lhcbids to track and fit  
    for(iCoord = mcoord.begin(); iCoord != mcoord.end(); ++iCoord ) {
      LHCb::MuonTileID tile= (*iCoord)->key();
      LHCb::LHCbID id(tile);
      mtrack->addToLhcbIDs(id);
      ids_init.push_back(id);
    }
  
    if (msgLevel(MSG::DEBUG) ) debug()<<"ids ready to get chi2"<<endmsg;

  
    if (m_FindQuality) {
      // get chi2 value
      LHCb::Track mtrack_partial;
      if (msgLevel(MSG::DEBUG) ) debug()<<"myMuIDTool="<<myMuIDTool()<<endmsg;
      if (!ids_init.empty()) {
        StatusCode sc = myMuIDTool()-> muonCandidate(*mother, mtrack_partial, ids_init);
        if (!sc.isFailure()) {
          std::vector<LHCb::LHCbID>::const_iterator id;
          for(id = mtrack_partial.lhcbIDs().begin() ;
              id !=  mtrack_partial.lhcbIDs().end() ; id++){
            if (msgLevel(MSG::DEBUG) ) {
              debug()<< "id is muon? "<<id->isMuon()<<endmsg;
              if (id->isMuon())
                debug()<< "id station  " << id->muonID().station()<<endmsg;
              debug()<< "id channelID="<< id->channelID()<<endmsg;
            }
          }
          
          StatusCode sc2 = myMuIDTool()->muonQuality(mtrack_partial,Quality);
          if (!sc2.isFailure()) {
            if (msgLevel(MSG::DEBUG) ) debug()<<"\t Quality="<< Quality<<endmsg;
            mtrack->setChi2PerDoF(Quality);
          }
          else {
            if (msgLevel(MSG::DEBUG) ) debug()<<"Error when preparing track to fit"<<endmsg;
          }  
        }
      }
    }
  }


  mtrack->setType(LHCb::Track::Muon);
  mtrack->setHistory(LHCb::Track::MuonID);


  mtrack->addInfo(LHCb::Track::MuonChi2perDoF,Quality);
  mtrack->addInfo(LHCb::Track::MuonMomentumPreSel,mupid.PreSelMomentum());
  mtrack->addInfo(LHCb::Track::MuonInAcceptance,mupid.InAcceptance());
  mtrack->addInfo(LHCb::Track::IsMuonLoose,mupid.IsMuonLoose());
  mtrack->addInfo(LHCb::Track::IsMuon,mupid.IsMuon());
  mtrack->addInfo(LHCb::Track::IsMuonTight,mupid.IsMuonTight());

  double Dist=-1.;
  double DLL=-1.;
  double NShared=-1;
  if (mupid.IsMuonLoose()) {
    Dist    = m_dist_out;
    DLL     = mupid.MuonLLMu()-mupid.MuonLLBg();
    NShared = mupid.nShared();
  }
  if (msgLevel(MSG::DEBUG) ) debug()<< "makeMuonTrack:: Key, p,Dist, DLL, NShared: "
                                    <<mtrack->key()<< " " << mtrack->p() << " " << Dist<<" "<<DLL<<" "<<NShared<<endmsg;

  mtrack->addInfo(LHCb::Track::MuonDist2,Dist);
  mtrack->addInfo(LHCb::Track::MuonDLL,DLL);
  mtrack->addInfo(LHCb::Track::MuonNShared,NShared);

  //double CLQuality=-1.;
  //double CLArrival=-1.;
  //mtrack->addInfo(LHCb::Track::MuonCLQuality,CLQuality);
  //mtrack->addInfo(LHCb::Track::MuonCLArrival,CLArrival);


  return mtrack;
}

double land2(Double_t *x, Double_t *par) {

  double result = 0;
  Double_t landau1 = TMath::Landau(x[0],par[0],par[1]);
  Double_t landau2 = TMath::Landau(x[0],par[2],par[3]);
  result = (landau1 + par[4]*landau2);

  return result;

}
