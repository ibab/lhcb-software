// $Id: RichRecAlgBase.cpp,v 1.8 2003-07-03 14:46:40 jonesc Exp $
// Include files

// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/IToolSvc.h"

// Event
#include "Event/RichRecStatus.h"

// local
#include "RichRecBase/RichRecAlgBase.h"

//-----------------------------------------------------------------------------
// Implementation file for class : RichRecAlgBase
//
// 05/04/2002 : Chris Jones
//-----------------------------------------------------------------------------

// Standard constructor
RichRecAlgBase::RichRecAlgBase( const std::string& name,
                                ISvcLocator* pSvcLocator )
  : Algorithm ( name, pSvcLocator ),
    m_richTracks(0),
    m_richPixels(0),
    m_richSegments(0),
    m_richPhotons(0),
    m_richStatus(0) { 
  
  declareProperty( "ToolRegistryName", m_regName = "RichToolRegistry" );

}

// Destructor
RichRecAlgBase::~RichRecAlgBase() {};

// Initialise
StatusCode RichRecAlgBase::initialize() {

  // Get the current message service printout level
  IntegerProperty msgLevel;
  IProperty* algIProp;
  this->queryInterface( IID_IProperty, (void**)&algIProp );
  msgLevel.assign( algIProp->getProperty( "OutputLevel" ) );
  m_msgLevel = msgLevel;
  algIProp->release();

  // Get pointer to Rich Tool Registry
  if ( !toolSvc()->retrieveTool( "RichToolRegistry",  m_regName, m_toolReg) ) {
    MsgStream msg( msgSvc(), name() );
    msg << MSG::ERROR << "RichToolRegistry not found" << endreq;
    return StatusCode::FAILURE;
  }

  // initialise data pointers
  acquireTool("RichSegmentCreator", m_segTool);
  m_richSegments = &(m_segTool->richSegments());
  acquireTool("RichPhotonCreator", m_photTool);
  m_richPhotons = &(m_photTool->richPhotons());
  acquireTool("RichPixelCreator", m_pixTool);
  m_richPixels = &(m_pixTool->richPixels());
  acquireTool("RichTrackCreator", m_tkTool);
  m_richTracks = &(m_tkTool->richTracks());
  acquireTool("RichStatusCreator", m_statTool);
  m_richStatus = &(m_statTool->richStatus());

  return StatusCode::SUCCESS;
};

//  Finalize
StatusCode RichRecAlgBase::finalize() {

  // Release all tools
  releaseTool( m_toolReg );
  releaseTool( m_pixTool );
  releaseTool( m_tkTool );
  releaseTool( m_statTool );
  releaseTool( m_segTool );
  releaseTool( m_photTool );

  return StatusCode::SUCCESS;
}
