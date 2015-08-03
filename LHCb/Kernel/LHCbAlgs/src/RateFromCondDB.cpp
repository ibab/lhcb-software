// Include files 

// local
#include "RateFromCondDB.h"
#include "GaudiKernel/SystemOfUnits.h"

//-----------------------------------------------------------------------------
// Implementation file for class : RateFromCondDB
//
// 2011-08-10 : Patrick Koppenburg
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( RateFromCondDB )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
RateFromCondDB::RateFromCondDB( const std::string& type,
                          const std::string& name,
                          const IInterface* parent )
  : GaudiTool ( type, name , parent ),
    m_condTrigger(0),
    m_rate(0.)
{
  declareInterface<RateFromCondDB>(this);

  declareProperty("Condition", m_condition = "Conditions/Online/LHCb/RunInfo/NoBiasSettings","Name of Condition");
  declareProperty("LineRate", m_lineRate = "NoBiasRate","Name of Line Rate");
  declareProperty("Unit", m_unit = 1000., "Unit of rate in CondDB (default: kHz)");
  
}
//=============================================================================
// Initialize
//=============================================================================
bool RateFromCondDB::initializeCondDB(){
  if (UNLIKELY( msgLevel(MSG::DEBUG))) debug() << "Looking for " << m_condition << endmsg;
  if (this->existDet<Condition>(m_condition)) {
      // Register condition and read parameters values
    registerCondition(m_condition,
                      m_condTrigger, &RateFromCondDB::i_updateConditions);   
    if (UNLIKELY( msgLevel(MSG::DEBUG))) debug() << m_condition << " found" << endmsg;
  } else {
    warning() << m_condition << " Not found. I suggest you try to get it from TCK." << endmsg;
    return false ;    
  }
  if (UNLIKELY( msgLevel(MSG::DEBUG))) debug() << "Got condition: " << m_condition << endmsg;  
  StatusCode sc = runUpdate() ;  // initial update of DB
  if (!sc) Exception("runUpdate failed");
  return true;
}
//=============================================================================
// get rate - Moved from Jaap Panman's 
//=============================================================================
double RateFromCondDB::getRate() const {
  // get rate from CondDB
  if (UNLIKELY( msgLevel(MSG::DEBUG))) debug() << "==> rate from CondDB: " << m_rate << " Hz" << endmsg;
  return m_rate;
}
//=========================================================================
//  Extract data from Trigger
//=========================================================================
StatusCode RateFromCondDB::i_updateConditions() {
  if (UNLIKELY( msgLevel(MSG::DEBUG))) debug() << "callback" << endmsg;
  if (!m_condTrigger) Exception("NULL m_condTrigger in i_updateConditions()");
  if (!m_condTrigger->exists(m_lineRate)){
    err() << "Conditions " << m_condition << " does not contain " << m_lineRate << endmsg;
    err() << "Fix your options!" << endmsg;
    return StatusCode::FAILURE ;
  }
  m_rate = (double) m_condTrigger->param<double>(m_lineRate)*m_unit;
  
  if (UNLIKELY( msgLevel(MSG::DEBUG))) debug() << "Updated " << m_lineRate << " to " << m_rate << endmsg ;
  return StatusCode::SUCCESS;
}

//=============================================================================
