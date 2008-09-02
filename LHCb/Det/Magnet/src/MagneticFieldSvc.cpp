// $Id: MagneticFieldSvc.cpp,v 1.39 2008-09-02 09:11:50 cattanem Exp $

// Include files
#include "GaudiKernel/SvcFactory.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SystemOfUnits.h"

#include "MagneticFieldSvc.h"
#include "MagnetCondLocations.h"
#include "IMagFieldTool.h"

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
                                                         m_fieldTool(0),
                                                         m_DC06FieldUp(0),
                                                         m_DC06FieldDown(0),
                                                         m_RealFieldUp(0),
                                                         m_RealFieldDown(0),
                                                         m_updMgrSvc(0),
                                                         m_toolSvc(0)
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

  // Obsolete properties, will be removed soon, do not use!
  declareProperty( "UseRealMap",    m_useRealMap   = false );
  declareProperty( "FieldMapFile",  m_filename     = "" );
  declareProperty( "FieldMapFileQ1",m_qfilename[0] = "" );
  declareProperty( "FieldMapFileQ2",m_qfilename[1] = "" );
  declareProperty( "FieldMapFileQ3",m_qfilename[2] = "" );
  declareProperty( "FieldMapFileQ4",m_qfilename[3] = "" );
  declareProperty( "CondPath", m_condPath = "" );
  
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

  // Backward compatibility of old options
  if( m_filename != "" || m_qfilename[0] != "" || m_qfilename[1] != ""
                       || m_qfilename[2] != "" || m_qfilename[3] != "") {
    if( m_mapFileNames.size() > 0 ) {
      log << MSG::ERROR << "Both old and new style FieldMapFile options set, don't know what to do!" << endmsg;
      return StatusCode::FAILURE;
    }
    else if( m_filename != "" && m_qfilename[0] == "" && m_qfilename[1] == ""
                              && m_qfilename[2] == "" && m_qfilename[3] == "") {
      log << MSG::WARNING << "Using obsolete DC06 option FieldMapFile, please change to FieldMapFiles" << endmsg;
      m_mapFileNames.push_back( m_filename );
    }
    else if( m_filename == "" && m_qfilename[0] != "" && m_qfilename[1] != ""
                              && m_qfilename[2] != "" && m_qfilename[3] != "") {
      log << MSG::WARNING << "Using obsolete real map options FieldMapFileQ*, please change to FieldMapFiles" << endmsg;
      m_mapFileNames.push_back( m_qfilename[0] );
      m_mapFileNames.push_back( m_qfilename[1] );
      m_mapFileNames.push_back( m_qfilename[2] );
      m_mapFileNames.push_back( m_qfilename[3] );
    }
    else {
      log << MSG::ERROR << "Using invalid combination of FieldMapFile and FieldMapfileQ* options" << endmsg;
      return StatusCode::FAILURE;
    }
  }
  if( m_condPath != "" )
    log << MSG::WARNING << "Obsolete property CondPath given but has no effect" << endmsg;

  // Tool service, for access to the Field map tool(s)
  status = service("ToolSvc", m_toolSvc );
  if ( status.isFailure() ) {
    log << MSG::ERROR << "Cannot find the ToolSvc" << endmsg;
    return status;
  }

  if( m_useConstField ) {
    // Constant field requested, do not use any field map
    log << MSG::WARNING << "using constant magnetic field with field vector "
        << m_constFieldVector << " (Tesla)" << endmsg;
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
  
  // Value of polarity irrelevant when using options
  // Parse the file via the appropriate tool
  return updateTool( 1 );
}

//=============================================================================
// QueryInterface
//=============================================================================
StatusCode MagneticFieldSvc::queryInterface( const InterfaceID& riid, 
                                             void** ppvInterface      ) 
{
  if ( IID_IMagneticFieldSvc.versionMatch(riid) ) {
    *ppvInterface = (IMagneticFieldSvc*)this;
    addRef();
    return StatusCode::SUCCESS;
  } else if ( IID_ILHCbMagnetSvc.versionMatch(riid) ) {
    *ppvInterface = (ILHCbMagnetSvc*)this;
    addRef();
    return StatusCode::SUCCESS;
  }
  return Service::queryInterface(riid,ppvInterface);
}

