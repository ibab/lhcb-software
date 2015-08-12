// Include files
#include <limits>
#include <vector>
#include <map>
#include <algorithm>


#include "Event/StateParameters.h"

#include "TrackInterfaces/IAddTTClusterTool.h"
#include "TrackInterfaces/ITrackMomentumEstimate.h"

// local
#include "PatMatchTool.h"

//-----------------------------------------------------------------------------
// Implementation file for class : PatMatchTool
//
// 2008-04-25 : Johannes Albrecht
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY(PatMatchTool)

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
PatMatchTool::PatMatchTool(const std::string& type,
                           const std::string& name, const IInterface* parent) :
GaudiTool(type, name , parent),
  m_fastMomentumTool(0), m_addTTClusterTool(0)
{
  declareInterface<IMatchTool>(this);
  declareInterface<ITrackMatch>(this);
  
  // -- The main cut value
  declareProperty("maxMatchChi2"        , m_maxChi2              = 4.8);
  // -- Parameters for z position of matching in x-coordinate
  declareProperty("zMagnet"             , m_zMagnet              = 5197.83 * Gaudi::Units::mm);
  declareProperty("zMagnetTx2"          , m_zMagnetTx2           = -1372.63 * Gaudi::Units::mm);
  declareProperty("zMagnetDsl"          , m_zMagnetDsl           = 13.5453     );
  declareProperty("zMagnetDsl2"         , m_zMagnetDsl2          = 297.853 * Gaudi::Units::mm);
  declareProperty("zMagnetSeedX"        , m_zMagnetSeedX         = 0.0251294 );
  // -- Parameters for matching in y-coordinate
  declareProperty("zMatchY"             , m_zMatchY              = 8420. * Gaudi::Units::mm);
  declareProperty("MagnetBendDsl"       , m_magnetBendDsl        = -646.047);
  declareProperty("MagnetBendDslY"      , m_magnetBendDslY       = -44118.5);
  // -- Tolerances
  declareProperty("dxTol"               , m_dxTol                =    8. * Gaudi::Units::mm);
  declareProperty("dxTolSlope"          , m_dxTolSlope           =   60. * Gaudi::Units::mm);
  declareProperty("dyTol"               , m_dyTol                =    6. * Gaudi::Units::mm);
  declareProperty("dyTolSlope"          , m_dyTolSlope           =  300. * Gaudi::Units::mm);
  
  declareProperty("MinPt"               , m_minPt                =  0. * Gaudi::Units::MeV );
  declareProperty("MinMomentum"         , m_minMomentum          =  0. * Gaudi::Units::GeV );
  declareProperty("FastMomentumToolName", m_fastMomentumToolName = "FastMomentumEstimate");
  declareProperty("AddTTClusters"       , m_addTT                = true);
  declareProperty("AddTTClusterName"    , m_addTTClusterName     = "PatAddTTCoord");
  declareProperty("writeNNVariables"    , m_writeNNVariables     = true);
  // -- Use the Velo and Seed tracks in more than one match candidate?
  declareProperty("RejectVeloUsed"      , m_rejectVeloUsed       = true);
  declareProperty("RejectSeedUsed"      , m_rejectSeedUsed       = true);
  // -- Skip Velo tracks if used before
  declareProperty("VeloVetoTracksName", m_veloVetoTracksNames = {});
  
}
//=============================================================================
// Destructor
//=============================================================================
PatMatchTool::~PatMatchTool() {}

//=============================================================================

