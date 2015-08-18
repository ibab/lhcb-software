// Include files
// boost
#include <boost/foreach.hpp>
// LoKi
#include "LoKi/TrackCuts.h"
#include "LoKi/VertexCuts.h"
#include "LoKi/ParticleCuts.h"
#include "LoKi/BeamLineFunctions.h"
#include "LoKi/Algs.h"

// local
#include "LLParticlesFromRecVertices.h"

using namespace LoKi;
using namespace LoKi::Cuts;

//-----------------------------------------------------------------------------
// Implementation file for class : LLParticlesFromRecVertices
//
// @author Neal Gauvin
// @author Victor Coco
// @author Pieter David
// @date   2012-03-27
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( LLParticlesFromRecVertices )

namespace {
  typedef LoKi::Constant<const LHCb::VertexBase*,double> _VDOUBLE;
  typedef LoKi::Constant<const LHCb::VertexBase*,bool>   _VBOOL;
}

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
  LLParticlesFromRecVertices::LLParticlesFromRecVertices( const std::string& name,
                                                          ISvcLocator* pSvcLocator)
    : DaVinciAlgorithm( name , pSvcLocator )
    , m_pt(400.)

    , m_p2s(NULL)

    , m_chargedProto(NULL)

    , m_RHO( _VDOUBLE(0.0) )
    , m_Z( _VDOUBLE(0.0) )
    , m_VERTEXCUT( _VBOOL(false) )
    , m_UPPVZ( LoKi::Constant<std::vector<const LHCb::RecVertex*>,double>(LoKi::Constants::HugeDistance) )

{
  // Vertex "input" location, Inputs is used for the daughters
  declareProperty("RecVertexLocations"
                  , m_RVLocations
                  , "Input RecVertex containers" );

  // PV selection
  declareProperty("RequireUpstreamPV"
                  , m_requireUpstreamPV = true
                  , "Require a PV upstream of the candidate in a fiducial volumee (MinZ,MaxZ, MaxRho, #tracks)");
  declareProperty("FirstPVMaxRho"
                  , m_FirstPVMaxRho = 0.3*Gaudi::Units::mm
                  , "Maximal rho position of the \"most upstream\" PV" );

  declareProperty("FirstPVMinZ"
                  , m_FirstPVMinZ = -150.0*Gaudi::Units::mm
                  , "Minimal z position of the \"most upstream\" PV" );

  declareProperty("FirstPVMaxZ"
                  , m_FirstPVMaxZ = 150.0*Gaudi::Units::mm
                  , "Maximal z position of the \"most upstream\" PV" );

  declareProperty("FirstPVMinNumTracks"
                  , m_FirstPVNumTracks = 10
                  , "Minimal number of tracks required in the \"most upstream\" PV" );

  // daughter track selection
  declareProperty("UseVeloTracks"
                  , m_useVeloTracks = false
                  , "Allow Velo-only daughters" );

  declareProperty("VerticesFromVeloOnly"
                  , m_verticesFromVeloOnly = true
                  , "Hlt (true) or Stripping (false)" );

  declareProperty("MaxChi2NonVeloOnly"
                  , m_chi2NonVeloOnly = 5.0
                  , "Maximum track Chi2 for daughter tracks that are not Velo-only" );

  // mother/combination cuts
  declareProperty("LLPName"
                  , m_LLPName = "~chi_10"
                  , "LLP ParticleID name" );

  declareProperty("MinMass"
                  , m_LLPMinMass = 0.0
                  , "Minimal reconstructed mass of the LLP" );

  declareProperty("MinSumPT"
                  , m_LLPMinSumPT = 0.0*Gaudi::Units::GeV
                  , "Minimal sum-PT of the LLP" );

  declareProperty("MinRho"
                  , m_LLPMinR = 0.3*Gaudi::Units::mm
                  , "Minimal LLP RHO distance to the beam line" );

  declareProperty("MinNumTracks"
                  , m_LLPMinNumTracks = 1
                  , "Minimal number of daughter tracks of the LLP" );

  declareProperty("MaxFractE1Track"
                  , m_LLPMaxFractEFromOne = 0.85
                  , "Maximal allowed fraction of the energy contributed by one daughter" );

  declareProperty("MaxFractTrwHitBefore"
                  , m_LLPMaxFractTrWithUpstream = 0.49
                  , "Maximal allowed fraction of daughters with a hit upstream of the vertex" );

  declareProperty("ComputeMatterVeto"
                  , m_computeMatterVeto = false
                  , "Add InMatterVeto flag to the created particles" );

  declareProperty("ApplyMatterVeto"
                  , m_applyMatterVeto = false
                  , "Reject candidates inside matter veto" );

  // output location for Velo protoparticles
  declareProperty("VeloProtoParticlesLocation"
                  , m_VeloProtoPLocation = "Hlt2/" + this->name() + "/VeloProtoP"
                  , "TES output location for created Velo protoparticles" );

  declareProperty("ChargedProtoParticlesLocation"
                  , m_chargedProtoLocation = LHCb::ProtoParticleLocation::Charged
                  , "Input protoparticles location" );
}

