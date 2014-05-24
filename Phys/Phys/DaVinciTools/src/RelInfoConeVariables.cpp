#include "GaudiKernel/ToolFactory.h"
#include "Event/Particle.h"
#include "Event/RelatedInfoMap.h"
// kernel
#include "GaudiKernel/PhysicalConstants.h"

// local
#include "RelInfoConeVariables.h"
#include "ConeInfo.h"

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
                                const IInterface* parent) : GaudiTool ( type, name , parent )
{
  declareInterface<IRelatedInfoTool>(this);
  declareProperty( "ConeNumber", m_coneNumber = 1,
                   "Number of cone variables record (1-4)");
  declareProperty( "ConeAngle", m_coneAngle = 1.0,
                   "Set the deltaR of the cone (default = 1.0), in radians");
  declareProperty( "TrackType", m_trackType = 3,
                   "Set the type of tracks which are considered inside the cone (default = 3)");
  declareProperty( "Variables", m_variables, 
                   "List of variables to store (store all if empty)");

  m_keys.clear(); 

  std::vector<std::string>::const_iterator ivar; 
  for (ivar = m_variables.begin(); ivar != m_variables.end(); ivar++) {
    short int key = ConeInfo::indexByName( *ivar ); 
    if (key != ConeInfo::Unknown) {
      m_keys.push_back( key );
    } else {
      warning() << "Unknown variable " << *ivar << ", skipping" << endmsg; 
    }
  }

}