//=============================================================================
// FieldVector: find the magnetic field value at a given point in space
//=============================================================================
StatusCode MagneticFieldSvc::fieldVector(const Gaudi::XYZPoint&  r,
                                               Gaudi::XYZVector& bf ) const {

  if( m_useConstField ) {
    bf.SetXYZ( m_constFieldVector[0]*Gaudi::Units::tesla,
               m_constFieldVector[1]*Gaudi::Units::tesla,
               m_constFieldVector[2]*Gaudi::Units::tesla );
    return StatusCode::SUCCESS;
  }

  // Forward request to the tool
  m_fieldTool->fieldVector( r, bf );
  return StatusCode::SUCCESS;
  
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
  }
   
  // Update the field map file
  if( !m_mapFromOptions ) {
    m_mapFileNames.clear();
    
    // ******* Check I have the correct convention!!
    std::vector<std::string> files;
    if( polarity > 0 )
      files = m_mapFilesUpPtr->param<std::vector<std::string> >("Files");
    else
      files = m_mapFilesDownPtr->param<std::vector<std::string> >("Files");

    for ( std::vector<std::string>::const_iterator iF = files.begin(); iF != files.end(); ++iF ) {
      m_mapFileNames.push_back( m_mapFilePath + *iF );
    }
  }
  
  log << MSG::DEBUG << "Field map files updated: " << m_mapFileNames << endmsg;
  log << MSG::DEBUG << "Scale factor updated: "    << m_scaleFactor << endmsg;

  // Finally update the appropriate tool
  return updateTool( polarity );
}

//=============================================================================
StatusCode MagneticFieldSvc::updateTool( int polarity )
//=============================================================================
{
  // Depending on the polarity and the type of field map, update the tool
  MsgStream log(msgSvc(), name());
  StatusCode sc = StatusCode::FAILURE;

  if( polarity == 0 ) {
    polarity = 1;
    log << MSG::INFO << "Polarity not set, using default = 1 (UP) " << endmsg;
  }

  if(  m_mapFileNames.size() == 1 ) {
    // DC06 case
    if( polarity > 0 ) {
      if( 0 == m_DC06FieldUp ) {
        sc = m_toolSvc->retrieveTool( "MagFieldToolDC06", "MagToolDC06Up",
                                      m_DC06FieldUp, this );
        if( sc.isFailure() ) {
          log << MSG::ERROR << "Could not retrieve MagToolDC06Up" << endmsg;
          return sc;
        }
        m_fieldTool = m_DC06FieldUp;
      }
    }
    else {
      if( 0 == m_DC06FieldDown ) {
        sc = m_toolSvc->retrieveTool( "MagFieldToolDC06", "MagToolDC06Down",
                                      m_DC06FieldDown, this );
        if( sc.isFailure() ) {
          log << MSG::ERROR << "Could not retrieve MagToolDC06Down" << endmsg;
          return sc;
        }
        m_fieldTool = m_DC06FieldDown;
      }
    }
  }
  else if( m_mapFileNames.size() == 4 ) {
    // Real map case
    if( polarity > 0 ) {
      if( 0 == m_RealFieldUp ) {
        sc = m_toolSvc->retrieveTool( "MagFieldTool", "MagToolRealUp",
                                      m_RealFieldUp, this );
        if( sc.isFailure() ) {
          log << MSG::ERROR << "Could not retrieve MagToolRealUp" << endmsg;
          return sc;
        }
        m_fieldTool = m_RealFieldUp;
      }
    }
    else {
      if( 0 == m_RealFieldDown ) {
        sc = m_toolSvc->retrieveTool( "MagFieldTool", "MagToolRealDown",
                                      m_RealFieldDown, this );
        if( sc.isFailure() ) {
          log << MSG::ERROR << "Could not retrieve MagToolRealDown" << endmsg;
          return sc;
        }
        m_fieldTool = m_RealFieldDown;
      }
    }
  }
  else {
    log << MSG::ERROR 
        << "Wrong number of field map files, don't know what to do" << endmsg;
    return StatusCode::FAILURE;
  }

  // Update the Tool
  return m_fieldTool->updateMap( m_mapFileNames, m_scaleFactor );
}
