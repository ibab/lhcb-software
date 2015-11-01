#include "Event/Particle.h"
#include "Event/RelatedInfoMap.h"
// kernel
#include "GaudiKernel/PhysicalConstants.h"
#include "Kernel/RelatedInfoNamed.h"

// local
#include "RelInfoConeVariables.h"

#include <string>

//-----------------------------------------------------------------------------
// Implementation file for class : RelInfoConeVariables
// Converted from TupleToolTrackIsolation by A. Poluektov
//
// 2009-05-06 : Michel De Cian
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_TOOL_FACTORY( RelInfoConeVariables )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
RelInfoConeVariables::RelInfoConeVariables( const std::string& type,
                                            const std::string& name,
                                            const IInterface* parent)
: GaudiTool ( type, name , parent ),
  m_tracksLocation("")
{
  declareInterface<IRelatedInfoTool>(this);
  declareProperty( "ConeAngle", m_coneAngle = 1.0,
                   "Set the deltaR of the cone (default = 1.0), in radians");
  declareProperty( "TrackType", m_trackType = 3,
                   "Set the type of tracks which are considered inside the cone (default = 3)");
  declareProperty( "Variables", m_variables,
                   "List of variables to store (store all if empty)");
  declareProperty( "TracksLocation", m_tracksLocation,
                   "Location for tracks to create the cone (use all best tracks by default)");
  declareProperty( "Inputs", m_inputs,
                   "Location for tracks to create the cone (use all best tracks by default)");
}

//=============================================================================
// Destructor
//=============================================================================
RelInfoConeVariables::~RelInfoConeVariables() {}

StatusCode RelInfoConeVariables::initialize()
{
  const StatusCode sc = GaudiTool::initialize();
  if ( sc.isFailure() ) return sc;

  m_keys.clear();

  if (m_tracksLocation.size() != 0) {
     warning() << "The TracksLocation property is deprecated, please use the Inputs property" << endmsg;
  }
  if (m_inputs.empty() && m_tracksLocation.empty()) {
     m_inputs.push_back(LHCb::TrackLocation::Default);
  }

  if ( m_variables.empty() )
  {
    if ( msgLevel(MSG::DEBUG) ) debug() << "List of variables empty, adding all" << endmsg;
    m_keys.push_back( RelatedInfoNamed::CONEANGLE );
    m_keys.push_back( RelatedInfoNamed::CONEMULT );
    m_keys.push_back( RelatedInfoNamed::CONEPX );
    m_keys.push_back( RelatedInfoNamed::CONEPY );
    m_keys.push_back( RelatedInfoNamed::CONEPZ );
    m_keys.push_back( RelatedInfoNamed::CONEP  );
    m_keys.push_back( RelatedInfoNamed::CONEPT );
    m_keys.push_back( RelatedInfoNamed::CONEPXASYM );
    m_keys.push_back( RelatedInfoNamed::CONEPYASYM );
    m_keys.push_back( RelatedInfoNamed::CONEPZASYM );
    m_keys.push_back( RelatedInfoNamed::CONEPASYM );
    m_keys.push_back( RelatedInfoNamed::CONEPTASYM );
    m_keys.push_back( RelatedInfoNamed::CONEDELTAETA );
    m_keys.push_back( RelatedInfoNamed::CONEDELTAPHI );
  } else {

    for ( const auto& var : m_variables )
    {
      short int key = RelatedInfoNamed::indexByName( var );
      if (key != RelatedInfoNamed::UNKNOWN)
      {
        m_keys.push_back( key );
        if ( msgLevel(MSG::DEBUG) )
          debug() << "Adding variable " << var << ", key = " << key << endmsg;
      }
      else
      {
        warning() << "Unknown variable " << var << ", skipping" << endmsg;
      }
    }
  }

  return sc;
}