// ExtraInfo keys
int LLParticlesFromRecVertices::Rho = 52;
int LLParticlesFromRecVertices::SigmaRho = 53;
int LLParticlesFromRecVertices::SigmaZ = 54;
int LLParticlesFromRecVertices::FractDaughterTracksWithUpstreamHit = 55;
int LLParticlesFromRecVertices::MaxFractEnergyInSingleTrack = 56;
int LLParticlesFromRecVertices::InMatterVeto = 51;
int LLParticlesFromRecVertices::SumPT = 57;

//=============================================================================
// Destructor
//=============================================================================
LLParticlesFromRecVertices::~LLParticlesFromRecVertices() {}

namespace {
  /**
   * Helper function to test if a track has a hit upstream of specified vertex
   */
  bool TrHasHitBeforeVertex( const LHCb::Track* tr, const LHCb::VertexBase* vx, DeVelo* velo )
  {
    double minZ = LoKi::Constants::HugeDistance;
    double hitZ;
    BOOST_FOREACH( LHCb::LHCbID id, tr->lhcbIDs() )
    {
      if ( id.isVelo() ) {
        hitZ = velo->sensor( id.veloID() )->z();
        //std::cout << "Hit at z=" << hitZ << std::endl;
        if ( hitZ < minZ ) { minZ = hitZ; }
      }
    }
    //std::cout << "Lowest-z hit: " << minZ << std::endl;
    // if first hit is at 6 sigma before the vertex count it
    return ( ( ( minZ - vx->position().z() ) / vx->covMatrix()(2,2) ) < -5. );
  }

  /**
   * True if the particle has the Track as a Velo-only track ancestor
   */
  class IsVeloOnlyAncestor
  {
  public:
    IsVeloOnlyAncestor( const LHCb::Track* track )
      : m_track(track)
    {}
    bool operator() ( const LHCb::Particle* part )
    {
      if ( part->proto() && ( t_track = part->proto()->track() ) ) {
        BOOST_FOREACH( t_ancTrack, t_track->ancestors() )
        {
          if ( t_ancTrack->checkType(LHCb::Track::Velo) && t_ancTrack->containsLhcbIDs(m_track->lhcbIDs()) ) {
            return true;
          }
        }
      }
      return false; // if matched, we already returned "true"
    }
  private:
    const LHCb::Track* m_track;    // track to match to
    const LHCb::Track* t_track;    // track of the particle to test
    const LHCb::Track* t_ancTrack; // iterator for particle track ancestors
  };
  /**
   * True if the particle is made from the track
   */
  class IsTrack
  {
  public:
    IsTrack( const LHCb::Track* track )
      : m_track(track)
    {}
    inline bool operator() ( const LHCb::Particle* part )
    {
      // particle needs to have a protoparticle AND that should have the track
      return part->proto() && operator()( part->proto() ) ;
    }
    bool operator() ( const LHCb::ProtoParticle* proto )
    {
      if ( ( t_track = proto->track() ) ) {
        if ( t_track->containsLhcbIDs( m_track->lhcbIDs() ) ) {
          return true;
        }
      }
      return false; // if matched, we already returned "true"
    }
  private:
    const LHCb::Track* m_track;    // track to match to
    const LHCb::Track* t_track;    // track of the particle to test
  };

