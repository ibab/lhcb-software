// Include files

//Event
#include "Event/RecVertex.h"      // Sean Modif
#include "Event/TwoProngVertex.h"     // Sean Modif

// From PartProp
#include "Kernel/IParticlePropertySvc.h"    // Sean Modif
#include "Kernel/ParticleProperty.h"    // Sean Modif

// local
#include "PatMakeV0.h"
#include "GaudiKernel/SystemOfUnits.h"

//-----------------------------------------------------------------------------
// Implementation file for class : PatMakeV0
//
// 2008-31-01 : Maurice Benayoun & Olivier Callot
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( PatMakeV0 )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
PatMakeV0::PatMakeV0( const std::string& name,
                      ISvcLocator* pSvcLocator)
  : GaudiHistoAlg  ( name , pSvcLocator )
{
  declareProperty( "zVtxImprovMn",       m_zMinVtxImp =    200. * Gaudi::Units::mm );   //mm   minimum zV unused
  declareProperty( "zVtxImprovMx",       m_zMaxVtxImp =   2300. * Gaudi::Units::mm );   //mm   maximum zV unused
  declareProperty( "pMin",               m_pMinRich1 =  2000. * Gaudi::Units::MeV ); // minimum pt of charged tracks
  declareProperty( "OutputContainer",    m_outputName = "Rec/Track/FromKs" );
  declareProperty( "outputV0Container",  m_outputV0ContainerName=LHCb::RecVertexLocation::V0); //S.Brisbane 07/01/2008
  declareProperty( "zRef",               m_zRefRich1 =  2350. * Gaudi::Units::mm ); // max limit  for vtx reconstruction
  declareProperty( "zKsMin",             m_zKsMin    =     0. * Gaudi::Units::mm ); // min limit  for vtx reconstruction
  declareProperty( "pionMass",           m_Pion   =  139.57018 * Gaudi::Units::MeV );
  declareProperty( "kaonMass",           m_Kshort  =   497.648 * Gaudi::Units::MeV );
  declareProperty( "protonMass",  m_Proton     =   938.272029 * Gaudi::Units::MeV );
  declareProperty( "lambdaMass",  m_Lambda     =  1115.683* Gaudi::Units::MeV  );
  declareProperty( "massCutNsig",        m_Nsig   =   3.0  );      // # of sigmas Cut for Ks / Lambda/LambdaBar
  declareProperty( "kaonSig",            m_SigK0   =   20. * Gaudi::Units::MeV );    // sigma for Ks Cut
  declareProperty( "lambdaSig",          m_SigLambda     =   6. * Gaudi::Units::MeV ); // sigma (anti)Lambda Cut
  declareProperty( "ImpactParam",        m_impactParV0  =   20. * Gaudi::Units::mm );   // mm
  declareProperty( "ImpactParamTrkX",    m_impactParTrkX =   60. * Gaudi::Units::mm );  // mm
  declareProperty( "ImpactParamTrkY",    m_impactParTrkY =   40. * Gaudi::Units::mm );  // mm
  declareProperty( "ImpactParamSlp",     m_impactParSlp = 0.25 );       // SlpX or SlpY (rd)  origin 0.25
  declareProperty( "SigmaXKs",           m_sigmaX     =    4.43  * Gaudi::Units::mm );   //  reco vtx
  declareProperty( "SigmaYKs",           m_sigmaY     =    3.72  * Gaudi::Units::mm );   //  reco vtx
  declareProperty( "SigmaZKs",           m_sigmaZ     =    77.16 * Gaudi::Units::mm );   //  reco vtx (unused)
  declareProperty( "IterateVtx",         m_iterVtx    =     5     );      // # iter in vtx fit
  declareProperty( "AngleCut",           m_angleCut    =   25.e-03     );   //  angle between vtx dir and MomDir
  declareProperty( "CutPT",              m_cutPt     =   0.01 * Gaudi::Units::GeV );   // minimum pt of the V0 in GeV, 0 = no cut
  declareProperty( "Chi2VTXLim",         m_chi2VtxLim        =  10. );  //  max chi2 for V0 vertex fit      (histos 5 to 7
  declareProperty( "Chi2VTXPrimLim",     m_primVtxOKLim      =   5. ); //  min chi2 for Prim vertex normalized dist.(h 17 to 19
  declareProperty( "impactCurrentNorm", m_impactCurNormLim  =   4. ); //  minimum V0 impact parameter at origin (histos 127/128
}
//=============================================================================
// Destructor
//=============================================================================
PatMakeV0::~PatMakeV0() {}

