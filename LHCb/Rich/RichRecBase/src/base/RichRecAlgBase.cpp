// $Id: RichRecAlgBase.cpp,v 1.5 2003-07-02 09:02:26 jonrob Exp $
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

// interfaces
#include "RichRecBase/IRichSegmentCreator.h"
#include "RichRecBase/IRichTrackCreator.h"
#include "RichRecBase/IRichPhotonCreator.h"
#include "RichRecBase/IRichPixelCreator.h"
#include "RichRecBase/IRichStatusCreator.h"

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
    m_richStatus(0),
    m_toolList() { }

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
  if ( !toolSvc()->retrieveTool( "RichToolRegistry", m_toolReg) ) {
    MsgStream msg( msgSvc(), name() );
    msg << MSG::ERROR << "RichToolRegistry not found" << endreq;
    return StatusCode::FAILURE;
  }
  
  // initialise data pointers
  IRichSegmentCreator * segTool; acquireTool("RichSegmentCreator", segTool);
  m_richSegments = &(segTool->richSegments());
  IRichPhotonCreator * photTool; acquireTool("RichPhotonCreator", photTool);
  m_richPhotons = &(photTool->richPhotons());
  IRichPixelCreator * pixTool; acquireTool("RichPixelCreator", pixTool);
  m_richPixels = &(pixTool->richPixels());
  IRichTrackCreator * tkTool; acquireTool("RichTrackCreator", tkTool);
  m_richTracks = &(tkTool->richTracks());
  IRichStatusCreator * statTool; acquireTool("RichStatusCreator", statTool);
  m_richStatus = &(statTool->richStatus());

  return StatusCode::SUCCESS;
};

//  Finalize
StatusCode RichRecAlgBase::finalize() {

  // Release all tools
  for ( ToolList::iterator it = m_toolList.begin();
        it != m_toolList.end(); ++it ) {
    if ( (*it).second ) { releaseTool((*it).first); }
  }
  if ( m_toolReg ) { toolSvc()->releaseTool( m_toolReg ); m_toolReg = NULL; }

  return StatusCode::SUCCESS;
}
