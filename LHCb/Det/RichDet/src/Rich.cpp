// Include files
#include <cassert>

#include "RichDet/Rich.h"
#include "RichDet/Rich1PhotonReflector.h"
#include "RichDet/Rich2PhotonReflector.h"
#include "RichDet/Rich1AerogelRadiator.h"
#include "RichDet/RichXGasRadiator.h"
#include "RichDet/SICBPhotonDetector.h"
#include "RichDet/RichParameters.h"

Rich::Rich (const RichID id)
  : m_id(id)
{
  switch (id) {
  case Rich1:
    m_radiator.push_back( new Rich1AerogelRadiator(*this) );
    m_radiator.push_back( new RichXGasRadiator(PhotonRadiator::C4F10, *this) );
    m_reflector = new Rich1PhotonReflector(*this);
   break;
  case Rich2:
    m_radiator.push_back( new RichXGasRadiator(PhotonRadiator::CF4, *this) );
    m_reflector = new Rich2PhotonReflector(*this);
    break;
  default:
    break;
  }
  m_detector  = new SICBPhotonDetector(*this);
  
  assert(m_radiator.size() > 0 );
  assert(m_reflector != NULL );
  assert(m_detector != NULL );
}

Rich::~Rich()
{
  for(vector<PhotonRadiator*>::iterator radiator = m_radiator.begin();
      radiator != m_radiator.end();
      ++radiator) {
    delete *radiator;
  };
  delete m_reflector;
  delete m_detector;
}

void Rich::update (const RichParameters &parameter)
{
  assert( m_reflector != NULL );
  m_reflector->update(parameter);
  assert( m_detector != NULL );
  m_detector->update(parameter);

  for(vector<PhotonRadiator*>::iterator radiator = m_radiator.begin();
      radiator != m_radiator.end();
      ++radiator) {

    assert( *radiator != NULL );
    (*radiator)->update(parameter);

  }
}

/// This should be replaced by a routine using the Particle Properties Service
double Rich::particleMass (const ParticleCode code)
{
  assert( code >= Electron && code < MaxParticleCode );

  switch ( code ) {
  case Electron :
    return 0.00051;
  case Muon :
    return 0.10566;
  case Pion :
    return 0.13957;
  case Kaon :
    return 0.49368;
  case Proton :
    return 0.93827;
  };

  return 0; // Never reached, but compiler complains otherwise

}

