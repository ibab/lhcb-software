
//============================================================================
/** @file RichToolBase.cpp
 *
 *  Implementation file for tool base class : RichToolBase
 *
 *  CVS Log :-
 *  $Id: RichToolBase.cpp,v 1.5 2005-02-02 09:59:34 jonrob Exp $
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
#include "RichKernel/RichToolBase.h"

// Standard constructor, initializes variables
RichToolBase::RichToolBase( const std::string& type,
                            const std::string& name,
                            const IInterface* parent )
  : GaudiTool ( type, name, parent ),
    m_toolReg ( 0 )
{
  declareProperty( "ToolRegistryName", m_regName = "RichToolRegistry" );
}

StatusCode RichToolBase::initialize() 
{
  // Execute the base class initialize
  const StatusCode sc = GaudiTool::initialize();
  
  // Printout from initialization
  debug() << "Initialize" << endreq;
  
  // return status
  return sc;
}

StatusCode RichToolBase::finalize() 
{
  // Printout from finalization
  debug() << "Finalize" << endreq;

  // Finalise base class and return
  return GaudiTool::finalize();
}
