// Include files

#include "TfKernel/RecoFuncs.h"

// local
#include "PatVeloTTTool.h"

//-----------------------------------------------------------------------------
// Implementation file for class : PatVeloTTTool
//
// 2007-05-08 : Mariusz Witek
//-----------------------------------------------------------------------------


// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( PatVeloTTTool )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
PatVeloTTTool::PatVeloTTTool( const std::string& type,
                              const std::string& name,
                              const IInterface* parent )
  : GaudiTool( type, name , parent ),
  m_ttHitManager(0)
  , m_PatTTMagnetTool(0)
{
  declareInterface<ITracksFromTrack>(this);
  declareInterface<PatVeloTTTool>(this);
  

  declareProperty("MaxXSlope"          , m_maxXSlope        = 0.350);
  declareProperty("MaxYSlope"          , m_maxYSlope        = 0.300);
  declareProperty("MaxXSize"           , m_maxXSize         = 660. * Gaudi::Units::mm);
  declareProperty("MaxYSize"           , m_maxYSize         = 580. * Gaudi::Units::mm);
  declareProperty("centralHoleSize"    , m_centralHoleSize  =  39. * Gaudi::Units::mm);
  // Momentum determination
  declareProperty("minMomentum"        , m_minMomentum      = 0.8*Gaudi::Units::GeV);
  declareProperty("minPT"              , m_minPT            = 0.0*Gaudi::Units::GeV);
  declareProperty("maxPseudoChi2"      , m_maxPseudoChi2          = 10000.);
  declareProperty("maxSolutionsPerTrack"  , m_maxSolutionsPerTrack = 3);
  // Tolerances for extrapolation
  declareProperty("XTolerance"         , m_xTol             = 0.35 * Gaudi::Units::mm);
  declareProperty("XTolSlope"          , m_xTolSlope        = 350. * Gaudi::Units::mm);
  declareProperty("YTolerance"         , m_yTol             = 0.8  * Gaudi::Units::mm);
  declareProperty("YTolSlope"          , m_yTolSlope        = 0.2);
  // Grouping tolerance
  declareProperty("DxGroupTol"         , m_dxGroupTol       = 0.8 * Gaudi::Units::mm);
  declareProperty("DxGroupFactor"      , m_dxGroupFactor    = 0.25);
  declareProperty("passUnmatched"      , m_passUnmatched    = false);
  declareProperty("PrintVariables"     , m_PrintVariables    = false);

}
//=============================================================================
// Destructor
//=============================================================================
PatVeloTTTool::~PatVeloTTTool() {}

