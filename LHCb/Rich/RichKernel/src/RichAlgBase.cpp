// $Id: RichAlgBase.cpp,v 1.2 2004-07-12 14:51:18 jonrob Exp $

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
  const StatusCode sc = GaudiAlgorithm::initialize();

  // Printout from initialization
  debug() << "Initilize" << endreq;

  // return status
  return sc;
};

//  Finalize
StatusCode RichAlgBase::finalize()
{
  // Printout from finalization
  debug() << "Finalize" << endreq;

  // Finalise base class and return
  return GaudiAlgorithm::finalize();
}
