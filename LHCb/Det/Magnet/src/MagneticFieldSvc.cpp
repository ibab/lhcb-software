// Include files
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SystemOfUnits.h"

#include "MagneticFieldSvc.h"
#include "MagnetCondLocations.h"

#include "GaudiKernel/Vector3DTypes.h"
#include "GaudiKernel/Point3DTypes.h"
#include "GaudiKernel/IUpdateManagerSvc.h"
#include "GaudiKernel/IToolSvc.h"

#include <cstdlib>
#include <fstream>
#include <sstream>
#include <map>

/** @file MagneticFieldSvc.cpp
 *  Implementation of MagneticFieldSvc class
 *
 *  @author Edgar De Oliveira
 *  @date   2002-05-21
 *  Updated and further developed - Adlene Hicheur
 *  Updated and further developed - Marco Cattaneo
 */

DECLARE_SERVICE_FACTORY( MagneticFieldSvc )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
MagneticFieldSvc::MagneticFieldSvc( const std::string& name,
                                    ISvcLocator* svc )
: Service( name, svc ),
  m_forcedToUseDownMap(false),
  m_forcedToUseUpMap(false),
  m_forcedScaleFactor (9999),
  m_mapFromOptions(false),
  m_mapFilesUpPtr(0),
  m_mapFilesDownPtr(0),
  m_scaleUpPtr(0),
  m_scaleDownPtr(0),
  m_currentPtr(0),
  m_updMgrSvc(0),
  m_magFieldGridReader(*msgSvc()),
  m_isDown(false)
{

  m_constFieldVector.push_back( 0. );
  m_constFieldVector.push_back( 0. );
  m_constFieldVector.push_back( 0. );

  const char * fmroot = std::getenv("FIELDMAPROOT");
  if ( fmroot )
  {
    m_mapFilePath  = fmroot;
    m_mapFilePath += "/cdf/";
  }
  else
  {
    m_mapFilePath  = "";
  }

  declareProperty( "NominalCurrent", m_nominalCurrent = 5850,
                   "Nominal magnet current in Amps" );
  declareProperty( "FieldMapPath",  m_mapFilePath,
                   "Directory where field map files are located, including trailing separator" );
  declareProperty( "UseConditions", m_UseConditions = true );
  declareProperty( "UseSetCurrent", m_UseSetCurrent = false );
  declareProperty( "FieldMapFiles", m_mapFileNames,
                   "Vector of file names for the field map. If set, over-rides CondDB value" );
  declareProperty( "ForcedSignedCurrentScaling",   m_forcedScaleFactor = 9999,
                   "Factor by which to rescale the field map. If set, over-rides CondDB value" );
  declareProperty( "ForceToUseDownMap",         m_forcedToUseDownMap = false,
                   "Force to use down map. If set, over-rides CondDB value" );
  declareProperty( "ForceToUseUpMap",            m_forcedToUseUpMap = false,
                   "Force to use up map. If set, over-rides CondDB value" );
  declareProperty( "UseConstantField",    m_useConstField = false );
  declareProperty( "ConstantFieldVector", m_constFieldVector );

}
//=============================================================================
// Standard destructor
//=============================================================================
MagneticFieldSvc::~MagneticFieldSvc()
{
}

//=============================================================================
// Initialize
//=============================================================================
StatusCode MagneticFieldSvc::initialize()
{
  StatusCode status = Service::initialize();
  if( status.isFailure() ) return status;

  status = service("UpdateManagerSvc",m_updMgrSvc);
  if ( status.isFailure() )
  {
    MsgStream log(msgSvc(), name());
    log << MSG::ERROR << "Cannot find the UpdateManagerSvc" << endmsg;
    return status;
  }

  if ( m_useConstField )
  {
    // Constant field requested, do not use any field map
    MsgStream log(msgSvc(), name());
    log << MSG::WARNING << "using constant magnetic field with field vector "
        << m_constFieldVector << " (Tesla)" << endmsg;

    m_magFieldGridReader.fillConstantField( Gaudi::XYZVector(m_constFieldVector[0] * Gaudi::Units::tesla,
                                                             m_constFieldVector[1] * Gaudi::Units::tesla,
                                                             m_constFieldVector[2] * Gaudi::Units::tesla),
                                            m_magFieldGrid ) ;

    // register anyway with UpdateManagerSvc, so clients can register callbacks
    // transparently, even if they are never going to be called
    m_updMgrSvc->registerCondition(this);

  }
  else
  {

    if ( m_UseConditions )
    {
      // Normal case, use conditions database
      status = initializeWithCondDB();
    }
    else
    {
      status = initializeWithoutCondDB();
      // register anyway with UpdateManagerSvc, so clients can register callbacks
      // transparently, even if they are never going to be called
      m_updMgrSvc->registerCondition(this);
    }

  }

  // update the cached field polarity
  cacheFieldPolarity();

  return status;
}

