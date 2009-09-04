// $Id: MagneticFieldSvc.cpp,v 1.45 2009-09-04 12:15:02 wouter Exp $

// Include files
#include "GaudiKernel/SvcFactory.h"
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

/** @file MagneticFieldSvc.cpp
 *  Implementation of MagneticFieldSvc class
 *
 *  @author Edgar De Oliveira
 *  @date   2002-05-21
 *  Updated and further developed - Adlene Hicheur
 *  Updated and further developed - Marco Cattaneo
 */

DECLARE_SERVICE_FACTORY( MagneticFieldSvc );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
MagneticFieldSvc::MagneticFieldSvc( const std::string& name, 
                                    ISvcLocator* svc ) : Service( name, svc ),
                                                         m_mapFromOptions(false),
                                                         m_scaleFromOptions(false),
                                                         m_mapFilesUpPtr(0),
                                                         m_mapFilesDownPtr(0),
                                                         m_scaleUpPtr(0),
                                                         m_scaleDownPtr(0),
                                                         m_currentPtr(0),
                                                         m_updMgrSvc(0),
							 m_magFieldGridReader(*msgSvc())
{

  m_constFieldVector.push_back( 0. );
  m_constFieldVector.push_back( 0. );
  m_constFieldVector.push_back( 0. );

  if( std::getenv("FIELDMAPROOT") != NULL ) {
    m_mapFilePath  = getenv("FIELDMAPROOT");
    m_mapFilePath += "/cdf/";
  }
  else
    m_mapFilePath  = "";
  
  declareProperty( "NominalCurrent", m_nominalCurrent = 5850, 
                   "Nominal magnet current in Amps" );
  declareProperty( "FieldMapPath",  m_mapFilePath, 
                   "Directory where field map files are located, including trailing separator" );
  declareProperty( "UseConditions", m_UseConditions = true );
  declareProperty( "UseSetCurrent", m_UseSetCurrent = false );
  declareProperty( "FieldMapFiles", m_mapFileNames, 
                   "Vector of file names for the field map. If set, over-rides CondDB value" );
  declareProperty( "ScaleFactor",   m_scaleFactor = 9999.,
                   "Factor by which to rescale the field map. If set, over-rides CondDB value" );
  declareProperty( "Polarity",      m_polarity = 0,
                   "Polarity of the magnet. If set, over-rides CondDB value" );
  
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
  
  MsgStream log(msgSvc(), name());
  StatusCode status = Service::initialize();
  if( status.isFailure() ) return status;

  if( m_useConstField ) {
    // Constant field requested, do not use any field map
    log << MSG::WARNING << "using constant magnetic field with field vector "
        << m_constFieldVector << " (Tesla)" << endmsg;
    
    m_magFieldGridReader.fillConstantField( Gaudi::XYZVector(m_constFieldVector[0] * Gaudi::Units::tesla,
							     m_constFieldVector[1] * Gaudi::Units::tesla,
							     m_constFieldVector[2] * Gaudi::Units::tesla),
					    m_magFieldGrid ) ;
    
    return StatusCode::SUCCESS;
  }
  
  if( m_UseConditions ) {
    // Normal case, use conditions database
    status = initializeWithCondDB();
  }
  else {
    status = initializeWithoutCondDB();
  }
  
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
  if( m_mapFileNames.size() == 4 )
    return true;
  else
    return false;
}

