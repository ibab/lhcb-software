// $Id: ApplicationVersionFilter.cpp,v 1.3 2009-11-10 09:27:51 jonrob Exp $
// Include files

// local
#include "ApplicationVersionFilter.h"

//-----------------------------------------------------------------------------
// Implementation file for class : ApplicationVersionFilter
//
// 2009-11-06 : Chris Jones
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( ApplicationVersionFilter )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
  ApplicationVersionFilter::ApplicationVersionFilter( const std::string& name,
                                                      ISvcLocator* pSvcLocator)
    : GaudiAlgorithm ( name , pSvcLocator )
{
  declareProperty( "VersionRegex",   m_regex = "" );
  declareProperty( "HeaderLocation", m_loc = LHCb::ProcessHeaderLocation::Rec );
}

//=============================================================================
// Destructor
//=============================================================================
ApplicationVersionFilter::~ApplicationVersionFilter() {}

//=============================================================================
// Main execution
//=============================================================================
StatusCode ApplicationVersionFilter::execute()
{
  bool OK = true;

  try
  {
    if ( !m_regex.empty() )
    {
      const LHCb::ProcessHeader * header = getIfExists<LHCb::ProcessHeader>(m_loc);
      if ( header )
      {
        // Apply the regex to the application version
        OK = boost::regex_match( header->applicationVersion().c_str(),
                                 boost::regex(m_regex) );
      }
      else
      {
        OK = false;
      }
    }
  }
  catch ( const GaudiException & )
  {
    OK = false;
  }
  
  setFilterPassed(OK);

  return StatusCode::SUCCESS;
}

//=============================================================================