//=============================================================================
// Main method for VELOTT track rekonstruction
//=============================================================================
//=========================================================================
//  Initialisation, check parameters
//=========================================================================
StatusCode PatVeloTTTool::initialize ( ) {
  StatusCode sc = GaudiTool::initialize();
  if ( !sc ) return sc;

  m_debug = msgLevel( MSG::DEBUG );
  m_verbose = msgLevel( MSG::VERBOSE );
  if( m_debug ) debug() << "==> Initialize" << endmsg;

  m_PatTTMagnetTool = tool<PatTTMagnetTool>( "PatTTMagnetTool","PatTTMagnetTool");

  // m_zMidTT is a position of normalization plane which should to be close to z middle of TT ( +- 5 cm ).
  // Cashed once in PatVeloTTTool at initialization. No need to update with small TT movement.
  m_zMidTT    = m_PatTTMagnetTool->zMidTT();
  //  zMidField and distToMomentum isproperly recalculated in PatTTMagnetTool when B field changes
  double zMidField = m_PatTTMagnetTool->zMidField();
  double distToMomentum = m_PatTTMagnetTool->averageDist2mom();

  if( m_debug ){
    debug() << " MaxXSize           = " << m_maxXSize         << " mm"  << endmsg;
    debug() << " MaxYSize           = " << m_maxYSize         << " mm"  << endmsg;
    debug() << " MaxXSlope          = " << m_maxXSlope                  << endmsg;
    debug() << " MaxYSlope          = " << m_maxYSlope                  << endmsg;
    debug() << " centralHoleSize    = " << m_centralHoleSize  << " mm"  << endmsg;
    debug() << " minMomentum        = " << m_minMomentum      << " MeV" << endmsg;
    debug() << " maxPseudoChi2      = " << m_maxPseudoChi2    << "   "  << endmsg;
    debug() << " distToMomentum     = " << distToMomentum               << endmsg;
    debug() << " zMidField          = " << zMidField          << " mm"  << endmsg;
    debug() << " xTolerance         = " << m_xTol             << " mm"  << endmsg;
    debug() << " xTolSlope          = " << m_xTolSlope        << " mm"  << endmsg;
    debug() << " yTolerance         = " << m_yTol             << " mm"  << endmsg;
    debug() << " YTolSlope          = " << m_yTolSlope                  << endmsg;
    debug() << " DxGroupTol         = " << m_dxGroupTol       << " mm " << endmsg;
    debug() << " DxGroupFactor      = " << m_dxGroupFactor    << "    " << endmsg;
    debug() << " zMidTT             = " << m_zMidTT           << " mm"  << endmsg;
    debug() << " passUnmatched      = " << m_passUnmatched    << "   "  << endmsg;
  }

  if(m_PrintVariables)
  {
    info() << " MaxXSize           = " << m_maxXSize         << " mm"  << endmsg;
    info() << " MaxYSize           = " << m_maxYSize         << " mm"  << endmsg;
    info() << " MaxXSlope          = " << m_maxXSlope                  << endmsg;
    info() << " MaxYSlope          = " << m_maxYSlope                  << endmsg;
    info() << " centralHoleSize    = " << m_centralHoleSize  << " mm"  << endmsg;
    info() << " minMomentum        = " << m_minMomentum      << " MeV" << endmsg;
    info() << " minPT              = " << m_minPT            << " MeV" << endmsg;
    info() << " maxPseudoChi2      = " << m_maxPseudoChi2    << "   "  << endmsg;
    info() << " distToMomentum     = " << distToMomentum               << endmsg;
    info() << " zMidField          = " << zMidField          << " mm"  << endmsg;
    info() << " xTolerance         = " << m_xTol             << " mm"  << endmsg;
    info() << " xTolSlope          = " << m_xTolSlope        << " mm"  << endmsg;
    info() << " yTolerance         = " << m_yTol             << " mm"  << endmsg;
    info() << " YTolSlope          = " << m_yTolSlope                  << endmsg;
    info() << " DxGroupTol         = " << m_dxGroupTol       << " mm " << endmsg;
    info() << " DxGroupFactor      = " << m_dxGroupFactor    << "    " << endmsg;
    info() << " zMidTT             = " << m_zMidTT           << " mm"  << endmsg;
    info() << " passUnmatched      = " << m_passUnmatched    << "   "  << endmsg;
  }

  std::vector<float> nfact;
  for (double dydz = -0.3; dydz < 0.3; dydz+=0.02) {
    double dxdz = 0.0;
    m_PatTTMagnetTool->dxNormFactorsTT(dydz, nfact);
    double dist2mom = m_PatTTMagnetTool->dist2mom(dydz);

    if( m_debug )
      debug() << format(" val dist2mom %10.4e # %7.2f %7.2f  ratios: %7.3f %7.3f %7.3f %7.3f",
                        dist2mom, dxdz, dydz, nfact[0],nfact[1],nfact[2],nfact[3]) << endmsg;
  }


  m_ttHitManager   = tool<Tf::TTStationHitManager <PatTTHit> >("PatTTStationHitManager");

  return StatusCode::SUCCESS;
}


//=========================================================================
// Main reconstruction method
//=========================================================================
StatusCode 
PatVeloTTTool::tracksFromTrack(const LHCb::Track & velotrack, std::vector<LHCb::Track*>& outtracks )
{

  std::vector<PatVTTTrack> vttTracks;
  getCandidates(velotrack, vttTracks);
  
  simpleFitTracks(vttTracks);
  localCleanUp(vttTracks);
  selectBestTracks(vttTracks);
  prepareOutputTracks(vttTracks, outtracks);
  // check if there any candidates within TT acceptance have been found if not --> pass on input track
  if(m_passUnmatched && outtracks.size()<1){
    outtracks.push_back(new LHCb::Track(velotrack));
  }
  return StatusCode::SUCCESS;

}