  // DEBUG PRINTOUT
  void printUpPVZSelection( MsgStream& out
                            , const LHCb::RecVertex::Range& PVs
                            , const double& maxZ
                            , const int& minNumTracks )
  {
    out << "\n" <<
      ( PVs >> LoKi::select<const LHCb::RecVertex*>(
                                                    ( monitor( VZ, out.stream(), "\n", "\nPV Candidate\nPV Z: " ) < maxZ )
                                                    &&   monitor( RV_TrHAS(TrBACKWARD) && RV_TrHAS(! TrBACKWARD), out.stream(), "\n", "Forward and backward tracks in PV: " )
                                                    && ( monitor( RV_TrNUM(TrALL), out.stream(), "\n", "Number of tracks in PV: " ) >= minNumTracks )
                                                    ) >> LoKi::min_value<const LHCb::RecVertex*>( VZ )
        ) << " is the minimal PV Z\n"
        << endmsg;
  }
  void printRecVertexCandidateSummary( MsgStream& out
                                       , const LHCb::RecVertex* rv
                                       , const double& minZ )
  {
    out << "\nRecVertex LLP candidate"
        << "\nNumber of tracks   : " << RV_TrNUM(TrALL)(rv)
        << "\nHas backward track : " << RV_TrHAS(TrBACKWARD)(rv)
        << "\nRHO                : " << ( VX_BEAMSPOTRHO(5.0)(rv) )
        << "\nZ | UpPV Z         : " << ( VZ(rv) ) << " | " << minZ
        << endmsg;
  }
  void printLLPCandidateSummary( MsgStream& out
                                 , int numLongDaugs
                                 , int numVeloDaugs
                                 , const Gaudi::LorentzVector& momentum
                                 , double motherSumPT
                                 , const int& numDaugsWithHitBefore
                                 , const double& maxDaugE )
  {
    out << "\nCreated a LLP candidate "
        << "\nwith " << numLongDaugs << " long and " << numVeloDaugs << " Velo-only daughters"
        << "\nTotal momentum : " << momentum
        << "\nMass           : " << momentum.M()
        << "\nSumPT          : " << motherSumPT
        << "\n# daugHitUp    : " << numDaugsWithHitBefore
        << "\nMaxFractE      : " << maxDaugE/momentum.E()
        << endmsg;
  }
  void printSelectedLLP( MsgStream& out, const LHCb::Particle* llp, bool matterVeto )
  {
    out   << "Added info to particle: \n"
          << " - RHO       : " << llp->info(LLParticlesFromRecVertices::Rho, -1.0)                                  << "\n"
          << " - SigmaRHO  : " << llp->info(LLParticlesFromRecVertices::SigmaRho, -1.0)                             << "\n"
          << " - SigmaZ    : " << llp->info(LLParticlesFromRecVertices::SigmaZ, -1.0)                               << "\n"
          << " - FracUpHit : " << llp->info(LLParticlesFromRecVertices::FractDaughterTracksWithUpstreamHit, -1.0)   << "\n"
          << " - MaxFractE : " << llp->info(LLParticlesFromRecVertices::MaxFractEnergyInSingleTrack, -1.0)          << "\n"
          << " - SumPT     : " << llp->info(LLParticlesFromRecVertices::SumPT, -1.0)                                << "\n";
    if ( matterVeto ) {
      out << " - InMatter  : " << llp->info(LLParticlesFromRecVertices::InMatterVeto, -1.0)                         << "\n";
    }
    out   << endmsg;

  }
}


