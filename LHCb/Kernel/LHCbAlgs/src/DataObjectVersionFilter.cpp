// $Id: DataObjectVersionFilter.cpp,v 1.2 2009-11-09 15:01:11 jonrob Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 

// local
#include "DataObjectVersionFilter.h"

//-----------------------------------------------------------------------------
// Implementation file for class : DataObjectVersionFilter
//
// 2009-11-06 : Chris Jones
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( DataObjectVersionFilter );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
DataObjectVersionFilter::DataObjectVersionFilter( const std::string& name,
                                                  ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
{
  declareProperty( "MinVersion", m_minV = 0 );
  declareProperty( "MaxVersion", m_maxV = 99 );
  declareProperty( "DataObjectLocation", m_loc = "" );
}

//=============================================================================
// Destructor
//=============================================================================
DataObjectVersionFilter::~DataObjectVersionFilter() {} 

//=============================================================================
// Main execution
//=============================================================================
StatusCode DataObjectVersionFilter::execute() 
{
  bool OK = true;

  try
  {
    const DataObject * data = get<DataObject>(m_loc);
    const unsigned char ver = data->version();
    if ( msgLevel(MSG::DEBUG) )
      debug() << "version = " << (unsigned int)ver << endmsg;
    OK = ver <= m_maxV && ver >= m_minV;
  }
  catch ( const GaudiException & )
  {
    OK = false;
  }
  
  setFilterPassed(OK);

  return StatusCode::SUCCESS;
}

//=============================================================================
