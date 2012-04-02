// $Id: TupleToolStripping.cpp,v 1.1 2010-04-26 12:49:59 pkoppenb Exp $
// Include files

// from Gaudi
#include "GaudiKernel/ToolFactory.h"

// local
#include "TupleToolStripping.h"
#include "Event/HltDecReports.h"
#include "Event/HltDecReport.h"
#include "GaudiAlg/Tuple.h"
#include "GaudiAlg/TupleObj.h"

// gaudirun.py ~/cmtuser/Saved-options/DVTestStripping-ReadDST.py ~/cmtuser/Saved-options/DiMuon_100419.py | tee test

//-----------------------------------------------------------------------------
// Implementation file for class : TupleToolStripping
//
// 2010-04-23 : Patrick Koppenburg
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( TupleToolStripping )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
  TupleToolStripping::TupleToolStripping( const std::string& type,
                                          const std::string& name,
                                          const IInterface* parent )
    : TupleToolBase ( type, name , parent )
{
  declareInterface<IEventTupleTool>(this);
  /// @todochange with rootontes
  declareProperty("StrippigReportsLocations", m_location = "/Event/Strip/Phys/DecReports" );
}

//=============================================================================
// Destructor
//=============================================================================
TupleToolStripping::~TupleToolStripping() {}

//=============================================================================

StatusCode TupleToolStripping::fill( Tuples::Tuple& tuple )
{

  const LHCb::HltDecReports* dr = NULL;
  if ( exist<LHCb::HltDecReports>(m_location,false) )
  {
    dr = get<LHCb::HltDecReports>(m_location,false);
  }
  else if ( exist<LHCb::HltDecReports>(m_location) )
  {
    dr = get<LHCb::HltDecReports>(m_location);
  }

  if ( dr )
  {
    if(msgLevel(MSG::DEBUG)) debug() << "There are " << dr->size() << " DecReports at " << m_location << endmsg ;
    const std::vector<std::string> & names = dr->decisionNames() ;
    if(msgLevel(MSG::VERBOSE)) verbose() << "NAMES: " << names << endmsg ;
    unsigned int i = 0 ;
    for ( std::vector<std::string>::const_iterator s = names.begin() ;
          s != names.end() ; ++s )
    {
      if(msgLevel(MSG::VERBOSE)) verbose() << "Trying " << i << " " << *s << endmsg ;
      if ( dr->hasDecisionName(*s) )
      {
        const LHCb::HltDecReport* report = dr->decReport(*s);
        if ( !report ) Exception("Cannot find report "+*s);
        if(msgLevel(MSG::VERBOSE)) verbose() << *s << " says " << report->decision() << endmsg ;
        if ( !tuple->column( *s, report->decision() ) ) return StatusCode::FAILURE;
      }
      else
      {
        Exception("Don't have report name "+*s);
      }
      ++i;
    }
  }
  else
  {
    return Warning( "No DecReports at "+m_location, StatusCode::SUCCESS, 1);
  }

  return StatusCode::SUCCESS;
}

//=============================================================================