//=========================================================================
// Get all the VeloTT track candidates
//=========================================================================
void PatVeloTTTool::getCandidates( const LHCb::Track& veloTrack, std::vector<PatVTTTrack>& vttTracks){

  if(m_debug) debug() << "Entering getCandidates" << endmsg;

  
  //===========================================================================
  //== try to match
  //===========================================================================

  const LHCb::Track* veloTr = &veloTrack;

  if( m_debug ) debug() << "Input Velo track address: " << veloTr << endmsg;

  const LHCb::State& state = *(veloTr->stateAt(LHCb::State::EndVelo));

  double slX = state.tx();
  double slY = state.ty();

  // skip tracks outside TT
  if( m_maxXSlope < fabs( slX ) || m_maxYSlope < fabs( slY ) ) return;

  double xAtMidTT = state.x() + slX*(m_zMidTT-state.z());
  double yAtMidTT = state.y() + slY*(m_zMidTT-state.z());

  // skip tracks pointing into central hole of TT
  if(fabs(xAtMidTT) < m_centralHoleSize && fabs(yAtMidTT) < m_centralHoleSize) return;
  
  double theta = sqrt(slX*slX+slY*slY);
  // protect against unphysical angles, should not happen
  if(theta < 0.002) theta = 0.002;

  double distToMomentum = m_PatTTMagnetTool->averageDist2mom();

  double minP = ((m_minPT/theta)>m_minMomentum) ? (m_minPT/theta):m_minMomentum;
  
  double maxTol = fabs(1. / ( distToMomentum * minP ));

  if(m_debug) debug() << " maxWindow: " << maxTol << endmsg;

  if( m_debug )   debug() << " Start with the track" << " tx,ty  " << slX << " " << slY  << endmsg;


  // The candidate based on the Velo track
  PatVTTTrack cand( veloTr );

  std::vector<float> normFact;
  m_PatTTMagnetTool->dxNormFactorsTT( slY,  normFact);


  //--------------------------------------------------------------------------
  // Loop on regions
  //--------------------------------------------------------------------------

  for (Tf::TTStationID sta=0; sta<m_ttHitManager->maxStations(); sta++){
    for (Tf::TTLayerID lay=0; lay<m_ttHitManager->maxLayers(); lay++){
      for (Tf::TTRegionID reg=0; reg<m_ttHitManager->maxRegions(); reg++){
        if( 0 == m_ttHitManager->hits(sta,lay,reg).size()) continue;



        const Tf::ITTHitCreator::STRegion* regionB = m_ttHitManager->region(sta,lay,reg);

        double zTTReg = regionB->z();
        double x = cand.xAtZ(zTTReg);
        double y = cand.yAtZ(zTTReg);

        // yTol has a term depending on the momentum
        // ---> add a term proportional to the maxTol for dx and the refine it when dx will be known

        double xTol = maxTol;
        double yTol = m_yTol + m_yTolSlope * maxTol;
        if(regionB->xmax() < x - xTol ||
           regionB->xmin() > x + xTol) continue;
        if(regionB->ymax() < y - yTol ||
           regionB->ymin() > y + yTol) continue;


        double tol = maxTol;

        //--------------------------------------------------------------------------
        // Loop on hits
        //--------------------------------------------------------------------------
        Tf::TTStationHitManager<PatTTHit>::HitRange range = m_ttHitManager->hits(sta, lay, reg);

       for ( PatTTHits::const_iterator itH = range.begin();
              range.end() != itH; ++itH ) {


	 if ((*itH)->hit()->ignore())
	   continue;

          double xOnTrack = cand.xAtZ( (*itH)->z() );
          double yOnTrack = cand.yAtZ( (*itH)->z() );
          double yAt0 = cand.yAtZ( 0 );

          double dyDz = cand.slopeY();


          updateTTHitForTrack((*itH),yAt0, dyDz);
          double dx = (*itH)->x() - xOnTrack;



          // Scale to the reference reg

          dx = dx * normFact[(*itH)->planeCode()];
          double fabsdx = fabs(dx);

	  if(tol > fabsdx){

            // Now refine the tolerance in Y
            if( yOnTrack + (m_yTol + m_yTolSlope * fabsdx) < (*itH)->hit()->yMin() ||
                yOnTrack - (m_yTol + m_yTolSlope * fabsdx) > (*itH)->hit()->yMax() ) continue;

            cand.storeHit( dx, *itH );

          }
        } // over hits

      } // over regs itL
    }
  }

  // Important to sort here: the bestLists assumes hits are sorted
  cand.sortHits();

  if(m_debug) cand.printLists(debug());

  // Numbering warning : layer 0 is the 1st layer

  // The choice of clusters for this Velo track: accept several solutions
  std::vector<PatTTHits> theSolutions;
  theSolutions.reserve(10); // reserve in case of many solutions for this Velo track

  // Try with 3 or 4 clusters in at least 3 different layers
  cand.bestLists(m_dxGroupTol, m_dxGroupFactor, theSolutions, msgSvc(), name(), m_debug);
  if(m_debug){
    debug() << "This Velo track has " << theSolutions.size()
            << " possible solution(s) with 3 or 4 layers fired before clean-up" << endmsg;
  }



  // If there is only one candidate for this Velo track: these solution won't be cleaned-up
  int nSolutions = theSolutions.size();
  bool unique = (nSolutions == 1);

  // Create the candidates with corresponding best clusters
  std::vector<PatTTHits>::const_iterator itheSolutions;
  for(itheSolutions = theSolutions.begin(); itheSolutions != theSolutions.end(); ++itheSolutions){

    PatVTTTrack candidate( veloTr );

    PatTTHits theClusters = *itheSolutions;

    if(m_debug){
      debug() << " the solution " << itheSolutions - theSolutions.begin()
              << " has " << (*itheSolutions).size() << " clusters : ";
    }

    // Attach clusters to candidate, compute mean dx ...
    saveCandidate(theClusters, candidate);

    // If there is only one candidate for this Velo track
    candidate.setUnique(unique);

    // Add this solution
    vttTracks.push_back(candidate);

  } // loop over the solutions for this Velo track

  if(m_debug) debug() << "Leaving getCandidates" << endmsg;

}