//=============================================================================
// Initialization
//=============================================================================
StatusCode PatMatchTool::initialize()
{
  StatusCode sc = GaudiTool::initialize();
  if (sc.isFailure()) return sc;    // error printed already by GaudiAlgorithm

  if (UNLIKELY(msgLevel(MSG::DEBUG)))
    debug() << "==> Initialize" << endmsg;

  if ( m_addTT) m_addTTClusterTool = tool<IAddTTClusterTool>(m_addTTClusterName);

  m_fastMomentumTool = tool<ITrackMomentumEstimate>(m_fastMomentumToolName);

  return StatusCode::SUCCESS;
}
//=============================================================================
// Match single combination only
//=============================================================================
StatusCode PatMatchTool::matchSingle(const LHCb::Track& velo,
                                     const LHCb::Track& seed , LHCb::Track& output, double& chi2)
{
  // calculate quality of match between tracks
  chi2 = getChi2Match(velo, seed);

  if (m_maxChi2 > chi2) {
    // set states and flags of output track
    makeTrack(velo, seed, output, chi2);
    if (m_addTT) m_addTTClusterTool->addTTClusters(output);
    return StatusCode::SUCCESS;
  }

  return StatusCode::FAILURE;
}

//=============================================================================
// Check if Velo or T-Seed should be processed.
//=============================================================================
bool PatMatchTool::acceptTrack(const LHCb::Track& track) const
{
  if (track.checkFlag( LHCb::Track::Invalid) ) return false;
  if (track.type()==LHCb::Track::Velo){
    if (track.checkFlag( LHCb::Track::Backward) ) return false;
    for (auto names : m_veloVetoTracksNames ){
      if (exist<LHCb::Track::Range>( names )){
        for (auto longTrack : get<LHCb::Track::Range>( names ) ) { 
          auto ancestors = longTrack->ancestors();
          if (!(std::none_of( std::begin( ancestors ), std::end( ancestors ), 
                              [&](const LHCb::Track* t) { return t == &track; } )))
            return false;
        }
      }else{
        debug() << "Could not find veto tracks at: " << names << endmsg;
      }
      
    }
  }
  return true;
}


