// $Id: PVLocator.cpp,v 1.1 2004-10-27 13:49:59 pkoppenb Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/ToolFactory.h" 

// from Event model
#include "Event/Vertex.h" 

// local
#include "PVLocator.h"

//-----------------------------------------------------------------------------
// Implementation file for class : PVLocator
//
// 2004-10-27 : Patrick KOPPENBURG
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
static const  ToolFactory<PVLocator>          s_factory ;
const        IToolFactory& PVLocatorFactory = s_factory ; 


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
PVLocator::PVLocator( const std::string& type,
                      const std::string& name,
                      const IInterface* parent )
  : GaudiTool ( type, name , parent )
{
  declareInterface<IPVLocator>(this);

  m_PVLocation = VertexLocation::Primary ; // offline PV is default
  declareProperty( "PVLocation", m_PVLocation );
}
//=============================================================================
// Destructor
//=============================================================================
PVLocator::~PVLocator() {}; 

//=============================================================================
// get PV
//=============================================================================
std::string PVLocator::getPVLocation(void) const {
  verbose() << "Returning PV location " <<  m_PVLocation << endreq ;
  return m_PVLocation ;
};
//=============================================================================
// set PV
//=============================================================================
StatusCode PVLocator::setPVLocation(std::string & PV){
  m_PVLocation = PV ;
  info() << "Set new PV location " << PV << endreq ;
  return StatusCode::SUCCESS ;
};