//=============================================================================
// Initialization
//=============================================================================
StatusCode PatMakeV0::initialize() {
  StatusCode sc = GaudiHistoAlg::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  initialiseRichVelo_Ks();  // initialise reco for Kshort/Downstream tracks

  m_plotChi2=book(7 , " chi2 distribution of improved vtx fit, no cut ", 0.,10.,50);
  // Full Reco histos
  m_recoMomentum = book(510, " Reco  : track momentum (GeV), no cut ", -100.,100.,400);
  m_xOrg = book(511," Reco  : x PhSp at Origin (mm) before cuts",-100.,100.,200);
  m_yOrg = book(512," Reco  : y PhSp at Origin (mm) before cuts",-100.,100.,200);
  m_txOrg = book(513," Reco  : tx PhSp at Origin (rd) before cuts",-1.,1.,100);
  m_tyOrg = book(514," Reco  : ty PhSp at Origin (rd) before cuts",-1.,1.,100);

  m_vtxMomAngAll=book(601," angle (mrd) vtx Mom, All pairs reco",-10.,190.,200);
  m_ptRec=book(602," pT of Reconstructed V0 (GeV) " ,0.,5.,100);

  m_invMassRecoKs = book(1201, " Reco : Kshort Invariant Mass (MeV) ",0.,1000.,200);
  m_invMassRecoLambda = book(1202, " Reco : Lambda Invariant Mass (MeV) ",500.,1500.,200);
  m_invMassRecoLambdaBar = book(1203, " Reco : LambdaBar Invariant Mass (MeV) ",500.,1500.,200);

  m_impactAtOriginReco = book2D(1300," Reco : (x,y) impact at z = 0 (mm) ", -50.,50.,100,-50.,50.,100);
  m_multiplicity = book(1199, " Reco : Multiplicite ",0.,25.,25);
  m_impactAtOriginChoice = book2D(1500," Reco : (x,y) impact at z = 0 (mm) ", -50.,50.,100,-50.,50.,100);

  m_zRec = book(1700,"  Reco :  z Loc of V0 Vtx (mm)",-100.,2500.,260);
  m_rVsz = book2D(1400," Reco : VTX tranverse distance versus z(mm) ",0.,2500.,250,0.,500.,100);

  m_xyPrimVtx       = book2D(1495,"Calibration : x,y primary vertex",-2,2,20,-2,2,20);
  m_zPrimVtxh       = book(1496, " Calibration : zPrimary Vertex ",-200.,300.,200);
  m_checkPrimChi2   = book(1497, " Choice : chi2 for prim. vtx test (primOK)",0.,200.,200);

  m_codeMassSelect   = book(1600," Selected : code mass distribution of solutions",-0.5,14.5,15);
  m_wrongCase       = book(1601," Selected :  impossible solutions",-0.5,14.5,15);

  m_KsMassSelect   = book(1800, " Selected : Kshort Invariant Mass (MeV) ",0.,1000.,200);
  m_LambdaMassSelect   = book(1801, " Selected : Lambda Invariant Mass (MeV) ",500.,1500.,200);
  m_LambdaBarMassSelect   = book(1802, " Selected : antiLambda Invariant Mass (MeV) ",0.,1500.,200);
  m_nSolSelectTotal= book(1808, " Selected : Total Multiplicite ",-10.,10.,20);
  m_nSolSelect     = book(1809, " Selected : Multiplicite Distribution",0.,25.,25);
  m_bestchi2Select = book(1820, " Selected :  chi2 distribution", 0.,10.,10);
  m_zRecSelect     = book(1830, " Selected : Reco z Loc of Ks Vtx (mm)",-100.,2500.,260);

  if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) debug() << "==> Initialize" << endmsg;

  //the particle property service is initialised with the properties in condDB
  LHCb::IParticlePropertySvc* ppSvc = svc<LHCb::IParticlePropertySvc>( "LHCb::ParticlePropertySvc", true );
  const LHCb::ParticleProperty* partP = ppSvc->find( "KS0" );
  m_K0sPID = LHCb::ParticleID( partP->pdgID());
  partP = ppSvc->find( "Lambda0" );
  m_lambdaPID =  LHCb::ParticleID(partP->pdgID());
  partP = ppSvc->find( "Lambda~0" );
  m_lambdaBarPID =  LHCb::ParticleID(partP->pdgID());
  // end changes (probably should use partProperties for the masses too...can change this later

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode PatMakeV0::execute() {

  if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) debug() << "==> Execute" << endmsg;

  LHCb::Tracks* downs  = get<LHCb::Tracks>( LHCb::TrackLocation::Downstream );
  if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) 
    debug() << "Start from " << downs->size() << " downstream tracks." << endmsg;

  LHCb::RecVertices* primaryVertices = get<LHCb::RecVertices>( LHCb::RecVertexLocation::Velo3D );
  if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) 
    debug() << "Found " << primaryVertices->size() << " primary vertices" << endmsg;

  m_outputContainer = new LHCb::Tracks();
  m_outputV0Container = new V0s();      // Sean Modif

  put( m_outputContainer, m_outputName );
  put ( m_outputV0Container, m_outputV0ContainerName );   // Sean Modif

  m_pos.clear();
  m_neg.clear();
  m_solutions.clear();
  m_selected.clear();
  m_zPrimVtx.clear();  // CORRECTION DEC. 2007

  for ( LHCb::Tracks::const_iterator itT = downs->begin(); downs->end() != itT; ++itT ) {

    LHCb::State& m_state=(*itT)->closestState(2350.);  // close to TT mid Location

    // check that tack momentum is above threshold
    double trackMomentum=1./fabs(m_state.qOverP());
    bool tooLowMom = trackMomentum < m_pMinRich1;
    if(tooLowMom){
      if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) {
        debug() << " ... track " << (*itT)->key() << " skipped " << endmsg;
        debug() << format( "   p%10.2f ",trackMomentum) << " is too low momentum " << endmsg;
      }
      

      continue;  // <<<< ==============  skip track (too low momentum)
    }

    // construct track trajectory parameters

    createInputState( m_state );
    computeParamRichVelo_Ks_Der();
    computeParamRichVelo_Ks();

    // check track phase space
    bool skipTrk=checkPhaseSpaceAtOrigin();
    if(skipTrk) {
      if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) {
        debug() << " ... track " << (*itT)->key() 
                << " not in phase space at origin " << endmsg;
        debug() << format( "   x%7.2f y%7.2f tx%8.4f ty%8.4f",
                           xCoordFast( 0. ), yCoordFast( 0. ),
                           xSlopeFast( 0. ), ySlopeFast( 0. ) ) << endmsg;
      }
      continue;  // <<<< ==============  skip track (not in phase space)
    }

    storeTrackParams_prov();

    int charge =1;
    if(m_state.qOverP() < 0) charge =-1;

    m_recoMomentum->fill(1./m_state.qOverP()/1000);

    LocalV0Track temp;
    temp.track    = *itT;
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
      if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) 
        debug() << "Selected + track " << (*itT)->key() << endmsg;
      m_pos.push_back( temp );
    } else {
      // negative tracks
      if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) 
        debug() << "Selected - track " << (*itT)->key() << endmsg;
      m_neg.push_back( temp );
    }
  }  // end loop (LHCb::Tracks::const_iterator itT)
  //---------------------------------------------------------------
  // store PRIMARY VERTICES

  for( LHCb::RecVertices::const_iterator itV = primaryVertices->begin();
       primaryVertices->end() != itV; ++itV ){
    m_zPrimVtxh->fill( (*itV)->position().z() );
    m_xyPrimVtx->fill( (*itV)->position().x(), (*itV)->position().y() );
    m_zPrimVtx.push_back( (*itV)->position().z() );
  }

  int n_solV0;
  double distInSigma;
  int codeSolMass=0;

  int iNeg_rem=-1;

  for (unsigned int iPos=0;iPos<m_pos.size(); iPos++){
    n_solV0=0;
    distInSigma=10000;

    for (unsigned int iNeg=0;iNeg<m_neg.size(); iNeg++){

      // Full Reco
      if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) 
        debug() << " try pos " << iPos << " neg " << iNeg
                << " = seed " << m_pos[iPos].track->key() 
                << " and " << m_neg[iNeg].track->key() << endmsg;

      StatusCode  sc= intersection( m_pos[iPos], m_neg[iNeg] );
      if ( sc.isFailure() ) {
        if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) 
          debug() << " ... Failure for intersection" << endmsg;
        continue;
      }

      StatusCode sc1b =improveVTX(  m_pos[iPos], m_neg[iNeg] );
      if( sc1b.isFailure() ){
        if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) 
           debug () << " ... failure for improveVtx" << endmsg;
        continue;
      }

      // after this call : m_zV0Vtx,m_xV0Vtx,m_yV0VTx and m_chiSq are computed and loaded


      StatusCode sc2 = reco4Momentum( m_pos[iPos], m_neg[iNeg] );

      double xPhSp, txPhSp, yPhSp, tyPhSp;

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

      // chi2 vertex fit cut
      //    double zV=m_zV0Vtx; // with old storeV0Info method
      m_plotChi2->fill(m_chiSq);
      if(m_chiSq  > m_chi2VtxLim) {
        if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) 
          debug() << "    .. V0 vertex has bad chiSq, skip solution  "
                  << m_chiSq << endmsg;
        continue;  // to speed up computing
      }

      // V0 impact parameter cut
      double impactV0=sqrt(m_xImpact*m_xImpact +m_yImpact*m_yImpact);
      if(impactV0 > m_impactParV0 ) {

        //  if(fabs(m_xImpact) > m_impactParV0 || fabs(m_yImpact) > m_impactParV0 ) {} // CHANGE from dec. 2007

        if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) {
          debug() << "... V0  has too small impact parameter=" 
                  << impactV0 << "  (mm), skip solution  " << endmsg;
          debug() << "... impact x = " << m_xImpact << ",  y = "
                  << m_yImpact << "mm" << endmsg;
        }
        
        continue;
      }

      // coupure en  zVtx  ******** MOST DRAMATIC CUT **********
      double primOK =CheckPrimaryVtx( m_pos[iPos], m_neg[iNeg] );
      if( primOK < m_primVtxOKLim ) {
        if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) 
          debug() << " ... V0  has primOK too small : " << primOK
                  << " , skip solution" << endmsg;
        continue;
      }

      // invariant mass cut

      double ma=m_Kshort+m_Nsig*m_SigK0;
      double mb=m_Kshort-m_Nsig*m_SigK0;
      bool testMassK0= (m_invariantKsMass-ma)*(m_invariantKsMass-mb) < 0;

      ma=m_Lambda+m_Nsig*m_SigLambda;
      mb=m_Lambda-m_Nsig*m_SigLambda;

      bool testMassLambda= (m_invariantLambdaMass-ma)*(m_invariantLambdaMass-mb) < 0;

      bool testMassLambdaBar= (m_invariantLambdaBarMass-ma)*(m_invariantLambdaBarMass-mb) < 0;

      bool testMass=(testMassK0 || testMassLambda || testMassLambdaBar);

      if( !testMass ) {
        if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) {
          debug() << "   ... no invariant mass in any V0 window : " << endmsg;
          debug() << "   ... ....     Ks  = "<< m_invariantKsMass << " MeV " <<endmsg;
          debug() << "   ... .... Lambda  = "<< m_invariantLambdaMass << " MeV " <<endmsg;
          debug() << "   ... ..LambdaBar  = "<< m_invariantLambdaBarMass << " MeV " <<endmsg;
        }
        
        continue;
      }


      // coupure en impact au "vrai VTX"
      double impactCurrentNormed = NormedSmallestImpactParameter();
      if( impactCurrentNormed > m_impactCurNormLim ) {
        if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) 
          debug() << "   ... too small V0 impact parameter "
                  << impactCurrentNormed << endmsg;
        continue;
      }

      // cut on angle between line of flight and V0 3-momentum
      double angle=vtxMomAngle(primaryVertices);
      m_vtxMomAngAll->fill(angle*1000.);  // plot mrd

      if(angle > m_angleCut) {
        if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) 
          debug() << "   ... too large angle  V0 (momentum,line of flight) "
                  << angle  << endmsg;
        continue;
      }

      // cut out too low momentum V0
      double ptRec=sqrt(m_V0Mom[1]*m_V0Mom[1]+m_V0Mom[2]*m_V0Mom[2]);
      m_ptRec->fill(ptRec/1000.);   //plot GeV
      if( ptRec<m_cutPt ) {
        if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) 
          debug() << "   ... too small  V0 transverse momentum "
                  << m_chiSq << endmsg;
        continue;
      }


      n_solV0++;
      // check overlapping assignments; foreseen assignments :1,2, 4, 3 and 5 ; unexpected 6, 7
      codeSolMass=0;
      if(testMassK0) {
        codeSolMass +=(EncodedCompatiblePID) K0s;
        m_invMassRecoKs->fill(m_invariantKsMass);
      }
      if(testMassLambda){
        codeSolMass +=(EncodedCompatiblePID) Lambda;
        m_invMassRecoLambda->fill(m_invariantLambdaMass);
      }
      if(testMassLambdaBar) {
        codeSolMass +=(EncodedCompatiblePID) LambdaBar;
        m_invMassRecoLambdaBar->fill(m_invariantLambdaBarMass);
      }

      m_impactAtOriginReco->fill(m_xImpact,m_yImpact);
      m_checkPrimChi2->fill(primOK);

      double rr=sqrt(m_xV0Vtx*m_xV0Vtx+m_yV0Vtx*m_yV0Vtx);
      double z=m_zV0Vtx;
      m_rVsz->fill(z,rr);
      double dist=m_chiSq;

      // flag and store the best chi2
      if(dist < distInSigma) {
        distInSigma=dist;
        //    storeV0Info(zV,primOK,codeSolMass);
        double vtxLoc[3];
        vtxLoc[0]=m_zV0Vtx;
        vtxLoc[1]=m_xV0Vtx;
        vtxLoc[2]=m_yV0Vtx;
        storeV0Info(vtxLoc,primOK,codeSolMass);
        m_angle=vtxMomAngle(primaryVertices);
        iNeg_rem=iNeg;
        if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) 
          debug() << "  +++ Good solution found" << endmsg;
      }
    }        // end (inner) loop on 0 < trks

    if(n_solV0 > 0) {
      m_multiplicity->fill( ((double) n_solV0) );
      m_impactAtOriginChoice->fill(m_xImpactRef,m_yImpactRef );
      m_zRec->fill( m_V0vtxRefZ);

      PatV0Candidate temp( m_pos[iPos].track,m_neg[iNeg_rem].track, m_bestSol, m_invariantV0MassRef, m_V0vtxRefZ,
                           m_primOK, m_allInvariantMasses,m_angle,codeSolMass, m_xImpactRef,m_yImpactRef,
                           m_momTrackPlus, m_momTrackMinus,m_vtxRef);
      m_solutions.push_back( temp );
    }
  }       // end loop on >0 trks

  selectSols();

  return SUCCESS;
}