//=========================================================================
// Attach clusters to candidate, compute mean dx ...
//=========================================================================
void PatVeloTTTool::saveCandidate(PatTTHits& theClusters,PatVTTTrack& candidate){

  // Attach best clusters to the candidate, recompute the scaled dx for each cluster and the mean dx
  int nClusters = 0;
  double distTot = 0.;
  double distTotSquare = 0.;
  int maskFiredLayers = 0;

  double distToMomentum = m_PatTTMagnetTool->averageDist2mom();

  std::vector<float> normFact;
  double dyDz = candidate.slopeY();
  m_PatTTMagnetTool->dxNormFactorsTT( dyDz,  normFact);

  PatTTHits::const_iterator itClus;
  for(itClus = theClusters.begin(); itClus != theClusters.end(); ++itClus){

    PatTTHit* cluster = *itClus;

    double xOnTrack = candidate.xAtZ( (*itClus)->z() );
    double yAt0 = candidate.yAtZ(0);
    dyDz = candidate.slopeY();
    updateTTHitForTrack((*itClus),yAt0, dyDz);
    double dx = (*itClus)->x() - xOnTrack;

    // Scale to the reference reg using exact factors
    dx = dx * normFact[(*itClus)->planeCode()];

    if(m_debug) debug() << format("%7.2f(%1d) ", dx, cluster->planeCode()) << " " << cluster;

    // Add the cluster
    candidate.storeHit( dx, cluster);

    distTot += dx;
    distTotSquare += (dx*dx);
    nClusters++;

    // what layers were fired
    int mask = 1 << cluster->planeCode();
    maskFiredLayers |= mask;

  } // for this solution clusters

  // Store these clusters to candidate
  candidate.storeClusters();

  double dist = distTot/double(nClusters);
  candidate.setDx(dist);
  double distSquare = distTotSquare/double(nClusters);
  double VarDist = (distSquare - (dist*dist));
  candidate.setDxVar(VarDist);

  double qop = distToMomentum * dist ;
  candidate.setQOverP(qop);

  if(m_debug) debug() << " -> Dx: " << candidate.Dx() << " , DxVar: " << candidate.DxVar() << endmsg;

  // set what layers were fired
  candidate.setTTLayersFiredMask(maskFiredLayers);
  // set how many were fired
  candidate.setNTTLayersFired(maskFiredLayers);
}


