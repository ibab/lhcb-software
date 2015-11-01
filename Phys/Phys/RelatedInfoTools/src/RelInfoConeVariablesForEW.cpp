
// local
#include "RelInfoConeVariablesForEW.h"

//-----------------------------------------------------------------------------
// Implementation file for class : RelInfoConeVariablesForEW
//
// 2009-05-06 : Michel De Cian
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_TOOL_FACTORY( RelInfoConeVariablesForEW )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
RelInfoConeVariablesForEW::RelInfoConeVariablesForEW( const std::string &type,
                                                      const std::string &name,
                                                      const IInterface *parent)
: GaudiTool ( type, name, parent )
{
  declareInterface<IRelatedInfoTool>( this );

  declareProperty( "ConeAngle", m_coneAngle = 0.,
                   "Set the deltaR of the cone around the seed" );

  declareProperty( "TrackType", m_trackType = 3,
                   "Set the type of tracks which are considered inside the cone" );

  declareProperty( "ExtreParticlesLocation", m_extraParticlesLocation = "StdAllNoPIDsMuons",
                   "Set the type of particles which are considered in the charged cone" );
  declareProperty( "ExtrePhotonsLocation", m_extraPhotonsLocation = "StdLooseAllPhotons",
                   "Set the type of photons which are considered in the neutral cone" );
  declareProperty( "Variables", m_variables,
                   "List of variables to store (store all if empty)");
}

//=============================================================================
// Destructor
//=============================================================================
RelInfoConeVariablesForEW::~RelInfoConeVariablesForEW() {}


StatusCode RelInfoConeVariablesForEW::initialize()
{
  const StatusCode sc = GaudiTool::initialize();
  if ( sc.isFailure() ) return sc;

  m_keys.clear();

  if ( m_variables.empty() ) {

    if ( msgLevel(MSG::DEBUG) ) debug() << "List of variables empty, adding all" << endmsg;
    m_keys.push_back( RelatedInfoNamed::EWCONEANGLE); 
    m_keys.push_back( RelatedInfoNamed::EWCONEMULT); 
    m_keys.push_back( RelatedInfoNamed::EWCONEPX); 
    m_keys.push_back( RelatedInfoNamed::EWCONEPY); 
    m_keys.push_back( RelatedInfoNamed::EWCONEPZ); 
    m_keys.push_back( RelatedInfoNamed::EWCONEVP); 
    m_keys.push_back( RelatedInfoNamed::EWCONEVPT); 
    m_keys.push_back( RelatedInfoNamed::EWCONESP ); 
    m_keys.push_back( RelatedInfoNamed::EWCONESPT); 
    m_keys.push_back( RelatedInfoNamed::EWCONETP ); 
    m_keys.push_back( RelatedInfoNamed::EWCONETPT); 
    m_keys.push_back( RelatedInfoNamed::EWCONEMINPTE); 
    m_keys.push_back( RelatedInfoNamed::EWCONEMAXPTE); 
    m_keys.push_back( RelatedInfoNamed::EWCONEMINPTMU); 
    m_keys.push_back( RelatedInfoNamed::EWCONEMAXPTMU); 
    m_keys.push_back( RelatedInfoNamed::EWCONENMULT); 
    m_keys.push_back( RelatedInfoNamed::EWCONENPX); 
    m_keys.push_back( RelatedInfoNamed::EWCONENPY); 
    m_keys.push_back( RelatedInfoNamed::EWCONENPZ); 
    m_keys.push_back( RelatedInfoNamed::EWCONENVP); 
    m_keys.push_back( RelatedInfoNamed::EWCONENVPT); 
    m_keys.push_back( RelatedInfoNamed::EWCONENSP); 
    m_keys.push_back( RelatedInfoNamed::EWCONENSPT); 
  
  } else {

    for ( const auto& var : m_variables )
    {
      short int key = RelatedInfoNamed::indexByName( var );
      if (key != RelatedInfoNamed::UNKNOWN) {
        m_keys.push_back( key );
        if ( msgLevel(MSG::DEBUG) )
          debug() << "Adding variable " << var << ", key = " << key << endmsg;
      } else {
        warning() << "Unknown variable " << var << ", skipping" << endmsg;
      }
    }
  }

  return sc;
}


