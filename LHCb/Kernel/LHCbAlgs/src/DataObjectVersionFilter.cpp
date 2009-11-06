// $Id: DataObjectVersionFilter.cpp,v 1.1 2009-11-06 18:04:23 jonrob Exp $
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
  declareProperty( "MinVersion", m_minV = -99999 );
  declareProperty( "MaxVersion", m_maxV =  99999 );
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
    OK = data->version() <= m_maxV && data->version() >= m_minV;
  }
  catch ( const GaudiException & )
  {
    info() << "BAAAAAAAAAAAAAAAAAAAAAAAA" << endmsg;
    OK = false;
  }
  
  setFilterPassed(OK);

  return StatusCode::SUCCESS;
}

//=============================================================================