//============================================================

void PatMakeV0::selectSols(){
  m_selected.clear();

  if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) 
    debug() << " nSolutions " << m_solutions.size() << endmsg;

  // select as solution for the pair the best V0 vtx fit chi2
  // on the negative track (positive is already single by construction)

  std::vector<PatV0Candidate>::iterator itS, itS2;
  for ( itS = m_solutions.begin(); m_solutions.end() > itS; ++itS ) {
    PatV0Candidate& cand = (*itS);
    if( !cand.keep() ) continue;
    for ( itS2 = itS+1;  m_solutions.end() != itS2; ++itS2 ) {
      PatV0Candidate& cand2 = (*itS2);
      if ( cand.negTrk() != cand2.negTrk() ) continue;
      if( !cand2.keep() ) continue;
      if( cand.chi2() > cand2.chi2() ){
        cand.setKeep( false );
      } else{
        cand2.setKeep( false );
      }
    }
  }

  // count solutions

  for ( itS = m_solutions.begin(); m_solutions.end() != itS; ++itS ) {
    PatV0Candidate& cand = (*itS);
    if ( !cand.keep() ) continue;
    m_selected.push_back( cand );
  }

  if(m_selected.size() > 0 ) m_nSolSelect->fill((double) m_selected.size() );

  if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) 
    debug() << "   Selected " << m_selected.size() << "  V0 candidates" << endmsg;

  for ( itS = m_selected.begin(); m_selected.end() != itS; ++itS ) {
    PatV0Candidate& cand = (*itS);

    int codeMass=cand.codeSolMass();
    m_codeMassSelect->fill( codeMass );
    m_nSolSelectTotal->fill(0.);

    if(codeMass == 1){
      m_KsMassSelect->fill( cand.invMassK0() );
    } else if (codeMass == 2) {
      m_LambdaMassSelect->fill( cand.invMassLambda() );
    } else if (codeMass == 3) {
      m_KsMassSelect->fill( cand.invMassK0() );
      m_LambdaMassSelect->fill( cand.invMassLambda() );
    }else if (codeMass == 4) {
      m_LambdaBarMassSelect->fill( cand.invMassLambdaBar() );
    }else if (codeMass == 5) {
      m_KsMassSelect->fill( cand.invMassK0() );
      m_LambdaBarMassSelect->fill( cand.invMassLambdaBar() );
    }else if (codeMass > 5) {
      m_wrongCase->fill( 0.);
    }

    m_bestchi2Select->fill( cand.chi2() );
    m_zRecSelect->fill( cand.zVtxKs() );
    if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) 
      debug() << "   Pos " << cand.posTrk()->key()
              << " Neg " << cand.negTrk()->key()
              << " z " << cand.zVtxKs() << " mass " << cand.massKs() << endmsg;
    // provisional output
    LHCb::Track* sel = cand.negTrk()->clone();
    m_outputContainer->add( sel );
    sel =  cand.posTrk()->clone();
    m_outputContainer->add( sel );
    // Sean outPut
    StatusCode sc;
    sc=copyV0ParamsForTES(cand);
    if(sc==StatusCode::SUCCESS) MakeV0(cand.posTrk(),cand.negTrk(),codeMass);
  }
}
//=======================================================================
void PatMakeV0::storeV0Info(double zV, double primOK,int codeSolMass){
  m_xImpactRef=m_xImpact;
  m_yImpactRef=m_yImpact;
  m_V0vtxRefZ=zV;
  m_primOK= primOK ;  //unused now
  m_bestSol=m_chiSq;

  m_codeSolMass=codeSolMass;  // degeneracy coded from zero to 7;1=K0,2=Lambda,4=LambdaBar
  // zero should never be found (pair dropped out)
  m_invariantV0MassRef   =m_invariantKsMass; //redundent with m_allInvariantMasses[0]
  m_allInvariantMasses[0]=m_invariantKsMass;
  m_allInvariantMasses[1]=m_invariantLambdaMass;
  m_allInvariantMasses[2]=m_invariantLambdaBarMass;

}
//=======================================================================
void PatMakeV0::storeV0Info(double vtxLoc[], double primOK,int codeSolMass){
  m_xImpactRef=m_xImpact;
  m_yImpactRef=m_yImpact;
  m_V0vtxRefZ= vtxLoc[0];

  for(int i=0;i<3;i++){
    m_vtxRef[i]=vtxLoc[i];
  }

  m_primOK= primOK ;  //unused now
  m_bestSol=m_chiSq;

  m_codeSolMass=codeSolMass;  // degeneracy coded from zero to 7;1=K0,2=Lambda,4=LambdaBar
  // zero should never be found (pair dropped out)
  m_invariantV0MassRef   =m_invariantKsMass; //redundent with m_allInvariantMasses[0]
  m_allInvariantMasses[0]=m_invariantKsMass;
  m_allInvariantMasses[1]=m_invariantLambdaMass;
  m_allInvariantMasses[2]=m_invariantLambdaBarMass;

}
//=======================================================================
void PatMakeV0::createInputState(LHCb::State& m_state ){

  //creates the local state vector m_vdat
  // in standard LHCb units

  m_vdat[0]=m_state.x();
  m_vdat[1]=m_state.y();
  m_vdat[2]=m_state.tx();
  m_vdat[3]=m_state.ty();
  m_vdat[4]=m_state.qOverP();
  m_vdat[5]=m_state.z();
}
//=============================================================================
void PatMakeV0::initialiseRichVelo_Ks()
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
  }

  initialiseRich1_Ks();

  initialiseRich1_Ks_Der();

}
//=============================================================================
void PatMakeV0::initialiseRich1_Ks_Der(){

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

void PatMakeV0::initialiseRich1_Ks(){

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
void PatMakeV0::builtVectProj
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

void PatMakeV0::computeParamRichVelo_Ks() const {

  /*
    compute the coefficients of the z-polynomial for extrapolation in rich1
    (coordinates) specialized to Kshorts/Seeds
  */


  double vectPar[55];  // third degree max in phase space var's.
  int jpar = 0;
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

void PatMakeV0::computeParamRichVelo_Ks_Der() const {


  /*
    compute the coefficients of the z-polynomial for extrapolation in rich1
    (slopeses) specialized to Kshorts/Seeds
  */

  double vectPar[55];  // third degree max in phase space var's.
  int jpar = 0;
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
void PatMakeV0::storeTrackParams_prov(){

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
void PatMakeV0::recoFromParams_prov(double z,double & xRec,double & yRec,double & txRec,double & tyRec){
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
double PatMakeV0::xCoordFast(double z) const {

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
double PatMakeV0::yCoordFast(double z) const {

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
double PatMakeV0::xSlopeFast(double z) const {
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
double PatMakeV0::ySlopeFast(double z) const {

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
StatusCode PatMakeV0::MakeV0(const LHCb::Track * posTrack,const LHCb::Track * negTrack,int  IDsFlag)
{

  Gaudi::XYZPoint vertLoc(m_vtxRef[1],m_vtxRef[2],m_vtxRef[0]);
  LHCb::TwoProngVertex* v0 = new LHCb::TwoProngVertex(vertLoc);

  v0->setChi2 (m_chiSq);
  v0->setNDoF(1);

  v0->addToTracks( posTrack );
  v0->addToTracks( negTrack );

  //store the daughter momenta as SVector (tx,ty,qOverP)
  double pPlus = sqrt(m_momTrackPlus[1]*m_momTrackPlus[1]+
                      m_momTrackPlus[2]*m_momTrackPlus[2]+ m_momTrackPlus[3]*m_momTrackPlus[3]);

  double pMinus = sqrt(m_momTrackMinus[1]*m_momTrackMinus[1]+
                       m_momTrackMinus[2]*m_momTrackMinus[2]+ m_momTrackMinus[3]*m_momTrackMinus[3]);

  // add momenta tx,ty,qop?
  v0->momA()(0)=  m_momTrackPlus[1]/pPlus;
  v0->momA()(1)=  m_momTrackPlus[2]/pPlus;
  v0->momA()(2)=  1./pPlus;

  v0->momB()(0)=  m_momTrackMinus[1]/pMinus;
  v0->momB()(1)=  m_momTrackMinus[2]/pMinus;
  v0->momB()(2)=  -1./pMinus;

  if( UNLIKELY( msgLevel(MSG::VERBOSE) ) ) {
    verbose() <<v0->position().z()<<" vertex of v0 " <<endmsg;
    verbose()<<"ks mass sq is " <<  m_invariantKsMass <<" same as "
             << v0->mass(m_Pion, m_Pion) <<endmsg;
  }

  //Discussion point : Is it possible to include the off diagonal components using your algorithm
  Gaudi::SymMatrix3x3 locErrors;
  ROOT::Math::SVector<double, 3> errorSqDiagonal ( m_sigmaX*m_sigmaX,m_sigmaY*m_sigmaY,m_sigmaZ*m_sigmaZ);
  locErrors.SetDiagonal(errorSqDiagonal);
  v0->setCovMatrix(locErrors);

  if (IDsFlag){
    //check the bitMasks against the information encoded in IDsFlag
    if(IDsFlag & (PatMakeV0::EncodedCompatiblePID )K0s ) v0->addPID(m_K0sPID);
    if(IDsFlag & (PatMakeV0::EncodedCompatiblePID )Lambda ) v0->addPID(m_lambdaPID);
    if(IDsFlag & (PatMakeV0::EncodedCompatiblePID )LambdaBar ) v0->addPID(m_lambdaBarPID);
  }

  v0->addInfo( LHCb::TwoProngVertex::IPx, m_xImpactRef);
  v0->addInfo( LHCb::TwoProngVertex::IPy, m_yImpactRef);

  //things to do
  //add positions of closest approach of daughters to the V0
  //Co-variance matrix

  m_outputV0Container->add(v0);

  return StatusCode::SUCCESS;


}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode PatMakeV0::finalize() {

  if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) debug() << "==> Finalize" << endmsg;

  return GaudiHistoAlg::finalize();  // must be called after all other actions
}