//=============================================================================
// Calculate cone variables
//=============================================================================
StatusCode RelInfoConeVariablesForEW::calculateRelatedInfo( const LHCb::Particle *top,
                                                            const LHCb::Particle *seed)
{

  if ( msgLevel(MSG::DEBUG) )
    debug() << "==> Fill" << endmsg;

  // -- The vector m_decayParticles contains all the particles that belong to the given decay
  // -- according to the decay descriptor.

  // -- Clear the vector with the particles in the specific decay
  m_decayParticles.clear();

  // -- Add the mother (prefix of the decay chain) to the vector
  if ( msgLevel(MSG::DEBUG) )
    debug() << "Filling particle with ID " << top->particleID().pid() << endmsg;
  m_decayParticles.push_back( top );

  // -- Save all particles that belong to the given decay in the vector m_decayParticles
  saveDecayParticles( top );

  // -- Get all particles in the event
  LHCb::Particles *parts = get<LHCb::Particles>( "Phys/" + m_extraParticlesLocation + "/Particles" );
  if ( parts->empty() )
  {
    if ( msgLevel(MSG::WARNING) )
      warning() << "Could not retrieve extra-particles. Skipping" << endmsg;
    return StatusCode::FAILURE;
  }

  if ( seed ) 
  {

    // -- Retrieve information in the charged cone
    int multiplicity = 0;
    std::vector < double > vectorP;
    double scalarP = 0.;
    double scalarPt = 0.;
    double minimumPtE = 0.;
    double maximumPtE = 0.;
    double minimumPtMu = 0.;
    double maximumPtMu = 0.;

    StatusCode sc = ChargedCone( seed, parts, m_coneAngle, multiplicity,
                                 vectorP, scalarP, scalarPt,
                                 minimumPtE, maximumPtE,
                                 minimumPtMu, maximumPtMu );

    if ( sc.isFailure() ) { multiplicity = -1; }

    // -- Create a vector with the summed momentum of all tracks in the cone
    const Gaudi::XYZVector coneMomentum( vectorP[0], vectorP[1], vectorP[2] );

    // -- Create a vector with the summed momentum of all tracks in the cone + seed
    const Gaudi::XYZVector totalMomentum( seed->momentum().X() + coneMomentum.X(),
                                          seed->momentum().Y() + coneMomentum.Y(),
                                          seed->momentum().Z() + coneMomentum.Z() );

    // -- Fill the tuple with the variables
    m_mult = multiplicity;
    m_px = coneMomentum.X();
    m_py = coneMomentum.Y();
    m_pz = coneMomentum.Z();
    m_vp = std::sqrt( coneMomentum.Mag2() );
    m_vpt = std::sqrt( coneMomentum.Perp2() );
    m_sp = scalarP;
    m_spt = scalarPt;
    m_tp = std::sqrt( totalMomentum.Mag2() );
    m_tpt = std::sqrt( totalMomentum.Perp2() );
    if ( m_coneAngle == 0. )
    {
      m_minpte = minimumPtE;
      m_maxpte = maximumPtE;
      m_minptmu = minimumPtMu;
      m_maxptmu = maximumPtMu;
    }

    // -- Retrieve information in the neutral cone
    int nmultiplicity = 0;
    std::vector < double > nvectorP;
    double nscalarP = 0.;
    double nscalarPt = 0.;

    // -- Get all photons in the event
    LHCb::Particles *photons = get<LHCb::Particles>( "Phys/" + m_extraPhotonsLocation + "/Particles" );
    if ( !photons->empty() )
    {
      StatusCode nsc = NeutralCone( seed, photons, m_coneAngle,
                                    nmultiplicity, nvectorP, nscalarP, nscalarPt );
      if ( nsc.isFailure() )
        nmultiplicity = -1;
    }
    else
    {
      if ( msgLevel(MSG::WARNING) )
        Warning("Could not retrieve photons");
      nmultiplicity = -1;
      nvectorP.push_back( 0 );
      nvectorP.push_back( 0 );
      nvectorP.push_back( 0 );
    }

    // -- Create a vector with the summed momentum of all tracks in the cone
    const Gaudi::XYZVector neutralConeMomentum( nvectorP[0], nvectorP[1], nvectorP[2] );

    // -- Fill the tuple with the variables
    m_nmult = nmultiplicity;
    m_npx =   neutralConeMomentum.X();
    m_npy =   neutralConeMomentum.Y();
    m_npz =   neutralConeMomentum.Z();
    m_nvp =   std::sqrt( neutralConeMomentum.Mag2() );
    m_nvpt =  std::sqrt( neutralConeMomentum.Perp2() );
    m_nsp =   nscalarP;
    m_nspt =  nscalarPt;

    m_map.clear();

    for ( const auto key : m_keys )
    {

      float value = 0;
      switch (key) {
      case RelatedInfoNamed::EWCONEANGLE : value = m_coneAngle; break;
      case RelatedInfoNamed::EWCONEMULT  : value = m_mult; break;
      case RelatedInfoNamed::EWCONEPX    : value = m_px; break;
      case RelatedInfoNamed::EWCONEPY    : value = m_py; break;
      case RelatedInfoNamed::EWCONEPZ    : value = m_pz; break;
      case RelatedInfoNamed::EWCONEVP    : value = m_vp; break;
      case RelatedInfoNamed::EWCONEVPT   : value = m_vpt; break;
      case RelatedInfoNamed::EWCONESP    : value = m_sp; break;
      case RelatedInfoNamed::EWCONESPT   : value = m_spt; break;
      case RelatedInfoNamed::EWCONETP    : value = m_tp; break;
      case RelatedInfoNamed::EWCONETPT   : value = m_tpt; break;
      case RelatedInfoNamed::EWCONEMINPTE   : value = m_minpte; break;
      case RelatedInfoNamed::EWCONEMAXPTE   : value = m_maxpte; break;
      case RelatedInfoNamed::EWCONEMINPTMU  : value = m_minptmu; break;
      case RelatedInfoNamed::EWCONEMAXPTMU  : value = m_maxptmu; break;
      case RelatedInfoNamed::EWCONENMULT    : value = m_nmult; break;
      case RelatedInfoNamed::EWCONENPX      : value = m_npx; break;
      case RelatedInfoNamed::EWCONENPY      : value = m_npy; break;
      case RelatedInfoNamed::EWCONENPZ      : value = m_npz; break;
      case RelatedInfoNamed::EWCONENVP      : value = m_nvp; break;
      case RelatedInfoNamed::EWCONENVPT     : value = m_nvpt; break;
      case RelatedInfoNamed::EWCONENSP      : value = m_nsp; break;
      case RelatedInfoNamed::EWCONENSPT     : value = m_nspt; break;
      default: value = 0.; break;
      }

      debug() << "  Inserting key = " << key << ", value = " << value << " into map" << endmsg;

      m_map.insert( std::make_pair( key, value) );
    }


  }
  else {

    if ( msgLevel(MSG::WARNING) )
      warning() << "The seed particle is not valid. Skipping" << endmsg;
    return StatusCode::FAILURE;

  }

  return StatusCode::SUCCESS;

}
//=============================================================================
// Save the particles in the decay chain (recursive function)
//=============================================================================
void RelInfoConeVariablesForEW::saveDecayParticles( const LHCb::Particle *top )
{

  // -- Fill all the daugthers in m_decayParticles
  for ( const auto& dau : top->daughters() )
  {

    // -- If the particle is stable, save it in the vector, or...
    if ( dau->isBasicParticle() ) {
      if ( msgLevel(MSG::DEBUG) )
        debug() << "Filling particle with ID " << dau->particleID().pid() << endmsg;
      m_decayParticles.push_back( dau );
    }
    else {
      // -- if it is not stable, call the function recursively
      m_decayParticles.push_back( dau );
      if ( msgLevel(MSG::DEBUG) )
        debug() << "Filling particle with ID " << dau->particleID().pid() << endmsg;
      saveDecayParticles( dau );
    }

  }

  return;

}
//=============================================================================
// Loop over all the tracks in the cone which do not belong to the desired decay
//=============================================================================
StatusCode RelInfoConeVariablesForEW::ChargedCone( const LHCb::Particle *seed,
                                                   const LHCb::Particles *parts,
                                                   const double rcut,
                                                   int &mult,
                                                   std::vector < double > &vP,
                                                   double &sP, double &sPt,
                                                   double &minPtE, double &maxPtE,
                                                   double &minPtMu, double &maxPtMu )
{

  // -- Initialize values
  mult = 0;
  double sPx = 0.;
  double sPy = 0.;
  double sPz = 0.;
  sP  = 0.;
  sPt = 0.;
  minPtE = 1.e10;
  int minQE = 0;
  maxPtE = 0.;
  int maxQE = 0;
  minPtMu = 1.e10;
  int minQMu = 0;
  maxPtMu = 0.;
  int maxQMu = 0;

  // -- Get the 4-momentum of the seed particle
  const Gaudi::LorentzVector& seedMomentum = seed->momentum();

  for ( const LHCb::Particle *particle : *parts )
  {

    const LHCb::ProtoParticle *proto = particle->proto();
    if ( proto ) {

      const LHCb::Track *track = proto->track();
      if ( track ) {

        // -- Check if the track belongs to the decay itself
        bool isInDecay = isTrackInDecay( track );
        if ( isInDecay )
          continue;

        // -- Get the 3-momentum of the track
        const Gaudi::XYZVector& trackMomentum = track->momentum();

        // -- Calculate the difference in Eta and Phi between the seed particle and a track
        double deltaPhi = fabs( seedMomentum.Phi() - trackMomentum.Phi() );
        if ( deltaPhi > M_PI )
          deltaPhi = 2 * M_PI - deltaPhi;
        double deltaEta = seedMomentum.Eta() - trackMomentum.Eta();
        double deltaR   = std::sqrt( deltaPhi * deltaPhi + deltaEta * deltaEta );

        if ( track->type() == m_trackType ) {
          if ( ( rcut == 0. ) || ( deltaR < rcut ) ) {
            // -- Calculate vector information
            sPx += trackMomentum.X();
            sPy += trackMomentum.Y();
            sPz += trackMomentum.Z();

            // -- Calculate scalar information
            sP  += std::sqrt( trackMomentum.Mag2() );
            sPt += std::sqrt( trackMomentum.Perp2() );

            mult++;
          }

          if ( rcut == 0. ) {

            // Extra Electron
            double prsE = 50.;
            double eCalEoP = .10;
            double hCalEoP = .05;
            if ( proto->info( LHCb::ProtoParticle::CaloPrsE, -1. ) > prsE ) {
              if ( proto->info( LHCb::ProtoParticle::CaloEcalE, -1. ) / track->p() > eCalEoP ) {
                if ( ( proto->info( LHCb::ProtoParticle::CaloHcalE, -1. ) > 0 ) && ( proto->info( LHCb::ProtoParticle::CaloHcalE, -1. ) / track->p() < hCalEoP ) ) {
                  if ( track->pt() < minPtE ) {
                    minPtE = track->pt();
                    minQE  = track->charge();
                  }
                  if ( track->pt() > maxPtE ) {
                    maxPtE = track->pt();
                    maxQE  = track->charge();
                  }
                }
              }
            }

            // Extra Muon
            double minP = 10.e3;
            if ( track->p() > minP ) {
              const LHCb::MuonPID *muonPID = proto->muonPID();
              if ( muonPID ) {
                if ( muonPID->IsMuon() ) {
                  if ( track->pt() < minPtMu ) {
                    minPtMu = track->pt();
                    minQMu  = track->charge();
                  }
                  if ( track->pt() > maxPtMu ) {
                    maxPtMu = track->pt();
                    maxQMu  = track->charge();
                  }
                }
              }
            }

          }
        }
      }
    }
  }

  vP.push_back( sPx );
  vP.push_back( sPy );
  vP.push_back( sPz );

  if ( minPtE == 1.e10 )
    minPtE = 0.;
  if ( maxPtE == 0. )
    maxPtE = 0.;
  minPtE *= minQE;
  maxPtE *= maxQE;

  if ( minPtMu == 1.e10 )
    minPtMu = 0.;
  if ( maxPtMu == 0. )
    maxPtMu = 0.;
  minPtMu *= minQMu;
  maxPtMu *= maxQMu;

  return StatusCode::SUCCESS;

}