//=============================================================================
// Finalize
//=============================================================================
StatusCode MagneticFieldSvc::finalize()
{
  return Service::finalize();
}

//=============================================================================
bool MagneticFieldSvc::useRealMap() const
//=============================================================================
{
  return ( m_mapFileNames.size() == 4 );
}

//=============================================================================
StatusCode MagneticFieldSvc::initializeWithCondDB()
//=============================================================================
{
  MsgStream log(msgSvc(), name());

  // Polarity and current
  if ( m_UseSetCurrent )
  {
    m_updMgrSvc->registerCondition( this, MagnetCondLocations::Set,
                                    &MagneticFieldSvc::i_updateConditions, m_currentPtr );
  }
  else
  {
    m_updMgrSvc->registerCondition( this, MagnetCondLocations::Measured,
                                    &MagneticFieldSvc::i_updateConditions, m_currentPtr );
  }

  // FieldMap file name(s). If not over-ridden by options, get from CondDB

  if ( !m_mapFileNames.empty() )
  {
    log << MSG::WARNING
        << "Requested condDB but using manually set field map file name(s) = "
        << m_mapFileNames << endmsg;

    m_mapFromOptions = true;
    const StatusCode sc =
      ( m_mapFileNames.size() == 1 ?
        m_magFieldGridReader.readDC06File( m_mapFileNames.front(), m_magFieldGrid ) :
        m_magFieldGridReader.readFiles   ( m_mapFileNames,         m_magFieldGrid ) ) ;
    if ( sc.isFailure() ) return sc;
  }
  else
  {
    m_updMgrSvc->registerCondition( this, MagnetCondLocations::FieldMapFilesUp,
                                    &MagneticFieldSvc::i_updateConditions, m_mapFilesUpPtr );
    m_updMgrSvc->registerCondition( this, MagnetCondLocations::FieldMapFilesDown,
                                    &MagneticFieldSvc::i_updateConditions, m_mapFilesDownPtr );
  }


  // Scaling factor. If not over-ridden by options, get it from Options
  if(m_forcedScaleFactor < 9998. )
  {
    log << MSG::WARNING
        << "Requested condDB but using manually set signed scale factor = "
        << m_forcedScaleFactor  << endmsg;
    m_magFieldGrid.setScaleFactor( m_forcedScaleFactor ) ;
  }
  else
  {
    m_updMgrSvc->registerCondition( this, MagnetCondLocations::ScaleUp,
                                    &MagneticFieldSvc::i_updateConditions, m_scaleUpPtr );
    m_updMgrSvc->registerCondition( this, MagnetCondLocations::ScaleDown,
                                    &MagneticFieldSvc::i_updateConditions, m_scaleDownPtr );
  }

  // Initialize the service using the current conditions values
  return m_updMgrSvc->update(this);
}

//=============================================================================
StatusCode MagneticFieldSvc::initializeWithoutCondDB()
//=============================================================================
{

  MsgStream log(msgSvc(), name());
  log << MSG::WARNING << "Not using CondDB, entirely steered by options" << endmsg;

  if ( m_mapFileNames.empty() )
  {
    log << MSG::ERROR << "Field Map filename(s) not set" << endmsg;
    return StatusCode::FAILURE;
  }

  m_mapFromOptions = true;

  double scaleFactor = m_forcedScaleFactor;;

  if( m_forcedScaleFactor > 9998. )
  {
    scaleFactor = 1.;
    if( UNLIKELY(log.level() <= MSG::DEBUG) )
      log << MSG::DEBUG << "Scale factor set to default = " << scaleFactor << endmsg;
  }

  m_magFieldGrid.setScaleFactor( scaleFactor ) ;

  // update the field
  const StatusCode sc =
    ( m_mapFileNames.size() == 1 ?
      m_magFieldGridReader.readDC06File( m_mapFileNames.front(), m_magFieldGrid ) :
      m_magFieldGridReader.readFiles   ( m_mapFileNames,         m_magFieldGrid ) );

  return sc ;
}