//=============================================================================
// Main execution
//=============================================================================
StatusCode PatMatchTool::match(const LHCb::Tracks& velos,
                               const LHCb::Tracks& seeds, LHCb::Tracks& matchs)
{
  // build a match-chi^2 sorted table of velo-seed matches
  std::vector<MatchCandidate> cands;
  cands.reserve(4 * std::max(velos.size(), seeds.size()));
  for( const LHCb::Track* vTr: velos) {
    if ( !acceptTrack(*vTr) ) continue;
    for( const LHCb::Track* sTr: seeds) {
      if ( !acceptTrack(*sTr) ) continue;
      const double dist = getChi2Match(*vTr, *sTr);
      if (m_maxChi2 > dist) {
        cands.push_back(MatchCandidate(vTr, sTr, dist));
      }
    }
  }
  std::stable_sort(cands.begin(), cands.end(), MatchCandidate::lowerByChi2());

  // for each track, tag if used or not.
  std::map<const LHCb::Track*,bool> used;
  // convert unused match candidates to tracks
  for(const MatchCandidate& cand: cands) {
    const LHCb::Track* vTr = cand.vTr();
    const LHCb::Track* sTr = cand.sTr();

    if (UNLIKELY(msgLevel(MSG::DEBUG))) {
      debug() << "Candidate Velo " << vTr->key() << " used " << used[ vTr ]
              << " Seed " << sTr->key() << " used " << used[ sTr ]
              << " dist " << cand.dist() << endmsg;
    }

    bool& vUsed = used[vTr];
    if( m_rejectVeloUsed ){
      if (vUsed) continue;
    }
    
    bool& sUsed = used[sTr];
    if( m_rejectSeedUsed ){
      if (sUsed) continue;
    }

    //== estimate q/p
    double qOverP, sigmaQOverP;
    bool const cubicFit = sTr->checkHistory(LHCb::Track::PatSeeding);
    const LHCb::State& vState = vTr->closestState(0.);
    const LHCb::State& sState =sTr->closestState(m_zMatchY);
    StatusCode sc = m_fastMomentumTool->calculate(&vState,&sState,
                                                  qOverP, sigmaQOverP, cubicFit);

    if (sc.isFailure()) {
      Warning("momentum determination failed!",sc).ignore();
      // assume the Velo/T station standalone reco do something reasonable
    } else {
      // adjust q/p and its uncertainty
      const double momentum = std::abs(1./qOverP);
      const double tx2 = vState.tx()*vState.tx();
      const double ty2 = vState.ty()*vState.ty();
      const double sinTrack = sqrt( 1. - 1./(1.+tx2 + ty2) );
      const double pt = sinTrack*momentum;
      if (momentum < m_minMomentum ) continue;
      if (pt < m_minPt ) continue;
    } 

    vUsed = sUsed = true;

    
    LHCb::Track* match = new LHCb::Track();
    matchs.insert(match);

    makeTrack(*vTr, *sTr, *match, cand.dist());

    if (m_addTT) {
      StatusCode sc = m_addTTClusterTool->addTTClusters(*match);
      if (sc.isFailure())
        Warning("adding TT clusters failed!",sc).ignore();
    }

    // added for NNTools -- check how many tracks have common hits
    // This is always 1.0 as we require that velo and seed tracks are used only once.
    if (m_writeNNVariables) match->addInfo(LHCb::Track::NCandCommonHits, 1.0);
  }//end loop match cands

  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode PatMatchTool::finalize()
{
  if (UNLIKELY(msgLevel(MSG::DEBUG))) debug() << "==> Finalize" << endmsg;

  return GaudiTool::finalize();
}

//=============================================================================
//  Calculate the matching chi2
//=============================================================================
double PatMatchTool::getChi2Match(const LHCb::Track& velo,
                                  const LHCb::Track& seed) const
{

  double chi2 = std::numeric_limits<double>::max();
  if (velo.checkFlag(LHCb::Track::Invalid)) return chi2;
  if (velo.checkFlag(LHCb::Track::Backward)) return chi2;
  if (seed.checkFlag(LHCb::Track::Invalid)) return chi2;

  const LHCb::State& vState = velo.closestState(0.);
  const LHCb::State& sState = seed.closestState(m_zMatchY);

  const double dSlope = vState.tx() - sState.tx();
  const double dSlopeY = vState.ty() - sState.ty();
  
  // -- This is the function that calculates the z position to match the x-coordinates
  // -- The parametrisation tries to take all (relevant) dependencies into account
  // -- and can be derived with the MatchFitParams package
  const double zForX = m_zMagnet + m_zMagnetDsl*std::abs(dSlope) + 
    m_zMagnetDsl2*dSlope*dSlope + m_zMagnetSeedX*std::abs(sState.x()) + 
    m_zMagnetTx2*vState.tx()*vState.tx();

  const double dxTol2 = m_dxTol * m_dxTol;
  const double dxTolSlope2 = m_dxTolSlope * m_dxTolSlope;

  const double xV = vState.x() + (zForX     - vState.z()) * vState.tx();
  
  // -- This is the function that calculates the 'bending' in y-direction
  // -- The parametrisation can be derived with the MatchFitParams package
  const double yV = vState.y() + (m_zMatchY - vState.z()) * vState.ty() +
    vState.ty() * ( m_magnetBendDsl*dSlope*dSlope + m_magnetBendDslY*dSlopeY*dSlopeY );
  
  const double xS = sState.x() + (zForX     - sState.z()) * sState.tx();
  const double yS = sState.y() + (m_zMatchY - sState.z()) * sState.ty();

  const double distX = xS - xV;
  const double distY = yS - yV;
  
  // -- These are sanity cuts, such that unreasonable chi2 values 
  // -- are not calculated
  if( std::abs( distX ) > 50.0 ) return chi2;
  if( std::abs( distY ) > 130.0 ) return chi2;


  const double dslx  = vState.tx() - sState.tx();
  const double tolX = dxTol2 + dslx * dslx *  dxTolSlope2;
  const double teta2 = vState.tx() * vState.tx() + vState.ty() * vState.ty();
  const double tolY  = m_dyTol * m_dyTol + teta2 * m_dyTolSlope * m_dyTolSlope;

  chi2 = distX * distX / tolX + distY * distY /tolY;

  const double dSlY = sState.ty() - vState.ty();
  chi2 += dSlY * dSlY / sState.errTy2() / 16.;

  return chi2;
}

//=============================================================================
//  Make LHCb track out of internal class
//=============================================================================
void PatMatchTool::makeTrack(const LHCb::Track& velo,
                             const LHCb::Track& seed, LHCb::Track& output, double chi2) const
{
  //== add ancestors
  output.addToAncestors(velo);
  output.addToAncestors(seed);
  //== Adjust flags
  output.setType(LHCb::Track::Long);
  output.setHistory(LHCb::Track::PatMatch);
  output.setPatRecStatus(LHCb::Track::PatRecIDs);
  output.addInfo(LHCb::Track::MatchChi2, chi2);
  //== copy LHCbIDs
  output.addSortedToLhcbIDs(velo.lhcbIDs());
  output.addSortedToLhcbIDs(seed.lhcbIDs());
  //== copy Velo and T states at the usual pattern reco positions
  std::vector<LHCb::State*> newstates;
  newstates.reserve(6);
  if (velo.hasStateAt(LHCb::State::ClosestToBeam))
    newstates.push_back(velo.stateAt(LHCb::State::ClosestToBeam)->clone());
  if (velo.hasStateAt(LHCb::State::FirstMeasurement))
    newstates.push_back(velo.stateAt(LHCb::State::FirstMeasurement)->clone());
  if (velo.hasStateAt(LHCb::State::EndVelo))
    newstates.push_back(velo.stateAt(LHCb::State::EndVelo)->clone());
  newstates.push_back(seed.closestState(StateParameters::ZBegT).clone());
  newstates.push_back(seed.closestState(StateParameters::ZMidT).clone());
  // make sure we don't include same state twice
  if (std::abs(newstates[newstates.size() - 2]->z() -
               newstates.back()->z()) < 300.) {
    delete newstates.back();
    newstates.pop_back();
  }
  newstates.push_back(seed.closestState(StateParameters::ZEndT).clone());
  // make sure we don't include same state twice
  if (std::abs(newstates[newstates.size() - 2]->z() -
               newstates.back()->z()) < 300.) {
    delete newstates.back();
    newstates.pop_back();
  }
  //== estimate q/p
  double qOverP, sigmaQOverP;
  bool const cubicFit = seed.checkHistory(LHCb::Track::PatSeeding);
  const LHCb::State& vState = velo.closestState(0.);
  const LHCb::State& sState =seed.closestState(m_zMatchY);
  StatusCode sc = m_fastMomentumTool->calculate(&vState,&sState,
                                                qOverP, sigmaQOverP, cubicFit);
  if (sc.isFailure()) {
    Warning("momentum determination failed!",sc).ignore();
    // assume the Velo/T station standalone reco do something reasonable
  } else {
    // adjust q/p and its uncertainty
    sigmaQOverP = sigmaQOverP * sigmaQOverP;
    for(LHCb::State* st: newstates) {
      st->covariance()(4,4) = sigmaQOverP;
      st->setQOverP(qOverP);
    }
  }
  //== add copied states to output track
  output.addToStates(newstates);
}
//=============================================================================
//  Wrapper for matching a single combination
//=============================================================================
StatusCode PatMatchTool::match(const LHCb::Track& veloTrack,
                               const LHCb::Track& tTrack,
                               LHCb::Track& matchedTrack,
                               double& quality, double& quality2)
{
  quality2=0;
  StatusCode sc = this->matchSingle(veloTrack, tTrack, matchedTrack, quality);
  if (sc.isFailure())
    if (UNLIKELY(msgLevel(MSG::DEBUG))) debug()<<"matching failed !"<<endmsg;

  return sc;
}

// vim: sw=4:tw=78:ft=cpp
