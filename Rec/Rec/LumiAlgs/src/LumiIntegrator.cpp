// $Id: LumiIntegrator.cpp,v 1.1 2009-06-12 08:17:50 panmanj Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/ToolFactory.h" 

// event model
#include "Event/LumiFSR.h"
#include "Event/LumiIntegral.h"

// local
#include "LumiIntegrator.h"

//-----------------------------------------------------------------------------
// Implementation file for class : LumiIntegrator
//
// 2009-03-16 : Jaap Panman
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( LumiIntegrator );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
LumiIntegrator::LumiIntegrator( const std::string& type,
                                const std::string& name,
                                const IInterface* parent )
  : GaudiTool ( type, name , parent )
{
  declareInterface<ILumiIntegrator>(this);
  m_count_files = 0;
  m_count_events = 0;
}
//=============================================================================
// Destructor
//=============================================================================
LumiIntegrator::~LumiIntegrator() {} 

//=============================================================================
// Integrate Lumi FSR data
//=============================================================================
StatusCode LumiIntegrator::integrate( LHCb::LumiFSR& fsr ){
  m_LumiSum += fsr;
  return StatusCode::SUCCESS; 
}

StatusCode LumiIntegrator::integrate( LHCb::LumiFSR* fsr ){
  m_LumiSum += (*fsr);
  return StatusCode::SUCCESS; 
}

StatusCode LumiIntegrator::integrate( LHCb::LumiIntegral& fsr ){
  m_LumiSum += fsr;
  return StatusCode::SUCCESS; 
}

StatusCode LumiIntegrator::integrate( LHCb::LumiIntegral* fsr ){
  m_LumiSum += (*fsr);
  return StatusCode::SUCCESS; 
}

//=============================================================================
// Retrieve the integrated Lumi FSR data
//=============================================================================
const LHCb::LumiIntegral& LumiIntegrator::integral( ) const{
  return m_LumiSum; 
}

//=============================================================================
// Count the number of events for a given file
//=============================================================================
void LumiIntegrator::countEvents( ) {
  m_count_events++;
}

//=============================================================================
// Retrieve the event count for a file
//=============================================================================
unsigned long LumiIntegrator::events( ) const{
  return m_count_events; 
}

//=============================================================================
// Check the event count for a file
//=============================================================================
bool LumiIntegrator::checkEvents( ) const{
  return true;
}
