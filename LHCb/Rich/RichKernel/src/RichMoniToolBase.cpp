
//============================================================================
/** @file RichMoniToolBase.cpp
 *
 *  Implementation file for tool base class : RichMoniToolBase
 *
 *  CVS Log :-
 *  $Id: RichMoniToolBase.cpp,v 1.3 2005-02-02 09:59:34 jonrob Exp $
 *
 *  @author Chris Jones    Christopher.Rob.Jones@cern.ch
 *  @date   2002-07-26
 */
//============================================================================

// from Gaudi
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/GaudiException.h"

// local
#include "RichKernel/RichMoniToolBase.h"

// Standard constructor, initializes variables
RichMoniToolBase::RichMoniToolBase( const std::string& type,
                                    const std::string& name,
                                    const IInterface* parent )
  : GaudiTupleTool ( type, name, parent ),
    m_toolReg ( 0 )
{
  declareProperty( "ToolRegistryName", m_regName = "RichToolRegistry" );
}

StatusCode RichMoniToolBase::initialize()
{
  // Execute the base class initialize
  const StatusCode sc = GaudiTupleTool::initialize();

  // Printout from initialization
  debug() << "Initialize" << endreq;

  // return status
  return sc;
}

StatusCode RichMoniToolBase::finalize()
{
  // Printout from finalization
  debug() << "Finalize" << endreq;

  // Finalise base class and return
  return GaudiTupleTool::finalize();
}