//=========================================================================
// Simple fit of vtt candidates
//=========================================================================
void PatVeloTTTool::simpleFitTracks(std::vector<PatVTTTrack>& vttTracks) {

  std::vector<PatVTTTrack>::iterator ivttTrB;
  for(ivttTrB = vttTracks.begin(); ivttTrB != vttTracks.end(); ++ivttTrB){

    if(ivttTrB->badCandidate()) continue;

    simpleFit(*ivttTrB);

  }

}

//=========================================================================
// A kind of global track fit in VELO and TT
// The pseudo chi2 consists of two contributions:
//  - chi2 of Velo track x slope
//  - chi2 of a line in TT
// The two track segments go via the same (x,y) point
// at z corresponding to the half Bdl of the track
//
// Only q/p and chi2 of outTr are modified
//
//=========================================================================
void PatVeloTTTool::simpleFit(PatVTTTrack& vtttr) {

  PatTTHits theHits = vtttr.clusters();
  int nHits = theHits.size();
  if(3>nHits) {
    return;
  }

  const LHCb::Track* velotr = vtttr.track();

  // Get End Velo state
  const LHCb::State& pStateVelo = *(velotr->stateAt(LHCb::State::EndVelo));
  // Velo track parameters
  double xVelo      = pStateVelo.x();
  double yVelo      = pStateVelo.y();
  double zVelo      = pStateVelo.z();
  double xSlopeVelo = pStateVelo.tx();
  double ySlopeVelo = pStateVelo.ty();

  // starting TT track parameters
  double zTT      = m_zMidTT;

  // get bdl and and z of half bdl for the track
  double zOrigin =  zVelo-yVelo/ySlopeVelo;
  double bdl      = m_PatTTMagnetTool->bdlIntegral(ySlopeVelo,zOrigin,zVelo);
  double zmid     = m_PatTTMagnetTool->zBdlMiddle(ySlopeVelo,zOrigin,zVelo);

  double c11,c12,c13,c21,c22,c23;

  c11 = 0.;
  c12 = 0.;
  c13 = 0.;
  c21 = 0.;
  c22 = 0.;
  c23 = 0.;

  double z0 = zTT;

  for ( PatTTHits::const_iterator itH = theHits.begin();
        theHits.end() != itH; itH++ ) {

    PatTTHit* pdigi = (*itH);

    double y0 = yVelo -  ySlopeVelo * zVelo;
    updateTTHitForTrack(pdigi, y0, ySlopeVelo);

    double ui = pdigi->x();

    double ci = pdigi->hit()->cosT();
    double dz = pdigi->z() - z0;
    double wi = pdigi->hit()->weight();

    c11 += wi * ci;
    c12 += wi * ci * dz;
    c13 += wi * ui;
    c22 += wi * ci * dz * dz;
    c23 += wi * ui * dz;
  }
  // add chi2 from VELO slope

  double xmid = xVelo + xSlopeVelo*(zmid-zVelo);
  // fixed velo slope error. Momentum dependent error + iteration is unstable
  double sigmaVeloSlope = 0.10*Gaudi::Units::mrad;
  double wb = sigmaVeloSlope*(zmid - zVelo);
  wb=1./(wb*wb);
  c11 += wb;
  c12 += wb*(zmid-z0);
  c13 += wb*xmid;
  c22 += wb*(zmid-z0)*(zmid-z0);
  c23 += wb*xmid*(zmid-z0);

  c21 = c12;

  double xTTFit, xSlopeTTFit;  // x pos and slope of track at TT after fit
  double den=c11*c22-c21*c12;
  if(fabs(den)>1.0e-8) {
    xTTFit      = (c13*c22-c23*c12)/(c11*c22-c21*c12);
    xSlopeTTFit = (c13*c21-c23*c11)/(c12*c21-c22*c11);
  }
  else  {
    xTTFit = 0.;
    xSlopeTTFit = 0.;
    vtttr.setChi2PerDoF(1.0e19);
    return;
  }

  // new VELO slope x
  double xb = xTTFit+xSlopeTTFit*(zmid-zTT);
  double xSlopeVeloFit = (xb-xVelo)/(zmid-zVelo);

  double chi2VeloSlope = (xSlopeVelo - xSlopeVeloFit)/sigmaVeloSlope;
  chi2VeloSlope = chi2VeloSlope * chi2VeloSlope;

  // calculate final chi2
  double chi2TT = 0.;

  if(m_debug) debug() << " Velo-TT track refit. Points:" << endmsg;

  for ( PatTTHits::const_iterator itH = theHits.begin();
        theHits.end() != itH; itH++ ) {

    PatTTHit* pdigi = (*itH);

    double zd    = pdigi->z();
    double xd    = xTTFit + xSlopeTTFit*(zd-zTT);
    double du    = xd - pdigi->x();
    double chi2p = (du*du)*pdigi->hit()->weight();
    chi2TT += chi2p;

    // debug only
    if(m_debug){
      debug()  << format( "  hit at z %7.2f dist %7.3f err %7.3f chi2 %7.3f",
                          zd, du, sqrt(1./pdigi->hit()->weight()),chi2p )
               << endmsg;
    }
  }
  double chi2Global  = chi2TT + chi2VeloSlope;
  chi2Global /= double((nHits+1-2));

  // calculate q/p
  double sinInX  = xSlopeVeloFit/sqrt(1.+xSlopeVeloFit*xSlopeVeloFit);
  double sinOutX = xSlopeTTFit/sqrt(1.+xSlopeTTFit*xSlopeTTFit);

  double qp=0.;
  if(fabs(bdl)>1.e-8) {
    double qpxz2p=1./sqrt(1.+ySlopeVelo*ySlopeVelo);
    qp=-qpxz2p*(sinInX-sinOutX)/bdl*3.3356/Gaudi::Units::GeV;
  }

  // Next block commented MC 2011-11-22, these variables are not used anywhere
  //  double bdlmom=0.;
  //  if(fabs(sinInX-sinOutX)>1.e-8) {
  //    double pxz2p=sqrt(1.+ySlopeVelo*ySlopeVelo);
  //    double pxz=-bdl/(3.3356*(sinInX-sinOutX));
  //    bdlmom = pxz * pxz2p * Gaudi::Units::GeV;
  //  }

  if(m_debug) {
    double mom_orig =0.;
    if(vtttr.qOverP() != 0.0 ) mom_orig = 1./vtttr.qOverP();
    double bdlmom2=0.;
    if(fabs(qp)>1.e-8) bdlmom2 = 1/qp;
    debug() << " Original / Refit momenta  :" << mom_orig << " " << bdlmom2 << endmsg;
  }

  // update track state (qOverP and chiSquared only)

  vtttr.setQOverP(qp);

  vtttr.setChi2PerDoF(chi2Global);

  return;

}


