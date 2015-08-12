#include "Event/Particle.h"
// kernel
#include "GaudiKernel/PhysicalConstants.h"

// local
#include "ConeVariables.h"

//-----------------------------------------------------------------------------
// Implementation file for class : ConeVariables
// Converted from TupleToolTrackIsolation by A. Poluektov
//
// 2009-05-06 : Michel De Cian
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_TOOL_FACTORY( ConeVariables )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
  ConeVariables::ConeVariables( const std::string& type,
                                const std::string& name,
                                const IInterface* parent) : GaudiTool ( type, name , parent )
{
  declareInterface<IExtraInfoTool>(this);
  declareProperty( "ConeNumber", m_coneNumber = 1,
                   "Number of cone variables record (1-4)");
  declareProperty( "ConeAngle", m_coneAngle = 1.0,
                   "Set the deltaR of the cone (default = 1.0), in radians");
  declareProperty( "TrackType", m_trackType = 3,
                   "Set the type of tracks which are considered inside the cone (default = 3)");
  declareProperty( "Variables", m_variables, 
                   "List of variables to store (store all if empty)");
}

//=============================================================================
// Destructor
//=============================================================================
ConeVariables::~ConeVariables() {}

//=============================================================================
// Fill Cone Info structure
//=============================================================================
StatusCode ConeVariables::calculateExtraInfo( const LHCb::Particle *top,
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
void ConeVariables::saveDecayParticles( const LHCb::Particle *top)
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
ConeVariables::ConeP(const LHCb::Particle *part,
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
bool ConeVariables::isTrackInDecay(const LHCb::Track* track){

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


int ConeVariables::getFirstIndex(void) {
  switch(m_coneNumber) {
  case 1: return LHCb::Particle::Cone1Angle;
  case 2: return LHCb::Particle::Cone2Angle;
  case 3: return LHCb::Particle::Cone3Angle;
  case 4: return LHCb::Particle::Cone4Angle;
  default: return LHCb::Particle::Cone1Angle;
  }
}

int ConeVariables::getNumberOfParameters(void) {
  return 14; // This tool returns 14 parameters
}

int ConeVariables::getInfo(int index, double & value, std::string & name) {

  switch( index - getFirstIndex() + LHCb::Particle::Cone1Angle) {
    case LHCb::Particle::Cone1Angle    : value = m_coneAngle;    name = "angle"; break;
    case LHCb::Particle::Cone1Mult     : value = (double)m_mult; name = "mult"; break;
    case LHCb::Particle::Cone1PX       : value = m_px; name = "px"; break;
    case LHCb::Particle::Cone1PY       : value = m_py; name = "py"; break;
    case LHCb::Particle::Cone1PZ       : value = m_pz; name = "pz"; break;
    case LHCb::Particle::Cone1P        : value = m_p;  name = "p" ; break;
    case LHCb::Particle::Cone1PT       : value = m_pt; name = "pt"; break;
    case LHCb::Particle::Cone1PXAsym   : value = m_pxasy; name = "pxasy"; break;
    case LHCb::Particle::Cone1PYAsym   : value = m_pyasy; name = "pyasy"; break;
    case LHCb::Particle::Cone1PZAsym   : value = m_pzasy; name = "pzasy"; break;
    case LHCb::Particle::Cone1PAsym    : value = m_pasy;  name = "pasy";  break;
    case LHCb::Particle::Cone1PTAsym   : value = m_ptasy; name = "ptasy"; break;
    case LHCb::Particle::Cone1DeltaEta : value = m_deltaEta; name = "deltaEta"; break;
    case LHCb::Particle::Cone1DeltaPhi : value = m_deltaPhi; name = "deltaPhi"; break;
    default: break;
  }

  if (!m_variables.empty()) {
    if (std::find(m_variables.begin(), m_variables.end(), name) == m_variables.end() ) return 0;
  }

  return 1;

}