//=============================================================================
// Destructor
//=============================================================================
RelInfoConeVariables::~RelInfoConeVariables() {}

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

  // -- Add the mother (prefix of the decay chain) to the vector
  if ( msgLevel(MSG::DEBUG) ) debug() << "Filling particle with ID " << top->particleID().pid() << endmsg;
  m_decayParticles.push_back( top );

  // -- Save all particles that belong to the given decay in the vector m_decayParticles
  saveDecayParticles( top );

  // -- Get all tracks in the event
  LHCb::Tracks* tracks = get<LHCb::Tracks>(LHCb::TrackLocation::Default);
  if ( tracks->empty() )
  {
    if ( msgLevel(MSG::WARNING) ) warning() << "Could not retrieve tracks. Skipping" << endmsg;
    return StatusCode::FAILURE;
  }

  bool test = true;

  if( part )
  {

    if ( msgLevel(MSG::VERBOSE) ) verbose() << "Filling variables with conesize " << m_coneAngle << endmsg;

    // -- Retrieve momentum information of tracks in the cone
    std::pair<std::vector<double>, int> myPair = ConeP(part, tracks, m_coneAngle);
    const std::vector<double> & myVector = myPair.first;

    double conePx = myVector[0];
    double conePy = myVector[1];
    double conePz = myVector[2];

    double conePx2 = conePx*conePx;
    double conePy2 = conePy*conePy;

    double coneP = std::sqrt( conePx2 + conePy2 + conePz*conePz );
    double conePt = std::sqrt( conePx2 + conePy2 );

    // -- Create a vector with the summed momentum of all tracks in the cone
    Gaudi::XYZVector momentumInCone;
    momentumInCone.SetX(conePx);
    momentumInCone.SetY(conePy);
    momentumInCone.SetZ(conePz);

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
    
    std::vector<short int>::const_iterator ikey; 
    for (ikey = m_keys.begin(); ikey != m_keys.end(); ikey++) {
      
      float value = 0;
      switch (*ikey) {
        case ConeInfo::Angle : value = m_coneAngle; break;
        case ConeInfo::Mult  : value = m_mult; break;
        case ConeInfo::PX    : value = m_px; break;
        case ConeInfo::PY    : value = m_py; break;
        case ConeInfo::PZ    : value = m_pz; break;
        case ConeInfo::P     : value = m_p; break;
        case ConeInfo::PT    : value = m_pt; break;
        case ConeInfo::PXAsym   : value = m_pxasy; break;
        case ConeInfo::PYAsym   : value = m_pyasy; break;
        case ConeInfo::PZAsym   : value = m_pzasy; break;
        case ConeInfo::PAsym    : value = m_pasy; break;
        case ConeInfo::PTAsym   : value = m_ptasy; break;
        case ConeInfo::DeltaEta : value = m_deltaEta; break;
        case ConeInfo::DeltaPhi : value = m_deltaPhi; break;
      }
    
      m_map.insert( std::make_pair( *ikey, value) );
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

  // -- Get the daughters of the top particle
  const SmartRefVector< LHCb::Particle > & daughters = top->daughters();

  // -- Fill all the daugthers in m_decayParticles
  for( SmartRefVector< LHCb::Particle >::const_iterator idau = daughters.begin() ; idau != daughters.end() ; ++idau){

    // -- If the particle is stable, save it in the vector, or...
    if( (*idau)->isBasicParticle() ){
      if ( msgLevel(MSG::DEBUG) ) debug() << "Filling particle with ID " << (*idau)->particleID().pid() << endmsg;
      m_decayParticles.push_back( (*idau) );
    }else{
      // -- if it is not stable, call the function recursively
      m_decayParticles.push_back( (*idau) );
      if ( msgLevel(MSG::DEBUG) ) debug() << "Filling particle with ID " << (*idau)->particleID().pid() << endmsg;
      saveDecayParticles( (*idau) );
    }

  }

}
//=============================================================================
// Loop over all the tracks in the cone which do not belong to the desired decay
//=============================================================================
std::pair< std::vector<double>, int>
RelInfoConeVariables::ConeP(const LHCb::Particle *part,
                     const LHCb::Tracks* tracks,
                     const double rcut)
{

  // -- Get the (4-) momentum of the particle in question
  Gaudi::LorentzVector partMomentum = part->momentum();
  double sumPx = 0.0;
  double sumPy = 0.0;
  double sumPz = 0.0;
  int counter = 0;

  for( LHCb::Tracks::const_iterator it = tracks->begin(); it != tracks->end(); ++it){
    LHCb::Track* track = (*it);

    // -- Check if the track belongs to the decay itself
    bool isInDecay = isTrackInDecay(track);
    if(isInDecay) continue;

    // -- Get the (3-) momentum of the track
    Gaudi::XYZVector trackMomentum = track->momentum();
    //double tracketa = track->pseudoRapidity();
    double trackpx = trackMomentum.X();
    double trackpy = trackMomentum.Y();
    double trackpz = trackMomentum.Z();

    // -- Calculate the difference in Eta and Phi between the particle in question and a track
    double deltaPhi = fabs( partMomentum.Phi() - trackMomentum.Phi() );
    if(deltaPhi > M_PI) deltaPhi  = 2*M_PI-deltaPhi;

    double deltaEta = partMomentum.Eta() - trackMomentum.Eta();

    double deltaR = std::sqrt(deltaPhi * deltaPhi + deltaEta * deltaEta);

    // -- Add the tracks to the summation if deltaR is smaller than the cut value of deltaR
    if(deltaR < rcut && track->type()== m_trackType ){
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
bool RelInfoConeVariables::isTrackInDecay(const LHCb::Track* track){

  bool isInDecay = false;

  for(  std::vector<const LHCb::Particle*>::iterator it = m_decayParticles.begin() ; it != m_decayParticles.end() ; ++it ){

    const LHCb::ProtoParticle* proto = (*it)->proto();
    if(proto){
      const LHCb::Track* myTrack = proto->track();

      if(myTrack){

        if(myTrack == track){
          if ( msgLevel(MSG::DEBUG) ) debug() << "Track is in decay, skipping it" << endmsg;
          isInDecay = true;
        }
      }

    }

  }

  return isInDecay;
}

RelatedInfoMap* RelInfoConeVariables::getInfo(void) {
  return &m_map; 
}

std::string RelInfoConeVariables::infoPath(void) {
  std::stringstream ss;
  ss << std::string("Particle2CV") << m_coneNumber << std::string("Relations");
  return ss.str(); 
}