//=============================================================================
// Fill Cone Info structure
//=============================================================================
StatusCode RelInfoConeVariables::calculateRelatedInfo( const LHCb::Particle *top,
                                                       const LHCb::Particle *part )
{

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Fill" << endmsg;

  // -- The vector m_decayParticles contains all the particles that belong to the given decay
  // -- according to the decay descriptor.

  // -- Clear the vector with the particles in the specific decay
  m_decayParticles.clear();
  m_tracksStorage.clear();

  // -- Add the mother (prefix of the decay chain) to the vector
  if ( msgLevel(MSG::DEBUG) ) debug() << "Filling particle with ID " << top->particleID().pid() << endmsg;
  m_decayParticles.push_back( top );

  // -- Save all particles that belong to the given decay in the vector m_decayParticles
  saveDecayParticles( top );

  for (auto location : m_inputs) {
    // -- Get tracks
    if ( msgLevel(MSG::DEBUG) ) debug() << "Will use tracks from " << location << endmsg;
    auto tracks = getIfExists<LHCb::Tracks>(location);
    if ( tracks ) {
      for (auto track : *tracks) {
        m_tracksStorage.push_back( track );
      }
    } else {
      if (location.find("/Particles") != (location.size() - std::string{"/Particles"}.length())) {
        location += "/Particles";
      }
      if (exist<LHCb::Particle::Range>(evtSvc(), location)) {
        auto parts = get<LHCb::Particle::Range>(location);
        for(const LHCb::Particle * part : parts ) {
          const LHCb::ProtoParticle* proto = part->proto();
          if (proto) {
            const LHCb::Track* track = proto->track();
            m_tracksStorage.push_back( track );
          }
        }
      } else {
        warning() << "Could not retrieve particles. Skipping" << endmsg;
         return StatusCode::FAILURE;
      }
    }
  }

  bool test = true;

  if( part )
  {

    if ( msgLevel(MSG::VERBOSE) ) verbose() << "Filling variables with conesize " << m_coneAngle << endmsg;

    // -- Retrieve momentum information of tracks in the cone
    std::pair<std::vector<double>,int> myPair = ConeP(part, &m_tracksStorage, m_coneAngle);
    const std::vector<double> & myVector = myPair.first;

    const double conePx = myVector[0];
    const double conePy = myVector[1];
    const double conePz = myVector[2];

    const double conePx2 = conePx*conePx;
    const double conePy2 = conePy*conePy;

    const double coneP  = std::sqrt( conePx2 + conePy2 + conePz*conePz );
    const double conePt = std::sqrt( conePx2 + conePy2 );

    // -- Create a vector with the summed momentum of all tracks in the cone
    const Gaudi::XYZVector momentumInCone( conePx, conePy, conePz );

    // -- Calculate the difference in Eta and Phi between the summed momentum of all tracks in the cone and the
    // -- track of the particle in question
    double deltaEta = part->momentum().Eta() - momentumInCone.Eta();
    double deltaPhi = fabs(part->momentum().Phi() - momentumInCone.Phi());
    if(deltaPhi > M_PI) deltaPhi  = 2*M_PI-deltaPhi;

    // -- Fill the tuple with the variables
    m_px = conePx;
    m_py = conePy;
    m_pz = conePz;
    m_pt = conePt;
    m_p = coneP;
    m_mult = myPair.second;

    // -- Fill the difference in Eta and Phi between the summed momentum of all tracks in the cone and the
    // -- track of the particle in question
    m_deltaEta = deltaEta;
    m_deltaPhi = deltaPhi;

    // -- Fill the asymmetry information
    m_pxasy = (part->momentum().Px() - conePx)/(part->momentum().Px() + conePx);
    m_pyasy = (part->momentum().Py() - conePy)/(part->momentum().Py() + conePy);
    m_pzasy = (part->momentum().Pz() - conePz)/(part->momentum().Pz() + conePz);
    m_pasy  = (part->p() - coneP)/(part->p() + coneP);
    m_ptasy = (part->pt() - conePt)/(part->pt() + conePt);

    m_map.clear();

    for ( const auto key : m_keys )
    {

      float value = 0;
      switch (key) {
      case RelatedInfoNamed::CONEANGLE : value = m_coneAngle; break;
      case RelatedInfoNamed::CONEMULT  : value = m_mult; break;
      case RelatedInfoNamed::CONEPX    : value = m_px; break;
      case RelatedInfoNamed::CONEPY    : value = m_py; break;
      case RelatedInfoNamed::CONEPZ    : value = m_pz; break;
      case RelatedInfoNamed::CONEP     : value = m_p; break;
      case RelatedInfoNamed::CONEPT    : value = m_pt; break;
      case RelatedInfoNamed::CONEPXASYM   : value = m_pxasy; break;
      case RelatedInfoNamed::CONEPYASYM   : value = m_pyasy; break;
      case RelatedInfoNamed::CONEPZASYM   : value = m_pzasy; break;
      case RelatedInfoNamed::CONEPASYM    : value = m_pasy; break;
      case RelatedInfoNamed::CONEPTASYM   : value = m_ptasy; break;
      case RelatedInfoNamed::CONEDELTAETA : value = m_deltaEta; break;
      case RelatedInfoNamed::CONEDELTAPHI : value = m_deltaPhi; break;
      default: value = 0.; break;
      }

      debug() << "  Inserting key = " << key << ", value = " << value << " into map" << endmsg;

      m_map.insert( std::make_pair(key,value) );
    }

  }
  else
  {
    if ( msgLevel(MSG::WARNING) ) warning() << "The particle in question is not valid" << endmsg;
    return StatusCode::FAILURE;
  }

  return StatusCode(test);
}
//=============================================================================
// Save the particles in the decay chain (recursive function)
//=============================================================================
void RelInfoConeVariables::saveDecayParticles( const LHCb::Particle *top)
{

  // -- Fill all the daugthers in m_decayParticles
  for ( const auto& dau : top->daughters() )
  {

    // -- If the particle is stable, save it in the vector, or...
    if ( dau->isBasicParticle() )
    {
      if ( msgLevel(MSG::DEBUG) ) debug() << "Filling particle with ID " << dau->particleID().pid() << endmsg;
      m_decayParticles.push_back( dau );
    }
    else
    {
      // -- if it is not stable, call the function recursively
      m_decayParticles.push_back( dau );
      if ( msgLevel(MSG::DEBUG) ) debug() << "Filling particle with ID " << dau->particleID().pid() << endmsg;
      saveDecayParticles( dau );
    }

  }

}
//=============================================================================
// Loop over all the tracks in the cone which do not belong to the desired decay
//=============================================================================
std::pair< std::vector<double>, int>
RelInfoConeVariables::ConeP(const LHCb::Particle *part,
                            const std::vector<const LHCb::Track*> *tracks,
                            const double rcut)
{

  // -- Get the (4-) momentum of the particle in question
  const Gaudi::LorentzVector& partMomentum = part->momentum();
  double sumPx = 0.0;
  double sumPy = 0.0;
  double sumPz = 0.0;
  int counter = 0;

  for ( std::vector<const LHCb::Track*>::const_iterator itrack = tracks->begin(); itrack != tracks->end(); itrack++ )
//  for ( const LHCb::Track * track : *tracks )
  {
    const LHCb::Track* track = *itrack;

    // -- Check if the track belongs to the decay itself
    bool isInDecay = isTrackInDecay(track);
    if(isInDecay) continue;

    // -- Get the (3-) momentum of the track
    const Gaudi::XYZVector& trackMomentum = track->momentum();
    //double tracketa = track->pseudoRapidity();
    const double trackpx = trackMomentum.X();
    const double trackpy = trackMomentum.Y();
    const double trackpz = trackMomentum.Z();

    // -- Calculate the difference in Eta and Phi between the particle in question and a track
    double deltaPhi = fabs( partMomentum.Phi() - trackMomentum.Phi() );
    if(deltaPhi > M_PI) deltaPhi  = 2*M_PI-deltaPhi;

    const double deltaEta = partMomentum.Eta() - trackMomentum.Eta();

    const double deltaR = std::sqrt(deltaPhi * deltaPhi + deltaEta * deltaEta);

    // -- Add the tracks to the summation if deltaR is smaller than the cut value of deltaR
    if(deltaR < rcut && track->type()== m_trackType )
    {
      sumPx += trackpx;
      sumPy += trackpy;
      sumPz += trackpz;
      counter++;
    }

  }

  // -- Fill a vector and return it
  std::vector<double> myVector;
  myVector.push_back(sumPx);
  myVector.push_back(sumPy);
  myVector.push_back(sumPz);

  std::pair<std::vector<double>, int> myPair;
  myPair.first = myVector;
  myPair.second = counter;

  return myPair;

}
//=============================================================================
// Check if the track is already in the decay
//=============================================================================
bool RelInfoConeVariables::isTrackInDecay(const LHCb::Track* track)
{

  bool isInDecay = false;

  for ( const LHCb::Particle * part : m_decayParticles )
  {
    const LHCb::ProtoParticle* proto = part->proto();
    if(proto)
    {
      const LHCb::Track* myTrack = proto->track();
      if(myTrack)
      {
        if(myTrack == track)
        {
          if ( msgLevel(MSG::DEBUG) ) debug() << "Track is in decay, skipping it" << endmsg;
          isInDecay = true;
        }
      }

    }

  }

  return isInDecay;
}

LHCb::RelatedInfoMap* RelInfoConeVariables::getInfo(void)
{
  return &m_map;
}