//=========================================================================
// Apply local clean-up for all track candidates based on the same Velo tracks
//=========================================================================
void PatVeloTTTool::localCleanUp(std::vector<PatVTTTrack>& vttTracks){

  if(m_debug) debug() << "Entering localCleanUp" << endmsg;

  std::vector<PatVTTTrack>::iterator ivttTrB;
  for(ivttTrB = vttTracks.begin(); ivttTrB != vttTracks.end(); ++ivttTrB){

    if(ivttTrB->badCandidate()) continue;
    if(ivttTrB->nTTLayersFired() == 0) continue; // Ignore candidates with no clusters
    if(ivttTrB->unique()) continue; // Ignore if only one candidate for a Velo track

    std::vector<PatVTTTrack>::iterator ivttTrE;
    for(ivttTrE = ivttTrB+1; ivttTrE != vttTracks.end(); ++ivttTrE){

      if(ivttTrE->badCandidate()) continue;
      if(ivttTrE->nTTLayersFired() == 0) continue; // Ignore candidates with no clusters
      if(ivttTrE->unique()) continue; // Ignore if only one candidate for a Velo track

      // The same Velo track: local clean-up!
      if(ivttTrB->track() != ivttTrE->track()) continue;

      if(m_debug){
        debug() << " Same Velo track candidate B has " << ivttTrB->nTTLayersFired() << " layers fired with Dx = "
                << ivttTrB->Dx() << " , DxVar = " << ivttTrB->DxVar()
                << endmsg;

        debug() << " Same Velo track candidate E has " << ivttTrE->nTTLayersFired() << " layers fired with Dx = "
                << ivttTrE->Dx() << " , DxVar = " << ivttTrE->DxVar()
                << endmsg;
      }

      // If more layers for B
      if(ivttTrB->nTTLayersFired() > ivttTrE->nTTLayersFired()){
        if(m_debug) debug() << "    keep candidate B, since more compatible clusters" << endmsg;
        ivttTrE->setBadCandidate(true);
      } // if B has more layers fired

      // Else if more layers for E
      else if(ivttTrB->nTTLayersFired() < ivttTrE->nTTLayersFired()){
        if(m_debug) debug() << "    keep candidate E, since more compatible clusters" << endmsg;
        ivttTrB->setBadCandidate(true);
      } // if E has more layers fired

      // Else same number of layers fired
      else{
        // take the one with smaller chi2.
        if(ivttTrB->chi2PerDoF() > ivttTrE->chi2PerDoF() ) {
          ivttTrB->setBadCandidate(true);
        } else {
          ivttTrE->setBadCandidate(true);
        }

      }

      // Go to next if ivttTrB is a bad candidate
      if(ivttTrB->badCandidate()) break;

    } // ivttTrE

  } // ivttTrB

  // Check what we have left, only in debug mode
  if(m_debug){
    int nVeloTTBeforeLocalCleanUp = 0;
    int nVeloTTAfterLocalCleanUp = 0;

    for(ivttTrB = vttTracks.begin(); ivttTrB != vttTracks.end(); ++ivttTrB){
      nVeloTTBeforeLocalCleanUp++;
      if(ivttTrB->badCandidate()) continue;
      nVeloTTAfterLocalCleanUp++;
    }
    debug() << "Number of VeloTT candidates before: " << nVeloTTBeforeLocalCleanUp
            << " and after local clean-up: "<<  nVeloTTAfterLocalCleanUp << endmsg;

    debug() << "Leaving localCleanUp" << endmsg;
  }
}

