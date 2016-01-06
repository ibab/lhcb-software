// $Id: CheckMCEventTool.cpp,v 1.3 2007-01-12 10:10:51 cattanem Exp $
// Include files 

// Event model
#include "Event/MCHeader.h"
#include "Event/MCParticle.h"
#include "Event/MCVertex.h"

// local
#include "CheckMCEventTool.h"

//-----------------------------------------------------------------------------
// Implementation file for class : CheckMCEventTool
//
// 2003-02-24 : Marco Cattaneo
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY(CheckMCEventTool)

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
CheckMCEventTool::CheckMCEventTool( const std::string& type,
                                    const std::string& name,
                                    const IInterface* parent )
  : GaudiTool ( type, name , parent ),
    m_event(-1),
    m_status( StatusCode::SUCCESS )
{
  declareInterface<ICheckTool>(this);
}

CheckMCEventTool::~CheckMCEventTool() {}

StatusCode CheckMCEventTool::check()
{
  // If already processed this event, return previous status
  LHCb::MCHeader* evt = get<LHCb::MCHeader>( LHCb::MCHeaderLocation::Default );
  if( m_event == evt->evtNumber() ) return m_status;

  m_event  = evt->evtNumber();
  m_status = StatusCode::SUCCESS;

  // Loop over MCVertices
  LHCb::MCVertices*  mcVerts = 
    get<LHCb::MCVertices>( LHCb::MCVertexLocation::Default );
  LHCb::MCVertices::iterator itV;
  for( itV = mcVerts->begin(); itV != mcVerts->end(); itV++ ) {
    SmartRefVector<LHCb::MCParticle> daughters = (*itV)->products();
    SmartRefVector<LHCb::MCParticle>::const_iterator itP;
    for( itP = daughters.begin(); itP != daughters.end(); itP++ ) {
      // Check that mother of daughter corresponds...
      if( 0 == (*itP)->originVertex() ) {
        err() << "Inconsistent mother-daughter relation!!"
              << "  Particle " << (*itP)->key() 
              << " is daughter of vertex " << (*itV)->key()
              << " but has no origin vertex" << endmsg;
        m_status = StatusCode::FAILURE;
      }
      else if( (*itP)->originVertex()->key() != (*itV)->key() ) {
        err() << "Inconsistent mother-daughter relation!!"
              << MSG::ERROR << "  Particle " << (*itP)->key() 
              << " is daughter of vertex " << (*itV)->key()
              << " but has vertex " << (*itP)->originVertex()->key()
              << " as mother" << endmsg;
        m_status = StatusCode::FAILURE;
      }
    }
  }

  return m_status;
}

//=============================================================================
