// $Id: RichAlgBase.cpp,v 1.1 2004-06-17 12:00:48 cattanem Exp $

// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/IToolSvc.h"

// local
#include "RichKernel/RichAlgBase.h"

//-----------------------------------------------------------------------------
// Implementation file for class : RichAlgBase
//
// 05/04/2002 : Chris Jones
//-----------------------------------------------------------------------------

// Standard constructor
RichAlgBase::RichAlgBase( const std::string& name,
                          ISvcLocator* pSvcLocator )
  : GaudiAlgorithm ( name, pSvcLocator ),
    m_toolReg  ( 0 )
{
  declareProperty( "ToolRegistryName", m_regName = "RichToolRegistry" );
}

// Destructor
RichAlgBase::~RichAlgBase() {};

// Initialise
StatusCode RichAlgBase::initialize()
{
  // Execute the base class initialize
  return GaudiAlgorithm::initialize();
};

//  Finalize
StatusCode RichAlgBase::finalize()
{
  // Finalise base class
  return GaudiAlgorithm::finalize();
}