//=========================================================================
// Remove worse pseudo chi2 vtt candidates
//=========================================================================
void PatVeloTTTool::selectBestTracks( std::vector<PatVTTTrack>& vttTracks) {

  // keep only best m_maxSolutionsPerTrack i.e. smallest chi2

  std::vector<PatVTTTrack>::iterator ivttTrB;

  // apply psudoChi2 cut

  for(ivttTrB = vttTracks.begin(); ivttTrB != vttTracks.end(); ++ivttTrB){
    if( ivttTrB->chi2PerDoF() > m_maxPseudoChi2 ) {
      ivttTrB->setBadCandidate(true);
    }

  }

  for(ivttTrB = vttTracks.begin(); ivttTrB != vttTracks.end(); ++ivttTrB){

    if(ivttTrB->badCandidate()) continue;
    if(ivttTrB->unique()) continue; // Ignore if only one candidate for a Velo track

    std::vector<PatVTTTrack*> pvtt;
    pvtt.push_back(&(*ivttTrB));
    std::vector<PatVTTTrack>::iterator ivttTrE;
    for(ivttTrE = ivttTrB+1; ivttTrE != vttTracks.end(); ++ivttTrE){

      if(ivttTrE->badCandidate()) continue;
      if(ivttTrE->unique()) continue; // Ignore if only one candidate for a Velo track

      if(ivttTrB->track() != ivttTrE->track()) continue;
      pvtt.push_back( &(*ivttTrE));
    }


    if((int)pvtt.size() > m_maxSolutionsPerTrack) {
      std::vector<PatVTTTrack*>::iterator iv;

      std::sort(pvtt.begin(),pvtt.end(),compPseudoChi2());

      if(m_debug) {
        debug() << pvtt.size() << " sorted vtt, chi2 = : ";
        for (iv=pvtt.begin(); iv != pvtt.end() ; iv++) {
          debug() << (*iv)->chi2PerDoF() << " " ;
        }
        debug() << endmsg;
      }

      for (iv=pvtt.begin()+m_maxSolutionsPerTrack; iv != pvtt.end() ; iv++) {
        (*iv)->setBadCandidate(true);
        if(m_debug) debug() << (*iv)->chi2PerDoF() << " bad chi2" << endmsg;
      }


    }

  }
}

