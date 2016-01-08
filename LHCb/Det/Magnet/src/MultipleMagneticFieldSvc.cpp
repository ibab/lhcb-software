// Include files
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/GaudiException.h"

#include "MultipleMagneticFieldSvc.h"

#include "GaudiKernel/Vector3DTypes.h"
#include "GaudiKernel/Point3DTypes.h"

#include <cstdlib>

/** @file MultipleMagneticFieldSvc.cpp
 *  Implementation of MultipleMagneticFieldSvc class
 *
 *  @author Magnus Lieng
 *  @date   2008-07-03
 */

DECLARE_SERVICE_FACTORY( MultipleMagneticFieldSvc )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
MultipleMagneticFieldSvc::MultipleMagneticFieldSvc( const std::string& name,
            ISvcLocator* svc ) : base_class( name, svc )
{
  declareProperty( "MagneticFieldServices",    m_names );
}
//=============================================================================
// Standard destructor
//=============================================================================
MultipleMagneticFieldSvc::~MultipleMagneticFieldSvc() = default;

//=============================================================================
// Initialize
//=============================================================================
StatusCode MultipleMagneticFieldSvc::initialize()
{
  MsgStream log(msgSvc(), name());
  StatusCode sc = Service::initialize();
  if( sc.isFailure() ) return sc;

  /// Getting the member magnetic field services
  for ( const auto&  name : m_names ) {
      m_magneticFieldSvcs.emplace_back( service(name,true) );
      if(!m_magneticFieldSvcs.back()) {
        log << MSG::WARNING << " Member " << name << " not found" << endmsg;
        m_magneticFieldSvcs.pop_back();
      }
  }

  /// Warn if no fields are set up
  if( m_magneticFieldSvcs.empty() ) {
    log << MSG::WARNING << " No member fields configured" << endmsg;
  }
  return StatusCode::SUCCESS;
}

//=============================================================================
// Finalize
//=============================================================================
StatusCode MultipleMagneticFieldSvc::finalize()
{
  // release the used fields
  m_magneticFieldSvcs.clear();
  return Service::finalize();
}

//=============================================================================
// FieldVector: find the magnetic field value at a given point in space
//=============================================================================
Gaudi::XYZVector MultipleMagneticFieldSvc::fieldVector(const Gaudi::XYZPoint& r) const
{
  /// sum field strength from member fields
  return std::accumulate(m_magneticFieldSvcs.begin(),m_magneticFieldSvcs.end(),
                         ROOT::Math::XYZVector{ 0.,0.,0.},
                         [&](ROOT::Math::XYZVector bf,  const ILHCbMagnetSvc* fs)
                         { return bf + fs->fieldVector(r); } );
}

//=============================================================================
// UseRealMap: Check if any member fields use real maps
//=============================================================================
bool MultipleMagneticFieldSvc::useRealMap() const
{
  return std::any_of(m_magneticFieldSvcs.begin(),m_magneticFieldSvcs.end(),
                     [](const ILHCbMagnetSvc* fs) {
                         return fs->useRealMap();
  });
}



//=============================================================================
// polarity: meaningless for multiple fields unless they are all the same
//=============================================================================
bool MultipleMagneticFieldSvc::isDown() const {

    auto neq = std::adjacent_find( m_magneticFieldSvcs.begin(), m_magneticFieldSvcs.end(),
                                 [](const ILHCbMagnetSvc* first, const ILHCbMagnetSvc* second) {
                            return first->isDown() != second->isDown();
    } );
    if ( neq!=m_magneticFieldSvcs.end()) {
      throw GaudiException( "Invalid call to MultipleMagneticFieldSvc::isDown()",
                            "INCONSISTENT POLARITIES", StatusCode::FAILURE );
    }
    // if empty, we're neither down nor up... so we anser 'isDown' with 'no'
    return !m_magneticFieldSvcs.empty() && m_magneticFieldSvcs.front()->isDown();
}


//=============================================================================
// signedRelativeCurrent: meaningless for multiple fields unless they
// are all the same polarity
//=============================================================================

double MultipleMagneticFieldSvc::signedRelativeCurrent() const
{
  /// Attaining scale factor of member fields (The average)
  auto scale = std::accumulate(m_magneticFieldSvcs.begin(),m_magneticFieldSvcs.end(),
                               0., [](double s, const ILHCbMagnetSvc* fs)
                               { return s + fs->signedRelativeCurrent(); } );
  return !m_magneticFieldSvcs.empty() ? scale/m_magneticFieldSvcs.size() : 0.;
}
