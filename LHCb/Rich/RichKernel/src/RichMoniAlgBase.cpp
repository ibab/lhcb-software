
//-----------------------------------------------------------------------------
/** @file RichMoniAlgBase.cpp
 *
 *  Implementation file for class : RichMoniAlgBase
 *
 *  CVS Log :-
 *  $Id: RichMoniAlgBase.cpp,v 1.2 2005-01-13 15:03:36 jonrob Exp $
 *  $Log: not supported by cvs2svn $
 *  Revision 1.1  2005/01/07 13:16:42  jonrob
 *  Add to CVS
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
#include "RichKernel/RichMoniAlgBase.h"

// Standard constructor
RichMoniAlgBase::RichMoniAlgBase( const std::string& name,
                                  ISvcLocator* pSvcLocator )
  : GaudiTupleAlg ( name, pSvcLocator ),
    m_toolReg  ( 0 )
{
  declareProperty( "ToolRegistryName", m_regName = "RichToolRegistry" );
}

// Destructor
RichMoniAlgBase::~RichMoniAlgBase() {};

// Initialise
StatusCode RichMoniAlgBase::initialize()
{
  // Execute the base class initialize
  const StatusCode sc = GaudiTupleAlg::initialize();

  // Printout from initialization
  debug() << "Initialize" << endreq;

  // return status
  return sc;
};

// Main execute method
StatusCode RichMoniAlgBase::execute()
{
  // All algorithms should re-implement this method
  return Error ( "Default RichMoniAlgBase::execute() called !!" );
}

//  Finalize
StatusCode RichMoniAlgBase::finalize()
{
  // Printout from finalization
  debug() << "Finalize" << endreq;

  // Finalise base class and return
  return GaudiTupleAlg::finalize();
}
