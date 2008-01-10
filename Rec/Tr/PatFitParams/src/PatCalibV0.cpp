// $Id: PatCalibV0.cpp,v 1.1 2008-01-10 15:36:43 ocallot Exp $
// Include files

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h"

#include "Event/RecVertex.h"
// local
#include "PatCalibV0.h"
#include "GaudiKernel/SystemOfUnits.h"

//-----------------------------------------------------------------------------
// Implementation file for class : PatCalibV0
//
// 2007-02-09 : Maurice Benayoun & Olivier Callot
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( PatCalibV0 );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
PatCalibV0::PatCalibV0( const std::string& name,
                        ISvcLocator* pSvcLocator)
  : GaudiHistoAlg  ( name , pSvcLocator )
{
  declareProperty( "zVtxImprovMn",       m_zMinVtxImp =    200. * Gaudi::Units::mm );      // mm   minimum zV (unused)
  declareProperty( "zVtxImprovMx",       m_zMaxVtxImp =   2300. * Gaudi::Units::mm );    // mm   maximum zV (unused)
  declareProperty( "pMin",               m_pMinRich1 =  2000. * Gaudi::Units::MeV );  // minimum pt of charged tracks
  declareProperty( "zRef",               m_zRefRich1 =  2350. * Gaudi::Units::mm );  //max limit  for vtx reconstruction
  declareProperty( "zKsMin",             m_zKsMin    =     0. * Gaudi::Units::mm );  //min limit  for vtx reconstruction
  declareProperty( "protonMass",  m_Proton    =   938.272029 * Gaudi::Units::MeV );
  declareProperty( "pionMass",           m_Pion      =   139.57018 * Gaudi::Units::MeV );
  declareProperty( "kaonMass",           m_Kshort    =   497.648 * Gaudi::Units::MeV );
  declareProperty( "lambdaMass",  m_Lambda    =  1115.683* Gaudi::Units::MeV  );
  declareProperty( "massCutNsig",        m_Nsig          =   3.0  );          // # of sigmas Cut for Ks / Lambda/LambdaBar
  declareProperty( "kaonSig",            m_SigK0     =   20. * Gaudi::Units::MeV );   // sigma Ks Cut  :: histo 200
  declareProperty( "lambdaSig",          m_SigLambda   =  6. * Gaudi::Units::MeV );// sigma (anti)Lambda Cut::  histo 201/203
  declareProperty( "ImpactParam",  m_impactParV0 = 20. * Gaudi::Units::mm ); // mm origin 30 (DERIVED from histo 300, sqrt(x2+y2)
  declareProperty( "ImpactParamTrkX",    m_impactParTrkX =   60. * Gaudi::Units::mm );    // mm origin 60 (HISTO 11
  declareProperty( "ImpactParamTrkY",    m_impactParTrkY =   40. * Gaudi::Units::mm );    //  mm origin 40 (HISTO 12
  declareProperty( "ImpactParamSlp",     m_impactParSlp  = 0.25 );            //    SlpX or SlpY (rd) origin 0.25(HISTO 13/14
  declareProperty( "SigmaXKs",           m_sigmaX     =    4.43 * Gaudi::Units::mm );     //    reco vtx  histo 101
  declareProperty( "SigmaYKs",           m_sigmaY     =    3.72 * Gaudi::Units::mm );     //    reco vtx   :: histo 102
  declareProperty( "SigmaZKs",           m_sigmaZ     =   77.16 * Gaudi::Units::mm );     //    reco vtx  histo 100 (unused)
  declareProperty( "IterateVtx",         m_iterVtx    =       5     );       // # iter in improved vtx fit
  declareProperty( "AngleCut",           m_angleCut   = 25.e-03 ); //maximum angle ( vtx dir , MomDir) in radians (histos 121/122
  declareProperty( "CheckPT",            m_checkPt    =   true    );       //  if true check of V0 pT is active
  declareProperty( "CutPT", m_cutPt = 0.01 * Gaudi::Units::GeV );//minimum pt of the V0 *in GeV*, 0 = no cut (hist 130 to 132
  declareProperty( "Chi2VTXLim",         m_chi2VtxLim    =  10.); //  max chi2 for V0 vertex fit                  (histos 5 to 7
  declareProperty( "Chi2VTXPrimLim",     m_primVtxOKLim  =   5.); //  min chi2 for Prim vertex normalized distance(histos 17 to 19
  declareProperty( "impactCurrentNorm",  m_impactCurNormLim  =   4.); //  minimum V0 impact parameter at origin (histos 127/128
}
//=============================================================================
// Destructor
//=============================================================================
PatCalibV0::~PatCalibV0() {}