//=============================================================================
// QueryInterface
//=============================================================================
StatusCode MagneticFieldSvc::queryInterface( const InterfaceID& riid,
                                             void** ppvInterface      )
{
  if ( IMagneticFieldSvc::interfaceID().versionMatch(riid) )
  {
    *ppvInterface = (IMagneticFieldSvc*)this;
    addRef();
    return StatusCode::SUCCESS;
  }
  else if ( ILHCbMagnetSvc::interfaceID().versionMatch(riid) )
  {
    *ppvInterface = (ILHCbMagnetSvc*)this;
    addRef();
    return StatusCode::SUCCESS;
  }
  return Service::queryInterface(riid,ppvInterface);
}

//=============================================================================
StatusCode MagneticFieldSvc::i_updateConditions()
//=============================================================================
{
  MsgStream log(msgSvc(), name());
  if( UNLIKELY(log.level() <= MSG::DEBUG) )
    log << MSG::DEBUG << "updateConditions called" << endmsg;

  if (m_forcedToUseDownMap && m_forcedToUseUpMap)
    log << MSG::WARNING
        << "inconsistent settings, forced to use Down AND Uo map = " << endmsg;

  double polarity = 0;
  if ( m_forcedToUseDownMap ) { polarity = -1.0; }
  if ( m_forcedToUseUpMap   ) { polarity = +1.0; }
  if ( !m_forcedToUseDownMap && !m_forcedToUseUpMap )
  {
    polarity = m_currentPtr->param<int>("Polarity");
  }

  // Update the scale factor
  if ( m_forcedScaleFactor > 9998. )
  {
    const double current = m_currentPtr->param<double>("Current");

    const std::vector<double> coeffs =
      ( polarity > 0  ?
        m_scaleUpPtr   -> param<std::vector<double> >("Coeffs") :
        m_scaleDownPtr -> param<std::vector<double> >("Coeffs") );

    const double scaleFactor = coeffs[0] + ( coeffs[1]*(current/m_nominalCurrent) );
    m_magFieldGrid.setScaleFactor( scaleFactor ) ;
  }

  // Update the field map file
  StatusCode sc = StatusCode::SUCCESS;
  if ( !m_mapFromOptions )
  {

    // Convention used: positive polarity is "Up" (+y), negative is "Down" (-y)
    std::vector<std::string> files;
    if( polarity > 0 )
    {
      files = m_mapFilesUpPtr->param<std::vector<std::string> >("Files");
    }
    else
    {
      files = m_mapFilesDownPtr->param<std::vector<std::string> >("Files");
    }

    // append the path
    for ( std::vector<std::string>::iterator iF = files.begin(); iF != files.end(); ++iF )
    {
      *iF = m_mapFilePath + *iF ;
    }

    // test the cache
    if(  m_mapFileNames != files )
    {
      // update the cache
      m_mapFileNames = files ;
      // update the field
      sc = ( m_mapFileNames.size() == 1 ?
             m_magFieldGridReader.readDC06File( m_mapFileNames.front(), m_magFieldGrid ) :
             m_magFieldGridReader.readFiles   ( m_mapFileNames,         m_magFieldGrid ) ) ;
      if ( UNLIKELY(log.level() <= MSG::DEBUG) )
        log << MSG::DEBUG << "Field map files updated: " << m_mapFileNames << endmsg;
    }
  }

  // update the cached field polarity
  cacheFieldPolarity();

  // Print a message
  static std::map<std::string,unsigned long long> nUpdates;
  std::ostringstream mess;
  mess << "Map scaled by factor "            << m_magFieldGrid.scaleFactor()
       << " with polarity internally used: " << polarity
       << " signed relative current: "       << signedRelativeCurrent();
  if      ( nUpdates[mess.str()]++ <  2 ) 
  { log << MSG::INFO << mess.str() << endmsg; }
  else if ( nUpdates[mess.str()]   == 2 ) 
  { log << MSG::INFO << "Message '" << mess.str() << "' is SUPPRESSED" << endmsg;  }

  return sc ;
}

//=============================================================================

Gaudi::XYZVector MagneticFieldSvc::fieldVector( const Gaudi::XYZPoint& xyz ) const
{
  return m_magFieldGrid.fieldVector(xyz) ;
}

//=============================================================================

bool MagneticFieldSvc::isDown() const
{
  return m_isDown ;
}

//=============================================================================

void MagneticFieldSvc::cacheFieldPolarity()
{
  m_isDown = m_magFieldGrid.fieldVectorClosestPoint(Gaudi::XYZPoint(0,0,5200)).y() < 0 ;
}
