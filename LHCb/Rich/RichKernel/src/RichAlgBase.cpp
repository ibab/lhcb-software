
//-----------------------------------------------------------------------------
/** @file RichAlgBase.cpp
 *
 *  Implementation file for class : RichAlgBase
 *
 *  CVS Log :-
 *  $Id: RichAlgBase.cpp,v 1.4 2004-07-26 17:53:17 jonrob Exp $
 *  $Log: not supported by cvs2svn $
 *
 *  @author Chris Jones    Christopher.Rob.Jones@cern.ch
 *  @date   2002-04-05
 */
//-----------------------------------------------------------------------------

// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/IToolSvc.h"

// local
#include "RichKernel/RichAlgBase.h"

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
  debug() << "Initialize" << endreq;

  // return status
  return sc;
};

// Main execute method
StatusCode RichAlgBase::execute()
{
  // All algorithms should re-implement this method
  return Error ( "Default RichAlgBase::execute() called !!" );
}

//  Finalize
StatusCode RichAlgBase::finalize()
{
  // Printout from finalization
  debug() << "Finalize" << endreq;

  // Finalise base class and return
  return GaudiAlgorithm::finalize();
}
