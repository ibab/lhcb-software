// Include files
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SystemOfUnits.h"
#include "GaudiKernel/GaudiException.h"

#include "MultipleMagneticFieldSvc.h"

#include "GaudiKernel/Vector3DTypes.h"
#include "GaudiKernel/Point3DTypes.h"

#include <cstdlib>
#include <fstream>

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
            ISvcLocator* svc ) : Service( name, svc ),
                                  m_names(0),
                                  m_magneticFieldSvcs(0)
{
  declareProperty( "MagneticFieldServices",    m_names );
}
//=============================================================================
// Standard destructor
//=============================================================================
MultipleMagneticFieldSvc::~MultipleMagneticFieldSvc()
{
}

//=============================================================================
// Initialize
//=============================================================================
StatusCode MultipleMagneticFieldSvc::initialize()
{
  MsgStream log(msgSvc(), name());
  StatusCode sc = Service::initialize();
  if( sc.isFailure() ) return sc;

  /// Getting the member magnetic field services
  std::vector<std::string>::const_iterator it;
  for ( it = m_names.begin(); m_names.end() != it; it++ ) {
    if( !(*it).empty()) {
      ILHCbMagnetSvc* mfSvc;
      sc = service( (*it), mfSvc, true );
      if( sc.isFailure() || mfSvc == 0) {
        log << MSG::WARNING << " Member " << (*it) << " not found" << endmsg;
      }
      else {
        m_magneticFieldSvcs.push_back(mfSvc);
      }
    }
  }
  /*
  /// Inintializing member fields
  if( m_magneticFieldSvcs.size() ) {
    std::vector<ILHCbMagnetSvc*>::iterator field;
    for( field = m_magneticFieldSvcs.begin(); field != m_magneticFieldSvcs.end(); field++) {
      sc = (*field)->initialize();
      if( sc.isFailure() ){
        log << MSG::WARNING << " Member field failed to initialize" << endmsg;
        m_magneticFieldSvcs.erase(field);
      }
    }
  }
  */
   
  /// Warn if no fields are set up
  if( m_magneticFieldSvcs.size() == 0 ) {
    log << MSG::WARNING << " No member fields configured" << endmsg;
  }
  return StatusCode::SUCCESS;
}

//=============================================================================
// Finalize
//=============================================================================
StatusCode MultipleMagneticFieldSvc::finalize()
{   
  MsgStream log(msgSvc(), name());
  StatusCode sc = StatusCode::SUCCESS;
  // release the used tools
  if( m_magneticFieldSvcs.size() ) {
    std::vector<ILHCbMagnetSvc*>::iterator field;
    for( field = m_magneticFieldSvcs.begin(); field != m_magneticFieldSvcs.end(); field++) {
      sc = (*field)->release();
      if( sc.isFailure() ){
        log << MSG::WARNING << " Member field failed to finalize" << endmsg;
        m_magneticFieldSvcs.erase(field);
      }
    }
  }
  return sc && Service::finalize();
}


//=============================================================================
// QueryInterface
//=============================================================================
StatusCode MultipleMagneticFieldSvc::queryInterface( const InterfaceID& riid,
                                             void** ppvInterface      )
{   
  if ( IMagneticFieldSvc::interfaceID().versionMatch(riid) ) {
    *ppvInterface = (IMagneticFieldSvc*)this;
    addRef();
    return StatusCode::SUCCESS;
  } else if ( ILHCbMagnetSvc::interfaceID().versionMatch(riid) ) {
    *ppvInterface = (ILHCbMagnetSvc*)this;
    addRef();
    return StatusCode::SUCCESS;
  }
  return Service::queryInterface(riid,ppvInterface);
} 

//=============================================================================
// FieldVector: find the magnetic field value at a given point in space
//=============================================================================
StatusCode MultipleMagneticFieldSvc::fieldVector(const Gaudi::XYZPoint&  r,
                                               Gaudi::XYZVector& bf ) const
{
  /// Attaining field strength from member fields
  bf.SetXYZ( 0.0, 0.0, 0.0 );
  StatusCode sc;
  Gaudi::XYZVector tempVector;
  std::vector<ILHCbMagnetSvc*>::const_iterator field;
  for( field = m_magneticFieldSvcs.begin(); field != m_magneticFieldSvcs.end(); field++) {
    sc = (*field)->fieldVector(r, tempVector);
    if( sc.isFailure() ) return sc;
    bf+=tempVector;
  }
  return StatusCode::SUCCESS;      
}

//=============================================================================
// UseRealMap: Check if any member fields use real maps
//=============================================================================
bool MultipleMagneticFieldSvc::useRealMap() const
{
  bool realMap = false;
  std::vector<ILHCbMagnetSvc*>::const_iterator field;
  for( field = m_magneticFieldSvcs.begin(); field != m_magneticFieldSvcs.end(); field++) {
    realMap = realMap || (*field)->useRealMap();
  }
  return realMap;
}



//=============================================================================
// polarity: meaningless for multiple fields unless they are all the same
//=============================================================================
bool MultipleMagneticFieldSvc::isDown() const {

    std::vector<ILHCbMagnetSvc*>::const_iterator field;
    bool isDown = (*(m_magneticFieldSvcs.begin()))->isDown();
    for( field = m_magneticFieldSvcs.begin(); field != m_magneticFieldSvcs.end(); field++) {
    if( (*field)->isDown() != isDown)
      throw GaudiException( "Invalid call to MultipleMagneticFieldSvc::isDown()",
                            "INVALID_MULTIPLE_POLARITY", StatusCode::FAILURE );
    }
    return isDown;
}


//=============================================================================
// signedRelativeCurrent: meaningless for multiple fields unless they 
// are all the same polarity
//=============================================================================

double MultipleMagneticFieldSvc::signedRelativeCurrent() const
{
  /// Attaining scale factor of member fields (The average)
  
  double scale    = 0.0;
  double sumScale = 0.0;
  int numFields = 0;
  std::vector<ILHCbMagnetSvc*>::const_iterator field;
  
  for( field = m_magneticFieldSvcs.begin(); field != m_magneticFieldSvcs.end(); field++) {
    ++numFields;
    sumScale += (*field)->signedRelativeCurrent();
  }
  
  if( numFields != 0 )
    scale = sumScale/numFields;
  
  return scale;
}