//=========================================================================
// Create the Velo-TT tracks
//=========================================================================
void PatVeloTTTool::prepareOutputTracks( std::vector<PatVTTTrack>& vttTracks,
                                         std::vector<LHCb::Track*>& outtracks)
{

  if(m_debug) debug() << "Entering prepareOutputTracks" << endmsg;

  double zMidField = m_PatTTMagnetTool->zMidField();

  std::vector<PatVTTTrack>::iterator ivttTrB;
  for(ivttTrB = vttTracks.begin(); ivttTrB != vttTracks.end(); ++ivttTrB){

    PatVTTTrack cand = *ivttTrB;

    bool ok = cand.badCandidate();
    if(m_debug) debug() << " good candidate? " << (!ok) << endmsg;
    if(cand.badCandidate()) continue;

    PatTTHits candClusters = cand.clusters();
    if(m_debug) debug() << " with n clusters " << candClusters.size() << endmsg;
    if (candClusters.size() == 0) continue;

    if(m_debug){
      debug() << "Creating Velo-TT candidate with "
              << candClusters.size() << " TT clusters with dx: " << cand.Dx() << endmsg;
    }

    const LHCb::Track* veloTr = cand.track();
    const LHCb::State& state = *(veloTr->stateAt(LHCb::State::EndVelo));
    double tx = state.tx();
    double ty = state.ty();
    double dist = cand.Dx();

    //== Handle states. copy Velo one, add TT.

    double qop = cand.qOverP();
    if(m_debug) debug() << " output track Q/P = " << qop << " p " << 1/qop << endmsg;

    LHCb::Track* outTr = new LHCb::Track();
    // reset the track
    outTr->reset();

    outTr->copy(*veloTr);
    // set q/p in all of the existing states
    const std::vector< LHCb::State * > states = outTr->states();
    std::vector< LHCb::State * >::const_iterator iState;
    for ( iState = states.begin() ; iState != states.end() ; ++iState ){
      (*iState)->setQOverP(qop);
    }
    //== Add a new state...
    LHCb::State temp;
    temp.setLocation( LHCb::State::AtTT );
    temp.setState( cand.xAtZ( m_zMidTT ) + dist,
                   cand.yAtZ( m_zMidTT ),
                   m_zMidTT,
                   tx + dist/(m_zMidTT - zMidField),
                   ty,
                   qop );


    outTr->addToStates( temp );

    if(m_debug) debug() << " added State " << temp.stateVector()
                        << " cov \n" << temp.covariance() << endmsg;

    for( PatTTHits::const_iterator iClusB = candClusters.begin();
         iClusB != candClusters.end(); ++iClusB){
      (*iClusB)->hit()->setUsed( true );

      outTr->addToLhcbIDs( (*iClusB)->hit()->lhcbID() );

      if(m_debug){
        debug() << " adding LHCb ID " << format( "%8x", (*iClusB)->hit()->lhcbID().lhcbID() ) << endmsg;
      }

    } // over PatTTCoords

    outTr->setType( LHCb::Track::Upstream );
    outTr->setHistory( LHCb::Track::PatVeloTT );
    outTr->addToAncestors( veloTr );
    outTr->setPatRecStatus( LHCb::Track::PatRecIDs );
    outTr->setChi2PerDoF(cand.chi2PerDoF());

    outtracks.push_back(outTr);

  } // for ivttTr

  if(m_debug) debug() << "Leaving prepareOutputTracks" << endmsg;
}




//=============================================================================