//=============================================================================
StatusCode MagneticFieldSvc::initializeWithCondDB() 
//=============================================================================
{
  MsgStream log(msgSvc(), name());

  StatusCode status = service("UpdateManagerSvc",m_updMgrSvc);
  if ( status.isFailure() ) {
    log << MSG::ERROR << "Cannot find the UpdateManagerSvc" << endmsg;
    return status;
  }

  // Polarity and current
  if( m_UseSetCurrent ) {
    m_updMgrSvc->registerCondition( this, MagnetCondLocations::Set,
                                    &MagneticFieldSvc::i_updateConditions, m_currentPtr );
  }
  else {
    m_updMgrSvc->registerCondition( this, MagnetCondLocations::Measured,
                                    &MagneticFieldSvc::i_updateConditions, m_currentPtr );
  }

  // FieldMap file name(s). If not over-ridden by options, get from CondDB
  m_mapFromOptions = false;
  if( m_mapFileNames.size() != 0 ) {
    log << MSG::WARNING 
        << "Requested condDB but using manually set field map file name(s) = "
        << m_mapFileNames << endmsg;
    m_mapFromOptions = true;
  }
  else {
    m_updMgrSvc->registerCondition( this, MagnetCondLocations::FieldMapFilesUp,
                                    &MagneticFieldSvc::i_updateConditions, m_mapFilesUpPtr );

    m_updMgrSvc->registerCondition( this, MagnetCondLocations::FieldMapFilesDown,
                                    &MagneticFieldSvc::i_updateConditions, m_mapFilesDownPtr );
  }
  

  // Scaling factor. If not over-ridden by options, get it from CondDB
  m_scaleFromOptions = false;
  if( m_scaleFactor < 9998. ) {
    log << MSG::WARNING 
        << "Requested condDB but using manually set scale factor = "
        << m_scaleFactor << endmsg;
    m_scaleFromOptions = true;
  }
  else {
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
  
  if( m_mapFileNames.size() == 0 ) {
    log << MSG::ERROR << "Field Map filename(s) not set" << endmsg;
    return StatusCode::FAILURE;
  }
  m_mapFromOptions = true;

  if( m_scaleFactor > 9998. ) {
    m_scaleFactor = 1.;
    log << MSG::DEBUG << "Scale factor set to default = " << m_scaleFactor << endmsg;
  }
  m_scaleFromOptions = true;
  m_magFieldGrid.setScaleFactor( m_scaleFactor ) ;

  // update the field
  StatusCode sc = m_mapFileNames.size() == 1 ?
    m_magFieldGridReader.readDC06File( m_mapFileNames.front(), m_magFieldGrid ) :
    m_magFieldGridReader.readFiles( m_mapFileNames, m_magFieldGrid ) ;
  
  return sc ;
}

//=============================================================================
// QueryInterface
//=============================================================================
StatusCode MagneticFieldSvc::queryInterface( const InterfaceID& riid, 
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
StatusCode MagneticFieldSvc::i_updateConditions() 
//=============================================================================
{
  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << "updateConditions called" << endmsg;

  int polarity;
  if( m_polarity != 0 ) {
    log << MSG::WARNING 
        << "Requested condDB but using manually set polarity = " << m_polarity << endmsg;
    polarity = m_polarity;
  }
  else
    polarity = m_currentPtr->param<int>("Polarity");
  
  // Update the scale factor
  if( !m_scaleFromOptions ) {
    double current = m_currentPtr->param<double>("Current");
    
    // ******* Check I have the correct convention!!
    std::vector<double> coeffs;
    if( polarity > 0 )
      coeffs = m_scaleUpPtr->param<std::vector<double> >("Coeffs");
    else
      coeffs = m_scaleDownPtr->param<std::vector<double> >("Coeffs");
  
    m_scaleFactor = coeffs[0] + ( coeffs[1]*(current/m_nominalCurrent) );
    m_magFieldGrid.setScaleFactor( m_scaleFactor ) ;
  }
   
  // Update the field map file
  StatusCode sc ;
  if( !m_mapFromOptions ) {
    
    // ******* Check I have the correct convention!!
    std::vector<std::string> files;
    if( polarity > 0 )
      files = m_mapFilesUpPtr->param<std::vector<std::string> >("Files");
    else
      files = m_mapFilesDownPtr->param<std::vector<std::string> >("Files");
    
    // append the path
    for ( std::vector<std::string>::iterator iF = files.begin(); iF != files.end(); ++iF ) 
      *iF = m_mapFilePath + *iF ;

    // test the cache
    if(  m_mapFileNames != files ) {
      // update the cache
      m_mapFileNames = files ;
      // update the field
      sc = m_mapFileNames.size() == 1 ?
	m_magFieldGridReader.readDC06File( m_mapFileNames.front(), m_magFieldGrid ) :
	m_magFieldGridReader.readFiles( m_mapFileNames, m_magFieldGrid ) ;
    }
  }
  
  log << MSG::DEBUG << "Field map files updated: " << m_mapFileNames << endmsg;
  log << MSG::INFO << "Map scaled by factor "    << m_scaleFactor << endmsg;
  return sc ;
}