//=============================================================================
// Initialization
//=============================================================================
StatusCode LLParticlesFromRecVertices::initialize() {
  StatusCode sc = DaVinciAlgorithm::initialize();
  if ( sc.isFailure() ) { return sc; }

  m_debug = msgLevel(MSG::DEBUG);
  m_verbose = msgLevel(MSG::VERBOSE);

  if ( msgLevel( MSG::DEBUG ) ) { debug() << "==> Initialize the LLParticlesFromRecVertices algorithm" << endmsg; }

  // Prey IDs
  const LHCb::ParticleProperty* Prey = ppSvc()->find(m_LLPName);
  if ( ! Prey ) { return Error("Cannot find particle property for " + m_LLPName, StatusCode::FAILURE); }
  m_LLPID = Prey->particleID();
  // Get the pion mass
  const LHCb::ParticleProperty* Ppion = ppSvc()->find("pi+");
  m_piMass = Ppion->mass();

  if ( m_computeMatterVeto ) { m_materialVeto = tool<IMatterVeto>( "MatterVetoTool" , "MatterVetoTool" ); }

  m_velo = getDet<DeVelo>(DeVeloLocation::Default);

  // locate LoKi Service
  svc<LoKi::ILoKiSvc>( "LoKiSvc" , true ) ;

  // construct functors
  m_RHO = VX_BEAMSPOTRHO(5.0);
  m_Z = VZ;

  m_VERTEXCUT =
    ( RV_TrNUM(TrALL) >= m_LLPMinNumTracks )
    && ( m_RHO > m_LLPMinR )  ;

  m_UPPVZ = LoKi::select<const LHCb::RecVertex*>(
                                                 ( VZ < m_FirstPVMaxZ ) && ( VZ > m_FirstPVMinZ )
                                                 && RV_TrHAS(TrBACKWARD) && RV_TrHAS(! TrBACKWARD)
                                                 && ( RV_TrNUM(TrALL) >= m_FirstPVNumTracks )
                                                 ) >> LoKi::min_value<const LHCb::RecVertex*>( m_Z );

  m_protoMap[ "pi+" ] = std::make_pair( tool<IProtoParticleFilter>( "ProtoParticleANNPIDFilter", "pion"  , this )
                                        , ppSvc()->find( "pi+" ) );
  m_protoMap[ "K+"  ] = std::make_pair( tool<IProtoParticleFilter>( "ProtoParticleANNPIDFilter", "kaon"  , this )
                                        , ppSvc()->find( "K+" ) );
  m_protoMap[ "p+"  ] = std::make_pair( tool<IProtoParticleFilter>( "ProtoParticleANNPIDFilter", "proton"  , this )
                                        , ppSvc()->find( "p+" ) );
  m_protoMap[ "e+"  ] = std::make_pair( tool<IProtoParticleFilter>( "ProtoParticleANNPIDFilter", "electron", this )
                                        , ppSvc()->find( "e+" ) );
  m_protoMap[ "mu+" ] = std::make_pair( tool<IProtoParticleFilter>( "ProtoParticleMUONFilter"  , "muon"    , this )
                                        , ppSvc()->find( "mu+" ) );

  m_p2s = tool<IParticle2State>("Particle2State");
  if ( m_p2s == NULL ) {
    return Error("Could not locate Particle2State tool", StatusCode::FAILURE);
  }

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode LLParticlesFromRecVertices::execute()
{
  if( msgLevel(MSG::DEBUG) ) { debug() << "==> Execute the LLParticlesFromRecVertices algorithm" << endmsg; }

  setFilterPassed(false); // Mandatory. Set to true if event is accepted.

  // TES container with charged protoparticles (from Brunel)
  if ( ! m_verticesFromVeloOnly ) {
    if ( !exist<LHCb::ProtoParticles>( m_chargedProtoLocation ) ) {
      return Error("No LHCb::ProtoParticles found at " + m_chargedProtoLocation, StatusCode::FAILURE);
    }
    m_chargedProto = get<LHCb::ProtoParticles>( m_chargedProtoLocation );
  }

  m_veloProtoParticles = new LHCb::ProtoParticles();
  put( m_veloProtoParticles, m_VeloProtoPLocation );

  // Get the z of the most upstream PV
  double upPVZ = primaryVertices() >> m_UPPVZ;
  if (m_verbose) { printUpPVZSelection( verbose(), primaryVertices(), m_FirstPVMaxZ, m_FirstPVNumTracks ); }

  int nInputVertices = 0;
  int nAcceptedVertices = 0;

  if ( ( ! m_requireUpstreamPV ) || upPVZ != LoKi::Constants::HugeDistance ) {
    ++counter("#events with upPVZ");

    LHCb::RecVertex::Range recVertices;
    BOOST_FOREACH( std::string rvLoc, m_RVLocations )
    {
      if ( ! exist<LHCb::RecVertex::Range>(rvLoc) ) {
        return Error("No LHCb::RecVertex::Range found at " + rvLoc, StatusCode::FAILURE);
      }
      recVertices = get<LHCb::RecVertex::Range>(rvLoc);
      if ( ! recVertices.empty() ) {
        nInputVertices += recVertices.size();
        BOOST_FOREACH( const LHCb::RecVertex* rv, recVertices )
        {
          if (m_verbose) { printRecVertexCandidateSummary( verbose(), rv, upPVZ ); }
          if ( m_VERTEXCUT(rv) && ( ( ! m_requireUpstreamPV ) || ( m_Z(rv) >= upPVZ ) ) ) {
            if ( RecVertex2Particle(rv) ) { ++nAcceptedVertices; }
          }
        }
      }
    }
  }

  if ( nAcceptedVertices > 0 ) { setFilterPassed(true); }

  counter("#input vertices") += nInputVertices;
  counter("#accepted candidates") += nAcceptedVertices;

  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode LLParticlesFromRecVertices::finalize() {
  if( msgLevel( MSG::DEBUG ) ) { debug() << "==> Finalize" << endmsg; }

  // reset functors
  m_RHO = _VDOUBLE(0.0);
  m_Z = _VDOUBLE(0.0);
  m_VERTEXCUT = _VBOOL(false);
  m_UPPVZ = LoKi::Constant<std::vector<const LHCb::RecVertex*>,double>(LoKi::Constants::HugeDistance);

  return DaVinciAlgorithm::finalize();
}


//=============================================================================
// Turn RecVertex into Particle
//=============================================================================
const LHCb::Particle* LLParticlesFromRecVertices::RecVertex2Particle( const LHCb::RecVertex* rv )
{
  if (m_debug) { debug() << "==> Making LLParticle out of RecVertex" << endmsg; }

  Gaudi::LorentzVector motherMomentum;
  double motherSumPT = 0;

  // daughters with long tracks
  m_selectedDaughters.clear();
  const LHCb::Particle* daug;
  // daughters from Velo-only tracks
  m_selectedVeloDaughters.clear();
  Gaudi::LorentzVector daugTrackMomentum;

  // Specific for stripping
  // pointers to newly created particles (used to clean up if needed)
  m_tmpParticles.clear();

  // Values for cut calculated during loop on tracks
  double maxDaugE = 0.;
  int numDaugTracksWithHitsBefore = 0;
  double fractTracksWithHitBeforeVertex = 0.0;

  BOOST_FOREACH( const LHCb::Track* iVtxTrack, rv->tracks() )
  {
    daug = NULL;
    if ( iVtxTrack->checkType(LHCb::Track::Velo) ) {
      if ( m_verticesFromVeloOnly ) { // Hlt2 case : try to find the particle with the upgraded track
        LHCb::Particle::ConstVector::const_iterator daugIt = std::find_if( i_particles().begin(), i_particles().end(), IsVeloOnlyAncestor(iVtxTrack) );
        if ( daugIt != i_particles().end() ) {
          daug = (*daugIt);
        }
      } else {} // Stripping case : there is no (proto)particle with this track, so need to create it anyhow
    } else {
      if ( m_verticesFromVeloOnly ) { // Hlt2 case : find the upgraded track
        LHCb::Particle::ConstVector::const_iterator daugIt = std::find_if( i_particles().begin(), i_particles().end(), IsTrack(iVtxTrack) );
        if ( daugIt != i_particles().end() ) {
          daug = (*daugIt);
        }
      } else { // Stripping case : create the best-pid particle (and keep a pointer)
        LHCb::ProtoParticles::const_iterator daugProtoIt = std::find_if( m_chargedProto->begin(), m_chargedProto->end(), IsTrack(iVtxTrack) );
        if ( daugProtoIt != m_chargedProto->end() ) {
          LHCb::Particle* daugNC = makeBestNNPIDParticle( *daugProtoIt );
          if ( daugNC != NULL ) {
            m_tmpParticles.push_back(daugNC);
            daug = daugNC;
          }
        }
      }
    }

    if ( daug != NULL ) { // particle found or created
      if (m_verbose) { verbose() << "-> Found particle associated to track: " << (*daug) << endmsg; }
      if ( daug->proto()->track()->chi2PerDoF() < m_chi2NonVeloOnly ) {
        if (m_verbose) { verbose() << "Pass track chi2 cut, adding to LLP. Momentum: " << daug->momentum() << endmsg; }
        motherMomentum += daug->momentum();
        motherSumPT += daug->momentum().Pt();
        m_selectedDaughters.push_back(daug);

        // update cuts
        if ( TrHasHitBeforeVertex(iVtxTrack, rv, m_velo) ) {
          ++numDaugTracksWithHitsBefore;
          if (m_verbose) { verbose() << "Has hit before vertex!" << endmsg; }
        }
        if ( daug->momentum().E() > maxDaugE ) {
          maxDaugE = daug->momentum().E();
          if (m_verbose) { verbose() << "Has highest energy: " << maxDaugE << endmsg; }
        }
      }
    } else {
      if ( iVtxTrack->checkType( LHCb::Track::Velo ) ) { // both online and offline: no particles for Velo-only tracks
        if (m_verbose) { verbose() << "-> Found NO particle associated to track, making a default Velo particle instead" << endmsg; }
        daugTrackMomentum = getDefaultVeloOnlyMomentum(iVtxTrack);
        if ( m_useVeloTracks  ) {
          if (m_verbose) { verbose() << "Adding to LLP with momentum: " << daugTrackMomentum << endmsg; }
          motherMomentum += daugTrackMomentum;
          motherSumPT += daug->momentum().Pt();
        } else {
          if (m_verbose) { verbose() << "Momentum: " << daugTrackMomentum << ". Adding without momentum" << endmsg; }
        }
        m_selectedVeloDaughters.push_back(Track4(iVtxTrack, daugTrackMomentum));
      } else {
        Warning("-> Found NO particle associated to track or could not create a best-PID particle, and the track is not a Velo track" );
      }
    }
  }

  if (m_debug) { printLLPCandidateSummary(debug(), m_selectedDaughters.size(), m_selectedVeloDaughters.size(), motherMomentum, motherSumPT, numDaugTracksWithHitsBefore, maxDaugE ); }

  LHCb::Particle* prey = NULL;
  if ( ( motherMomentum.M() > m_LLPMinMass )
       && ( motherSumPT > m_LLPMinSumPT )
       && ( ( fractTracksWithHitBeforeVertex =  1.*numDaugTracksWithHitsBefore/(m_selectedDaughters.size()+m_selectedVeloDaughters.size()) ) < m_LLPMaxFractTrWithUpstream )
       && ( maxDaugE/motherMomentum.E() < m_LLPMaxFractEFromOne )
       && ( ! ( m_applyMatterVeto && m_materialVeto->isInMatter(rv->position()) ) )
       ) { // passes all cuts, make the composite
    if (m_debug) { debug() << "==> Constructing the particle to put on the TES" << endmsg; }

    LHCb::Vertex* preyVtx = new LHCb::Vertex(rv->position());
    preyVtx->setNDoF(rv->nDoF());
    preyVtx->setChi2(rv->chi2());
    preyVtx->setCovMatrix(rv->covMatrix());
    if (m_verbose) { verbose() << "Created vertex " << (*preyVtx) << " with position " << preyVtx->position() << ", Chi2/NDof=" << preyVtx->chi2() << "/" << preyVtx->nDoF() << " and covariance matrix " << preyVtx->covMatrix() << "\n" << endmsg; }

    prey = new LHCb::Particle(m_LLPID);
    prey->setMomentum(motherMomentum);
    prey->setMeasuredMass(motherMomentum.M());
    prey->setMeasuredMassErr(0.);

    Gaudi::SymMatrix4x4 momCovMatrix = Gaudi::SymMatrix4x4(ROOT::Math::SMatrixIdentity());
    prey->setMomCovMatrix(momCovMatrix);
    prey->setPosCovMatrix(rv->covMatrix());
    Gaudi::Matrix4x3 PosMomCovMatrix;
    prey->setPosMomCovMatrix(PosMomCovMatrix);
    prey->setEndVertex(preyVtx);
    prey->setReferencePoint(preyVtx->position());
    if (m_verbose) { verbose() << "Mother particle " << (*prey) << "\n" << endmsg; }

    prey->addInfo(LLParticlesFromRecVertices::Rho, m_RHO(rv) );
    prey->addInfo(LLParticlesFromRecVertices::SigmaRho, sqrt( rv->covMatrix()(0,0) + rv->covMatrix()(1,1) ) );
    prey->addInfo(LLParticlesFromRecVertices::SigmaZ, sqrt( rv->covMatrix()(2,2) ) );
    prey->addInfo(LLParticlesFromRecVertices::FractDaughterTracksWithUpstreamHit, fractTracksWithHitBeforeVertex);
    prey->addInfo(LLParticlesFromRecVertices::MaxFractEnergyInSingleTrack, maxDaugE/motherMomentum.E());
    prey->addInfo(LLParticlesFromRecVertices::SumPT, motherSumPT);
    if ( m_computeMatterVeto ) { prey->addInfo(LLParticlesFromRecVertices::InMatterVeto, m_materialVeto->isInMatter(preyVtx->position()) ); }
    if (m_verbose) { printSelectedLLP( verbose(), prey, m_computeMatterVeto ); }

    BOOST_FOREACH( const LHCb::Particle* selDaug, m_selectedDaughters )
    {
      preyVtx->addToOutgoingParticles(selDaug);
      prey->addToDaughters(selDaug);
    }
    if (m_verbose) { verbose() << "Added " << m_selectedDaughters.size() << " long daughters" << endmsg; }
    BOOST_FOREACH( Track4 veloTrackWithMom, m_selectedVeloDaughters )
    {
      LHCb::ProtoParticle* protoPion = new LHCb::ProtoParticle();
      m_veloProtoParticles->add( protoPion );
      protoPion->setTrack(veloTrackWithMom.first);
      LHCb::Particle* pion = new LHCb::Particle();
      pion->setProto(protoPion);
      pion->setMomentum(veloTrackWithMom.second);

      preyVtx->addToOutgoingParticles(pion);
      prey->addToDaughters(pion);
    }
    if (m_verbose) { verbose() << "Added " << m_selectedVeloDaughters.size() << " Velo-only daughters" << endmsg; }

    markNewTree(prey);
  } else {
    if ( ! m_verticesFromVeloOnly ) { // stripping case : in case not markNewTree'd, delete the early made particles
      for ( std::vector<LHCb::Particle*>::iterator pIt = m_tmpParticles.begin(); pIt != m_tmpParticles.end(); ++pIt ) {
        delete (*pIt);
      }
    }
  }

  return prey;
}

//=============================================================================
// Get the momentum of a fixed PT particle out of a Velo-only track
//=============================================================================
Gaudi::LorentzVector LLParticlesFromRecVertices::getDefaultVeloOnlyMomentum( const LHCb::Track* tr )
{
  double sx = tr->slopes().x();
  double sy = tr->slopes().y();
  double pz = m_pt/sqrt( sx*sx + sy*sy );
  double e = sqrt( m_piMass*m_piMass + m_pt*m_pt + pz*pz );
  if (m_verbose) { verbose() << "Slopes " << sx << " (x) and " << sy << " (y); pt=" << m_pt << ", pz=" << pz << " and e=" << e << "." << endmsg; }

  return Gaudi::LorentzVector( sx*pz, sy*pz, pz, e );
}

//=============================================================================
// Create Particle from ProtoParticle, PID based on best probNN
//=============================================================================
LHCb::Particle* LLParticlesFromRecVertices::makeBestNNPIDParticle( const LHCb::ProtoParticle* proto )
{
  bool pid_found(false);

  const LHCb::ParticleProperty* pprop = NULL ;

  // test RICH links
  if ( proto->hasInfo( LHCb::ProtoParticle::RichPIDStatus ) ){
    const LHCb::RichPID * rpid = proto->richPID();
    if ( !rpid ){
      Error( "ProtoParticle has RICH information but NULL RichPID SmartRef !" ).ignore();
    }
  }
  // test MUON links
  if ( proto->hasInfo( LHCb::ProtoParticle::MuonPIDStatus ) ){
    const LHCb::MuonPID * mpid = proto->muonPID();
    if ( !mpid ){
      Error( "ProtoParticle has MUON information but NULL MuonPID SmartRef !" ).ignore();
    }
  }

  //Create a map of PID and ProbNN pairs
  typedef std::map< double , std::string > MapProbNNPID;

  MapProbNNPID  probNNpid ;

  probNNpid[ proto->info( LHCb::ProtoParticle::ProbNNpi   ,-0.5) ] =  "pi+" ;
  probNNpid[ proto->info( LHCb::ProtoParticle::ProbNNp    ,-0.5) ] =  "p+"  ;
  probNNpid[ proto->info( LHCb::ProtoParticle::ProbNNk    ,-0.5) ] =  "K+"  ;
  probNNpid[ proto->info( LHCb::ProtoParticle::ProbNNe    ,-0.5) ] =  "e+"  ;
  probNNpid[ proto->info( LHCb::ProtoParticle::ProbNNmu   ,-0.5) ] =  "mu+" ;

  // the map is by construction sorted by key value (lower first)
  MapProbNNPID::iterator iNN = probNNpid.end();
  --iNN;

  if (m_verbose) { verbose() << "Ordered hypothesis: type " << (*iNN).first << " " << (*iNN).second <<endmsg; }
  // Reverse loop, check if filter is satisfied

  pid_found = m_protoMap[(*iNN).second].first->isSatisfied( proto );

  while( !pid_found && iNN != probNNpid.begin() ){
    --iNN;
    if (m_verbose) { verbose() << "Ordered hypothesis: type " << (*iNN).first << " with proba "<< (*iNN).second << endmsg; }
    pid_found = m_protoMap[(*iNN).second].first->isSatisfied( proto );
  }
  if( pid_found ){
    pprop = m_protoMap[(*iNN).second].second;
    if (m_verbose) { verbose() << "PID applied: " << (*iNN).second << endmsg; }
  }

  // make a new Particle
  LHCb::Particle* part = new LHCb::Particle();
  // if no PID information, apply pion hypothesis
  if ( !pid_found ){
    pprop = m_protoMap["pi+"].second ;
    if (m_verbose) { verbose() << "PID applied: pi+ (by default)" << endmsg; }
  }

  // Start filling particle with orgininating ProtoParticle
  part->setProto(proto);

  // ParticleID
  const int pID = pprop->particleID().pid() * (int)(proto->charge());
  part->setParticleID( LHCb::ParticleID( pID ) );
  // Mass and error from the PID hypothesis
  part->setMeasuredMass(pprop->mass());
  part->setMeasuredMassErr(0);

  const LHCb::Track* tk = proto->track() ;

  //Remaining info at the first state...
  StatusCode sc = m_p2s->state2Particle( tk->firstState(), *part );
  if ( sc.isFailure() ) {
    delete part;
    part = NULL;
  }

  return part;
}