//=============================================================================
// Initialization
//=============================================================================
StatusCode PatCalibV0::initialize() {
  StatusCode sc = GaudiHistoAlg::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  initialiseRichVelo_Ks();  // initialise reco for Kshort tracks

  m_plotChi2=book(5, "   chi2 of improved vtx fit (all pairs)", 0.,25.,50);
  m_plotChi2Ks=book(6 , " (Truth) chi2 of improved vtx fit (Ks pairs only)", 0.,25.,50);
  m_plotChi2Lambda=book(7 , " (Truth) chi2 of improved vtx fit (Lambda pairs only)", 0.,25.,50);

  m_resolution = book(8, " Truth : reco momentum resolution (percents) all mc-tracks  ", -50.,50.,100);

  m_pQuality=book2D(9, "Truth : reco momentum resolution (percents) vs true mom. (GeV) ",-100.,100.,200,-50.,50.,100);
  m_recoMomentum = book(10, " Calibration : reco momentum (GeV) ", -60.,60.,200);

  m_xOrg = book(11,"Calibration, Accepted and Reco Downstream Tracks: x PhSp at Origin (mm)",-100.,100.,100);
  m_yOrg = book(12,"Calibration, Accepted and Reco Downstream Tracks: y PhSp at Origin (mm)",-100.,100.,100);
  m_txOrg = book(13,"Calibration, Accepted and Reco Downstream Tracks : tx PhSp at Origin (rd)",-1.,1.,100);
  m_tyOrg = book(14,"Calibration, Accepted and Reco Downstream Tracks: ty PhSp at Origin (rd)",-1.,1.,100);

  m_zPrimVtxh = book(15, "  Primary Vertex zCoord (mm) ",-200.,300.,200);
  m_xyPrimVtx = book2D(16," Primary Vertex (x,y) Coord (mm)",-2,2,20,-2,2,20);

  m_checkPrimChi2 = book(17," Calibration : chi2 for prim. vtx test (all pairs)",0.,200.,200);
  m_checkPrimChi2Ks = book(18,"Truth : chi2 for prim. vtx test primOK (true Ks)",0.,200.,200);
  m_checkPrimChi2Lambda = book(19,"Truth : chi2 for prim. vtx test primOK (true Lambda)",0.,200.,200);
  m_checkPrimChi2LambdaBar = book(20,"Truth : chi2 for prim. vtx test primOK (true LambdaBar)",0.,200.,200);

  m_trkX_K0=book(51,"Truth :All Accepted Ks,  Track Phase Space at Origin  x (mm)",-100.,100.,100);
  m_trkY_K0=book(52,"Truth :All  Accepted Ks,  Track Phase Space at Origin  y (mm)",-100.,100.,100);
  m_trkSlpX_K0=book(53,"Truth :All  Accepted Ks,  Track Phase Space at Origin  slope x (rd)",-1.,1.,100);
  m_trkSlpY_K0=book(54,"Truth :All  Accepted Ks,  Track Phase Space at Origin  slope y (rd)",-1.,1.,100);

  m_trkX_Lambda=book(61,"Truth :All Accepted Lambda,  Track Phase Space at Origin  x (mm)",-100.,100.,100);
  m_trkY_Lambda=book(62,"Truth :All  Accepted Lambda,  Track Phase Space at Origin  y (mm)",-100.,100.,100);
  m_trkSlpX_Lambda=book(63,"Truth :All  Accepted Lambda,  Track Phase Space at Origin  slope x (rd)",-1.,1.,100);
  m_trkSlpY_Lambda=book(64,"Truth :All  Accepted Lambda,  Track Phase Space at Origin  slope y (rd)",-1.,1.,100);

  m_trkX_LambdaBar=book(71,"Truth :All Accepted LambdaBar,  Track Phase Space at Origin  x (mm)",-100.,100.,100);
  m_trkY_LambdaBar=book(72,"Truth :All  Accepted LambdaBar,  Track Phase Space at Origin  y (mm)",-100.,100.,100);
  m_trkSlpX_LambdaBar=book(73,"Truth :All  Accepted LambdaBar,  Track Phase Space at Origin  slope x (rd)",-1.,1.,100);
  m_trkSlpY_LambdaBar=book(74,"Truth :All  Accepted LambdaBar,  Track Phase Space at Origin  slope y (rd)",-1.,1.,100);



  m_MCdz_K0 = book(100, " Truth : good Ks vtx zTrue-zRec (mm) ", -1000.,1000.,50);
  m_MCdx_K0 = book(101, " Truth : good Ks vtx xTrue-xRec  (mm) ", -100.,100.,50);
  m_MCdy_K0 = book(102, " Truth : good Ks vtx yTrue-yRec  (mm) ", -100.,100.,50);

  m_MCdz_Lambda = book(105, " Truth : good (anti)Lambda  vtx zTrue-zRec (mm) ", -1000.,1000.,50);
  m_MCdx_Lambda = book(106, " Truth : good (anti)Lambda  vtx xTrue-xRec  (mm) ", -100.,100.,50);
  m_MCdy_Lambda = book(107, " Truth : good (anti)Lambda  vtx yTrue-yRec  (mm) ", -100.,100.,50);


  m_nbPrimVtx=book(109," number of primary vertices",-4.5,10.5,15);
  m_zTrue = book(110," Truth : True z Loc of V0  Vtx (mm) ",-100.,2500.,50);
  m_zRec = book(120, " Truth :  Reco z Loc of V0 Vtx (mm) ",-100.,2500.,50);

  m_vtxMomAngAll=book(121," angle (mrd) vtx Mom, All pairs reco",-90.,90.,180);
  m_vtxMomAngV0=book(122," angle (mrd) vtx Mom, true V0 reco",-90.,90.,180);

  m_impactParamV0=book(125, " Truth :       Reco Impact parameter at Prim Vtx s(mm) (V0 Only) ",0.,25.,50);
  m_impactParam  =book(126, " Calibration : Reco Impact parameter at Prim Vtx s(mm) (all Pairs kept) ",0.,25.,50);
  m_impactParamV0Norm=book(127, " Truth :       Reco Impact parameter at Prim Vtx s   normed (V0 Only) ",0.,10.,20);
  m_impactParamNorm  =book(128, " Calibration :  Reco Impact parameter at Prim Vtx s normed (all Pairs kept) ",0.,10.,20);

  m_ptMCTrue_K0 = book(130," Truth : True pT of Ks (GeV)",0.,5.,100);
  m_ptMCTrue_Lambda= book(131," Truth : True pT of Lambda (GeV)",0.,5.,100);
  m_ptMCTrue_LambdaBar= book(132," Truth : True pT of LambdaBar (GeV)",0.,5.,100);

  m_ptMCRec_K0 = book(140," Truth : Reco pT of Ks (GeV)",0.,5.,100);
  m_ptMCRec_Lambda = book(141," Truth : Reco pT of Lambda(GeV)",0.,5.,100);
  m_ptMCRec_LambdaBar = book(142," Truth : Reco pT of LambdaBar (GeV)",0.,5.,100);

  m_invMassKs = book(200, " Truth : Kshort Invariant Mass (MeV) ",0.,1000.,100);
  m_invMassBothLambda = book(201, " Truth : merged Lambda and LambdaBar Invariant Mass (MeV) ",500.,1500.,200);
  m_invMassLambda = book(202, " Truth : Lambda Invariant Mass (MeV) ",500.,1500.,200);
  m_invMassLambdaBar = book(203, " Truth : LambdaBar Invariant Mass (MeV) ",500.,1500.,200);

  m_invMassAllPairs = book(251, " Truth : Kshort Invariant Mass (MeV) all pairs ",0.,1000.,50);
  m_impactAtOrigin= book2D(300," Truth : ALL V0 (x,y) impact at z = 0 (mm) ", -50.,50.,100,-50.,50.,100);

  m_multVsnVTX   = book2D(1599,"SelectSols : multiplicite versus Numbers of Prim Vtx",-2.5,8.5,11,-4.5,10.5,15);
  m_multiplicitySelect        = book(1600,"SelectSols : Pair Multiplicity distribution ",0.,25.,25);
  m_multiplicityPairSelect  = book(1601,"SelectSols  : Pair Total Multiplicity  ",-5.,5.,10);

  m_V0Degeneracy  = book(1605," SelectSols : V0 kinds in selected solutions",-0.5,14.5,15);

  m_KsMassSelect    = book(1800, " SelectSols : Kshort Invariant Mass (MeV) ",0.,1000.,100);
  m_LambdaMassSelect    = book(1801, " SelectSols : Lambda Invariant Mass (MeV) ",500.,1500.,100);
  m_antiLambdaMassSelect    = book(1802, " SelectSols : antiLambda Invariant Mass (MeV) ",500.,1500.,100);
  m_bestchi2Select  = book(1820,"  SelectSols :  chi2 distribution (all pairs)", 0.,10.,10);
  m_bestPrimOKSelect = book(1830,"  SelectSols :  chi2 dist to Prim Vtx (all pairs)",0.,25.,25);
  m_zRecSelect      = book(1840,"  SelectSols : Reco z Loc of V0 Vtx (mm) (all pairs)",-100.,2500.,100);

  m_armanteros_clear  =book2D(2001," Armanteros plot unambiguous assign.",-1.25,1.25,100,0.,0.3,100);
  m_armanteros_ambigu =book2D(2002," Armanteros plot  ambiguous assign.",-1.25,1.25,100,0.,0.3,100);
  m_armanteros_K0  =book2D(2101," Armanteros plot  true Kshort",-1.25,1.25,100,0.,0.3,100);
  m_armanteros_Lambda  =book2D(2102," Armanteros plot true Lambda",-1.25,1.25,100,0.,0.3,100);
  m_armanteros_antiLambda  =book2D(2104," Armanteros plot  true antiLambda",-1.25,1.25,100,0.,0.3,100);

  // a few counters
  for(int i=0;i<10;i++) {
    m_nCounters[i]=0;
    m_nCountersNoVelo[i]=0;

    m_nCounters_K0[i]=0;
    m_nCounters_NoVelo_K0[i]=0;

    m_nCounters_LambdaBar[i]=0;
    m_nCounters_NoVelo_LambdaBar[i]=0;


    m_nCounters_Lambda[i]=0;
    m_nCounters_NoVelo_Lambda[i]=0;
  }
  m_nK0courts=0;
  m_nLambda=0;
  m_nLambdaBar=0;
  m_nLambda_b=0;
  m_nLambda_b_Bar=0;
  m_acc_Ks=0;
  m_acc_Lambda=0;
  debug() << "==> Initialize" << endmsg;

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode PatCalibV0::execute() {

  debug() << "==> Execute" << endmsg;

  MCTrackInfo trackInfo( evtSvc(), msgSvc() );  //cf lignes 398 et 281 dans PatChecker.cpp
  // 21 Mai 2007
  /*  -------->> a desactiver apres usage <<---------------
      LHCb::MCParticle* part = *itP;
      bool isLong  = trackInfo.hasVeloAndT( part );
      isLong = isLong && ( abs( part->particleID().pid() ) != 11 ); // and not electron
      bool over5       = 5000. < fabs( part->p() );
      bool isInVelo    = trackInfo.hasVelo( part );
      bool strangeDown = false;
      bool fromKsFromB = false;
      strangeDown =  trackInfo.hasT( part ) &&  trackInfo.hasTT( part );

      info() << "=== Missed long track > 10 GeV in Forward === Velo Space = "
      << foundSpace << format( " flags %8x",  trackInfo.fullInfo( part ) )<< endreq;
      abs( part->particleID().pid() ) != 11 electrons
      if ( 0 != mother->originVertex()->mother() ) {
      if  ( 310 == pid &&
      2 == part->originVertex()->products().size()  &&
      mother->originVertex()->mother()->particleID().hasBottom() ) {
      fromKsFromB = true;
      }
      }
  */

  // ------------------------variables for MC Calibration (start)

  double xxPos=-999999;
  double yyPos=-999999;
  double zzPos=-999999;
  double xxNeg=-999999;
  double yyNeg=-999999;
  double zzNeg=-999999;
  double xPhSp=-999999;
  double yPhSp=-999999;
  double txPhSp=999999;
  double tyPhSp=999999;
  LHCb::MCParticle* mcP=0;
  const LHCb::MCParticle* parent=0;
  m_GrandParentCode=0;
  bool hasNoVeloPair=false;

  LHCb::Tracks* downs  = get<LHCb::Tracks>( LHCb::TrackLocation::Downstream );
  debug() << "Start from " << downs->size() << " downstream tracks." << endreq;

  LHCb::RecVertices* primaryVertices = get<LHCb::RecVertices>( LHCb::RecVertexLocation::Velo3D );
  debug() << "Found " << primaryVertices->size() << " primary vertices" << endreq;
  m_nbPrimVtx->fill( (double)(primaryVertices->size()) );
  m_pos.clear();
  m_neg.clear();
  m_solutions.clear();
  m_selected.clear();
  m_zPrimVtx.clear();

  for ( LHCb::Tracks::const_iterator itT = downs->begin(); downs->end() != itT; ++itT ) {
    mcP = mcTruth( *itT );
    LHCb::State& m_state=(*itT)->closestState(2350.);  // close to TT mid Location

    // construct track trajectory parameters
    m_QOverPTruth =999999999;

    createInputState( m_state );
    computeParamRichVelo_Ks_Der();
    computeParamRichVelo_Ks();
    // check track phase space
    bool skipTrk=checkPhaseSpaceAtOrigin();
    if(skipTrk) {
      debug() << " ... track " << (*itT)->key() << " not in phase space at origin " << endreq;
      debug() << format( "   x%7.2f y%7.2f tx%8.4f ty%8.4f",
                         xCoordFast( 0.), yCoordFast( 0. ),
                         xSlopeFast( 0. ), ySlopeFast( 0. ) ) << endreq;

      //      continue;   // not active *for checks*
    }

    storeTrackParams_prov();
    storePhaseSpaceAtOrigin();
    int charge =1;
    if(m_state.qOverP() < 0) charge =-1;

    m_recoMomentum->fill(1./m_state.qOverP()/1000); // GeV
    bool hasNoVeloTrack=true;
    int trackCode=0;
    int parentCode=0;
    int grandParentCode=0;
    if(mcP){
      double pTruth=mcP->p();
      double dp=pTruth-fabs(1./m_state.qOverP());
      m_resolution->fill(dp/pTruth*100.);
      m_pQuality->fill(pTruth/1000.,dp/pTruth*100.);
      hasNoVeloTrack=!trackInfo.hasVelo( mcP );
      trackCode=mcP->particleID().pid(); //
      parent=mcP->mother();
      // testLambda_b(mcP); // test of genealogy
      if(parent) {
        const LHCb::ParticleID & partIDmother=parent->particleID();
        parentCode=partIDmother.pid();

        if(parentCode==310) {
          m_nK0courts +=1;
        } else if(parentCode == 3122) {
          m_nLambda +=1;
        } else if(parentCode == -3122){
          m_nLambdaBar +=1;
        }
        const LHCb::MCParticle* grandMother=parent->mother();
        if(grandMother){
          grandParentCode=grandMother->particleID().pid();
          if(grandParentCode == 5122) {
            m_nLambda_b +=1;
          } else if(grandParentCode == -5122) {
            m_nLambda_b_Bar +=1;
          }
        }
      }
    }

    LocalV0Track temp;
    temp.track       = *itT;
    temp.mc          = mcP;      // pointeur a la trace  ou 0
    temp.parent      = parent;     // pointeur  au  parent ou 0
    temp.trackCode   =  trackCode;   // 211 = pi+, 2212=p
    temp.parentCode  = parentCode;    // 310 =Ks, 3122= Lambda, -3122  LambdaBar
    temp.grandParentCode  = grandParentCode;  // 511 =Bd, 5122=Lambda_b, -5122=antiLambda_b
    temp.hasNoVeloTrack   = hasNoVeloTrack;

    temp.notInPhSp= skipTrk;
    temp.xOrg= m_xPhSpOrg;   // computed in storePhaseSpaceAtOrigin()
    temp.yOrg= m_yPhSpOrg;
    temp.slpxOrg= m_slpxPhSpOrg;
    temp.slpyOrg= m_slpyPhSpOrg;


    temp.cx[0]    = m_trackExpansionX[0];
    temp.cx[1]    = m_trackExpansionX[1];
    temp.cx[2]    = m_trackExpansionX[2];

    temp.cy[0]    = m_trackExpansionY[0];
    temp.cy[1]    = m_trackExpansionY[1];

    temp.slpx[0]  = m_slopeExpansionX[0];
    temp.slpx[1]  = m_slopeExpansionX[1];
    temp.slpx[2]  = m_slopeExpansionX[2];
    temp.slpx[3]  = m_slopeExpansionX[3];
    temp.slpx[4]  = m_slopeExpansionX[4];

    temp.slpy[0]  = m_slopeExpansionY[0];
    temp.slpy[1]  = m_slopeExpansionY[1];
    temp.slpy[2]  = m_slopeExpansionY[2];

    temp.p        = 1./fabs( m_state.qOverP() );

    if(charge > 0) {
      // positive tracks
      m_pos.push_back( temp );
      /*
        info()  << "Selected + track " << (*itT)->key()
        << " with track code "  << trackCode
        << " with parent code " <<temp.parentCode
        << "  and grand parent code " << temp.grandParentCode
        << endreq;
      */
    } else {
      // negative tracks
      m_neg.push_back( temp );
      /*
        info()  << "Selected - track " << (*itT)->key()
        << " with track code "  << trackCode
        << " with parent code "  <<temp.parentCode
        << "  and grand parent code " << temp.grandParentCode
        << endreq;
      */
    }
  }  // end loop (LHCb::Tracks::const_iterator itT)
  //---------------------------------------------------------------
  // PRIMARY VERTICES

  for( LHCb::RecVertices::const_iterator itV = primaryVertices->begin();
       primaryVertices->end() != itV; ++itV ){
    m_zPrimVtxh->fill( (*itV)->position().z() );
    m_xyPrimVtx->fill( (*itV)->position().x(), (*itV)->position().y() );
    m_zPrimVtx.push_back( (*itV)->position().z() );

  }
  m_nbPrimVtxInEvt=m_zPrimVtx.size();

  int n_solV0;
  int codeSolMass;
  double distInSigma;

  int iNeg_rem=-1;



  for (unsigned int iPos=0;iPos<m_pos.size(); iPos++){   //LOOP ON POSITIVE TRACK
    n_solV0=0;
    distInSigma=10000;
    m_impactCurrent=999999;
    m_impactCurrentNormed=999999;
    const LHCb::MCParticle* i_MCPos = m_pos[iPos].mc;
    bool plusNotInPhaseSpace=m_pos[iPos].notInPhSp ;    // if true : not in Ph Sp
    if(!(i_MCPos == 0)){
      if(!(i_MCPos->originVertex() == 0)){
        const Gaudi::XYZPoint Position = i_MCPos->originVertex()->position();
        xxPos=Position.X();
        yyPos=Position.Y();
        zzPos=Position.Z();
      }
    }


    for (unsigned int iNeg=0;iNeg<m_neg.size(); iNeg++){  //LOOP ON NEGATIVE TRACK
      const LHCb::MCParticle* i_MCNeg=m_neg[iNeg].mc;
      bool minusNotInPhaseSpace=m_neg[iNeg].notInPhSp ;   // if true : not in Ph Sp

      if(!(i_MCNeg == 0)){
        if(!(i_MCNeg->originVertex() == 0)){
          const Gaudi::XYZPoint Position = i_MCNeg->originVertex()->position();
          xxNeg=Position.X();
          yyNeg=Position.Y();
          zzNeg=Position.Z();
        }
      }

      bool pairInPhaseSpace=!(plusNotInPhaseSpace || minusNotInPhaseSpace);  // if true:  in Ph Sp
      hasNoVeloPair=m_pos[iPos].hasNoVeloTrack && m_neg[iNeg].hasNoVeloTrack;

      // a few counters
      bool trueKs=MCTruthPair_K0(i_MCPos,i_MCNeg);
      bool trueLambda=MCTruthPair_Lambda(i_MCPos,i_MCNeg);
      bool trueLambdaBar=MCTruthPair_LambdaBar(i_MCPos,i_MCNeg);
      bool trueV0=trueKs || trueLambda || trueLambdaBar;
      if(trueV0 && m_checkPt){
        double ppt= m_neg[iNeg].parent->pt();
        // if(ppt/1000. < m_cutPt) trueV0=false ; // CUT OUT LOW PT V0/KSHORT
        if(ppt < m_cutPt) trueV0=false ; // CUT OUT LOW PT V0/KSHORT
      }

      if( trueV0 ){
        m_GrandParentCode=0;
        const LHCb::MCParticle* grandParent=m_neg[iNeg].parent->mother();
        int pid=500;
        if(!(grandParent==0) ) pid=grandParent->particleID().pid();

        if(trueKs){
          m_acc_Ks++;
          m_nCounters[0]++;      // accepted V0 with both tracks, no velo piece
          m_nCounters_K0[0]++;
          if(hasNoVeloPair) {
            m_nCountersNoVelo[0]++;
            m_nCounters_NoVelo_K0[0]++;
          }
          if(pairInPhaseSpace) {
            m_nCounters[1]++;  // accepted V0 with both tracks *in PhSp*
            m_nCounters_K0[1]++;  // accepted Ks with both tracks *in PhSp*
            if(hasNoVeloPair) {
              m_nCountersNoVelo[1]++;
              m_nCounters_NoVelo_K0[1]++;
            }
          }

          m_trkX_K0->fill(m_pos[iPos].xOrg);
          m_trkY_K0->fill(m_pos[iPos].yOrg);
          m_trkSlpX_K0->fill(m_pos[iPos].slpxOrg);
          m_trkSlpY_K0->fill(m_pos[iPos].slpyOrg);

          m_trkX_K0->fill(m_neg[iNeg].xOrg);
          m_trkY_K0->fill(m_neg[iNeg].yOrg);
          m_trkSlpX_K0->fill(m_neg[iNeg].slpxOrg);
          m_trkSlpY_K0->fill(m_neg[iNeg].slpyOrg);

          double pt= m_neg[iNeg].parent->pt();
          m_ptMCTrue_K0->fill(pt/1000.); // Ks pT in GeV

          int ppid=abs(pid);
          m_GrandParentCode=ppid;
          if(m_GrandParentCode==511) {
            m_nCounters[2]++;           // accepted V0 from Bd with both tracks
            m_nCounters_K0[2]++;           // accepted Ks from Bd with both tracks
            if(hasNoVeloPair) {
              m_nCountersNoVelo[2]++;
              m_nCounters_NoVelo_K0[2]++;
            }

            if(pairInPhaseSpace){
              m_nCounters[3]++;    // accepted V0 from Bd with both tracks in PhSp
              m_nCounters_K0[3]++;    // accepted Ks from Bd with both tracks in PhSp
              if(hasNoVeloPair) {
                m_nCountersNoVelo[3]++;
                m_nCounters_NoVelo_K0[3]++;
              }

            }

          }

        } else if (trueLambda) {
          m_acc_Lambda++;
          m_nCounters_Lambda[0]++;
          m_nCounters[0]++;      // accepted V0 with both tracks, no velo piece
          if(hasNoVeloPair) {
            m_nCountersNoVelo[0]++;
            m_nCounters_NoVelo_Lambda[0]++;
          }
          if(pairInPhaseSpace) {
            m_nCounters[1]++;  // accepted V0 with both tracks *in PhSp*
            m_nCounters_Lambda[1]++;  // accepted Ks with both tracks *in PhSp*
            if(hasNoVeloPair) {
              m_nCounters_NoVelo_Lambda[1]++;
            }
          }

          m_trkX_Lambda->fill(m_pos[iPos].xOrg);
          m_trkY_Lambda->fill(m_pos[iPos].yOrg);
          m_trkSlpX_Lambda->fill(m_pos[iPos].slpxOrg);
          m_trkSlpY_Lambda->fill(m_pos[iPos].slpyOrg);

          m_trkX_Lambda->fill(m_neg[iNeg].xOrg);
          m_trkY_Lambda->fill(m_neg[iNeg].yOrg);
          m_trkSlpX_Lambda->fill(m_neg[iNeg].slpxOrg);
          m_trkSlpY_Lambda->fill(m_neg[iNeg].slpyOrg);

          double pt= m_neg[iNeg].parent->pt();
          m_ptMCTrue_Lambda->fill(pt/1000.); // Ks pT in GeV
          m_GrandParentCode=pid;
          if(m_GrandParentCode==5122) {
            m_nCounters[2]++;           // accepted V0 from Bd with both tracks
            m_nCounters_Lambda[2]++; // accepted Lambda from Lambda_b
            if(hasNoVeloPair) {
              m_nCounters_NoVelo_Lambda[2]++;
            }

            if(pairInPhaseSpace){
              m_nCounters[3]++;    // accepted V0 from Bd with both tracks in PhSp
              m_nCounters_Lambda[3]++;  // accepted Lambda  from Bd with both tracks OK
              if(hasNoVeloPair) {
                m_nCounters_NoVelo_Lambda[3]++;
              }

            }

          }
        } else if (trueLambdaBar) {
          m_acc_LambdaBar++;
          m_nCounters[0]++;      // accepted V0 with both tracks, no velo piece
          m_nCounters_LambdaBar[0]++;
          if(hasNoVeloPair) {
            m_nCountersNoVelo[0]++;
            m_nCounters_NoVelo_LambdaBar[0]++;
          }
          if(pairInPhaseSpace) {
            m_nCounters[1]++;  // accepted V0 with both tracks *in PhSp*
            m_nCounters_LambdaBar[1]++;  // accepted Lambda with both tracks *in PhSp*
            if(hasNoVeloPair) {
              m_nCounters_NoVelo_LambdaBar[1]++;
            }
          }

          m_trkX_LambdaBar->fill(m_pos[iPos].xOrg);
          m_trkY_LambdaBar->fill(m_pos[iPos].yOrg);
          m_trkSlpX_LambdaBar->fill(m_pos[iPos].slpxOrg);
          m_trkSlpY_LambdaBar->fill(m_pos[iPos].slpyOrg);

          m_trkX_LambdaBar->fill(m_neg[iNeg].xOrg);
          m_trkY_LambdaBar->fill(m_neg[iNeg].yOrg);
          m_trkSlpX_LambdaBar->fill(m_neg[iNeg].slpxOrg);
          m_trkSlpY_LambdaBar->fill(m_neg[iNeg].slpyOrg);

          double pt= m_neg[iNeg].parent->pt();
          m_ptMCTrue_LambdaBar->fill(pt/1000.); // Ks pT in GeV
          m_GrandParentCode=pid;
          if(m_GrandParentCode==-5122) {
            m_nCounters[2]++;           // accepted V0 from Bd with both tracks
            m_nCounters_LambdaBar[2]++; // accepted Lambda from Lambda_b
            if(hasNoVeloPair) {
              m_nCounters_NoVelo_LambdaBar[2]++;
            }

            if(pairInPhaseSpace){
              m_nCounters[3]++;    // accepted V0 from Bd with both tracks in PhSp
              m_nCounters_LambdaBar[3]++;// accepted LambdaBar from Lambda_bBar with both tracks OK
              if(hasNoVeloPair) {
                m_nCounters_NoVelo_LambdaBar[3]++;
              }

            }

          }
        }


      }
      trueV0 =trueV0 && pairInPhaseSpace; // from now on trueV0 == true V0 with both trks in Ph. Sp.

      // -------->> Full Reco  <<------------------

      if(!pairInPhaseSpace) continue;     // only examine inside chosen phase space

      // remove pairs with too low energy tracks
      bool tooLowMom=(m_pos[iPos].p < m_pMinRich1) || (m_neg[iNeg].p <m_pMinRich1);
      if( tooLowMom ) continue;

      StatusCode  sc= intersection( m_pos[iPos], m_neg[iNeg] );
      if ( sc.isFailure() ) {
        debug() << " ... Failure for intersection" << endreq;
        continue;
      }

      StatusCode sc1b =improveVTX(  m_pos[iPos], m_neg[iNeg] );
      //       StatusCode sc1b =improveVTX_new(  m_pos[iPos], m_neg[iNeg] );
      if( sc1b.isFailure() ){
        debug () << " ... failure for improveVtx_new" << endreq;
        continue;
      }

      // after this call : m_zV0Vtx,m_xV0Vtx,m_yV0VTx and m_chiSq are computed and loaded
      //

      double zV=m_zV0Vtx;
      double xV=m_xV0Vtx;
      double yV=m_yV0VTx;
      m_plotChi2->fill(m_chiSq);

      double ptRec=sqrt(m_V0Mom[1]*m_V0Mom[1]+m_V0Mom[2]*m_V0Mom[2]);
      // ptRec /=1000.;  // GeV

      double primOK =CheckPrimaryVtx( m_pos[iPos], m_neg[iNeg] );

      StatusCode sc2 = reco4Momentum( m_pos[iPos], m_neg[iNeg] );
      // ***** attention see reco4Momentum to regenerate for all V0 *****

      //   std::cout << " execute  : armanteros x, y ="<<m_Armanteros<<" , " <<m_pTrans<<std::endl;

      if( trueKs ) {       // tests reco vtx vs MCtruth ; resolution plots
        m_plotChi2Ks->fill(m_chiSq);
        m_checkPrimChi2Ks->fill(primOK);
        m_MCdz_K0->fill(zzNeg-zV);
        m_MCdx_K0->fill(xxNeg-xV);
        m_MCdy_K0->fill(yyNeg-yV);
        m_ptMCRec_K0->fill(ptRec/1000.);
      } else if( trueLambda || trueLambdaBar){
        m_plotChi2Lambda->fill(m_chiSq);
        m_MCdz_Lambda->fill(zzNeg-zV);
        m_MCdx_Lambda->fill(xxNeg-xV);
        m_MCdy_Lambda->fill(yyNeg-yV);
        if( trueLambda) {
          m_ptMCRec_Lambda->fill(ptRec/1000.);
          m_checkPrimChi2Lambda->fill(primOK);
        } else if(trueLambdaBar){
          m_ptMCRec_LambdaBar->fill(ptRec/1000.);
          m_checkPrimChi2LambdaBar->fill(primOK);
        }
      }

      if(m_chiSq  > m_chi2VtxLim ) {
        debug() << "    .. very bad chiSq " << m_chiSq << endreq;
        continue;  // to speed up computing
      }

      if(m_checkPt && ptRec < m_cutPt) continue;

      if(sc2.isFailure()) continue; // pair fail to provide usable invariant masses
      if(trueV0) {
        m_impactAtOrigin->fill(m_xImpact,m_yImpact);
        m_zTrue->fill(zzNeg);
        m_zRec->fill(zV);
        double impact=SmallestImpactParameter();
        m_impactParamV0->fill(impact);
        double normedImpact=NormedSmallestImpactParameter();
        m_impactParamV0Norm->fill(normedImpact);
        // check impact parameter at true Prim Vtx
        double qq=vtxMomAngle(primaryVertices);
        m_vtxMomAngV0->fill(qq*1000.);
      }

      // plot accepted pair track phase space
      RecoPhSp( m_pos[iPos].cx, m_pos[iPos].cy, m_pos[iPos].slpx, m_pos[iPos].slpy,
                xPhSp, txPhSp, yPhSp, tyPhSp);
      m_xOrg->fill(xPhSp);
      m_yOrg->fill(yPhSp);
      m_txOrg->fill(txPhSp);
      m_tyOrg->fill(tyPhSp);
      RecoPhSp( m_neg[iNeg].cx, m_neg[iNeg].cy, m_neg[iNeg].slpx, m_neg[iNeg].slpy,
                xPhSp, txPhSp, yPhSp, tyPhSp);
      m_xOrg->fill(xPhSp);
      m_yOrg->fill(yPhSp);
      m_txOrg->fill(txPhSp);
      m_tyOrg->fill(tyPhSp);

      double uu=sqrt(m_V0Mom[1]*m_V0Mom[1]+m_V0Mom[2]*m_V0Mom[2]);
      if ( trueKs ) {
        m_invMassKs->fill(m_invariantKsMass);
        m_ptMCRec_K0->fill(uu/1000.);
        m_nCounters[4]++;  // count all V0 passing the vtx cut
        m_nCounters_K0[4]++;
        if(hasNoVeloPair) {
          m_nCountersNoVelo[4]++;
          m_nCounters_NoVelo_K0[4]++;
        }
        if(m_GrandParentCode == 511){
          m_nCounters[5]++;
          m_nCounters_K0[5]++;
          if(hasNoVeloPair) {
            m_nCountersNoVelo[5]++;
            m_nCounters_NoVelo_K0[5]++;
          }
        }
      } else if ( trueLambda ) {
        m_invMassLambda->fill(m_invariantLambdaMass);
        m_invMassBothLambda->fill(m_invariantLambdaMass);
        m_ptMCRec_Lambda->fill(uu/1000.);
        m_nCounters[4]++;  // count all V0 passing the vtx cut
        m_nCounters_Lambda[4]++;
        if(hasNoVeloPair) m_nCounters_NoVelo_Lambda[4]++;
        if(m_GrandParentCode == 5122){
          m_nCounters[5]++;
          m_nCounters_Lambda[5]++;
          if(hasNoVeloPair)m_nCounters_NoVelo_Lambda[5]++;
        }

      } else if ( trueLambdaBar ) {
        m_invMassLambdaBar->fill(m_invariantLambdaBarMass);
        m_invMassBothLambda->fill(m_invariantLambdaBarMass);
        m_ptMCRec_LambdaBar->fill(uu/1000.);
        m_nCounters[4]++;  // count all V0 passing the vtx cut
        m_nCounters_LambdaBar[4]++;
        if(hasNoVeloPair) m_nCounters_NoVelo_LambdaBar[4]++;
        if(m_GrandParentCode == -5122){
          m_nCounters[5]++;
          m_nCounters_LambdaBar[5]++;
          if(hasNoVeloPair)m_nCounters_NoVelo_LambdaBar[5]++;
        }
      }

      // coupure en parametre d'impact du V0 a l'origine

      double impactV0=sqrt(m_xImpact*m_xImpact +m_yImpact*m_yImpact);
      if(impactV0 > m_impactParV0 ) continue;

      double impactCurrentNormed=NormedSmallestImpactParameter();
      //   primOK    cut plot

      m_checkPrimChi2->fill(primOK);

      // coupure en  masse invariante V0
      double ma=m_Kshort+m_Nsig*m_SigK0;
      double mb=m_Kshort-m_Nsig*m_SigK0;
      bool testMassK0= (m_invariantKsMass-ma)*(m_invariantKsMass-mb) < 0;

      ma=m_Lambda+m_Nsig*m_SigLambda;
      mb=m_Lambda-m_Nsig*m_SigLambda;

      bool testMassLambda= (m_invariantLambdaMass-ma)*(m_invariantLambdaMass-mb) < 0;

      bool testMassLambdaBar= (m_invariantLambdaBarMass-ma)*(m_invariantLambdaBarMass-mb) < 0;

      bool testMass=(testMassK0 || testMassLambda || testMassLambdaBar);

      bool keepV0=testMass && (m_chiSq <= m_chi2VtxLim) && (primOK > m_primVtxOKLim ) && 
        (impactCurrentNormed < m_impactCurNormLim) ;
      if(!keepV0){
        continue; //<-- skip pair
      }

      double angle=vtxMomAngle(primaryVertices);
      m_vtxMomAngAll->fill(angle*1000.);  // plot mrd
      if(fabs(angle) > m_angleCut) continue;


      n_solV0++;

      // check overlapping assignments; foreseen assignments :1,2, 4, 3 and 5 ; unexpected 6, 7
      codeSolMass=0;
      if(testMassK0) codeSolMass +=1;
      if(testMassLambda) codeSolMass +=2;
      if(testMassLambdaBar) codeSolMass +=4;


      // flag and store the best chi2
      double dist=m_chiSq;
      if(dist < distInSigma) {
        distInSigma=dist;

        bool check_K0=MCTruthPair_K0(i_MCPos,i_MCNeg);
        bool check_Lambda=MCTruthPair_Lambda(i_MCPos,i_MCNeg);
        bool check_LambdaBar=MCTruthPair_LambdaBar(i_MCPos,i_MCNeg);
        bool check=check_K0 || check_Lambda || check_LambdaBar;

        int iflagV0=0;  // for wrong pairs
        if(check_K0){
          iflagV0=1; // for Ks
        } else if(check_Lambda){
          iflagV0=2; // for Lambda
        } else if(check_LambdaBar){
          iflagV0=4; // for LambdaBar
        }

        int grandMotherCode=m_neg[iNeg].grandParentCode;
        bool grandMotherIsBd=(grandMotherCode==511)||(grandMotherCode==-511);
        bool grandMotherIsLambda_b=(grandMotherCode==5122)||(grandMotherCode==-5122);

        storeV0Info(iflagV0,zV,primOK,check,grandMotherIsBd,grandMotherIsLambda_b,codeSolMass);
        iNeg_rem=iNeg;
        m_impactCurrent=SmallestImpactParameter();
        m_impactCurrentNormed=NormedSmallestImpactParameter();
        m_angle=vtxMomAngle(primaryVertices);

        m_BdVtx[0]=zzNeg;  // not necessarily Bd, but also Lambda_b or antiLambda_b
        m_BdVtx[1]=xxNeg;
        m_BdVtx[2]=yyNeg;
      }

    }        // ****************** end (inner) loop on < 0 trks

    if(n_solV0 > 0) {
      m_Armanteros=m_ArmanterosRef;
      m_pTrans=m_pTransRef;
      //    std::cout << "execute before loading  : armanteros x, y ="<<m_Armanteros<<" , " <<m_pTrans<<std::endl;

      PatV0Candidate temp( m_pos[iPos].track,m_neg[iNeg_rem].track, hasNoVeloPair,
                           m_bestSol, m_allInvariantMasses, m_invariantV0MassRef, m_V0vtxRefZ,m_angle,
                           m_flagV0 , m_MCcheckV0,m_MCcheckBd, m_MCcheckLambda_b, m_primOK,m_codeSolMass,
                           m_Armanteros,m_pTrans );
      m_solutions.push_back( temp );
      m_impactParam->fill(m_impactCurrent);
      m_impactParamNorm->fill(m_impactCurrentNormed);
    }
  }                // ********************* end loop on >0 trks

  selectSols();

  return SUCCESS;
}

//============================================================

void PatCalibV0::selectSols(){

  m_selected.clear();
  bool selectOn=false;
  selectOn=true;

  // select as solution for the pair the best V0 vtx fit chi2
  // on the negative track (positive is already single by construction)

  std::vector<PatV0Candidate>::iterator itS, itS2;
  if(selectOn) {
    // info()  << " selectOn Loop " << endreq;
    for ( itS = m_solutions.begin(); m_solutions.end() > itS; ++itS ) {
      PatV0Candidate& cand = (*itS);

      if( !cand.keep() ) continue;

      for ( itS2 = itS+1;  m_solutions.end() != itS2; ++itS2 ) {
        PatV0Candidate& cand2 = (*itS2);
        /*
          info() <<" itS2 pair #"<< ++m<< ", Pos " << cand2.posTrk()->key()<< " Neg " << cand2.negTrk()->key()<<" keep ="
          << cand2.keep() << " chi2=" <<cand2.chi2() <<endreq;
        */
        if ( cand.negTrk() != cand2.negTrk() ) continue;
        //info()  <<" test neg tracks itS, itS2 = " <<  cand.negTrk()->key()<< " , " <<cand2.negTrk()->key()<<endreq;
        if( !cand2.keep() ) continue;
        if( cand.chi2() > cand2.chi2() ){
          cand.setKeep( false );
        } else{
          cand2.setKeep( false );
        }
        //info()  <<" test neg tracks itS, itS2 keep s = " << cand.keep()<< " , " << cand2.keep()<<endreq;
      }
    }
  }
  /*
  // try selecting the smallest angle
  double angleRef=0.;
  int ns=0;
  for ( itS = m_solutions.begin(); m_solutions.end() > itS; ++itS ) {
  PatV0Candidate& cand = (*itS);
  if( !cand.keep() ) continue;
  ns++;
  double angle_1=cand.angle();
  if(angle_1 > angleRef) angleRef= angle_1;
  }
  if(ns > 2) {
  for ( itS = m_solutions.begin(); m_solutions.end() > itS; ++itS ) {
  PatV0Candidate& cand = (*itS);
  if( !cand.keep() ) continue;
  double angle_1=cand.angle();
  if(angle_1 == angleRef) cand.setKeep( false );
  }

  } // ns > 2
  */
  // transfer kept solutions

  for ( itS = m_solutions.begin(); m_solutions.end() != itS; ++itS ) {
    PatV0Candidate& cand = (*itS);

    if ( !cand.keep() ) continue;
    m_selected.push_back( cand );
  }

  // next code is to ckeck and count only

  if(m_selected.size()== 0) return;

  m_multiplicitySelect->fill( (double) m_selected.size() );
  m_multVsnVTX->fill((double) m_nbPrimVtxInEvt,(double) m_selected.size() );

  for ( itS = m_selected.begin(); m_selected.end() != itS; ++itS ) {
    PatV0Candidate& cand = (*itS);

    m_V0Degeneracy->fill((double) cand.codeSolMass());
    m_multiplicityPairSelect->fill( 0.);

    // make counters
    bool hasNoVeloPair=cand.hasNoVeloPair();
    int codeMass= cand.codeSolMass();

    m_nCounters[8]++;  // attention coutn all pairs
    if(hasNoVeloPair) m_nCountersNoVelo[8]++;
    // Solution K0 ; count pairs inside K0 window
    if(codeMass==1 || codeMass==3 || codeMass==5 || codeMass==7){
      m_nCounters_K0[8]++;
    }
    // Solution Lambda ; count pairs inside Lambda window
    if(codeMass==2 || codeMass==3 || codeMass==6 || codeMass==7){
      m_nCounters_Lambda[8]++;
    }
    // Solution Lambdabar ; count pairs inside LambdaBar window
    if(codeMass==4 || codeMass==5 || codeMass==6 || codeMass==7){
      m_nCounters_LambdaBar[8]++;
    }
    int flagV0Kind= cand.flagTrueV0Kind();
    bool isTrueV0=cand.mcCheck();
    double Armanteros=cand.Armanteros();
    double pTrans=cand.pTrans();
    pTrans/=1000.; //GeV

    //    ArmanterosPodolansky;

    if(codeMass==1 || codeMass==2|| codeMass==4) m_armanteros_clear->fill(Armanteros,pTrans);
    if(codeMass==3 || codeMass==5) m_armanteros_ambigu->fill(Armanteros,pTrans);

    if(isTrueV0){

      if(flagV0Kind == 1){ // true Kshort's
        m_nCounters[6]++;
        m_KsMassSelect->fill( cand.invMassK0() );
        m_armanteros_K0->fill(Armanteros,pTrans);
        if(hasNoVeloPair) m_nCountersNoVelo[6]++;
        m_nCounters_K0[6]++;
        if(hasNoVeloPair) m_nCounters_NoVelo_K0[6]++;
        bool isKsBd=cand.mcCheckBd();
        if(isKsBd) {
          m_nCounters[7]++;  // count all V0 actually
          m_nCounters_K0[7]++;
          info() << " Bd found with x,y,z #"<< m_nCounters_K0[7]<<" coord Vtx ="
                 <<m_BdVtx[1] <<" , " << m_BdVtx[2] <<" , " << m_BdVtx[0] <<endreq;
          info() << " Bd found with     Pos " << cand.posTrk()->key()<< " Neg "
                 << cand.negTrk()->key()<<endreq;

          if(hasNoVeloPair) { // count all V0 actually
            m_nCountersNoVelo[7]++;
            m_nCounters_NoVelo_K0[7]++;
          }
        }
      }

      if(flagV0Kind == 2){ // true Lambda's
        m_nCounters[6]++;
        m_LambdaMassSelect->fill( cand.invMassLambda() );
        m_armanteros_Lambda->fill(Armanteros,pTrans);
        if(hasNoVeloPair) m_nCountersNoVelo[6]++;
        m_nCounters_Lambda[6]++;
        if(hasNoVeloPair) m_nCounters_NoVelo_Lambda[6]++;
        bool isLambdaLb=cand.mcCheckLambda_b();
        if(isLambdaLb) {
          m_nCounters[7]++;     // count all V0 actually
          m_nCounters_Lambda[7]++;
          info() << " Lambda_b found with x,y,z #"<< m_nCounters_Lambda[7]<<" coord Vtx ="
                 <<m_BdVtx[1] <<" , " << m_BdVtx[2] <<" , " << m_BdVtx[0] <<endreq;
          info() << "  Lambda_b found with     Pos " << cand.posTrk()->key()<< " Neg "
                 << cand.negTrk()->key()<<endreq;

          if(hasNoVeloPair) {
            m_nCountersNoVelo[7]++;  // count all V0 actually
            m_nCounters_NoVelo_Lambda[7]++;
          }
        }
      }


      if(flagV0Kind == 4){ // true antiLambda's
        m_nCounters[6]++;
        m_antiLambdaMassSelect->fill( cand.invMassLambdaBar() );
        m_armanteros_antiLambda->fill(Armanteros,pTrans);
        if(hasNoVeloPair) m_nCountersNoVelo[6]++;
        m_nCounters_LambdaBar[6]++;
        if(hasNoVeloPair) m_nCounters_NoVelo_LambdaBar[6]++;
        bool isLambdaLb=cand.mcCheckLambda_b();
        if(isLambdaLb) {
          m_nCounters[7]++;     // count all V0 actually
          m_nCounters_LambdaBar[7]++;
          info() << " antiLambda_b found with x,y,z #"<< m_nCounters_Lambda[7]<<" coord Vtx ="
                 <<m_BdVtx[1] <<" , " << m_BdVtx[2] <<" , " << m_BdVtx[0] <<endreq;
          info() << " antiLambda_b found with     Pos " << cand.posTrk()->key()<< " Neg "
                 << cand.negTrk()->key()<<endreq;

          if(hasNoVeloPair) {
            m_nCountersNoVelo[7]++;  // count all V0 actually
            m_nCounters_NoVelo_LambdaBar[7]++;
          }
        }
      }


    } // if(isTrueV0)

    m_bestchi2Select->fill( cand.chi2() );
    m_bestPrimOKSelect->fill( cand.primOK() );
    m_zRecSelect->fill( cand.zVtxKs() );
  } // end loop :: for ( itS = m_selected.begin()
}

//=======================================================================
void PatCalibV0::storeV0Info(int iflagV0,double zV, double primOK, bool check,
                             bool grandMotherIsBd,bool grandMotherIsLambda_b,int codeSolMass){
  m_xImpactRef=m_xImpact;
  m_yImpactRef=m_yImpact;
  m_pTransRef=m_pTrans;
  m_ArmanterosRef=m_Armanteros;
  //     std::cout << "    storeV0Info : armanteros x, y ="<<m_Armanteros<<" , " <<m_pTrans<<std::endl;
  m_V0vtxRefZ=zV;
  m_primOK= primOK ;  //unused now
  m_bestSol=m_chiSq;
  m_MCcheckV0=check;  //true for any V0
  m_flagV0=iflagV0; //if(m_MCcheck) ==> 0 = wrong pair, 1=Ks, 2=Lambda, 4=antiLambda

  m_codeSolMass=codeSolMass;      // degeneracy coded from zero to 7;1=K0,2=Lambda,4=LambdaBar
  // zero should never be found (pair dropped out)
  m_invariantV0MassRef   =m_invariantKsMass;   //redundent with m_allInvariantMasses[0]
  m_allInvariantMasses[0]=m_invariantKsMass;
  m_allInvariantMasses[1]=m_invariantLambdaMass;
  m_allInvariantMasses[2]=m_invariantLambdaBarMass;


  m_MCcheckBd=grandMotherIsBd;    // valid for particle and antiParticle
  m_MCcheckLambda_b=grandMotherIsLambda_b;  // valid for particle and antiParticle
}
//=======================================================================
bool PatCalibV0::MCTruthPair_K0(const LHCb::MCParticle* i_MCPos,
                                const LHCb::MCParticle* i_MCNeg){

  if(i_MCPos == 0 || i_MCNeg == 0) return false;
  const LHCb::MCParticle* parentP=0;
  const LHCb::MCParticle* parentN=0;

  const LHCb::MCVertex * originVPos= i_MCPos->originVertex();
  if(originVPos == 0)return false;
  const Gaudi::XYZPoint PositionP = originVPos->position();
  double zzPos=PositionP.Z();

  const LHCb::MCVertex * originVNeg= i_MCNeg->originVertex();
  if( originVNeg == 0)return false;
  const Gaudi::XYZPoint PositionN = originVNeg->position();
  double zzNeg=PositionN.Z();

  bool check=fabs(zzNeg-zzPos) < 1.e-02;

  if(!check) return false;
  parentP=  i_MCPos->mother();
  parentN=  i_MCNeg->mother();
  if (!parentP || !parentN) return false;
  const LHCb::ParticleID & partIDmotherP=parentP->particleID();
  const LHCb::ParticleID & partIDmotherN=parentN->particleID();

  bool parentIsKs=partIDmotherP.pid()==310 && partIDmotherN.pid()==310;

  return parentIsKs;
}
//=======================================================================
bool PatCalibV0::MCTruthPair_Lambda(const LHCb::MCParticle* i_MCPos,
                                    const LHCb::MCParticle* i_MCNeg){

  if(i_MCPos == 0 || i_MCNeg == 0) return false;
  const LHCb::MCParticle* parentP=0;
  const LHCb::MCParticle* parentN=0;

  const LHCb::MCVertex * originVPos= i_MCPos->originVertex();
  if(originVPos == 0)return false;
  const Gaudi::XYZPoint PositionP = originVPos->position();
  double zzPos=PositionP.Z();

  const LHCb::MCVertex * originVNeg= i_MCNeg->originVertex();
  if( originVNeg == 0)return false;
  const Gaudi::XYZPoint PositionN = originVNeg->position();
  double zzNeg=PositionN.Z();

  bool check=fabs(zzNeg-zzPos) < 1.e-02;

  if(!check) return false;
  parentP=  i_MCPos->mother();
  parentN=  i_MCNeg->mother();
  if (!parentP || !parentN) return false;
  const LHCb::ParticleID & partIDmotherP=parentP->particleID();
  const LHCb::ParticleID & partIDmotherN=parentN->particleID();

  bool parentIsLambda=partIDmotherP.pid()==3122 && partIDmotherN.pid()==3122;

  return parentIsLambda;
}
//=======================================================================
bool PatCalibV0::MCTruthPair_LambdaBar(const LHCb::MCParticle* i_MCPos,
                                       const LHCb::MCParticle* i_MCNeg){

  if(i_MCPos == 0 || i_MCNeg == 0) return false;
  const LHCb::MCParticle* parentP=0;
  const LHCb::MCParticle* parentN=0;

  const LHCb::MCVertex * originVPos= i_MCPos->originVertex();
  if(originVPos == 0)return false;
  const Gaudi::XYZPoint PositionP = originVPos->position();
  double zzPos=PositionP.Z();

  const LHCb::MCVertex * originVNeg= i_MCNeg->originVertex();
  if( originVNeg == 0)return false;
  const Gaudi::XYZPoint PositionN = originVNeg->position();
  double zzNeg=PositionN.Z();

  bool check=fabs(zzNeg-zzPos) < 1.e-02;

  if(!check) return false;
  parentP=  i_MCPos->mother();
  parentN=  i_MCNeg->mother();
  if (!parentP || !parentN) return false;
  const LHCb::ParticleID & partIDmotherP=parentP->particleID();
  const LHCb::ParticleID & partIDmotherN=parentN->particleID();

  bool parentIsLambdaBar=partIDmotherP.pid()==-3122 && partIDmotherN.pid()==-3122;

  return parentIsLambdaBar;
}
//=======================================================================
void PatCalibV0::createInputState(LHCb::State& m_state ){

  //creates the local state vector m_vdat
  // in standard LHCb units

  m_vdat[0]=m_state.x();
  m_vdat[1]=m_state.y();
  m_vdat[2]=m_state.tx();
  m_vdat[3]=m_state.ty();
  m_vdat[4]=m_state.qOverP();
  m_vdat[5]=m_state.z();

  //if(fabs(m_QOverPTruth) >=999999900) m_vdat[4]=m_QOverPTruth;
}
//=============================================================================
void PatCalibV0::initialiseRichVelo_Ks()
{
  /*
    performs the initialisations by calling previous methods
    and transform in order to use and get standard units
    (mm , rd, MeV/c)
    performs also some more initialisations
  */

  // compute vector for renormalisation to standard units (mm,rd MeV)
  m_vectUnits[0]=0.1; m_vectUnits[1]=0.1;  // first degree terms
  m_vectUnits[2]=1.; m_vectUnits[3]=1.;
  m_vectUnits[4]=1.e+03;    // 1/p

  int jpos = 4;     // second degree terms
  for(int i =0;i<5;i++){
    for(int j =i;j<5;j++){
      jpos+=1;
      m_vectUnits[jpos]=m_vectUnits[i]*m_vectUnits[j];
    }
  }
  // third degree terms
  for(int i =0;i<5;i++){
    for(int j =i;j<5;j++){
      for(int k =j;k<5;k++){
        jpos+=1;
        m_vectUnits[jpos]=m_vectUnits[i]*m_vectUnits[j]*m_vectUnits[k];
      }
    }
  }

  for(int i =0;i<5;i++){
    m_vectUnitsZ[i]=pow(0.1,i);    // convert to mm
    //   m_vectUnitsZ[i]=1.;   // no change cm
  }

  // more initialisations attention now fixed in declare Property
  // m_pMinRich1 = 2. * Gaudi::Units::GeV ;      // minimum momentum for extrapolation in rich 1/Velo

  initialiseRich1_Ks();

  initialiseRich1_Ks_Der();

}
//=============================================================================
void PatCalibV0::initialiseRich1_Ks_Der(){

  // FIT OF X/Y SLOPES FOR KS TRACKS in RICH1
  // data in cm's/GeV

  // phase space ( |x,y| <90 mm, |tx,ty|<0.3, pmin =2 Gev)

  // x z / y z  projections

  // degrees of polynomials x=f(z), y=f(z)
  m_ideg_x_Rich1_Ks_Der = 4;
  m_ideg_y_Rich1_Ks_Der = 2;

  // degree in phase space variables
  m_ideg_phx_Rich1_Ks_Der =  3;
  m_ideg_phy_Rich1_Ks_Der =  3;

  // numbers of parameters for each z coeffecient
  m_jparx_Rich1_Ks_Der = 55;
  m_jpary_Rich1_Ks_Der = 55;

  double v_0[]={
    -0.32420E-08,-0.33174E-08, 0.31626E-05,-0.12538E-06,-0.26331E-01,-0.28381E-09,

    0.16577E-09, 0.10260E-06,-0.35500E-07, 0.99249E-07, 0.45712E-10,-0.19702E-07,

    0.35106E-07, 0.11580E-07,-0.75204E-05, 0.43256E-05,-0.14749E-04,-0.90075E-05,

    0.17616E-04, 0.18655E-04, 0.78798E-10,-0.59715E-10,-0.37084E-07, 0.68144E-09,

    0.56873E-06, 0.10976E-09, 0.28728E-07,-0.35614E-07, 0.70788E-08, 0.42750E-05,

    -0.24197E-06,-0.27366E-03, 0.24269E-05, 0.37608E-05, 0.32087E-05, 0.29522E-10,

    -0.16491E-07,-0.14046E-07, 0.31527E-05,-0.33862E-05, 0.33473E-05,-0.59274E-05,

    0.18012E-05,-0.15572E-03,-0.20328E-06,-0.13665E-06,-0.17628E-06,-0.17456E-04,

    -0.28721E-07, 0.19954E-06,-0.44379E-04,-0.15209E-06,-0.13397E-04, 0.11748E-04,

    -0.56097E-04
  };
  double v_1[]={
    -0.11758E-08, 0.51024E-09, 0.18544E-06,-0.82963E-07,-0.73337E-04,-0.25381E-11,

    0.41056E-11, 0.10170E-08,-0.12531E-08, 0.20097E-08,-0.17243E-10,-0.74285E-10,

    0.37217E-08,-0.63707E-09,-0.37444E-07, 0.68733E-08,-0.20400E-06,-0.32061E-07,

    0.14367E-06,-0.10123E-05,-0.28236E-11,-0.30832E-11, 0.12138E-08, 0.17035E-09,

    -0.23233E-08, 0.70628E-11, 0.11416E-08,-0.17339E-08, 0.15029E-08,-0.13043E-06,

    -0.30708E-07, 0.35155E-07, 0.11500E-06,-0.15043E-06, 0.22523E-07, 0.12124E-11,

    -0.94665E-09,-0.56141E-09,-0.11842E-07,-0.10931E-06, 0.14280E-06,-0.13711E-06,

    0.62087E-07, 0.42020E-05, 0.20835E-07,-0.18395E-08,-0.15522E-08,-0.43248E-07,

    0.15210E-08, 0.21467E-08,-0.16700E-06,-0.21452E-09, 0.35465E-08, 0.11830E-06,

    0.39961E-05
  };
  double v_2[]={
    0.21013E-10,-0.95438E-11,-0.33678E-08, 0.16087E-08, 0.22920E-05, 0.49075E-13,

    -0.82946E-13,-0.20481E-10, 0.23971E-10,-0.34088E-10, 0.31242E-12, 0.28554E-11,

    -0.69340E-10, 0.88043E-11, 0.96732E-09,-0.27724E-09, 0.43666E-08, 0.91687E-09,

    -0.33342E-08, 0.18747E-07, 0.48601E-13, 0.56186E-13,-0.20718E-10,-0.35585E-11,

    0.78174E-11,-0.13534E-12,-0.20301E-10, 0.34015E-10,-0.28027E-10, 0.22092E-08,

    0.66533E-09, 0.11246E-07,-0.22736E-08, 0.25083E-08,-0.70526E-09,-0.22868E-13,

    0.18136E-10, 0.10622E-10, 0.14420E-09, 0.18745E-08,-0.28154E-08, 0.29130E-08,

    -0.11833E-08,-0.71474E-07,-0.36005E-09, 0.38295E-10, 0.34777E-10, 0.14389E-08,

    -0.27726E-10,-0.47911E-10, 0.46749E-08, 0.10001E-10, 0.41576E-09,-0.26418E-08,

    -0.73779E-07
  };
  double v_3[]={
    -0.10545E-12, 0.58746E-13, 0.17183E-10,-0.10592E-10,-0.13830E-07,-0.22029E-15,

    0.44055E-15, 0.10359E-12,-0.12875E-12, 0.12481E-12,-0.16863E-14,-0.18493E-13,

    0.37981E-12,-0.22114E-13,-0.58138E-11, 0.16915E-11,-0.26291E-10,-0.54935E-11,

    0.20067E-10,-0.10391E-09,-0.26374E-15,-0.33223E-15, 0.11087E-12, 0.22878E-13,

    0.13905E-12, 0.78762E-15, 0.11835E-12,-0.20032E-12, 0.15898E-12,-0.11699E-10,

    -0.43538E-11,-0.80509E-10, 0.13206E-10,-0.12226E-10, 0.39540E-11, 0.14288E-15,

    -0.10680E-12,-0.66748E-13,-0.60881E-12,-0.10690E-10, 0.17062E-10,-0.18672E-10,

    0.75491E-11, 0.50380E-09, 0.19104E-11,-0.21298E-12,-0.21063E-12,-0.87892E-11,

    0.16415E-12, 0.29716E-12,-0.28198E-10,-0.55697E-13,-0.18961E-11, 0.15952E-10,

    0.44419E-09
  };
  double v_4[]={
    0.15994E-15,-0.11543E-15,-0.27400E-13, 0.22346E-13, 0.31659E-10, 0.33754E-18,

    -0.73663E-18,-0.18211E-15, 0.22126E-15,-0.10492E-15, 0.28397E-17, 0.38189E-16,

    -0.65999E-15,-0.15408E-16, 0.12579E-13,-0.41328E-14, 0.53316E-13, 0.12206E-13,

    -0.41827E-13, 0.17459E-12, 0.43288E-18, 0.65412E-18,-0.17755E-15,-0.45780E-16,

    -0.74028E-15,-0.14746E-17,-0.23420E-15, 0.37985E-15,-0.28511E-15, 0.18384E-13,

    0.88073E-14, 0.22619E-12,-0.24573E-13, 0.16960E-13,-0.68096E-14,-0.29694E-18,

    0.20335E-15, 0.13950E-15,-0.13190E-15, 0.21240E-13,-0.33500E-13, 0.38740E-13,

    -0.16060E-13,-0.11247E-11,-0.31359E-14, 0.39919E-15, 0.44365E-15, 0.20418E-13,

    -0.30177E-15,-0.62902E-15, 0.62787E-13, 0.12242E-15, 0.46530E-14,-0.32990E-13,

    -0.84192E-12
  };
  for(int i=0; i<m_jparx_Rich1_Ks_Der;i++){
    m_xProj_Rich1_Ks_Der[0][i]=v_0[i]*m_vectUnits[i]*m_vectUnitsZ[0];
    m_xProj_Rich1_Ks_Der[1][i]=v_1[i]*m_vectUnits[i]*m_vectUnitsZ[1];
    m_xProj_Rich1_Ks_Der[2][i]=v_2[i]*m_vectUnits[i]*m_vectUnitsZ[2];
    m_xProj_Rich1_Ks_Der[3][i]=v_3[i]*m_vectUnits[i]*m_vectUnitsZ[3];
    m_xProj_Rich1_Ks_Der[4][i]=v_4[i]*m_vectUnits[i]*m_vectUnitsZ[4];
  }

  double w_0[]={
    0.11531E-07, 0.15943E-06,-0.26267E-05,-0.18611E-04, 0.25609E-05, 0.44670E-10,

    -0.32828E-09, 0.18094E-07, 0.12515E-07, 0.97971E-07, 0.24004E-09, 0.55649E-07,

    -0.67608E-07,-0.36677E-06,-0.23752E-06, 0.55261E-07, 0.55848E-06,-0.24596E-06,

    -0.87268E-06,-0.59134E-04,-0.39720E-11, 0.35910E-09, 0.41077E-08,-0.34544E-07,

    0.29087E-07,-0.23504E-09,-0.12122E-06, 0.60199E-07, 0.65942E-06,-0.71496E-06,

    0.79140E-05,-0.90323E-05,-0.27974E-05,-0.11164E-03, 0.10310E-05,-0.26610E-09,

    0.42787E-07, 0.93729E-07,-0.76509E-07, 0.90644E-05,-0.85665E-05,-0.28195E-03,

    -0.93840E-05, 0.19073E-04, 0.40892E-06,-0.12085E-07, 0.96680E-07,-0.99786E-07,

    -0.85593E-07,-0.16348E-05, 0.26227E-06,-0.13410E-06, 0.15367E-06,-0.19767E-07,

    -0.17224E-04
  };
  double w_1[]={
    0.13037E-10, 0.13265E-09, 0.38503E-08, 0.12525E-07, 0.12395E-07, 0.47819E-12,

    0.65102E-12,-0.10064E-09,-0.26249E-09, 0.74423E-09, 0.14402E-11, 0.15496E-09,

    -0.37229E-09,-0.11933E-08, 0.11574E-08, 0.97636E-09, 0.22023E-07, 0.14617E-09,

    -0.17423E-07,-0.73412E-07, 0.24585E-12,-0.72931E-12,-0.11047E-09,-0.67777E-10,

    0.26845E-09,-0.12538E-11, 0.37207E-09, 0.38573E-09,-0.55396E-08, 0.11888E-07,

    0.16870E-07,-0.61500E-07,-0.21805E-07,-0.93806E-07,-0.12337E-08, 0.43618E-12,

    0.20574E-09,-0.21161E-09,-0.55215E-10,-0.49471E-07,-0.47889E-07,-0.88334E-06,

    0.21935E-07, 0.13713E-07,-0.19217E-07, 0.12626E-09,-0.11807E-10,-0.42230E-09,

    -0.48611E-09,-0.43983E-08, 0.63032E-08, 0.35490E-09,-0.14011E-09,-0.11948E-07,

    -0.60709E-06
  };

  double w_2[]={
    -0.19688E-12,-0.35781E-11, 0.18445E-10, 0.29856E-09,-0.10903E-09,-0.29742E-14,

    0.31832E-14, 0.12844E-12, 0.77992E-12,-0.48277E-11,-0.11377E-13,-0.15988E-11,

    0.29975E-11, 0.12425E-10,-0.57371E-12,-0.51393E-11,-0.10351E-09, 0.38855E-11,

    0.89451E-10, 0.13888E-08,-0.83060E-15,-0.38813E-14, 0.32487E-12, 0.94217E-12,

    -0.16192E-11, 0.92750E-14, 0.82444E-12,-0.26233E-11, 0.98972E-11,-0.29008E-10,

    -0.22241E-09, 0.41626E-09, 0.13319E-09, 0.25488E-08,-0.14156E-10, 0.28730E-14,

    -0.16229E-11,-0.73852E-12, 0.16688E-11, 0.24426E-10, 0.35439E-09, 0.87783E-08,

    0.69286E-10,-0.41500E-09, 0.73932E-10,-0.22142E-12,-0.18465E-11, 0.35192E-11,

    0.35309E-11, 0.48489E-10,-0.31485E-10, 0.83993E-12,-0.22935E-11, 0.50529E-10,

    0.28811E-08
  };

  for(int i=0; i<m_jpary_Rich1_Ks_Der;i++){
    m_yProj_Rich1_Ks_Der[0][i]=w_0[i]*m_vectUnits[i]*m_vectUnitsZ[0];
    m_yProj_Rich1_Ks_Der[1][i]=w_1[i]*m_vectUnits[i]*m_vectUnitsZ[1];
    m_yProj_Rich1_Ks_Der[2][i]=w_2[i]*m_vectUnits[i]*m_vectUnitsZ[2];
  }

}
//================================================================================

void PatCalibV0::initialiseRich1_Ks(){

  // X/Y POSITIONS FOR KS TRACKS IN RICH1

  // data in cm's/GeV/c

  // phase space at origin ( |x,y| <90 mm, |tx,ty|<0.3, pmin =2 Gev)

  // x z / y z  projections

  // degrees of polynomials x=f(z), y=f(z)
  m_ideg_x_Rich1_Ks = 2;
  m_ideg_y_Rich1_Ks = 1;

  // degree in phase space variables
  m_ideg_phx_Rich1_Ks =  3;
  m_ideg_phy_Rich1_Ks =  3;

  // numbers of parameters for each z coeffecient
  m_jparx_Rich1_Ks = 55;
  m_jpary_Rich1_Ks = 55;

  double v_0[]={
    -0.24007E-06,-0.25968E-06,-0.48104E-03, 0.23708E-03, 0.44508E+01, 0.45847E-07,

    -0.23994E-07,-0.17103E-04, 0.58298E-05,-0.12226E-04,-0.80375E-08, 0.29520E-05,

    -0.61346E-05,-0.40580E-05, 0.12877E-02,-0.72877E-03, 0.26908E-02, 0.15242E-02,

    -0.30825E-02,-0.35533E-02,-0.12284E-07, 0.16389E-07, 0.59941E-05,-0.31144E-06,

    -0.10619E-03,-0.24712E-07,-0.75138E-05, 0.74383E-05,-0.22118E-05,-0.70369E-03,

    0.74651E-04, 0.45020E-01,-0.45791E-03,-0.71513E-03,-0.35876E-03,-0.81760E-08,

    0.38966E-05, 0.38948E-05,-0.57740E-03, 0.86412E-03,-0.76656E-03, 0.12740E-02,

    -0.49092E-03, 0.67585E-02, 0.23818E-04, 0.23122E-04, 0.31873E-04, 0.29421E-02,

    0.30921E-05,-0.36472E-04, 0.75635E-02, 0.24047E-04, 0.20971E-02,-0.20970E-02,

    0.44291E-02
  };
  double v_1[]={
    -0.21860E-07, 0.26344E-08, 0.67096E-05,-0.11348E-05,-0.32694E-01,-0.39735E-09,

    0.24316E-09, 0.14276E-06,-0.60075E-07, 0.16679E-06,-0.19822E-09,-0.21377E-07,

    0.95650E-07,-0.55704E-08,-0.95425E-05, 0.53131E-05,-0.20339E-04,-0.11267E-04,

    0.22995E-04, 0.10897E-04, 0.55075E-10,-0.12505E-09,-0.27410E-07, 0.16402E-08,

    0.61480E-06, 0.22131E-09, 0.57062E-07,-0.63035E-07, 0.27243E-07, 0.32773E-05,

    -0.35359E-06,-0.32233E-03, 0.42135E-05, 0.21416E-05, 0.35608E-05, 0.54863E-10,

    -0.31910E-07,-0.25798E-07, 0.37477E-05,-0.65839E-05, 0.56603E-05,-0.81545E-05,

    0.31546E-05,-0.11798E-03, 0.78645E-07,-0.19061E-06,-0.23414E-06,-0.21545E-04,

    -0.17439E-07, 0.26826E-06,-0.55674E-04,-0.18349E-06,-0.15977E-04, 0.15652E-04,

    -0.18509E-04
  };
  double v_2[]={
    0.10014E-09,-0.77701E-11,-0.20209E-07, 0.71589E-09, 0.58287E-04, 0.85952E-12,

    -0.61004E-12,-0.29788E-09, 0.15275E-09,-0.49085E-09, 0.10312E-11, 0.37689E-10,

    -0.30442E-09, 0.97879E-10, 0.17261E-07,-0.93580E-08, 0.38084E-07, 0.20280E-07,

    -0.42111E-07, 0.21203E-07,-0.37921E-14, 0.23974E-12, 0.45366E-11,-0.17926E-11,

    -0.67991E-09,-0.50840E-12,-0.10799E-09, 0.13693E-09,-0.79517E-10,-0.81668E-09,

    0.23826E-09, 0.55293E-06,-0.98744E-08, 0.42679E-08,-0.87649E-08,-0.86975E-13,

    0.67044E-10, 0.39977E-10,-0.53884E-08, 0.12442E-07,-0.10463E-07, 0.11895E-07,

    -0.46063E-08, 0.37243E-06,-0.81762E-09, 0.39476E-09, 0.41969E-09, 0.38239E-07,

    0.14388E-10,-0.48300E-09, 0.99641E-07, 0.34410E-09, 0.29837E-07,-0.28734E-07,

    -0.12159E-07
  };

  for(int i=0; i<m_jparx_Rich1_Ks;i++){
    m_xProj_Rich1_Ks[0][i]=v_0[i]*m_vectUnits[i]*m_vectUnitsZ[0]/m_vectUnitsZ[1];
    m_xProj_Rich1_Ks[1][i]=v_1[i]*m_vectUnits[i]*m_vectUnitsZ[1]/m_vectUnitsZ[1];
    m_xProj_Rich1_Ks[2][i]=v_2[i]*m_vectUnits[i]*m_vectUnitsZ[2]/m_vectUnitsZ[1];
  }

  double w_0[]={
    -0.21870E-05,-0.22398E-04, 0.41345E-03, 0.23325E-02,-0.42490E-03,-0.98983E-08,

    0.39722E-07,-0.17337E-05, 0.15700E-05,-0.21430E-04,-0.41846E-07,-0.97537E-05,

    0.11887E-04, 0.56841E-04, 0.20629E-04,-0.17650E-04,-0.30290E-03, 0.32042E-04,

    0.30071E-03, 0.88586E-02,-0.29588E-08,-0.38488E-07, 0.10528E-05, 0.52159E-05,

    -0.69871E-05, 0.47220E-07, 0.11351E-04,-0.12929E-04,-0.21199E-04,-0.82918E-04,

    -0.12026E-02, 0.19139E-02, 0.67539E-03, 0.15454E-01,-0.12272E-03, 0.33162E-07,

    -0.81640E-05,-0.11321E-04, 0.10677E-04,-0.58620E-03, 0.16956E-02, 0.48465E-01,

    0.11395E-02,-0.26700E-02, 0.15059E-03,-0.29803E-06,-0.12084E-04, 0.18473E-04,

    0.17308E-04, 0.26899E-03,-0.10073E-03, 0.15651E-04,-0.18852E-04, 0.12881E-03,

    0.86399E-02
  };
  double w_1[]={
    0.93098E-08, 0.10548E-06,-0.17625E-05,-0.11056E-04, 0.19995E-05, 0.47450E-10,

    -0.18085E-09, 0.76237E-08,-0.59597E-08, 0.98343E-07, 0.20037E-09, 0.43945E-07,

    -0.56232E-07,-0.27424E-06,-0.99086E-07, 0.79524E-07, 0.13525E-05,-0.14982E-06,

    -0.13506E-05,-0.41037E-04, 0.11356E-10, 0.18752E-09,-0.37410E-08,-0.24591E-07,

    0.31656E-07,-0.21303E-09,-0.56653E-07, 0.58004E-07, 0.13884E-06, 0.24801E-06,

    0.56771E-05,-0.87204E-05,-0.29736E-05,-0.73130E-04, 0.58977E-06,-0.15257E-09,

    0.37170E-07, 0.51674E-07,-0.50484E-07, 0.31442E-05,-0.77234E-05,-0.22185E-03,

    -0.51695E-05, 0.12599E-04,-0.61045E-06,-0.13567E-09, 0.58443E-07,-0.84508E-07,

    -0.78355E-07,-0.12406E-05, 0.45256E-06,-0.71492E-07, 0.89933E-07,-0.56242E-06,

    -0.38561E-04
  };

  for(int i=0; i<m_jpary_Rich1_Ks;i++){
    m_yProj_Rich1_Ks[0][i]=w_0[i]*m_vectUnits[i]*m_vectUnitsZ[0]/m_vectUnitsZ[1];
    m_yProj_Rich1_Ks[1][i]=w_1[i]*m_vectUnits[i]*m_vectUnitsZ[1]/m_vectUnitsZ[1];
  }

}
//=============================================================================
void PatCalibV0::builtVectProj
(int & jpar, double vectPar[],int idegPhsp) const {
  /*
    built the phase space vector at the suitable degree
    presently stops at 3rd degree
  */
  for(int i =0;i<5;i++){
    vectPar[i]=m_vdat[i];
  }
  int jpos = 4;
  if(idegPhsp >= 2){
    for(int i =0;i<5;i++){
      for(int j =i;j<5;j++){
        ++jpos;
        vectPar[jpos]=m_vdat[i]*m_vdat[j];
      }
    }

  }

  if(idegPhsp >= 3){
    for(int i =0;i<5;i++){
      for(int j =i;j<5;j++){
        for(int k =j;k<5;k++){
          ++jpos;
          vectPar[jpos]=m_vdat[i]*m_vdat[j]*m_vdat[k];
        }
      }
    }

  }
  ++jpar;
}
//=============================================================================

void PatCalibV0::computeParamRichVelo_Ks() const {

  /*
    compute the coefficients of the z-polynomial for extrapolation in rich1
    (coordinates) specialized to Kshorts/Seeds
  */


  double vectPar[55];  // third degree max in phase space var's.
  int jpar;
  int idegp;

  idegp=m_ideg_x_Rich1_Ks+1;

  builtVectProj(jpar,vectPar,m_ideg_phx_Rich1_Ks);
  for(int i=0;i<idegp; i++){
    double uu=0.;
    for(int j=0;j< m_jparx_Rich1_Ks; j++){
      uu += vectPar[j]*m_xProj_Rich1_Ks[i][j];
    }
    m_coefDevRich1_X[i] = uu;
  }

  idegp=m_ideg_y_Rich1_Ks+1;
  builtVectProj(jpar,vectPar,m_ideg_phy_Rich1_Ks);
  for(int i=0;i<idegp; i++){
    double uu=0.;
    for(int j=0;j<m_jpary_Rich1_Ks ; j++){
      uu += vectPar[j]*m_yProj_Rich1_Ks[i][j];
    }
    m_coefDevRich1_Y[i] = uu;
  }

}
//------------------------------------------------------------------------------------------------

void PatCalibV0::computeParamRichVelo_Ks_Der() const {


  /*
    compute the coefficients of the z-polynomial for extrapolation in rich1
    (slopeses) specialized to Kshorts/Seeds
  */

  double vectPar[55];  // third degree max in phase space var's.
  int jpar;
  int idegp;

  idegp=m_ideg_x_Rich1_Ks_Der+1;

  builtVectProj(jpar,vectPar,m_ideg_phx_Rich1_Ks_Der);
  for(int i=0;i<idegp; i++){
    double uu=0.;
    for(int j=0;j< m_jparx_Rich1_Ks_Der; j++){
      uu += vectPar[j]*m_xProj_Rich1_Ks_Der[i][j];
    }
    m_coefDevRich1_X_Der[i] = uu;
  }

  idegp=m_ideg_y_Rich1_Ks_Der+1;
  builtVectProj(jpar,vectPar,m_ideg_phy_Rich1_Ks_Der);
  for(int i=0;i<idegp; i++){
    double uu=0.;
    for(int j=0;j<m_jpary_Rich1_Ks_Der ; j++){
      uu += vectPar[j]*m_yProj_Rich1_Ks_Der[i][j];
    }
    m_coefDevRich1_Y_Der[i] = uu;
  }

}

//=============================================================================
void PatCalibV0::storeTrackParams_prov(){

  m_trackExpansionX[0]=m_vdat[0]-m_vdat[2]*m_zRefRich1 + m_coefDevRich1_X[0];
  m_trackExpansionX[1]=m_coefDevRich1_X[1]+m_vdat[2];
  m_trackExpansionX[2]=m_coefDevRich1_X[2];

  m_trackExpansionY[0]=m_vdat[1]-m_vdat[3]*m_zRefRich1 + m_coefDevRich1_Y[0];
  m_trackExpansionY[1]=m_vdat[3]+m_coefDevRich1_Y[1];

  m_slopeExpansionX[0]=m_vdat[2]+m_coefDevRich1_X_Der[0];
  m_slopeExpansionX[1]=m_coefDevRich1_X_Der[1];
  m_slopeExpansionX[2]=m_coefDevRich1_X_Der[2];
  m_slopeExpansionX[3]=m_coefDevRich1_X_Der[3];
  m_slopeExpansionX[4]=m_coefDevRich1_X_Der[4];

  m_slopeExpansionY[0]=m_vdat[3]+m_coefDevRich1_Y_Der[0];
  m_slopeExpansionY[1]=m_coefDevRich1_Y_Der[1];
  m_slopeExpansionY[2]=m_coefDevRich1_Y_Der[2];

  /*
    zz is the z coord where to compute the x,y, tx,ty values

    equations are :  xx= m_trackExpansionX[i]*zz**i  imax=2 imin=0
    yy= m_trackExpansionY[i]*zz**i  imax=1 imin=0

    dx=m_slopeExpansionX[i]*zz**i  imax=4 imin=0
    dy=m_slopeExpansionY[i]*zz**i  imax=2 imin=0
  */

}
//=============================================================================
void PatCalibV0::recoFromParams_prov(double z,double & xRec,double & yRec,double & txRec,double & tyRec){
  xRec=0.;
  txRec=0.;
  yRec = m_trackExpansionY[0]+m_trackExpansionY[1]*z;
  tyRec=0.;
  double coef = 1.;
  for(int i=0; i <3; i++){
    xRec  += m_trackExpansionX[i] * coef;
    tyRec += m_slopeExpansionY[i] * coef;
    coef *= z;
  }
  coef = 1;
  for(int i=0; i <5; i++){
    txRec  += m_slopeExpansionX[i] * coef;
    coef *= z;
  }


}
//=============================================================================
double PatCalibV0::xCoordFast(double z) const {

  /*
    computes x-coordinate at z
    should be called with the right vdat_input
  */
  double position;
  double xTrans;
  int idegp;
  int ideg_x_Rich1;
  ideg_x_Rich1=m_ideg_x_Rich1_Ks;

  idegp=ideg_x_Rich1+1;
  xTrans=m_vdat[0]+m_vdat[2]*(z - m_zRefRich1);
  double fx=xTrans;
  double coef = 1;
  for(int is=0; is <  idegp; is++){
    fx += m_coefDevRich1_X[is] * coef;
    coef *= z;
  }
  position = fx ;

  return position;  // en cm's /mm's   controler les unites

}
//=============================================================================
double PatCalibV0::yCoordFast(double z) const {

  /*
    computes y-coordinate at z
    should be called with the right vdat_input
  */

  double position;
  double yTrans;
  int idegp;

  int ideg_y_Rich1;
  ideg_y_Rich1=m_ideg_y_Rich1_Ks;

  idegp=ideg_y_Rich1+1;
  yTrans=m_vdat[1]+m_vdat[3]*(z - m_zRefRich1 );
  double fy=yTrans;
  double coef = 1.;
  for(int is=0; is <  idegp; is++){
    fy += m_coefDevRich1_Y[is] * coef;
    coef *= z;
  }
  position = fy ;

  return position;  // en cm's /mm's  controler les unites

}
//=============================================================================
double PatCalibV0::xSlopeFast(double z) const {
  /*
    computes x-slope at z
    should be called with the right vdat_input
  */

  double slope;
  double dxTrans;
  int idegp;

  int ideg_x_Rich1;
  //if(m_isKsTrack){
  ideg_x_Rich1=m_ideg_x_Rich1_Ks_Der;

  idegp=ideg_x_Rich1+1;
  dxTrans=m_vdat[2];
  double fx=0.;
  double coef = 1.;
  for(int is=0; is <  idegp ; is++){
    fx += m_coefDevRich1_X_Der[is] * coef;
    coef *= z;
  }
  slope = fx+dxTrans ;

  return slope;

}
//=============================================================================
double PatCalibV0::ySlopeFast(double z) const {

  /*
    computes y-slope at z
    should be called with the right vdat_input
  */

  double slope;
  double dyTrans;
  int idegp;

  int ideg_y_Rich1;
  ideg_y_Rich1=m_ideg_y_Rich1_Ks_Der;

  idegp=ideg_y_Rich1+1;
  dyTrans=m_vdat[3];
  double fy=0.;
  double coef = 1.;
  for(int is=0; is <  idegp; is++){
    fy += m_coefDevRich1_Y_Der[is] * coef;
    coef *= z;
  }
  slope = fy + dyTrans;

  return slope;

}
//=============================================================================
//  Finalize
//=============================================================================
StatusCode PatCalibV0::finalize() {
  /*
    accepted Ks (all) nCounters[0]
    dont Bd           nCounters[2]

    in Ph Sp Ks (all) nCounters[1]
    dont Bd           nCounters[3]

    in PhSp and m_chiSq  < 10
    Ks (all)    nCounters[4]
    dont Bd           nCounters[5]

    final selection
    Ks (all)    nCounters[6]
    dont Bd           nCounters[7]

    selected number of pairs nCounters[8] (bkg + signal)

  */
  info()  << "**************************************************************************"<<endreq;
  info()  << "    All tracks    "<<endreq;
  info()  << "        "<<endreq;
  info()  << "CalibKShort  :: total number of    V0 ="<< m_nCounters[0]
          << " , surviving Ph Sp Cuts = " << m_nCounters[1] << "[ "
          << ((double)m_nCounters[1])/((double)m_nCounters[0])*100 <<" %]"
          << " , and VTX reco = " <<m_nCounters[4]<< "[ " << ((double)m_nCounters[4])/((double)m_nCounters[0])*100 <<" %]"
          <<endreq;

  info()  << "CalibKShort  :: All V0 , final yield/[Efficiency] = "
          <<m_nCounters[6]<< "[ " << ((double) m_nCounters[6])/((double) m_nCounters[0])*100 <<" %]"
          << " , Dilution =" << ((double) m_nCounters[6])/((double) m_nCounters[8])*100 <<" %]"
          <<endreq;

  info()  << "CalibKShort  :: V0 Daugthers of Bd /(anti)Lambda_b :: *********" << endreq;
  info()  << "CalibKShort  :: total number of (Bd/(anti)Lambda_b) Ks/Lambda/(anti)Lambda ="<<  m_nCounters[2]
          << " , surviving Ph Sp Cuts = " <<  m_nCounters[3] << "[ "
          << ((double) m_nCounters[3])/((double) m_nCounters[2])*100 <<" %]"
          << " , and VTX reco = " << m_nCounters[5]<< "[ " << ((double) m_nCounters[5])/((double) m_nCounters[2])*100 <<" %]"
          <<endreq;

  info()  << "CalibKShort  ::  (Bd/(anti)Lambda_b)    Ks/Lambda/(anti)Lambda  final yield/Efficiency = "
          << m_nCounters[7]<< "[ " << ((double) m_nCounters[7])/((double) m_nCounters[2])*100 <<" %]"
          << " , Dilution =" << ((double) m_nCounters[7])/((double) m_nCounters[8])*100 <<" %]"
          <<endreq;
  info()  << "        "<< endreq;

  info()  << "--------------------------------------------------------------------------"<< endreq;
  info()  << "CalibKShort  :: total number of    Ks ="<< m_nCounters_K0[0]
          << " , surviving Ph Sp Cuts = " << m_nCounters_K0[1] << "[ "
          << ((double)m_nCounters_K0[1])/((double)m_nCounters_K0[0])*100 <<" %]"
          << " , and VTX reco = " <<m_nCounters_K0[4]<< "[ " 
          << ((double)m_nCounters_K0[4])/((double)m_nCounters_K0[0])*100 <<" %]"
          <<endreq;

  info()  << "CalibKShort  :: All Ks , final yield/[Efficiency] = "
          <<m_nCounters_K0[6]<< "[ " << ((double) m_nCounters_K0[6])/((double) m_nCounters_K0[0])*100 <<" %]"
          << " , Dilution =" << ((double) m_nCounters_K0[6])/((double) m_nCounters_K0[8])*100 <<" %]"
          <<endreq;

  info()  << "CalibKShort  :: Ks Daugthers of Bd :: *********" << endreq;
  info()  << "CalibKShort  :: total number of Bd Ks ="<<  m_nCounters_K0[2]
          << " , surviving Ph Sp Cuts = " <<  m_nCounters_K0[3] << "[ "
          << ((double) m_nCounters_K0[3])/((double) m_nCounters_K0[2])*100 <<" %]"
          << " , and VTX reco = " << m_nCounters_K0[5]<< "[ " 
          << ((double) m_nCounters_K0[5])/((double) m_nCounters_K0[2])*100 <<" %]"
          <<endreq;

  info()  << "CalibKShort  ::  Bd Ks  final yield/Efficiency = "
          << m_nCounters_K0[7]<< "[ " << ((double) m_nCounters_K0[7])/((double) m_nCounters_K0[2])*100 <<" %]"
          << " , Dilution =" << ((double) m_nCounters_K0[7])/((double) m_nCounters_K0[8])*100 <<" %]"
          <<endreq;
  info()  << "        "<< endreq;

  info()  << "--------------------------------------------------------------------------"<< endreq;
  info()  << "CalibKShort  :: total number of  Lambda    ="<< m_nCounters_Lambda[0]
          << " , surviving Ph Sp Cuts = " << m_nCounters_Lambda[1] << "[ "
          << ((double)m_nCounters_Lambda[1])/((double)m_nCounters_Lambda[0])*100 <<" %]"
          << " , and VTX reco = " <<m_nCounters_Lambda[4]<< "[ " 
          << ((double)m_nCounters_Lambda[4])/((double)m_nCounters_Lambda[0])*100 <<" %]"
          <<endreq;

  info()  << "CalibKShort  :: All Lambda , final yield/[Efficiency] = "
          <<m_nCounters_Lambda[6]<< "[ " << ((double) m_nCounters_Lambda[6])/((double) m_nCounters_Lambda[0])*100 <<" %]"
          << " , Dilution =" << ((double) m_nCounters_Lambda[6])/((double) m_nCounters_Lambda[8])*100 <<" %]"
          <<endreq;

  info()  << "CalibKShort  :: Lambda Daugthers of Lambda_b :: *********" << endreq;
  info()  << "CalibKShort  :: total number of Lambda from Lambda_b ="<<  m_nCounters_Lambda[2]
          << " , surviving Ph Sp Cuts = " <<  m_nCounters_Lambda[3] << "[ "
          << ((double) m_nCounters_Lambda[3])/((double) m_nCounters_Lambda[2])*100 <<" %]"
          << " , and VTX reco = " << m_nCounters_Lambda[5]<< "[ " 
          << ((double) m_nCounters_Lambda[5])/((double) m_nCounters_Lambda[2])*100 <<" %]"
          <<endreq;

  info()  << "CalibKShort  ::  Lambda_b  Lambda's final yield/Efficiency = "
          << m_nCounters_Lambda[7]<< "[ " << ((double) m_nCounters_Lambda[7])/((double) m_nCounters_Lambda[2])*100 <<" %]"
          << " , Dilution =" << ((double) m_nCounters_Lambda[7])/((double) m_nCounters_Lambda[8])*100 <<" %]"
          <<endreq;
  info()  << "        "<< endreq;

  info()  << "--------------------------------------------------------------------------"<< endreq;
  info()  << "CalibKShort  :: total number of  LambdaBar    ="<< m_nCounters_LambdaBar[0]
          << " , surviving Ph Sp Cuts = " << m_nCounters_LambdaBar[1] << "[ "
          << ((double)m_nCounters_LambdaBar[1])/((double)m_nCounters_LambdaBar[0])*100 <<" %]"
          << " , and VTX reco = " <<m_nCounters_LambdaBar[4]<< "[ " 
          << ((double)m_nCounters_LambdaBar[4])/((double)m_nCounters_LambdaBar[0])*100 <<" %]"
          <<endreq;

  info()  << "CalibKShort  :: All anti-Lambda , final yield/[Efficiency] = "
          <<m_nCounters_LambdaBar[6]<< "[ " << ((double) m_nCounters_LambdaBar[6])/((double) m_nCounters_LambdaBar[0])*100 <<" %]"
          << " , Dilution =" << ((double) m_nCounters_LambdaBar[6])/((double) m_nCounters_LambdaBar[8])*100 <<" %]"
          <<endreq;

  info()  << "CalibKShort  :: anti-Lambda Daugthers of anti-Lambda_b :: *********" << endreq;
  info()  << "CalibKShort  :: total number of anti-Lambda from anti-Lambda_b="<<  m_nCounters_LambdaBar[2]
          << " , surviving Ph Sp Cuts = " <<  m_nCounters_LambdaBar[3] << "[ "
          << ((double) m_nCounters_LambdaBar[3])/((double) m_nCounters_LambdaBar[2])*100 <<" %]"
          << " , and VTX reco = " << m_nCounters_LambdaBar[5]<< "[ " 
          << ((double) m_nCounters_LambdaBar[5])/((double) m_nCounters_LambdaBar[2])*100 <<" %]"
          <<endreq;

  info()  << "CalibKShort  ::  anti-Lambda_b  anti-Lambda's final yield/Efficiency = "
          << m_nCounters_LambdaBar[7]<< "[ " << ((double) m_nCounters_LambdaBar[7])/((double) m_nCounters_LambdaBar[2])*100 
          <<" %]"
          << " , Dilution =" << ((double) m_nCounters_LambdaBar[7])/((double) m_nCounters_LambdaBar[8])*100 <<" %]"
          <<endreq;
  info()  << "        "<< endreq;

  info()  << "**************************************************************************"<< endreq;
  info()  << "    tracks with NoVelo piece    "<<endreq;
  info()  << "        "<<endreq;

  info()  << "CalibKShort  :: total number of    Ks ="<< m_nCountersNoVelo[0]
          << " , surviving Ph Sp Cuts = " << m_nCountersNoVelo[1] << "[ "
          << ((double)m_nCountersNoVelo[1])/((double)m_nCountersNoVelo[0])*100 <<" %]"
          << " , and VTX reco = " <<m_nCountersNoVelo[4]<< "[ "
          << ((double)m_nCountersNoVelo[4])/((double)m_nCountersNoVelo[0])*100 <<" %]"
          <<endreq;

  info()  << "CalibKShort  :: All Ks , final yield/Efficiency = "
          <<m_nCountersNoVelo[6]<< "[ " << ((double) m_nCountersNoVelo[6])/((double) m_nCountersNoVelo[0])*100 <<" %]"
          << " , Dilution =" << ((double) m_nCountersNoVelo[6])/((double) m_nCountersNoVelo[8])*100 <<" %]"
          <<endreq;

  info()  << "CalibKShort  :: Ks Daugthers of Bd :: *** " << endreq;
  info()  << "CalibKShort  :: total number of Bd Ks ="<<  m_nCountersNoVelo[2]
          << " , surviving Ph Sp Cuts = " <<  m_nCountersNoVelo[3] << "[ "
          << ((double) m_nCountersNoVelo[3])/((double) m_nCountersNoVelo[2])*100 <<" %]"
          << " , and VTX reco = " << m_nCountersNoVelo[5]<< "[ "
          << ((double) m_nCountersNoVelo[5])/((double) m_nCountersNoVelo[2])*100 <<" %]"
          <<endreq;

  info()  << "CalibKShort  ::  Bd Ks  final yield/Efficiency = "
          << m_nCountersNoVelo[7]<< "[ " << ((double) m_nCountersNoVelo[7])/((double) m_nCountersNoVelo[2])*100 <<" %]"
          << " , Dilution =" << ((double) m_nCountersNoVelo[7])/((double) m_nCountersNoVelo[8])*100 <<" %]"
          <<endreq;
  info()  << "**************************************************************************"<<endreq;
  info()  << " nombre de K0 parents  (double count+X)  =" <<  m_nK0courts  << " , with accepted pairs ="<<m_acc_Ks <<endreq;
  info()  << " nombre de Lambda parents  (double count+X)    =" << m_nLambda << " , with accepted pairs ="<<m_acc_Lambda <<endreq;
  info()  << " nombre de LambdaBar parents (double count+X)  =" << m_nLambdaBar << " , with accepted pairs ="<<m_acc_LambdaBar
          <<endreq;
  info()  << " nombre de Lamba_b     (double count+X)    =" <<  m_nLambda_b  <<endreq;
  info()  << " nombre de Lamba_b_Bar (double count+X)    =" <<  m_nLambda_b_Bar <<endreq;
  info()  << "**************************************************************************"<<endreq;
  debug() << "==> Finalize" << endmsg;

  return GaudiHistoAlg::finalize();  // must be called after all other actions
}

//=============================================================================