StatusCode RelInfoConeVariablesForEW::NeutralCone( const LHCb::Particle *seed,
                                                   const LHCb::Particles *photons,
                                                   const double rcut,
                                                   int &mult,
                                                   std::vector<double> &vP,
                                                   double &sP,
                                                   double &sPt )
{

  // -- Initialize values
  mult = 0;
  double sPx = 0.;
  double sPy = 0.;
  double sPz = 0.;
  sP  = 0.;
  sPt = 0.;

  // -- Get the 4-momentum of the seed particle
  const Gaudi::LorentzVector& seedMomentum = seed->momentum();

  for ( const LHCb::Particle *photon : *photons )
  {

    // -- Get the 3-momentum of the photon
    const Gaudi::XYZVector& photonMomentum = photon->momentum().Vect();

    // -- Calculate the difference in Eta and Phi between the seed particle and a photons
    double deltaPhi = fabs( seedMomentum.Phi() - photonMomentum.Phi() );
    if ( deltaPhi > M_PI )
      deltaPhi = 2 * M_PI - deltaPhi;
    double deltaEta = seedMomentum.Eta() - photonMomentum.Eta();
    double deltaR   = std::sqrt( deltaPhi * deltaPhi + deltaEta * deltaEta );

    if ( ( rcut == 0. ) || ( deltaR < rcut ) ) {
      // -- Calculate vector information
      sPx += photonMomentum.X();
      sPy += photonMomentum.Y();
      sPz += photonMomentum.Z();

      // -- Calculate scalar information
      sP  += std::sqrt( photonMomentum.Mag2() );
      sPt += std::sqrt( photonMomentum.Perp2() );

      mult++;
    }
  }

  vP.push_back( sPx );
  vP.push_back( sPy );
  vP.push_back( sPz );

  return StatusCode::SUCCESS;

}
//=============================================================================
// Check if the track is already in the decay
//=============================================================================
bool RelInfoConeVariablesForEW::isTrackInDecay( const LHCb::Track *track ) {

  bool isInDecay = false;

  for ( const LHCb::Particle * part : m_decayParticles )
  {
    const LHCb::ProtoParticle *proto = part->proto();
    if ( proto ) {

      const LHCb::Track *myTrack = proto->track();
      if ( myTrack ) {

        if ( myTrack == track ) {
          if ( msgLevel( MSG::DEBUG ) )
            debug() << "Track is in decay, skipping it" << endmsg;
          isInDecay = true;
        }

      }
    }
  }

  return isInDecay;
}

LHCb::RelatedInfoMap* RelInfoConeVariablesForEW::getInfo(void)
{
  return &m_map;
}
